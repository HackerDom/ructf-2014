package feedback;

import feedback.http.HttpListener;
import feedback.http.IHttpListenerHandler;
import feedback.index.LuceneIndex;
import feedback.index.data.Vote;
import feedback.index.data.VoteType;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class Feedback {
	public static void main(String[] args) {
		try {
			if(args.length != 1 || args[0].equals("-?") || args[0].equals("--help")) {
				System.out.println("feedback.jar <port>");
				return;
			}

			int port = Integer.parseInt(args[0]);

			index = new LuceneIndex("index");

			Vote vote = new Vote();
			vote.id = UUID.randomUUID().toString();
			vote.type = VoteType.VISIBLE;
			vote.login = "admin";
			vote.title = "Hello!";
			vote.text = "We welcome your suggestions and comments!";
			vote.date = DateTime.now();

			index.addOrUpdateSubject(vote);
			index.commit();

			Runtime.getRuntime().addShutdownHook(new Thread() {
				@Override
				public void run()
				{
					System.out.println("shutting down");
					try {
						index.commit();
					} catch(IOException e) {
						e.printStackTrace();
					}
				}
			});

			BlockingQueue<Runnable> queue = new LinkedBlockingQueue<>();
			ThreadPoolExecutor executor = new ThreadPoolExecutor(LISTENER_THREADS_COUNT, LISTENER_THREADS_COUNT, Long.MAX_VALUE, TimeUnit.NANOSECONDS, queue);
			executor.allowCoreThreadTimeOut(false);
			executor.prestartAllCoreThreads();

			Map<String, IHttpListenerHandler> handlers = new HashMap<>();
			handlers.put("/", new StaticHandler("static"));
			handlers.put("/search", new SearchHandler(index));
			handlers.put("/put", new PutHandler(index));

			HttpListener listener = new HttpListener(port, handlers, executor);
			listener.start();
		} catch(Exception e) {
			log.error("Fatal", e);
		}
	}

	private static final int LISTENER_THREADS_COUNT = 4;
	private static final Logger log = LoggerFactory.getLogger(Feedback.class);
	private static LuceneIndex index;
}