#!/usr/bin/perl

use Socket;
use FMGCDB qw/db_init db_read db_write/;
use FMGC qw/btn_press sys_init/;
use MCDU qw/draw/;
use threads;

use constant {
    PORT => 44100,
    TIMEOUT => 10
};

($|, $SIG{'PIPE'}) = (1, 'IGNORE');

eval { db_init } or die "Can't initialize DB";
eval { sys_init } or die "Can't initialize FMGS";

socket $S, PF_INET, SOCK_STREAM, getprotobyname 'tcp';
setsockopt $S, SOL_SOCKET, SO_REUSEADDR, 1;
bind $S, sockaddr_in PORT, INADDR_ANY or die "Can't bind: $!";
listen $S, SOMAXCONN;

while (accept $C, $S) {
    my $thread = new threads(sub { eval { &process } }, $C);
    $thread->detach();
}

close $S;

sub process {
    my ($C, $c) = @_;

    sub recv_str {
        my $r = undef;
        for (1 .. (TIMEOUT * 100)) {
            next unless select ''  . $c, undef, undef, 0.01;
            while (select '' . $c, undef, undef, 0) {
                recv $C, ($_ = ''), 1, 0;
                $r =~ s/\r|\n//g, return $r if /\n/ || !length;
                $r .= $_;
            }
        }
        undef;
    }

    vec ($c = '', fileno ($C), 1) = 1;

    local $_ = '';
    send $C, &draw, 0;
    while (defined ($_ = recv_str)) { send $C, &draw, 0 if btn_press($_) }

    shutdown $C, 2;
}

