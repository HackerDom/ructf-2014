package feedback.index.data;

import org.codehaus.jackson.JsonParser;
import org.codehaus.jackson.map.DeserializationContext;
import org.codehaus.jackson.map.JsonDeserializer;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormatter;
import org.joda.time.format.ISODateTimeFormat;

import java.io.IOException;

public class SortableDateTimeDeserializer extends JsonDeserializer<DateTime> {
	public SortableDateTimeDeserializer() {
		formatter = ISODateTimeFormat.dateHourMinuteSecond().withZoneUTC();
	}

	@Override
	public DateTime deserialize(JsonParser parser, DeserializationContext dc) throws IOException {
		return deserialize(parser.getText());
	}

	public DateTime deserialize(String value) {
		return DateTime.parse(value, formatter);
	}

	private DateTimeFormatter formatter;
}