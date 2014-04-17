package ructf.scoresCache;

import java.sql.Timestamp;

public class TeamScore {
	public double score;
	public int round;
	public Timestamp time;
	
	public TeamScore(double score, int round, Timestamp time) {		
		this.score = score;
		this.round = round;
		this.time = time;
	}
}