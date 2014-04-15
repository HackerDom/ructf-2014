package feedback.index.data;

import org.codehaus.jackson.annotate.JsonProperty;

public class SearchResults {
	public SearchResults(int hits, Vote[] votes) {
		this.hits = hits;
		this.votes = votes;
	}

	@JsonProperty("hits")
	public int hits;

	@JsonProperty("votes")
	public Vote[] votes;
}