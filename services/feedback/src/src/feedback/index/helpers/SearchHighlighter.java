package feedback.index.helpers;

import feedback.index.data.Vote;
import feedback.utils.StringUtils;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.search.highlight.*;
import org.apache.lucene.search.postingshighlight.DefaultPassageFormatter;
import org.apache.lucene.search.postingshighlight.PassageFormatter;
import org.apache.lucene.search.postingshighlight.PostingsHighlighter;

public class SearchHighlighter {
	public SearchHighlighter(Analyzer analyzer) {
		this.analyzer = analyzer;
		formatter = new SimpleHTMLFormatter("<b>", "</b>");
		fragmenter = new NullFragmenter();
		encoder = new SimpleHTMLEncoder();

		postingsHighlighter = new PostingsHighlighter(256) {
			@Override
			protected PassageFormatter getFormatter(String fieldName) {
				return new DefaultPassageFormatter("<b>", "</b>", "... ", true) {
					@Override
					protected void append(StringBuilder dest, String content, int start, int end) {
						if(escape) {
							for (int i = start; i < end; i++) {
								char ch = content.charAt(i);
								switch(ch) {
									case '&':
										dest.append("&amp;");
										break;
									case '<':
										dest.append("&lt;");
										break;
									case '>':
										dest.append("&gt;");
										break;
									case '"':
										dest.append("&quot;");
										break;
									case '\'':
										dest.append("&#x27;");
										break;
									case '/':
										dest.append("&#x2F;");
										break;
									default:
										dest.append(ch);
								}
							}
						} else {
							dest.append(content, start, end);
						}
					}
				};
			}
		};
	}

	public void highlight(Vote[] votes, Query query, IndexSearcher searcher, TopDocs topDocs) {
		if(votes == null || votes.length == 0 || query == null)
			return;
		try {
			String[] texts = postingsHighlighter.highlight(IndexFields.text, query, searcher, topDocs, 2);
			for(int i = 0; i < votes.length; i++) {
				votes[i].text = texts[i] != null ? texts[i] : StringUtils.shorten(votes[i].text, 64, 256);
			}
		} catch(Exception ignored) {}
		for(Vote vote : votes) {
			vote.title = getFieldHighlighted(query, IndexFields.title, vote.title);
			vote.login = getFieldHighlighted(query, IndexFields.login, vote.login);
		}
	}

	private String getFieldHighlighted(Query query, String fieldName, String fieldValue) {
		try {
			QueryScorer queryScorer = new QueryScorer(query, fieldName);
			Highlighter highlighter = new Highlighter(formatter, encoder, queryScorer);
			highlighter.setTextFragmenter(fragmenter);
			String highlighted = highlighter.getBestFragment(analyzer, fieldName, fieldValue);
			return highlighted != null ? highlighted : encoder.encodeText(fieldValue);
		}
		catch(Exception ignored) {
			return encoder.encodeText(fieldValue);
		}
	}

	private final Analyzer analyzer;
	private final PostingsHighlighter postingsHighlighter;
	private final SimpleHTMLFormatter formatter;
	private final Fragmenter fragmenter;
	private final Encoder encoder;
}