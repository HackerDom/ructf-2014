package feedback.utils;

public final class StringUtils {
	public static boolean isBlank(String value) {
		return value == null || value.length() == 0;
	}

	public static boolean isNotBlank(String value) {
		return !isBlank(value);
	}

	public static String shorten(String val, int min, int max) {
		if(StringUtils.isBlank(val) || val.length() <= Math.max(min, max))
			return val;
		int len = 0;
		StringBuilder sb = new StringBuilder();
		for(String word : val.split(" "))
		{
			if((len += word.length() + 1) > min)
			{
				if(sb.length() == 0)
					sb.append(word.substring(0, min));
				break;
			}
			sb.append(' ');
			sb.append(word);
		}
		return sb.toString().trim() + "...";
	}
}
