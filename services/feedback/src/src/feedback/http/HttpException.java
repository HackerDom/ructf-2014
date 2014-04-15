package feedback.http;

import java.io.IOException;

public class HttpException extends IOException {
	public HttpException(int statusCode, String message) {
		super(message);
		this.statusCode = statusCode;
	}

	public HttpException(int statusCode, String message, Throwable throwable) {
		super(message, throwable);
		this.statusCode = statusCode;
	}

	public int getStatusCode() {
		return statusCode;
	}

	private int statusCode;
}