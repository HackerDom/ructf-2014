package feedback.handlers;

import feedback.auth.AuthToken;
import feedback.auth.TokenCrypt;
import feedback.auth.TokenHelper;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.index.LuceneIndex;
import feedback.index.data.Vote;
import feedback.index.data.VoteType;
import org.joda.time.DateTime;

import java.io.IOException;
import java.util.UUID;

public class PutHandler extends AjaxHandler<Vote> {
	public PutHandler(LuceneIndex index, TokenCrypt tokenCrypt) {
		super(Vote.class);
		this.index = index;
		this.tokenCrypt = tokenCrypt;
	}

	@Override
	protected AjaxResult handle(Vote vote, HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		if(vote == null)
			return AjaxResult.createError("Vote is empty");

		AuthToken authToken = TokenHelper.getToken(request, tokenCrypt);
		index.addOrUpdateSubject(preprocess(vote, authToken));

		return AjaxResult.createSuccess(vote.id);
	}

	private Vote preprocess(Vote vote, AuthToken authToken) {
		vote.id = UUID.randomUUID().toString().replaceAll("-", "");
		vote.date = DateTime.now();
		vote.type = VoteType.VISIBLE;
		vote.login = authToken != null ? authToken.getLogin() : "anonymous";
		return vote;
	}

	private LuceneIndex index;
	private TokenCrypt tokenCrypt;
}