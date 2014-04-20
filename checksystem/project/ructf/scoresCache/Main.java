package ructf.scoresCache;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import ructf.daemon.DaemonSettings;
import ructf.dbObjects.Service;
import ructf.dbObjects.Team;
import ructf.main.Constants;
import ructf.main.DatabaseManager;

public class Main {
	
	private static Logger logger = Logger.getLogger("ructf.scoresCache");
	
	private static String sqlGetRoundTimes = "SELECT time FROM rounds ORDER BY n";
	private static String sqlGetLastScores = "SELECT round, score.team_id, score.service_id, score.score, score.time FROM (SELECT team_id, service_id, MAX(time) AS time FROM score GROUP BY team_id, service_id) last_times INNER JOIN score ON last_times.time=score.time AND last_times.team_id=score.team_id AND last_times.service_id=score.service_id";
	private static String sqlCreateInitState = "INSERT INTO score (round, time, team_id, service_id, score) SELECT 0, '2009-01-01', teams.id, services.id, services.is_not_task::int * (select 100 * count(*) FROM teams) FROM teams CROSS JOIN services WHERE teams.enabled=TRUE";
	private static String sqlGetStealsOfRottenFlags = "SELECT flags.flag_data,flags.time,stolen_flags.victim_team_id,stolen_flags.victim_service_id,stolen_flags.team_id FROM flags INNER JOIN stolen_flags ON flags.flag_data=stolen_flags.flag_data WHERE flags.time > ? AND extract(epoch FROM now() - flags.time) > ?";
	private static String sqlInsertScore = "INSERT INTO score (round, time, team_id, service_id, score) VALUES (?,?,?,?,?)";
	
	private static String sqlSelectTaskScore = "SELECT stolen_task_flags.team_id, sum(flag_price.price) FROM stolen_task_flags INNER JOIN (select flag_data, 1.00000001 / count(*) as price FROM stolen_task_flags WHERE time < ? GROUP BY flag_data) as flag_price ON stolen_task_flags.flag_data = flag_price.flag_data GROUP BY stolen_task_flags.team_id";
		
	private static PreparedStatement stGetRoundTimes;
	private static PreparedStatement stGetLastScores;	
	private static PreparedStatement stCreateInitState;
	private static PreparedStatement stGetStealsOfRottenFlags;
	private static PreparedStatement stInsertScore;
	private static PreparedStatement stSelectTaskScore;
	
	public static void main(String[] args) {			
		PropertyConfigurator.configure(Constants.log4jConfigFile);
		logger.info("Started");
		try
		{
			if (args.length > 0)
				Constants.Initialize(args[0]);
			
			DatabaseManager.Initialize();
			
			Connection conn = DatabaseManager.CreateConnection();
			PrepareStatements(conn);
			
			Thread.currentThread().setName("SCORE");
			
			logger.info("Getting score state from db");
			Hashtable<TeamService,TeamScore> stateFromDb = GetStateFromDb();
			if (stateFromDb.isEmpty()) {
				logger.info("Creating init score in db");
				CreateInitStateInDb();
				logger.info("Score CreateInitStateInDb completed");
				stateFromDb = GetStateFromDb();
			}
			
			if(stateFromDb.size() == 0){
				logger.error("Still empty state. Possible empty 'teams' or 'services' tables? Exiting");
				return;
			}
									
			Timestamp lastKnownTime = GetLastKnownTime(stateFromDb);
			int lastRound = GetLastRound(stateFromDb);
			logger.info(String.format("Score LastKnownTime: %s, lastKnownRound: %d", lastKnownTime.toString(), lastRound));
			
			SLAworker slaWorker = new SLAworker();
			slaWorker.start();
			
			DoJobLoop(conn, stateFromDb, lastKnownTime, lastRound);
						
		} catch (Exception e) {
			logger.fatal("General error in main thread", e);
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	private static List<RottenStolenFlag> GetRottenStolenFlags(Timestamp fromCreationTime) throws SQLException {
		List<RottenStolenFlag> result = new LinkedList<RottenStolenFlag>();
		
		logger.info(String.format("Getting new score data from time %s with rottenTime %d", fromCreationTime.toString(), Constants.flagExpireInterval));
		stGetStealsOfRottenFlags.setTimestamp(1, fromCreationTime);
		stGetStealsOfRottenFlags.setDouble(2, Constants.flagExpireInterval);
		ResultSet res = stGetStealsOfRottenFlags.executeQuery();
		
		while (res.next()) {
			String flagData = res.getString(1);
			Timestamp time = res.getTimestamp(2);
			int owner = res.getInt(3);
			int service = res.getInt(4);
			int attacker = res.getInt(5);
			
			result.add(new RottenStolenFlag(flagData, time, owner, attacker, service));
		}
		
		return result;
	}
	
	private static Timestamp[] GetRoundTimes() throws SQLException{
		ResultSet res = stGetRoundTimes.executeQuery();		
		ArrayList<Timestamp> al = new ArrayList<Timestamp>();		
		while(res.next()) {
			Timestamp time = res.getTimestamp(1);
			al.add(time);
		}		
		Timestamp[] result = new Timestamp[al.size()];
		al.toArray(result);		
		return result;		
	}
	
	private static int FindLastFinishedRound(Timestamp[] roundTimes){
		if(roundTimes.length <= 1)
			return 0;
		
		Timestamp lastRoundTime = roundTimes[roundTimes.length - 1];
		int round = roundTimes.length - 2;
		for(; round > 0; round--)
			if(roundTimes[round].before(TimestampUtils.AddMillis(lastRoundTime, - Constants.flagExpireInterval*1000)))
				return round - 1;
		
		return 0;
	}
	
	private static void DoJobLoop(Connection conn, Hashtable<TeamService,TeamScore> state, Timestamp lastKnownTime, int lastRound) throws SQLException, InterruptedException {
		Timestamp lastCreationTime = TimestampUtils.AddMillis(lastKnownTime, - Constants.flagExpireInterval*1000);

		int totalTeamsCount = DatabaseManager.getTeams().size();		
		while (true) {
			List<RottenStolenFlag> flags = GetRottenStolenFlags(lastCreationTime);
			Timestamp[] roundTimes = GetRoundTimes();
			
			Hashtable<String, ArrayList<RottenStolenFlag>> grouped = new Hashtable<String, ArrayList<RottenStolenFlag>>();
			for (RottenStolenFlag flag : flags) {
				String flagData = flag.flagData;
				if (!grouped.containsKey(flagData))
					grouped.put(flagData, new ArrayList<RottenStolenFlag>());
				
				ArrayList<RottenStolenFlag> list = grouped.get(flagData);
				list.add(flag);
			}
			
			Set<RottenStolenFlag> flagTimes = new TreeSet<RottenStolenFlag>(new Comparator<RottenStolenFlag>(){
				public int compare(RottenStolenFlag a, RottenStolenFlag b){
	                return a.time.compareTo(b.time);
	            }
			});
			flagTimes.addAll(flags);
			

			for (RottenStolenFlag flag : flagTimes) {
				lastCreationTime = flag.time;
				
				int owner = flag.owner;
				int service = flag.service;
				double ownerTotalScore = state.get(new TeamService(owner, service)).score;				
				
				ArrayList<RottenStolenFlag> list = grouped.get(flag.flagData);
				int attackersCount = list.size();				

				double scoreFromOwner = Math.min(totalTeamsCount, ownerTotalScore);
				double scoreToEachAttacker = scoreFromOwner / attackersCount;
				
				Timestamp rottenTime = TimestampUtils.AddMillis(flag.time, Constants.flagExpireInterval*1000);
				try {
					conn.setAutoCommit(false);

					int round = BinarySearch(roundTimes, rottenTime);
					for(;lastRound < round; lastRound++){//TODO copy-paste
						logger.info(String.format("Populating score INNER round: %d", lastRound));
						Timestamp setTime = TimestampUtils.AddMillis(roundTimes[lastRound + 1], -1);
						
						Hashtable<Integer,Double> taskScores = GetTaskScores(setTime);
						
						for(TeamService ts : state.keySet()){
							if(DatabaseManager.getService(ts.service).getIsNotTask())
								InsertScore(lastRound, setTime, ts.team, ts.service, state.get(ts).score);
							else{
								double score = 0;
								if(taskScores.containsKey(ts.team))
									score = taskScores.get(ts.team);
								InsertScore(lastRound, setTime, ts.team, ts.service, score);
							}
						}						
					}
					
					for (RottenStolenFlag attackerFlag : list) {
						int attacker = attackerFlag.attacker;
						
						double attackerOldScore = state.get(new TeamService(attacker, service)).score;
						double attackerNewScore = attackerOldScore + scoreToEachAttacker;						
						InsertScore(round, rottenTime, attacker, service, attackerNewScore);
						state.put(new TeamService(attacker, service), new TeamScore(attackerNewScore, round, rottenTime));
						logger.info(String.format("Flag %s: (attacker, service) (%d, %d): score %f -> %f (delta = %f)", flag.flagData, attacker, service, attackerOldScore, attackerNewScore, scoreToEachAttacker));
					}
					
					double ownerNewScore = ownerTotalScore - scoreFromOwner;
					
					InsertScore(round, rottenTime, owner, service, ownerNewScore);
					state.put(new TeamService(owner, service), new TeamScore(ownerNewScore, round, rottenTime));
					logger.info(String.format("Flag %s: (owner, service) (%d, %d): score %f -> %f (delta = %f)", flag.flagData, owner, service, ownerTotalScore, ownerNewScore, -scoreFromOwner));
					
					conn.commit();
				}
				catch (SQLException exception)
				{
					try {
						conn.rollback();
						throw exception;
					} catch (SQLException rollbackException) {
						logger.error("Failed to rollback score transaction", rollbackException);
					}
					logger.error("Failed to insert score data in database", exception);
				}
				finally
				{
					try {
						conn.setAutoCommit(true);
					} catch (SQLException e) {
						logger.error("Failed to set autoCommit in database to true", e);
						throw e;
					}
				}				
			}
			
			try {
				conn.setAutoCommit(false);
				
				for(;lastRound < FindLastFinishedRound(roundTimes); lastRound++){ //TODO copy-paste
					logger.info(String.format("Populating score OUTER round: %d", lastRound));
					Timestamp setTime = TimestampUtils.AddMillis(roundTimes[lastRound + 1], -1);
					
					Hashtable<Integer,Double> taskScores = GetTaskScores(setTime);
					
					for(TeamService ts : state.keySet()){
						if(DatabaseManager.getService(ts.service).getIsNotTask())
							InsertScore(lastRound, setTime, ts.team, ts.service, state.get(ts).score);
						else{
							double score = 0;
							if(taskScores.containsKey(ts.team))
								score = taskScores.get(ts.team);
							InsertScore(lastRound, setTime, ts.team, ts.service, score);
						}
					}					
				}
			}
			catch (SQLException exception)
			{
				try {
					conn.rollback();
					throw exception;
				} catch (SQLException rollbackException) {
					logger.error("Failed to rollback score OUTER transaction", rollbackException);
				}
				logger.error("Failed to insert score OUTER data in database", exception);
			}
			finally
			{
				try {
					conn.setAutoCommit(true);
				} catch (SQLException e) {
					logger.error("Failed to set autoCommit OUTER in database to true", e);
					throw e;
				}
			}		
			
			logger.info("Sleeping Score ... ");
			Thread.sleep(10000);
		}
	}
	
	//-1 if array is empty or less than smallest
	private static int BinarySearch(Timestamp[] rounds, Timestamp t){
		int left = 0;
		int right = rounds.length;
		
		while(left < right && rounds[left].compareTo(t) <= 0)
		{
			int mid = left + (right - left) / 2;
			if (rounds[mid].compareTo(t) <= 0)
	            left = mid + 1;
			else
				right = mid;
		}
		return left - 1;
	}
	
	private static void InsertScore(int round, Timestamp time, int team, int service, double score) throws SQLException
	{
		stInsertScore.setInt(1, round);
		stInsertScore.setTimestamp(2, time);
		stInsertScore.setInt(3, team);
		stInsertScore.setInt(4, service);
		stInsertScore.setDouble(5, score);
		stInsertScore.execute();
	}


	private static Timestamp GetLastKnownTime(Hashtable<TeamService, TeamScore> stateFromDb) {
		Timestamp max = null;
		for (TeamScore ts : stateFromDb.values()) {
			if (max == null || max.before(ts.time))
				max = ts.time;
		}
		return max;
	}
	
	private static int GetLastRound(Hashtable<TeamService, TeamScore> stateFromDb) {
		int max = 0;
		for (TeamScore ts : stateFromDb.values()) {
			if (max < ts.round)
				max = ts.round;
		}
		return max;
	}	
	
	private static Hashtable<Integer,Double> GetTaskScores(Timestamp fromTimestamp) throws SQLException {
		stSelectTaskScore.setTimestamp(1, fromTimestamp);
		ResultSet res = stSelectTaskScore.executeQuery();
		
		Hashtable<Integer,Double> result = new Hashtable<Integer,Double>(); 
		
		while (res.next()) {
			int team = res.getInt(1);
			double score = res.getDouble(2);
			result.put(team, score);
		}
		return result;
	}

	private static void CreateInitStateInDb() throws SQLException {
		stCreateInitState.execute();
	}

	private static void PrepareStatements(Connection conn) throws SQLException{
		stGetRoundTimes = conn.prepareStatement(sqlGetRoundTimes);
		stGetLastScores = conn.prepareStatement(sqlGetLastScores);
		stCreateInitState = conn.prepareStatement(sqlCreateInitState);
		stGetStealsOfRottenFlags = conn.prepareStatement(sqlGetStealsOfRottenFlags);
		stInsertScore = conn.prepareStatement(sqlInsertScore);
		stSelectTaskScore = conn.prepareStatement(sqlSelectTaskScore);
	}
	
	private static Hashtable<TeamService, TeamScore> GetStateFromDb() throws SQLException{
		ResultSet res = stGetLastScores.executeQuery();
		
		Hashtable<TeamService, TeamScore> result = new Hashtable<TeamService, TeamScore>(); 
		
		while(res.next()){
			int round = res.getInt(1);
			int team = res.getInt(2);
			int service = res.getInt(3);
			double score = res.getDouble(4);			
			Timestamp time = res.getTimestamp(5);
			
			result.put(new TeamService(team, service), new TeamScore(score, round, time));
		}
		return result;		
	}
}
