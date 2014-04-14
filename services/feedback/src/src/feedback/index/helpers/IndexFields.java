package feedback.index.helpers;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

public class IndexFields {
	public IndexFields() {
		fields = new HashSet<>();
		fields.add(id);
		fields.add(login);
		fields.add(title);
		fields.add(text);

		fieldsArray = fields.toArray(new String[fields.size()]);

		boosts = new HashMap<>();
		for(String field : fields)
			boosts.put(field, defaultBoost);
	}

	public static final String id = "id";
	public static final String type = "type";
	public static final String login = "login";
	public static final String title = "title";
	public static final String text = "text";
	public static final String date = "date";

	public final Set<String> fields;
	public final String[] fieldsArray;
	public final HashMap<String, Float> boosts;

	private static final float defaultBoost = 1.0f;
}