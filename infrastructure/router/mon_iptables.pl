#!/usr/bin/env perl

%packets=();
%bytes=();

%aliases = ('checksystem' => 'cs',
            'devs' => 'dev');
$aliases{"team$_"} = sprintf 't%.02i', $_ for (1..16);
sub get_alias {
    $aliases{$_[0]} or $_[0];
}

open IPT, "iptables -L -vxn |", or die $!;
while (<IPT>) {
    if (/^Chain\s+(\w+) /) {
        if ($1 =~ /^mon_from_(\w+)$/) {
            $from = get_alias $1;
        } else {
            undef $from;
        }
    } elsif (/\s*(\d+)\s+(\d+)\s+mon_to_(\w+)/ and $from) {
        ($packets, $bytes, $to) = ($1, $2, get_alias $3);
        $packets{"$from-$to"} = $packets;
        $bytes{"$from-$to"} = $bytes;
    }
}
close IPT;

if (/\s*(\d+)\s+(\d+)\s+mon_to_(\w+)/ and $from) {
    ($packets, $bytes, $to) = ($1, $2, get_alias $3);
    $packets{"$from-$to"} = $packets;
    $bytes{"$from-$to"} = $bytes;
}

print scalar keys %packets, $/;

for $pair (sort keys %packets) {
    print "$pair $packets{$pair} $bytes{$pair}\n";
}
