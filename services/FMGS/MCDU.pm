package MCDU;

use Term::ANSIColor;
require Exporter;

our @ISA = qw/Exporter/;
our @EXPORT_OK = qw/
    set_title
    set_status
    set_action
    do_action
    select_line
    read_line
    put_line
    draw
    clean/;
our $VERSION = 1.0;

use constant {
    RWS => 6,
    SW => 24
};

my %cls = (
    DATA => 'white',
    ACTIVE => 'green',
    SELECTABLE => 'yellow',
    ERROR => 'bright_red',
    INFO => 'bright_yellow'
);

my (%dsp, %act, $sel);

sub set_title {
    $dsp{'TITLE'} = shift;
}

sub set_status {
    $dsp{'STATUS'} = shift;
}

sub set_action {
    my ($btn, $ref, @params) = @_;
    $act{$btn} = [$ref, \@params];
}

sub do_action {
    &{$act{$_[0]}->[0]}(@{$act{$_[0]}->[1]}) if exists $act{$_[0]}
}

sub select_line {
    $dsp{$sel}->[0] = (split /#/, $dsp{$sel}->[0], 2)[1] if $sel;
    $sel = $_[1] ? $_[0] : undef;
    $dsp{$sel}->[0] = 'ACTIVE#' . $dsp{$sel}->[0] if $sel;
}

sub read_line {
    my ($f, @btn) = @_;

    for (@btn) {
        next if length != 1;
        set_action($_, sub { $dsp{$sel}->[1] .= $_ });
        set_action('CLN', sub { $dsp{$sel}->[1] = ''; 1 });
        set_action('OK', sub {
            delete $act{$_} for (@btn, 'CLN', 'OK');
            $f->($dsp{$sel}->[1]) if $f;
            select_line($sel, 0); 1
        });
    }
}

sub put_line {
    my ($line, $pos, $text, $subtext) = @_;

    $dsp{"$line$pos"} = [$text, $subtext];
}

sub _color {
    return (/#/ ? $cls{[split /#/]->[0]} : $_[1]) for $_[0];
}

sub _align {
    my ($text, $len, $pos) = @_;
    my $text = [split /#/, $text]->[-1];
    for ($text) {
        my $delta = $len - length;
        if ($delta <= 0) {
            return substr $_, 0, $len if uc($pos) eq 'L';
            return substr $_, -$len if $pos eq 'R';
            return substr $_, (-$delta) >> 1, $len if $pos eq 'C';
            return ""
        }

        my ($sp1, $sp2) = (' ' x ($delta >> 1), ' ' x ($delta - ($delta >> 1)));
        return "$_$sp1$sp2" if $pos eq 'L';
        return "$sp1$sp2$_" if $pos eq 'R';
        return "$sp1$_$sp2" if $pos eq 'C';
        return $_ if $pos eq 'l';
        return ""
    }
}

sub draw {
    my $scr = '';

    $scr .= '    ' . colored(_align($dsp{'TITLE'}, SW, 'C'),
        'bold bright_white') . "\n\n";
    for my $row (1..RWS) {
        for ([0, 'bright_yellow'], [1, 'bright_cyan']) {
            my @t = ($dsp{"${row}L"}->[$_->[0]], $dsp{"${row}R"}->[$_->[0]]);

            my $l1 = _align($t[0], SW, 'l');
            my $l2 = _align($t[1], SW - length($l1), 'R');
            $scr .= colored($_->[0] ? '    ' : '[-] ', 'white') .
                colored($l1, _color($t[0], $_->[1])) .
                colored($l2, _color($t[1], $_->[1])) .
                colored($_->[0] ? '    ' : ' [-]', 'white') . "\n";
        }
    }
    $scr .= '    ' . colored(_align($dsp{'STATUS'}, SW, 'l'), 'yellow') . "\n";

    return $scr;
}

sub clean {
    set_title;
    set_status;
    map { for my $p ('L', 'R') { put_line($_, $p, '', '') } } 1..RWS;
}

1;

