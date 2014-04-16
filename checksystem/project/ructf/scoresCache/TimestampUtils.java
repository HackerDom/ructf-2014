package ructf.scoresCache;

import java.sql.Timestamp;

public class TimestampUtils {
	public static Timestamp AddMillis(Timestamp t, long milliseconds){
		Timestamp to = new Timestamp(t.getTime() + milliseconds);
		to.setNanos(t.getNanos());
		return to;
	}
}
