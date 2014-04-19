package feedback.handlers;

import feedback.auth.*;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.utils.StringUtils;
import org.joda.time.DateTime;

import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;

public class AuthHandler extends AjaxHandler<User> {
	public AuthHandler(UserIndex userIndex, TokenCrypt tokenCrypt) throws InvalidKeySpecException, NoSuchAlgorithmException, InvalidKeyException, IOException {
		super(User.class);
		this.userIndex = userIndex;
		this.tokenCrypt = tokenCrypt;
	}

	@Override
	protected AjaxResult handle(User user, HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		if(user == null || StringUtils.isBlank(user.login) || StringUtils.isBlank(user.password) || (user = userIndex.auth(user.login, user.password)) == null)
			return AjaxResult.createError("Invalid login/password");

		AuthToken authToken = new AuthToken(user.login, user.isAdmin, DateTime.now());
		TokenHelper.setToken(response, tokenCrypt, authToken);

		return AjaxResult.createSuccess("OK");
	}

	private final TokenCrypt tokenCrypt;
	private UserIndex userIndex;
}