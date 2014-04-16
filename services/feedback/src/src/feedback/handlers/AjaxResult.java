package feedback.handlers;

import org.codehaus.jackson.annotate.JsonProperty;

public class AjaxResult {
	private AjaxResult(String data, String error) {
		this.data = data;
		this.error = error;
	}

	public static AjaxResult createError(String error) {
		return new AjaxResult(null, error);
	}

	public static AjaxResult createSuccess(String data) {
		return new AjaxResult(data, null);
	}

	@JsonProperty("data")
	public String data;

	@JsonProperty("error")
	public String error;
}