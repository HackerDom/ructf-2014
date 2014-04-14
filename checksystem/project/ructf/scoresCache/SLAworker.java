package ructf.scoresCache;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.Hashtable;

import org.apache.log4j.Logger;

import ructf.main.CheckerExitCode;
import ructf.main.DatabaseManager;

public class SLAworker extends Thread{
	
	private static String sqlGetLastSla = "SELECT sla.team_id, sla.service_id, sla.successed, sla.failed, sla.time FROM (SELECT team_id, service_id, MAX(time) AS time FROM sla GROUP BY team_id, service_id) last_times INNER JOIN sla ON last_times.time=sla.time AND last_times.team_id=sla.team_id AND last_times.service_id=sla.service_id";
	private static String sqlGetLastAccessChecks = "SELECT team_id, service_id, status, count(*), max(time) FROM access_checks WHERE time > ? GROUP BY team_id, service_id, status";
	private static String sqlInsertSla = "INSERT INTO sla (team_id, service_id, successed, failed, time) VALUES (?, ?, ?, ?, ?)";
	private static PreparedStatement stGetLastSla;
	private static PreparedStatement stGetLastAccessChecks;
	private static PreparedStatement stInsertSla;
	
	
	private Logger logger = Logger.getLogger("ructf.scoresCache");
	private Connection conn;
	
	public SLAworker() throws SQLException {		
		this.conn = DatabaseManager.CreateConnection();
		PrepareStatements(conn);		
	}
	
	private Hashtable<TeamService, SLA> GetStateFromDb() throws SQLException {
		ResultSet res = stGetLastSla.executeQuery();
		
		Hashtable<TeamService, SLA> result = new Hashtable<TeamService, SLA>(); 
		
		while(res.next()){
			int team = res.getInt(1);
			int service = res.getInt(2);
			int successed = res.getInt(3);			
			int failed = res.getInt(4);
			Timestamp time = res.getTimestamp(5);
			
			result.put(new TeamService(team, service), new SLA(team, service, successed, failed, time));
		}
		return result;	
	}

	public void run() {
		try
		{
			logger.info("Getting SLA state from db");
			Hashtable<TeamService,SLA> stateFromDb = GetStateFromDb();
			Timestamp lastKnownTime = GetLastKnownTime(stateFromDb);
			if (lastKnownTime == null)
			{
				logger.info("Empty SLA state in db. Considering timestamp to start as 0");
				lastKnownTime = new Timestamp(0);
			}

			logger.info(String.format("SLA LastKnownTime: %s", lastKnownTime.toString()));			
			DoJobLoop(stateFromDb, lastKnownTime);
		}
		catch(Exception e){
			logger.fatal("General error in SLA thread", e);
			e.printStackTrace();
			System.exit(2);
		}		
	}
	
	private void DoJobLoop(Hashtable<TeamService, SLA> state, Timestamp lastKnownTime) throws SQLException, InterruptedException {		
		conn.setAutoCommit(false);		
		
		while (true) {
			logger.info(String.format("Getting new SLA data from time %s", lastKnownTime.toString()));
			stGetLastAccessChecks.setTimestamp(1, lastKnownTime);
			ResultSet res = stGetLastAccessChecks.executeQuery();
			
			Hashtable<TeamService, SLA> stateDelta = new Hashtable<TeamService, SLA>();
			
			while (res.next()) {				
				int team = res.getInt(1);
				int service = res.getInt(2);
				int status = res.getInt(3);
				if(CheckerExitCode.isUnknown(status))
					status = CheckerExitCode.Down.toInt();				
				int count = res.getInt(4);
				Timestamp time = res.getTimestamp(5);
				
				TeamService key = new TeamService(team, service);
				if(!stateDelta.containsKey(key))
					stateDelta.put(key, new SLA(team, service, 0, 0, time));
				SLA slaDelta = stateDelta.get(key);

				if(status == CheckerExitCode.OK.toInt())
					slaDelta.succeeded+=count;
				else
					slaDelta.failed+=count;
				slaDelta.time = Max(slaDelta.time, time);
			}			
			
			for (TeamService key : stateDelta.keySet()) {
				SLA slaDelta = stateDelta.get(key);
				if(!state.containsKey(key))
					state.put(key, slaDelta);
				else {
					SLA sla = state.get(key);					 
					sla.succeeded += slaDelta.succeeded;
					sla.failed += slaDelta.failed;
					sla.time = slaDelta.time;					
				}
				lastKnownTime = Max(state.get(key).time, lastKnownTime);
				logger.info(String.format("(Team, service) (%d, %d) delta: succeeded = %d, failed = %d, time = %s)", slaDelta.team, slaDelta.service, slaDelta.succeeded, slaDelta.failed, slaDelta.time.toString()));
			}
			
			try {
				for (TeamService key : stateDelta.keySet()) {
					SLA sla = state.get(key);
					
					stInsertSla.setInt(1, sla.team);
					stInsertSla.setInt(2, sla.service);
					stInsertSla.setInt(3, sla.succeeded);
					stInsertSla.setInt(4, sla.failed);
					stInsertSla.setTimestamp(5, sla.time);
					stInsertSla.execute();					
					
					int total = sla.succeeded + sla.failed;
					
					logger.info(String.format("(Team, service) (%d, %d) result: succeeded = %d, failed = %d (SLA %f), time = %s)", sla.team, sla.service, sla.succeeded, sla.failed, total > 0 ? ((double) sla.succeeded) / total : 1.0d, sla.time.toString()));
				}				
				
				conn.commit();
			}
			catch (SQLException exception)
			{
				try {
					conn.rollback();
					throw exception;
				} catch (SQLException rollbackException) {
					logger.error("Failed to rollback sla transaction", rollbackException);
				}
				logger.error("Failed to insert sla data in database", exception);
			}					
			
			logger.info("Sleeping SLA ... ");
			
			Thread.sleep(10000);
		}
	}
	
	private static Timestamp Max(Timestamp t1, Timestamp t2){
		if (t1.before(t2))
			return t2;
		return t1;
	}

	private static Timestamp GetLastKnownTime(Hashtable<TeamService, SLA> stateFromDb) {
		Timestamp max = null;
		for (SLA ts : stateFromDb.values()) {
			if (max == null || max.before(ts.time))
				max = ts.time;
		}
		return max;
	}
	
	private void PrepareStatements(Connection conn) throws SQLException{		
		stGetLastAccessChecks = conn.prepareStatement(sqlGetLastAccessChecks);
		stGetLastSla = conn.prepareStatement(sqlGetLastSla);
		stInsertSla = conn.prepareStatement(sqlInsertSla);
	}
}
