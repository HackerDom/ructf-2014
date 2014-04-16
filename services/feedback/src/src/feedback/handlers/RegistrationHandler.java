package feedback.handlers;

import feedback.auth.*;
import feedback.http.HttpListenerRequest;
import feedback.http.HttpListenerResponse;
import feedback.utils.StringUtils;
import org.joda.time.DateTime;

import java.io.IOException;

public class RegistrationHandler extends AjaxHandler<User> {
	public RegistrationHandler(UserIndex userIndex, TokenCrypt tokenCrypt) {
		super(User.class);
		this.userIndex = userIndex;
		this.tokenCrypt = tokenCrypt;
	}

	@Override
	protected AjaxResult handle(User user, HttpListenerRequest request, HttpListenerResponse response) throws IOException {
		if(user == null || StringUtils.isBlank(user.login) || StringUtils.isBlank(user.password))
			return AjaxResult.createError("Invalid login/password");

		user.isAdmin = false;

		if(!userIndex.add(user))
			return AjaxResult.createError("Login already exists");

		AuthToken authToken = new AuthToken(user.login, user.isAdmin, DateTime.now());
		TokenHelper.setToken(response, tokenCrypt, authToken);

		return AjaxResult.createSuccess("OK");
	}

	private UserIndex userIndex;
	private TokenCrypt tokenCrypt;
}
