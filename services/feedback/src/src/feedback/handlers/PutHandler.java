package feedback.handlers;

import feedback.auth.AuthToken;
import feedback.auth.TokenCrypt;
import feedback.auth.TokenHelper;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.index.LuceneIndex;
import feedback.index.data.Vote;
import feedback.index.data.VoteType;
import feedback.utils.StringUtils;
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

		if(StringUtils.isBlank(vote.title) || StringUtils.isBlank(vote.text))
			return AjaxResult.createError("Not all required fields are set");

		if(vote.title.length() > MAX_TITLE_LENGTH || vote.text.length() > MAX_TEXT_LENGTH)
			return AjaxResult.createError("Title/text too long");

		if(!(vote.type.equals(VoteType.VISIBLE) || vote.type.equals(VoteType.INVISIBLE)))
			return AjaxResult.createError("Invalid type");

		AuthToken authToken = TokenHelper.getToken(request, tokenCrypt);
		index.addOrUpdateSubject(preprocess(vote, authToken));

		return AjaxResult.createSuccess(vote.id);
	}

	private Vote preprocess(Vote vote, AuthToken authToken) {
		vote.id = UUID.randomUUID().toString().replaceAll("-", "");
		vote.date = DateTime.now();
		vote.login = authToken != null ? authToken.getLogin() : "anonymous";
		return vote;
	}

	private static final int MAX_TITLE_LENGTH = 128;
	private static final int MAX_TEXT_LENGTH = 512;

	private LuceneIndex index;
	private TokenCrypt tokenCrypt;
}