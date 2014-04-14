package FMGC;

require Exporter;
use FMGCDB qw/db_init/;

our @ISA = qw/Exporter/;
our @EXPORT_OK = qw//;
our $VERSION = 1.0;

db_init;



1;

