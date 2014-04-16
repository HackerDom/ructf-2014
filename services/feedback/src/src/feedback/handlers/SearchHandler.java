package feedback.handlers;

import feedback.auth.AuthToken;
import feedback.auth.TokenCrypt;
import feedback.auth.TokenHelper;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.http.IHttpListenerHandler;
import feedback.index.LuceneIndex;
import feedback.index.data.SearchResults;
import org.apache.lucene.queryparser.classic.ParseException;
import org.apache.lucene.search.highlight.InvalidTokenOffsetsException;
import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.map.ObjectWriter;
import org.codehaus.jackson.map.annotate.JsonSerialize;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;

public class SearchHandler implements IHttpListenerHandler {
	public SearchHandler(LuceneIndex index, TokenCrypt tokenCrypt) {
		this.index = index;
		this.tokenCrypt = tokenCrypt;
		ObjectMapper mapper = new ObjectMapper();
		mapper.setSerializationInclusion(JsonSerialize.Inclusion.NON_NULL);
		this.writer = mapper.writer();
	}

	@Override
	public void handle(HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		String query = request.getQueryParams().get("query");

		int top;
		String topValue = request.getQueryParams().get("top");
		try {
			top = topValue == null ? MAX_ITEMS : Math.min(MAX_ITEMS, Math.max(0, Integer.parseInt(topValue)));
		} catch(NumberFormatException e) {
			top = MAX_ITEMS;
		}

		AuthToken authToken = TokenHelper.getToken(request, tokenCrypt);

		try {
			SearchResults results = index.search(query, top, authToken != null && authToken.isAdmin());
			byte[] buffer = writer.writeValueAsBytes(results);
			response.setContentLength(buffer.length);
			response.setHeader("Content-Type", "application/json");
			response.getOutputStream().write(buffer);
		} catch(ParseException|InvalidTokenOffsetsException e) {
			log.error(String.format("Query failed '%s'", query), e);
		}
	}

	private static final int MAX_ITEMS = 100;
	private static final Logger log = LoggerFactory.getLogger(SearchHandler.class);
	private final ObjectWriter writer;
	private final LuceneIndex index;
	private TokenCrypt tokenCrypt;
}