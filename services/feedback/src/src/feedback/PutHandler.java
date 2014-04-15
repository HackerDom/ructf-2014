package feedback;

import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.http.IHttpListenerHandler;
import feedback.index.LuceneIndex;
import feedback.index.data.Vote;
import feedback.index.data.VoteType;
import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.map.ObjectReader;
import org.joda.time.DateTime;

import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

public class PutHandler implements IHttpListenerHandler {
	public PutHandler(LuceneIndex index) {
		this.index = index;
		this.reader = new ObjectMapper().reader(Vote.class);
	}

	@Override
	public void handle(HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		Vote vote = readVote(request);
		if(vote != null) {
			preprocess(vote);
			index.addOrUpdateSubject(vote);
		}
	}

	private Vote readVote(HttpListenerRequest request) throws IOException {
		try(InputStream stream = request.getRequestStream()) {
			return reader.readValue(stream);
		}
	}

	private void preprocess(Vote vote) {
		vote.id = UUID.randomUUID().toString();
		vote.date = DateTime.now();
		vote.type = VoteType.VISIBLE;
	}

	private LuceneIndex index;
	private ObjectReader reader;
}
