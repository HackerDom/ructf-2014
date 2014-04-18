#!/usr/bin/perl

use constant {
    DEBUG => 0,
    PORT => 44100,
    TIMEOUT => 1,

    CHECKER_OK => 101,
    CHECKER_NOFLAG => 102,
    CHECKER_MUMBLE => 103,
    CHECKER_DOWN => 104,
    CHECKER_ERROR => 110,

    VISUALIZER => '/tmp/checker.vis',

    CONNECTION_ERROR => "Could not connect to service",
    FLAG_NOT_FOUND => "Flag not found"
};

use Socket;
use Term::ANSIColor qw/colorstrip/;

my ($mode, $ip, $id, $flag) = @ARGV;
my %handlers = (
    'check' => \&check,
    'put' => \&put,
    'get' => \&get
);

socket $S, PF_INET, SOCK_STREAM, getprotobyname 'tcp';
$cr = connect $S, sockaddr_in PORT, inet_aton $ip;
do_exit(CHECKER_DOWN, CONNECTION_ERROR) unless $cr;

#vec($v = '', fileno($S), 1) = 1;
$| = 1;
$keys = 1;

$handlers{$mode}->($id, $flag, $ip);

sub do_exit {
    my ($code, $msg, $log) = @_;

    if (DEBUG) { $msg = "\nOK" if CHECKER_OK == $code; }

    print $msg;
    print STDERR $log;
    shutdown $S, 2;
    exit $code;
}

sub check {
    send_key('RST');

    local $_ = &get_all;
    do_exit(CHECKER_MUMBLE) unless /RuCTF2014 FMGS/;
    do_exit(CHECKER_OK);
}

sub next_flight {
    my $ip = shift;
    my $fn = "$ip.fmgs-last";

    my $last = 0;
    if (open F, '<', $fn) {
        $last = <F>;
        close F;
    }

    open F, '<', $ip;
    my ($n, $result) = $last;
    do { $result = <F> } while ($last--);
    close F;

    open F, '>', $fn;
    print F ($n + 1);
    close F;

    return [$result =~
        /^(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+\s+\S+)\s+(\S+\s+\S+)\s+(.*)$/];
}

sub visualize {
    my ($ip, $flight) = @_;

    open F, '>>', VISUALIZER;
    my $line = join ' ', @{$flight}[5, 7, 6];
    $line =~ s/\s+/, /g;
    print F "$ip:\t[$line]\n";
    close F;
}

sub coords {
    my @args = map { sprintf "%.4f", $_ } split /\s+/, shift;
    $args[0] = ($args[0] < 0) ? ((-$args[0]) . 'S') : "$args[0]N";
    $args[1] = ($args[1] < 0) ? ((-$args[1]) . 'W') : "$args[1]E";

    join ' ', @args;
}

sub make_route {
    my ($flag, $pts) = @_;
    my @points = map { coords($_) } ($pts =~ /(\S+\s+\S+)/g);

    my $max_len = 10;
    if (@points > $max_len) {
        my $d = @points / $max_len;
        my @p = map { $points[$d*$_] } 1 .. $max_len;
        @points = @p;
    }

    $flag .= join '', map { chr(65 + int(rand(25))) }
        1..(5*@points - length($flag));

    for (1..@points) {
        $points[$_ - 1] = substr($flag, 5*($_ - 1), 5) . ' ' . $points[$_ - 1];
    }

    @points;
}

sub put {
    my ($id, $flag, $ip) = @_;

    my $fl = next_flight($ip);
    send_key('RST');
    send_key('1L');

    send_key('1L');
    send_line($fl->[1]);
    send_key('OK');

    send_key('2L');
    send_line($fl->[0]);
    send_key('OK');

    send_key('1R');
    send_line($fl->[2] . ' ' . coords($fl->[4]));
    send_key('OK');

    send_key('2R');
    send_line($fl->[3] . ' ' . coords($fl->[5]));
    send_key('OK');

    send_key('5R');

    my @route = make_route($flag, $fl->[6]);
    for (1..@route) {
        send_key(($_ < 5 ? $_ : 5) . 'L');
        send_line($route[$_ - 1]);
        send_key('OK');

        send_key('PGDN') if $_ >= 5;
    }

    send_key('6R');
    send_key('6R');

    my $result = &get_all;
    # print STDERR "GOT: '$result'\n";
    do_exit(CHECKER_MUMBLE, 'error') if $result =~ /ERROR/;

    print $fl->[0];
    eval { visualize($ip, $fl) };
    do_exit(CHECKER_OK);
}

sub get {
    my ($id, $flag, $ip) = @_;

    send_key('RST');
    send_key('2L');
    send_line($id);
    send_key('OK');
    send_key('PGDN');

    my $result = &get_all;
    # print STDERR "GOT: '$result'\n";
    do_exit(CHECKER_MUMBLE, 'error') if $result =~ /ERROR/;
    for my $i (0..6) {
        do_exit(CHECKER_NOFLAG, "no flag")
            if index($result, substr($flag, 5*$i, 5)) == -1;
    }

    do_exit(CHECKER_OK);
}

sub send_key {
    my $msg = shift;
    send $S, "$msg\n", 0;
    ++ $k;
}

sub send_line {
    my $msg = join "\n", split //, $_[0];
    send $S, "$msg\n", 0;
    $k += length($_[0])
}

sub get_all {
    my $x = '';

    $x .= <$S> for 1..15*($k + 1);
    $k = 0;

    return $x;

=pod
    for (1..100*TIMEOUT) {
        next unless select '' . $v, undef, undef, 0.01;
        while (select '' . $v, undef, undef, 0.01) {
            recv $S, ($_ = ''), 1024, 0;
            return $x unless length;

            $x .= $_;
        }
    }

    return $x;
=cut
}

