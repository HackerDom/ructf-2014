package ructf.scoresCache;

import java.sql.Timestamp;

public class RottenStolenFlag {
	public String flagData;
	public Timestamp time;
	public int owner;
	public int attacker;
	public int service;
	
	public RottenStolenFlag(String flagData, Timestamp time, int owner,	int attacker, int service) {
		this.flagData = flagData;
		this.time = time;
		this.owner = owner;
		this.attacker = attacker;
		this.service = service;
	}
}
