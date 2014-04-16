package feedback.auth;

import org.codehaus.jackson.annotate.JsonProperty;

public class User {
	@JsonProperty("login")
	public String login;

	@JsonProperty("password")
	public String password;

	@JsonProperty("isAdmin")
	public boolean isAdmin;
}