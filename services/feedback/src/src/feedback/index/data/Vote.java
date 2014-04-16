package feedback.index.data;

import org.codehaus.jackson.annotate.JsonProperty;
import org.codehaus.jackson.map.annotate.JsonDeserialize;
import org.codehaus.jackson.map.annotate.JsonSerialize;
import org.joda.time.DateTime;

public class Vote {
	@JsonProperty("id")
	public String id;

	@JsonProperty("type")
	public String type;

	@JsonProperty("login")
	public String login;

	@JsonProperty("title")
	public String title;

	@JsonProperty("text")
	public String text;

	@JsonProperty("date")
	@JsonSerialize(using=DateTimeSerializer.class)
	@JsonDeserialize(using=DateTimeDeserializer.class)
	public DateTime date;
}