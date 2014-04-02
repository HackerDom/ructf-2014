package MCDU;

use Term::ANSIColor;
require Exporter;

our @ISA = qw/Exporter/;
our @EXPORT_OK = qw//;
our $VERSION = 1.0;

sub new {
    my $class = shift;

    return bless { }, $class;
}

sub display {
    my ($this, $F, $page) = @_;

    
}

1;

