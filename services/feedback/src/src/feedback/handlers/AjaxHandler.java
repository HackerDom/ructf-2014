package feedback.handlers;

import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.http.IHttpListenerHandler;
import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.map.ObjectReader;
import org.codehaus.jackson.map.ObjectWriter;
import org.codehaus.jackson.map.annotate.JsonSerialize;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.io.InputStream;

public abstract class AjaxHandler<In> implements IHttpListenerHandler {
	public AjaxHandler(Class clazz) {
		reader = new ObjectMapper().reader(clazz);
		ObjectMapper mapper = new ObjectMapper();
		mapper.setSerializationInclusion(JsonSerialize.Inclusion.NON_NULL);
		writer = mapper.writer();
	}

	@Override
	public void handle(HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		AjaxResult result;
		try {
			result = handle(readData(request), request, response);
		} catch(Exception e) {
			log.error("Failed to handle request", e);
			result = AjaxResult.createError("Unknown server error");
		}
		writeData(response, result);
	}

	protected abstract AjaxResult handle(In input, HttpListenerRequest request, HttpListenerResponse response) throws IOException;

	private In readData(HttpListenerRequest request) throws IOException {
		try(InputStream stream = request.getRequestStream()) {
			return reader.readValue(stream);
		}
	}

	public void writeData(HttpListenerResponse response, AjaxResult data) throws IOException {
		byte[] buffer = writer.writeValueAsBytes(data);
		response.setContentLength(buffer.length);
		response.setHeader("Content-Type", "application/json");
		response.getOutputStream().write(buffer);
	}

	private static final Logger log = LoggerFactory.getLogger(SearchHandler.class);
	private final ObjectReader reader;
	private final ObjectWriter writer;
}