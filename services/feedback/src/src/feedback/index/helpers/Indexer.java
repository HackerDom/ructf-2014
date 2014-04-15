package feedback.index.helpers;

import feedback.index.data.DateTimeDeserializer;
import feedback.index.data.DateTimeSerializer;
import feedback.index.data.Vote;
import feedback.utils.StringUtils;
import org.apache.lucene.document.*;
import org.apache.lucene.index.FieldInfo;

import java.io.IOException;

public class Indexer {
	public static Document toDoc(Vote vote) throws IOException {
		if(StringUtils.isBlank(vote.id) || StringUtils.isBlank(vote.type))
			throw new IOException("Id/type is not specified");

		Document doc = new Document();
		doc.add(new StringField(IndexFields.id, vote.id, Field.Store.YES));
		doc.add(new StringField(IndexFields.type, vote.type, Field.Store.NO));

		if(StringUtils.isNotBlank(vote.login))
			doc.add(new TextField(IndexFields.login, vote.login, Field.Store.YES));

		if(StringUtils.isNotBlank(vote.title))
			doc.add(new TextField(IndexFields.title, vote.title, Field.Store.YES));

		if(StringUtils.isNotBlank(vote.text)) {
			FieldType type = new FieldType();
			type.setIndexed(true);
			type.setStored(true);
			type.setTokenized(true);
			type.setIndexOptions(FieldInfo.IndexOptions.DOCS_AND_FREQS_AND_POSITIONS_AND_OFFSETS);
			doc.add(new Field(IndexFields.text, vote.text, type));
		}

		doc.add(new LongField(IndexFields.date, serializer.serialize(vote.date), Field.Store.YES));

		return doc;
	}

	public static Vote fromDoc(Document doc) {
		Vote vote = new Vote();
		vote.id = doc.get(IndexFields.id);
		vote.login = doc.get(IndexFields.login);
		vote.title = doc.get(IndexFields.title);
		vote.text = doc.get(IndexFields.text);
		vote.date = deserializer.deserialize(Long.parseLong(doc.get(IndexFields.date)));
		return vote;
	}

	private static DateTimeSerializer serializer = new DateTimeSerializer();
	private static DateTimeDeserializer deserializer = new DateTimeDeserializer();
}