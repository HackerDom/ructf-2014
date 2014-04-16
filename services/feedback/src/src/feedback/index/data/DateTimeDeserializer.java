package feedback.index.data;

import org.codehaus.jackson.JsonParser;
import org.codehaus.jackson.map.DeserializationContext;
import org.codehaus.jackson.map.JsonDeserializer;
import org.joda.time.DateTime;

import java.io.IOException;

public class DateTimeDeserializer extends JsonDeserializer<DateTime> {
	@Override
	public DateTime deserialize(JsonParser parser, DeserializationContext dc) throws IOException {
		return deserialize(parser.getLongValue());
	}

	public DateTime deserialize(long value) {
		return new DateTime(value);
	}
}