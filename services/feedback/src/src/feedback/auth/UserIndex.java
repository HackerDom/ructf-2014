package feedback.auth;

import feedback.utils.StringUtils;
import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.map.ObjectReader;
import org.codehaus.jackson.map.ObjectWriter;
import org.codehaus.jackson.map.annotate.JsonSerialize;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class UserIndex {
	public UserIndex(String usersFileName) throws IOException {
		File usersFile = new File(usersFileName);
		users = readFromFile(usersFile);
		fileWriter = new FileWriter(usersFile, true);
		ObjectMapper mapper = new ObjectMapper();
		mapper.setSerializationInclusion(JsonSerialize.Inclusion.NON_NULL);
		this.objectWriter = mapper.writer();
	}

	public User auth(String login, String password) {
		User user = users.get(login.toLowerCase());
		return user != null && user.password.equals(password) ? user : null;
	}

	public boolean add(User user) throws IOException {
		boolean added = users.putIfAbsent(user.login.toLowerCase(), user) == null;
		if(added) {
			String line = objectWriter.writeValueAsString(user);
			fileWriter.append("\n").append(line).append("\n");
			fileWriter.flush();
		}
		return added;
	}

	private static ConcurrentMap<String, User> readFromFile(File usersFile) throws IOException {
		ConcurrentMap<String, User> users = new ConcurrentHashMap<>();
		ObjectReader objectReader = new ObjectMapper().reader(User.class);
		try {
			try(FileInputStream stream = new FileInputStream(usersFile)) {
				try(InputStreamReader streamReader = new InputStreamReader(stream, UTF8)) {
					try(BufferedReader lineReader = new BufferedReader(streamReader)) {
						String ln;
						while((ln = lineReader.readLine()) != null) {
							String line = ln.trim();
							if(StringUtils.isBlank(line))
								continue;
							User user = null;
							try {
								user = objectReader.readValue(line);
							} catch(Exception e) {
								log.warn(String.format("Failed to parse '%s' from users file", line), e);
							}
							if(user != null && user.login != null) {
								users.put(user.login, user);
							}
						}
						log.info("Read '{}', found {}", new Object[] {usersFile.getPath(), users.size()});
						return users;
					}
				}
			}
		} catch(FileNotFoundException ignored) {
			return users;
		}
	}

	private static final String UTF8 = "UTF8";
	private static final Logger log = LoggerFactory.getLogger(UserIndex.class);
	private final ConcurrentMap<String, User> users;
	private final FileWriter fileWriter;
	private final ObjectWriter objectWriter;
}