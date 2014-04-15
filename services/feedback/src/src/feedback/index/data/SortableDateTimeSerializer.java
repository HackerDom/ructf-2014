package feedback.index.data;

import org.codehaus.jackson.JsonGenerator;
import org.codehaus.jackson.map.JsonSerializer;
import org.codehaus.jackson.map.SerializerProvider;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormatter;
import org.joda.time.format.ISODateTimeFormat;

import java.io.IOException;

public class SortableDateTimeSerializer extends JsonSerializer<DateTime> {
	public SortableDateTimeSerializer() {
		formatter = ISODateTimeFormat.dateHourMinuteSecond().withZoneUTC();
	}

	@Override
	public void serialize(DateTime dateTime, JsonGenerator jsonGenerator, SerializerProvider serializerProvider) throws IOException {
		jsonGenerator.writeString(serialize(dateTime));
	}

	public String serialize(DateTime dateTime) {
		return formatter.print(dateTime);
	}

	private DateTimeFormatter formatter;
}