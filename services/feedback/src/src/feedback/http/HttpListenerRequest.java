package feedback.http;

import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;

import java.io.InputStream;
import java.net.URI;
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
		return parseQueryString(uri.getQuery());
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
			map.put(pair[0], pair.length > 1 ? pair[1] : null);
		}
		return map;
	}

	private final String method;
	private final URI uri;
	private final Headers headers;
	private final InputStream requestStream;
	private Map<String, String> queryParams;
}