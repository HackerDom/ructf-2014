package feedback;

import feedback.http.HttpException;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.http.IHttpListenerHandler;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

import static org.apache.commons.io.FileUtils.iterateFiles;

public class StaticHandler implements IHttpListenerHandler {
	public StaticHandler(String dir) throws IOException {
		Path currentPath = Paths.get(new File(".").getAbsolutePath()).normalize().resolve(dir);
		iterateFiles(currentPath.toFile(), extensions, true).forEachRemaining(file -> {
			byte[] content = tryReadFile(file);
			if(content != null) {
				String path = FilenameUtils.separatorsToUnix(file.getPath().toLowerCase().substring(currentPath.toString().length()));
				files.put(path, content);
			}
		});
		byte[] content = files.getOrDefault(defaultDocument, null);
		if(content != null) files.put(root, content);
	}

	@Override
	public void handle(HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		String path = request.getRequestURI().getPath();
		byte[] content = files.getOrDefault(path, null);
		String mimeType = mimeTypes.getOrDefault(FilenameUtils.getExtension(path), null);
		if(content == null || mimeType == null) {
			throw new HttpException(404, "File not found");
		}
		response.setHeader("Content-Type", mimeType);
		response.setHeader("Cache-Control", "max-age=300");
		response.setContentLength(content.length);
		try(OutputStream stream = response.getOutputStream()) {
			stream.write(content);
		}
	}

	private static byte[] tryReadFile(File file) {
		try {
			return FileUtils.readFileToByteArray(file);
		} catch(IOException e) {
			log.error(String.format("Failed to read static file '%s'", file.getPath()), e);
			return null;
		}
	}

	private static final String root = "/";
	private static final String defaultDocument = "/default.html";

	private static final Map<String, String> mimeTypes = new HashMap<String, String>() {{
		put("", "text/html");
		put("htm", "text/html");
		put("html", "text/html");
		put("css", "text/css");
		put("js", "application/x-javascript");
		put("jpg", "image/jpeg");
		put("png", "image/png");
		put("gif", "image/gif");
	}};

	private static final String[] extensions = mimeTypes.keySet().toArray(new String[mimeTypes.size()]);

	private static final Logger log = LoggerFactory.getLogger(StaticHandler.class);
	private final Map<String, byte[]> files = new HashMap<>();
}