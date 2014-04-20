package feedback.http;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.Map;
import java.util.concurrent.ExecutorService;

public class HttpListener implements HttpHandler {
	public HttpListener(int port, Map<String, IHttpListenerHandler> handlers, ExecutorService executor) throws IOException {
		this.port = port;
		this.handlers = handlers;
		final InetSocketAddress address = new InetSocketAddress((InetAddress)null, port);
		server = HttpServer.create(address, SOCKET_BACKLOG);
		handlers.forEach((prefix, handler) -> server.createContext(prefix, this));
		server.setExecutor(executor);
	}

	public void start() throws IOException {
		server.start();
		log.info("Started listening on http://*:{}/", port);
	}

	@Override
	public void handle(final HttpExchange httpExchange) {
		long start = System.currentTimeMillis();
		try {
			HttpListenerRequest request = HttpListenerRequest.create(httpExchange);
			HttpListenerResponse response = HttpListenerResponse.create(httpExchange);

			IHttpListenerHandler handler = handlers.getOrDefault(httpExchange.getHttpContext().getPath(), null);
			if(handler == null) {
				sendError(httpExchange, 404, "Handler not found");
				return;
			}

			response.setHeader("Connection", "Keep-Alive");
			handler.handle(request, response);
			sendError(httpExchange, 200, "");

			//log.info("Handle: '{}', bytes sent {}, elapsed {} ms", new Object[]{httpExchange.getRequestURI(), response.getBytesWritten(), System.currentTimeMillis() - start});
		} catch(Exception e) {
			log.error("Handle failed: '{}', elapsed {} ms", new Object[] {httpExchange.getRequestURI(), System.currentTimeMillis() - start, e});

			int statusCode = 500;
			if(e instanceof HttpException)
				statusCode = ((HttpException)e).getStatusCode();

			sendError(httpExchange, statusCode, e.toString());
		} finally {
			httpExchange.close();
		}
	}

	private static void sendError(HttpExchange httpExchange, int code, String message) {
		try {
			if(httpExchange.getResponseCode() > 0)
				return;
			byte[] bytes = message.getBytes();
			httpExchange.sendResponseHeaders(code, bytes.length);
			httpExchange.getResponseHeaders().set("Content-Type", "text/plain; charset=utf-8");
			try(OutputStream stream = httpExchange.getResponseBody()) {
				stream.write(bytes);
			}
		} catch(Exception ignored) {
		}
	}

	private static final int SOCKET_BACKLOG = 64;
	private static final Logger log = LoggerFactory.getLogger(HttpListener.class);
	private final Map<String, IHttpListenerHandler> handlers;
	private final HttpServer server;
	private final int port;
}