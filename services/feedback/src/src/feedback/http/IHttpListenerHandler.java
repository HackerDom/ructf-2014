package feedback.http;

import java.io.IOException;

public interface IHttpListenerHandler {
	void handle(HttpListenerRequest request, HttpListenerResponse response) throws IOException;
}