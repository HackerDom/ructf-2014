package feedback.utils;

public final class StringUtils {
	public static boolean isBlank(String value) {
		return value == null || value.length() == 0;
	}

	public static boolean isNotBlank(String value) {
		return !isBlank(value);
	}
}
