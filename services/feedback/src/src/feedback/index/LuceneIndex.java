package feedback.index;

import feedback.index.data.SearchResults;
import feedback.index.data.Vote;
import feedback.index.data.VoteType;
import feedback.index.helpers.IndexFields;
import feedback.index.helpers.Indexer;
import feedback.index.helpers.SearchHighlighter;
import feedback.index.lucutils.SimpleNumberAwareAnalyzer;
import feedback.utils.StringUtils;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.index.Term;
import org.apache.lucene.index.TrackingIndexWriter;
import org.apache.lucene.queryparser.classic.MultiFieldQueryParser;
import org.apache.lucene.queryparser.classic.ParseException;
import org.apache.lucene.queryparser.classic.QueryParser;
import org.apache.lucene.search.*;
import org.apache.lucene.search.highlight.InvalidTokenOffsetsException;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.MMapDirectory;
import org.apache.lucene.util.ThreadInterruptedException;
import org.apache.lucene.util.Version;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;

public class LuceneIndex {
	public LuceneIndex(String indexPath) throws Exception {
		Directory directory = MMapDirectory.open(new File(indexPath));//new NRTCachingDirectory(NIOFSDirectory.open(new File(indexPath)), 4, 64);

		analyzer = new SimpleNumberAwareAnalyzer(Version.LUCENE_47);

		IndexWriterConfig writerConfig = new IndexWriterConfig(Version.LUCENE_47, analyzer);
		writerConfig.setOpenMode(IndexWriterConfig.OpenMode.CREATE_OR_APPEND);

		writer = new IndexWriter(directory, writerConfig);
		trackingWriter = new TrackingIndexWriter(writer);

		indexFields = new IndexFields();

		searcherManager = new SearcherManager(writer, true, null);
		ControlledRealTimeReopenThread<IndexSearcher> reopenThread = new ControlledRealTimeReopenThread<>(trackingWriter, searcherManager, 5.0, 0.2);
		reopenThread.setName("reopen-thread");
		reopenThread.setPriority(Math.min(Thread.currentThread().getPriority() + 2, Thread.MAX_PRIORITY));
		reopenThread.setDaemon(true);
		reopenThread.start();

		Thread commitThread = new Thread(() -> {
			boolean stop = false;
			while(!stop) {
				try {
					Thread.sleep(20000);
					commit();
				}
				catch(Exception ignored) {
					if(ignored instanceof ThreadInterruptedException)
						stop = true;
				}
			}
		}, "commit-thread");
		commitThread.setPriority(Math.max(Thread.currentThread().getPriority() - 2, Thread.MIN_PRIORITY));
		commitThread.setDaemon(true);
		commitThread.start();

		highlighter = new SearchHighlighter(analyzer);
		sort = new Sort(new SortField(IndexFields.date, SortField.Type.LONG, true));
	}

	public SearchResults search(String text, int top, String login, boolean all) throws ParseException, IOException, InvalidTokenOffsetsException {
		if(text == null) text = "";

		QueryParser parser = new MultiFieldQueryParser(Version.LUCENE_47, indexFields.fieldsArray, analyzer, indexFields.boosts);
		parser.setDefaultOperator(QueryParser.Operator.AND);
		parser.setAllowLeadingWildcard(true);

		//long start = System.currentTimeMillis();
		IndexSearcher searcher = searcherManager.acquire();

		try {
			String filter = "";
			filter = String.format("%s:%s", IndexFields.type, all ? "*" : VoteType.VISIBLE);
			if(StringUtils.isNotBlank(login)) {
				filter = String.format("(%s OR %s)", filter, String.format("%s:\"%s\"", IndexFields.login, login));
			}

			Query query = parser.parse(filter + " " + text); //WARN! Filter concatenation and no special characters escaping!

			TopDocs hits = searcher.search(query, top, sort);

			Vote[] results = new Vote[hits.scoreDocs.length];
			for(int i = 0; i < hits.scoreDocs.length; i++)
				results[i] = Indexer.fromDoc(searcher.doc(hits.scoreDocs[i].doc));

			highlighter.highlight(results, query, searcher, hits);

			//log.info("Search: '{}', hits {}, elapsed {} ms", new Object[] {text, hits.totalHits, System.currentTimeMillis() - start});

			return new SearchResults(hits.totalHits, results);
		} finally {
			searcherManager.release(searcher);
		}
	}

	public void addOrUpdateSubject(Vote vote) throws IOException {
		trackingWriter.updateDocument(new Term(IndexFields.id, vote.id), Indexer.toDoc(vote));
		searcherManager.maybeRefreshBlocking();
	}

	public void commit() throws IOException {
		//long start = System.currentTimeMillis();
		writer.commit();
		searcherManager.maybeRefresh();
		//log.info("Commit, elapsed {} ms", System.currentTimeMillis() - start);
	}

	private static final Logger log = LoggerFactory.getLogger(LuceneIndex.class);
	private final IndexFields indexFields;
	private final ReferenceManager<IndexSearcher> searcherManager;
	private final IndexWriter writer;
	private final TrackingIndexWriter trackingWriter;
	private final Analyzer analyzer;
	private final SearchHighlighter highlighter;
	private final Sort sort;
}