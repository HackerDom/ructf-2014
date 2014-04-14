package FMGCDB;

require Exporter;

our @ISA = qw/Exporter/;
our @EXPORT_OK = qw/db_init db_read db_write/;
our $VERSION = 1.0;

my %cache;

use constant {
    DB_FOLDER => 'db/'
};

sub db_init {
    return 1 if -d DB_FOLDER;
    mkdir DB_FOLDER or die "ERR_INIT";
}

sub db_read {
    my $key = shift;
    return $cache{$key} if exists $cache{$key};

    open F, '<', DB_FOLDER . $key or die "ERR_OPEN: '$key'";

    my $value = {};
    while (<F>) {
        chomp;
        my ($key, $val) = split /=/, $_, 2;
        $val = [split /;/, $val] if $val =~ /;/;
        $value->{$key} = $val;
    }

    close F;

    $cache{$key} = $value;
    return $value;
}

sub db_write {
    my ($key, $value) = @_;
    $cache{$key} = $value;

    local $\ = "\n";
    open F, '>', DB_FOLDER . $key or die "ERR_CREATE: '$key'";

    for (keys %$value) {
        my $val = ref $value->{$_} ? join ';', @{$value->{$_}} : $value->{$_};
        print F "$_=$val";
    }

    close F;
}

1;

