package feedback;

import feedback.auth.TokenCrypt;
import feedback.auth.UserIndex;
import feedback.handlers.*;
import feedback.http.HttpListener;
import feedback.http.IHttpListenerHandler;
import feedback.index.LuceneIndex;
import feedback.index.data.Vote;
import feedback.index.data.VoteType;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class Feedback {
	public static void main(String[] args) {
		try {
			if(args.length == 1 && (args[0].equals("-?") || args[0].equals("--help"))) {
				System.out.println("feedback.jar <port>");
				return;
			}

			int port = args.length >= 0 ? Integer.parseInt(args[0]) : DEFAULT_PORT;

			new File("data").mkdir();
			UserIndex userIndex = new UserIndex("data/users");
			TokenCrypt tokenCrypt = new TokenCrypt("data/key");
			index = new LuceneIndex("data/index");

			addHello();

			Runtime.getRuntime().addShutdownHook(new Thread(() -> {
				log.warn("Shutting down");
				try {
					index.commit();
				} catch(IOException e) {
					e.printStackTrace();
				}
			}));

			BlockingQueue<Runnable> queue = new LinkedBlockingQueue<>();
			ThreadPoolExecutor executor = new ThreadPoolExecutor(LISTENER_THREADS_COUNT, LISTENER_THREADS_COUNT, Long.MAX_VALUE, TimeUnit.NANOSECONDS, queue);
			executor.allowCoreThreadTimeOut(false);
			executor.prestartAllCoreThreads();

			Map<String, IHttpListenerHandler> handlers = new HashMap<>();
			handlers.put("/", new StaticHandler("static"));
			handlers.put("/search", new SearchHandler(index, tokenCrypt));
			handlers.put("/put", new PutHandler(index, tokenCrypt));
			handlers.put("/auth", new AuthHandler(userIndex, tokenCrypt));
			handlers.put("/register", new RegistrationHandler(userIndex, tokenCrypt));

			HttpListener listener = new HttpListener(port, handlers, executor);
			listener.start();
		} catch(Exception e) {
			log.error("Fatal", e);
		}
	}

	private static void addHello() throws IOException {
		Vote vote1 = new Vote();
		vote1.id = "00000000000000000000000000000000";
		vote1.type = VoteType.VISIBLE;
		vote1.login = "admin";
		vote1.title = "Hello!";
		vote1.text = "We welcome your suggestions and comments!";
		vote1.date = DateTime.now();

		Vote vote2 = new Vote();
		vote2.id = "ffffffffffffffffffffffffffffffff";
		vote2.type = VoteType.INVISIBLE;
		vote2.login = "admin";
		vote2.title = "Hello!";
		vote2.text = "Please check user messages!";
		vote2.date = DateTime.now();

		index.addOrUpdateSubject(vote1);
		index.addOrUpdateSubject(vote2);
		index.commit();
	}

	private static final int DEFAULT_PORT = 7654;
	private static final int LISTENER_THREADS_COUNT = 4;
	private static final Logger log = LoggerFactory.getLogger(Feedback.class);
	private static LuceneIndex index;
}