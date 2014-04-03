#!/usr/bin/perl -l

sub parse_aircraft {
    my @xs = split /\s+/, shift;
    $a = (($xs[0] eq 'Boeing') ? 'B' : '') . $xs[-1];
    return $a if $a =~ /^(?:(A3)|(B7)|(MD)|([EC]RJ)|(AN)|(IL)|(TU)|(YAK)).*/i;
    '';
}

sub parse_trail {
    my @res = ();
    for (0 .. @_) {
        push @res, $_[$_] if $_ % 4 < 2;
    }
    @res;
}

for (@ARGV) {
    open F, '<', $_;
    binmode F;

    while (sysread F, $n, 4) {
        sysread F, $len, 4;
        sysread F, $data, unpack 'V', $len;

        for ($data) {
            $flight = (/"flight":"(.*?)"/)[0];
            $from_iata = (/"from_iata":"(.*?)"/)[0];
            $to_iata = (/"to_iata":"(.*?)"/)[0];
            @from_pos = (/"from_pos":\[(.+?),(.+?)\]/)[0, 1];
            @to_pos = (/"to_pos":\[(.+?),(.+?)\]/)[0, 1];
            $aircraft = parse_aircraft((/"aircraft":"(.*?)"/)[0]);
            @trail = parse_trail(split /,/, (/"trail":\[(.*?)\]/)[0]);

            print ("$flight $aircraft $to_iata $from_iata " .
                (join " ", @to_pos) . " " .
                (join " ", @from_pos) . " " .
                (join " ", @trail))
                if $flight && $aircraft && $to_iata && $from_iata && @trail > 1;
        }
    }

    close F;
}

