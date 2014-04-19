package feedback.auth;

import com.sun.net.httpserver.Headers;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import org.apache.lucene.search.highlight.SimpleHTMLEncoder;
import org.joda.time.DateTime;
import org.joda.time.DateTimeZone;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;

import java.io.IOException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Locale;

public class TokenHelper {
	public static AuthToken getToken(HttpListenerRequest request, TokenCrypt tokenCrypt) {
		Headers headers = request.getRequestHeaders();
		if(headers != null) {
			List<String> cookies = headers.get("Cookie");
			if(cookies == null)
				return null;
			List<String> values = new ArrayList<>();
			for(String cookie : cookies) {
				Collections.addAll(values, cookie.split("\\s*;\\s*"));
			}
			String cookie = values.stream().filter(v -> v.startsWith("token=")).findFirst().orElse(null);
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
				String expires = RFC1123_DATE_FORMAT.print(DateTime.now().plusWeeks(1));
				String tokenCookieValue = String.format("token=%s; expires=%s; HttpOnly", value, expires);
				String loginCookieValue = String.format("login=%s; expires=%s;", URLEncoder.encode(SimpleHTMLEncoder.htmlEncode(authToken.getLogin()), UTF8).replaceAll("\\+", "%20"), expires);
				List<String> values = new ArrayList<>(2);
				values.add(tokenCookieValue);
				values.add(loginCookieValue);
				response.putHeaders(SET_COOKIE_HEADER, values);
			} catch(IOException e) {
				e.printStackTrace();
			}
		}
	}

	private static final String UTF8 = "UTF8";
	private static final String SET_COOKIE_HEADER = "Set-Cookie";

	private static final DateTimeFormatter RFC1123_DATE_FORMAT = DateTimeFormat
			.forPattern("EEE, dd MMM yyyy HH:mm:ss 'GMT'")
			.withLocale(Locale.US)
			.withZone(DateTimeZone.UTC);
}