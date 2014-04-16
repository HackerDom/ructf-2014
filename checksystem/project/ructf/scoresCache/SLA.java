package ructf.scoresCache;

import java.sql.Timestamp;

public class SLA {
	public int round;
	public int team;
	public int service;
	public int succeeded;
	public int failed;
	public Timestamp time;
	
	public SLA(int round, int team, int service, int successed, int failed, Timestamp time) {
		this.round = round;
		this.team = team;
		this.service = service;
		this.succeeded = successed;
		this.failed = failed;
		this.time = time;
	}
}


