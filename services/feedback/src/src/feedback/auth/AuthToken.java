package feedback.auth;

import org.apache.commons.io.output.ByteArrayOutputStream;
import org.joda.time.DateTime;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public class AuthToken {
	public AuthToken(String login, boolean isAdmin, DateTime dateTime) {
		this.login = login;
		this.isAdmin = isAdmin;
		this.dateTime = dateTime;
	}

	//Stream header: STREAM_MAGIC + STREAM_VERSION = 4 bytes
	//Block data header: TC_BLOCKDATA + len = 2 bytes
	//UTF: len + bytes = 2 + N bytes
	public byte[] serialize() throws IOException {
		try(ByteArrayOutputStream stream = new ByteArrayOutputStream()) {
			try(ObjectOutputStream writer = new ObjectOutputStream(stream)) {
				writer.writeUTF(login);
				writer.flush();
				writer.writeBoolean(isAdmin);
				writer.writeLong(dateTime.getMillis());
			}
			return stream.toByteArray();
		}
	}

	public static AuthToken deserialize(byte[] value) throws IOException {
		if(value == null) return null;
		try(ByteArrayInputStream stream = new ByteArrayInputStream(value)) {
			try(ObjectInputStream reader = new ObjectInputStream(stream)) {
				String login = reader.readUTF();
				boolean isAdmin = reader.readBoolean();
				long millis = reader.readLong();
				return new AuthToken(login, isAdmin, new DateTime(millis));
			}
		}
	}

	public String getLogin() {
		return login;
	}

	public boolean isAdmin() {
		return isAdmin;
	}

	private final String login;
	private final boolean isAdmin;
	private final DateTime dateTime;
}