package feedback.index.lucutils;

import org.apache.lucene.analysis.util.CharTokenizer;
import org.apache.lucene.util.Version;

import java.io.Reader;

public class NumberAwareLowerCaseTokenizer extends CharTokenizer {
	public NumberAwareLowerCaseTokenizer(Version matchVersion, Reader input) {
		super(matchVersion, input);
	}

	@Override
	protected boolean isTokenChar(int c) {
		return Character.isLetterOrDigit(c);
	}

	@Override
	protected int normalize(int c) {
		return Character.toLowerCase(c);
	}
}