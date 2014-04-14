package feedback.index.lucutils;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.util.Version;

import java.io.Reader;

public class SimpleNumberAwareAnalyzer extends Analyzer {
	public SimpleNumberAwareAnalyzer(Version matchVersion) {
		this.matchVersion = matchVersion;
	}

	@Override
	protected TokenStreamComponents createComponents(String s, Reader reader) {
		return new TokenStreamComponents(new NumberAwareLowerCaseTokenizer(matchVersion, reader));
	}

	private final Version matchVersion;
}