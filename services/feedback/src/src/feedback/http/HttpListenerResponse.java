package feedback.http;

import com.sun.net.httpserver.HttpExchange;
import org.apache.commons.io.output.NullOutputStream;
import org.apache.commons.io.output.ThresholdingOutputStream;

import java.io.IOException;
import java.io.OutputStream;

public class HttpListenerResponse {
	private HttpListenerResponse(HttpExchange exchange) {
		this.exchange = exchange;
	}

	public static HttpListenerResponse create(final HttpExchange exchange) {
		return new HttpListenerResponse(exchange);
	}

	public OutputStream getOutputStream() {
		if(outputStream == null) {
			outputStream = new ThresholdingOutputStream(1) {
				@Override
				protected OutputStream getStream() throws IOException {
					return underlyingStream;
				}

				@Override
				protected void thresholdReached() throws IOException {
					long contentLengthValue = contentLength;
					exchange.sendResponseHeaders(statusCode, contentLengthValue > 0L ? contentLengthValue : 0L);
					underlyingStream = exchange.getResponseBody();
				}

				private OutputStream underlyingStream = new NullOutputStream();
			};
		}
		return outputStream;
	}

	public void setStatusCode(int statusCode) {
		this.statusCode = statusCode;
	}

	public void setContentLength(long contentLength) {
		this.contentLength = contentLength;
	}

	public void setHeader(String name, String value) {
		this.exchange.getResponseHeaders().set(name, value);
	}

	public long getBytesWritten() {
		return outputStream == null ? 0 : outputStream.getByteCount();
	}

	private final HttpExchange exchange;
	private ThresholdingOutputStream outputStream;
	private Integer statusCode = 200;
	private Long contentLength = 0L;
}