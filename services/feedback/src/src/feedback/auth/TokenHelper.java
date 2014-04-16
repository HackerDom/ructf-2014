package feedback.auth;

import com.sun.net.httpserver.Headers;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import org.joda.time.DateTime;
import org.joda.time.DateTimeZone;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;

import java.io.IOException;
import java.util.List;
import java.util.Locale;

public class TokenHelper {
	public static AuthToken getToken(HttpListenerRequest request, TokenCrypt tokenCrypt) {
		Headers headers = request.getRequestHeaders();
		if(headers != null) {
			List<String> cookies = headers.get("Cookie");
			if(cookies == null)
				return null;
			String cookie = cookies.stream().filter(c -> c.startsWith("token=")).findFirst().orElse(null);
			if(cookie != null) {
				try {
					String cookieValue = cookie.substring(6);
					return AuthToken.deserialize(tokenCrypt.decrypt(cookieValue));
				} catch(IOException e) {
					e.printStackTrace();
				}
			}
		}
		return null;
	}

	public static void setToken(HttpListenerResponse response, TokenCrypt tokenCrypt, AuthToken authToken) {
		if(authToken != null) {
			try {
				String value = tokenCrypt.encrypt(authToken.serialize());
				String tokenCookieValue = String.format("token=%s; expires=%s; HttpOnly", value, RFC1123_DATE_FORMAT.print(DateTime.now().plusWeeks(1)));
				response.setHeader("Set-Cookie", tokenCookieValue);
			} catch(IOException e) {
				e.printStackTrace();
			}
		}
	}

	private static final DateTimeFormatter RFC1123_DATE_FORMAT = DateTimeFormat
			.forPattern("EEE, dd MMM yyyy HH:mm:ss 'GMT'")
			.withLocale(Locale.US)
			.withZone(DateTimeZone.UTC);
}