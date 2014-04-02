#!/usr/bin/perl

use Socket;
use FMGCDB qw/init fl_read fl_write/;
use threads;

use constant {
    PORT => 44100,
    TIMEOUT => 60
};

($|, $SIG{'PIPE'}) = (1, 'IGNORE');

eval { init } or die "Can't initialize db";

socket $S, PF_INET, SOCK_STREAM, getprotobyname 'tcp';
setsockopt $S, SOL_SOCKET, SO_REUSEADDR, 1;
bind $S, sockaddr_in PORT, INADDR_ANY or die "Can't bind: $!";
listen $S, SOMAXCONN;

while (accept $C, $S) {
    my $thread = new threads (\&process, $C);
    $thread->detach ();
}

close $S;


sub process {
    my ($C, $c) = @_;

    sub recv_str {
        my $r = '';
        for (1 .. (TIMEOUT * 100)) {
            next unless select ''  . $c, undef, undef, 0.01;
            while (select '' . $c, undef, undef, 0) {
                recv $C, ($_ = ''), 1, 0;
                $r =~ s/\r|\n//g, return $r if /\n/ || !length;
                $r .= $_;
            }
        }
    }

    vec ($c = '', fileno ($C), 1) = 1;


    $x = '';
    while ($x ne 'exit') {
        $x = recv_str;
        @x = split /\s/, $x;
        $info = undef;

        if ($x[0] eq 'read') {
            eval { $info = fl_read $x[1] };
            if ($@) {
                send $C, "ERR R_FLIGHT $x\n", 0;
                next;
            }

            $val = $info->{$x[2]};
            $val = "@$val" if ref $val;

            send $C, "$val\n", 0;
        }
        elsif ($x[0] eq 'write') {
            $info = {};
            eval { $info = fl_read $x[1] };

            $val = [ @x[3 .. $#x] ];
            $val = $val->[0] if @$val == 1;

            $info->{$x[2]} = $val;
            eval { fl_write ($x[1], $info) };
            if ($@) {
                send $C, "ERR W_FLIGHT $x\n", 0;
                next;
            }
        }
        else {
            send $C, "ECHO: $x\n", 0;
        }
    }

    shutdown $C, 2;
}

