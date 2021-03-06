package feedback.http;

import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;

import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URLDecoder;
import java.util.HashMap;
import java.util.Map;

public class HttpListenerRequest {
	private HttpListenerRequest(String method, URI uri, Headers headers, InputStream requestStream) {
		this.method = method;
		this.uri = uri;
		this.headers = headers;
		this.requestStream = requestStream;
	}

	public static HttpListenerRequest create(HttpExchange exchange) {
		return new HttpListenerRequest(exchange.getRequestMethod(), exchange.getRequestURI(), exchange.getRequestHeaders(), exchange.getRequestBody());
	}

	public String getRequestMethod() {
		return method;
	}

	public URI getRequestURI() {
		return uri;
	}

	public Map<String, String> getQueryParams() {
		return queryParams != null ? queryParams : (queryParams = getQueryParams(uri));
	}

	public Headers getRequestHeaders() {
		return headers;
	}

	public InputStream getRequestStream() {
		return requestStream;
	}

	private static Map<String, String> getQueryParams(URI uri) {
		return parseQueryString(uri.getRawQuery()); //.getQuery() has some problems with URL-decode
	}

	private static Map<String, String> parseQueryString(String query) {
		Map<String, String> map = new HashMap<>();
		if(query == null)
			return map;
		String[] parts = query.split("&");
		if(parts.length == 0)
			return map;
		for(String part : parts) {
			String[] pair = part.split("=");
			try {
				if(pair.length == 0)
					continue;
				String key = URLDecoder.decode(pair[0], UTF8);
				String value = pair.length > 1 ? URLDecoder.decode(pair[1], UTF8) : null;
				map.put(key, value);
			} catch(UnsupportedEncodingException e) {
				e.printStackTrace();
			}
		}
		return map;
	}

	private static final String UTF8 = "utf-8";

	private final String method;
	private final URI uri;
	private final Headers headers;
	private final InputStream requestStream;
	private Map<String, String> queryParams;
}