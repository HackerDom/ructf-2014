#!/usr/bin/perl -lw

use Mojo::UserAgent;

my ($ip, $id, $flag) = @ARGV;
my @parts = split ':', $id, 3;

my $ua = Mojo::UserAgent->new;

my $tx = $ua->post("http://$ip:7654/auth" => json => {login => $parts[0], password => $parts[1]});
if (my $res = $tx->success) {
	warn $res->body;
	my $tx = $ua->get("http://$ip:7654/search?query=" . $parts[2]);
	if (my $res = $tx->success) {
		my $hits = $res->json->{hits};
		if ($hits < 1) {
			warn 'too few "hits"';
			exit 102;
		}
		my @votes = @{$res->json->{votes}};
		unless (@votes) {
			warn 'votes is empty';
			exit 102;
		}
		my $title = $votes[0]->{title};
		if ($title =~ /$flag/) {
			exit 101;
		} else {
			exit 102;
		}
	}
} else {
	# my $
}
