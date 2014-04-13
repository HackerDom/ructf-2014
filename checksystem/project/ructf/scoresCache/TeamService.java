package ructf.scoresCache;

public class TeamService {
	int team;
	int service;
	
	public TeamService(int team, int service) {
		this.team = team;
		this.service = service;
	}
	
	@Override
    public boolean equals(Object obj) {
        if(obj != null && obj instanceof TeamService) {
        	TeamService s = (TeamService)obj;
            return team == s.team && service == s.service;
        }
        return false;
    }

    @Override
    public int hashCode() {
        return service * 31337 + team;
    }		
}