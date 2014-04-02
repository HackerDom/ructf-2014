package FMGCDB;

our @ISA = qw/Exporter/;
our @EXPORT_OK = qw/init fl_read fl_write/;
our $VERSION = 1.0;

use constant {
    DB_FOLDER => 'db/'
};

sub init {
    return 1 if -d DB_FOLDER;
    mkdir DB_FOLDER or die "ERR_INIT";
}

sub fl_read {
    my $flight = shift;
    open F, '<', DB_FOLDER . $flight or die "ERR_OPEN: '$flight'";

    my $data = {};
    while (<F>) {
        my ($key, $val) = split /=/, $_, 2;
        $val = [split /:/, $val] if $val =~ /:/;
        $data->{$key} = $val;
    }

    close F;
    return $data;
}

sub fl_write {
    my ($flight, $data) = @_;
    local $\ = "\n";
    open F, '>', DB_FOLDER . $flight or die "ERR_CREATE: '$flight'";

    for (keys %$data) {
        my $val = ref $data->{$_} ? join ':', @{$data->{$_}} : $data->{$_};
        print F "$_=$val";
    }

    close F;
}

1;

