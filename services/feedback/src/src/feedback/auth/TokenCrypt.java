package feedback.auth;

import org.apache.commons.io.FileUtils;
import sun.misc.BASE64Decoder;
import sun.misc.BASE64Encoder;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESedeKeySpec;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;

public class TokenCrypt {
	public TokenCrypt(String keyFileName) throws NoSuchAlgorithmException, InvalidKeyException, InvalidKeySpecException, IOException {
		File keyFile = new File(keyFileName);
		try {
			byte[] keyBytes = FileUtils.readFileToByteArray(keyFile);
			KeySpec keySpec = new DESedeKeySpec(keyBytes);
			SecretKeyFactory keyFactory = SecretKeyFactory.getInstance(DESede);
			key = keyFactory.generateSecret(keySpec);
		} catch(FileNotFoundException e) {
			key = KeyGenerator.getInstance(DESede).generateKey();
			FileUtils.writeByteArrayToFile(keyFile, key.getEncoded());
		}
	}

	public String encrypt(byte[] bytes) {
		try {
			Cipher cipher = Cipher.getInstance(DESede);
			cipher.init(Cipher.ENCRYPT_MODE, key);
			byte[] encrypted = cipher.doFinal(bytes);
			BASE64Encoder base64encoder = new BASE64Encoder();
			return URLEncoder.encode(base64encoder.encode(encrypted), UTF8);
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	//WARN! No message authentication code! User can tamper data!
	public byte[] decrypt(String token) {
		try {
			Cipher cipher = Cipher.getInstance(DESede);
			cipher.init(Cipher.DECRYPT_MODE, key);
			BASE64Decoder base64decoder = new BASE64Decoder();
			byte[] encrypted = base64decoder.decodeBuffer(URLDecoder.decode(token, UTF8));
			return cipher.doFinal(encrypted);
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	private static final String UTF8 = "UTF8";
	private static final String DESede = "DESede";
	private SecretKey key;
}