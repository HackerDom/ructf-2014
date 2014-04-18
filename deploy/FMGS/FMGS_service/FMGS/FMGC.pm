package FMGC;

require Exporter;
use Math::Trig;
use FMGCDB qw/db_read db_write/;
use MCDU qw/set_title set_status set_action del_action put_line
    do_action clean select_line read_line/;

our @ISA = qw/Exporter/;
our @EXPORT_OK = qw/sys_init btn_press/;
our $VERSION = 1.0;

sub _pack_point { join ":", @_ }

sub _unpack_point { split /:/, shift, 2 }

sub _store_pts {
    my ($param, $pts) = (shift, {});
    eval { $pts = db_read('POINTS') };
    for (@$param) {
        my ($name, $coor) = _unpack_point($_);
        $pts->{$name} = exists $pts->{$name} ? [$pts->{$name}, $coor] : $coor;
    }

    db_write('POINTS', $pts);
}

sub _dist {
    my ($w1, $l1) = &_unpack_point;
    my ($w2, $l2) = &_unpack_point;
    $_ *= 3.14159265/180 for ($w1, $w2, $l1, $l2);

    return 6371*acos(sin($w1)*sin($w2) + cos($w1)*cos($w2)*cos($l1 - $l2));
}

sub _min { [$_[1] => $_[0]]->[$_[0] <= $_[1]] }
sub _max { [$_[1] => $_[0]]->[$_[1] <= $_[0]] }

sub deparse {
    my @data = @_;
    set_status("ERROR"), return undef if @data != 2;
    set_status("ERROR"), return undef
        unless $data[0] =~ /^\d+\.\d+[NS]$/;
    set_status("ERROR"), return undef
        unless $data[1] =~ /^\d+\.\d+[WE]$/;

    $data[0] =~ /^(\d+\.\d+)([NS])$/;
    $data[0] = ($2 eq 'N' ? 1 : -1)*$1;

    $data[1] =~ /^(\d+\.\d+)([WE])$/;
    $data[1] = ($2 eq 'E' ? 1 : -1)*$1;

    @data;
}

sub convert_point {
    my ($name, $w, $l) = split /:/, shift;
    $w = ($w < 0) ? ((-$w) . 'S') : "${w}N";
    $l = ($l < 0) ? ((-$l) . 'W') : "${l}E";

    [$name, "$w $l"]
}

sub sys_init {
    my ($s_init, $f_init, $r_init);
    my (%flight, @route, @track, $r_first, $comment);

    $s_init = sub {
        clean;
        %flight = ();
        set_title('RuCTF2014 FMGS');

        put_line(1, 'L', 'SELECTABLE#INIT');
        set_action('1L', $f_init = sub {
            clean;
            set_title('FLIGHT INIT');
            put_line(1, 'L', 'INFO#AIRCRAFT', $flight{'AIRCRAFT'});
            set_action('1L', sub {
                select_line('1L', 1);
                read_line(
                    sub { $flight{'AIRCRAFT'} = shift },
                    ('A'..'Z', '0'..'9', '-')
                ); 1
            });

            put_line(2, 'L', 'INFO#FLIGHT', $flight{'FLIGHT'});
            set_action('2L', sub {
                select_line('2L', 1);
                read_line(
                    sub { $flight{'FLIGHT'} = shift },
                    ('A'..'Z', '0'..'9')
                ); 1
            });

            put_line(1, 'R', 'INFO#FROM', $flight{'FROM'});
            set_action('1R', sub {
                select_line('1R', 1);
                read_line(
                    sub { $flight{'FROM'} = shift },
                    ('A'..'Z', '0'..'9', ' ', '.')
                ); 1
            });

            put_line(2, 'R', 'INFO#TO', $flight{'TO'});
            set_action('2R', sub {
                select_line('2R', 1);
                read_line(
                    sub { $flight{'TO'} = shift },
                    ('A'..'Z', '0'..'9', ' ', '.')
                ); 1
            });

            put_line(5, 'R', 'SELECTABLE#ROUTE');
            set_action('5R', $r_init = sub {
                clean;
                set_title('ROUTE INIT');
                for my $i (0..4) {
                    my $idx = $r_first + $i;
                    put_line(1 + $i, 'L',
                        'DATA#' . $route[$idx]->[0], $route[$idx]->[1])
                        if $route[$idx];
                    set_action((1 + $i).'L', sub {
                        select_line((1 + $i).'L', 1);
                        read_line(
                            sub {
                                $route[$idx] = [split /\s+/, shift, 2];
                                &$r_init
                            },
                            ('A'..'Z', '0'..'9', ' ', '.', '=')
                        ); 1
                    });
                    last unless $route[$idx];
                }

                set_action('CLN', sub {
                    ($r_first, @route) = 0;
                    &$r_init
                });

                set_action('PGUP', sub {
                    $r_first = _max(0, $r_first - 5);
                    &$r_init
                });

                set_action('PGDN', sub {
                    $r_first = _min(_max(0, @route - 4), $r_first + 5);
                    &$r_init
                });

                put_line(6, 'L', 'SELECTABLE#<BACK');
                set_action('6L', sub {
                    del_action($_) for ('CLN', 'PGUP', 'PGDN');
                    &$f_init
                });

                put_line(1, 'R', 'SELECTABLE#AUTO');
                set_action('1R', sub {
                    my %ep;
                    for ('FROM', 'TO') {
                        my ($city, @coor) = split /\s+/, $flight{$_};
                        $ep{$_} = join ':', deparse(@coor);
                    }

                    my $rt = make_route(FROM => $ep{'FROM'}, TO => $ep{'TO'});

                    ($r_first, @route) = (0, map { convert_point($_) } @$rt);
                    &$r_init;
                });

                put_line(2, 'R', 'INFO#COMMENT', $comment);
                set_action('2R', sub {
                    select_line('2R', 1);
                    read_line(
                        sub { $comment = shift },
                        ('A'..'Z', '0'..'9', '-', ' ', '=')
                    ); 1
                });

                put_line(6, 'R', 'SELECTABLE#OK');
                set_action('6R', sub {
                    @track = map {
                        [$_->[0], deparse(split /\s+/, $_->[1])]
                    } @route;
                    del_action($_) for ('CLN', 'PGUP', 'PGDN');
                    &$f_init;
                });
            });

            put_line(6, 'L', 'SELECTABLE#<HOME');
            set_action('6L', $s_init);

            put_line(6, 'R', 'SELECTABLE#OK');
            set_action('6R', sub {
                my %ep;
                for ('FROM', 'TO') {
                    my ($city, @coor) = split /\s+/, $flight{$_};
                    $ep{$_} = [$city, deparse(@coor)];
                }

                &$s_init, return 1 if add_flight(
                    FLIGHT => $flight{'FLIGHT'},
                    AIRCRAFT => $flight{'AIRCRAFT'},
                    FROM => (join ':', @{$ep{'FROM'}}),
                    TO => (join ':', @{$ep{'TO'}}),
                    ROUTE => \@track);

                set_status("ERROR ADD"); 1
            });
        });

        put_line(2, 'L', 'SELECTABLE#F-PLN');
        set_action('2L', sub {
            my $v_flight = sub {
                my $flight = view_flight('FLIGHT' => shift);
                set_status('ERROR'), return unless defined $flight;

                clean;
                set_title('FLIGHT INFO');
                put_line(1, 'L', 'INFO#AIRCRAFT', $flight->[1]);
                put_line(2, 'L', 'INFO#FLIGHT', $flight->[0]);
                put_line(3, 'L', @{convert_point($flight->[2])});
                put_line(4, 'L', @{convert_point($flight->[3])});

                my $idx = 0;
                my $view_route = sub {
                    for my $i (0..4) {
                        my $p = $flight->[4]->[$i + $idx];
                        put_line(1 + $i, 'R', $p ? @{convert_point($p)} : '');
                    }
                    1
                };
                &$view_route;

                set_action('PGUP', sub {
                    $idx = _max(0, $idx - 5);
                    &$view_route
                });

                set_action('PGDN', sub {
                    $idx = _min(_max(0, @{$flight->[4]} - 4), $idx + 5);
                    &$view_route
                });

                put_line(6, 'L', 'SELECTABLE#<BACK');
                set_action('6L', $s_init);
            };

            select_line('2L', 1);
            read_line($v_flight, ('A'..'Z', '0'..'9')); 1
        });
    };

    set_action('RST', $s_init);

    &$s_init;
}

sub btn_press {
    &do_action;
}

sub add_flight {
    my (%params, %value) = @_;

    @value{'FLIGHT', 'AIRCRAFT', 'FROM', 'TO'} =
        @params{'FLIGHT', 'AIRCRAFT', 'FROM', 'TO'};
    $value{'ROUTE'} = [map { _pack_point(@$_) } @{$params{'ROUTE'}}];

    eval { db_write($value{'FLIGHT'}, \%value) } &&
    eval { _store_pts($value{'ROUTE'}) }
}

sub view_flight {
    my (%params, $info) = @_;

    eval { $info = db_read($params{'FLIGHT'}) };
    $@ ? undef :
    [@{$info}{'FLIGHT', 'AIRCRAFT', 'FROM', 'TO'}, $info->{'ROUTE'}]
}

sub add_route {
    my ($flight, @points, $info) = @_;

    eval { $info = db_read($flight) };
    return undef if $@;

    $info->{'ROUTE'} = [map { _pack_point(@$_) } @points];

    eval { db_write($flight, $info) } &&
    eval { _store_pts($info->{'ROUTE'}) }
}

sub view_route {
    my (%params, $info) = @_;

    eval { $info = db_read($params{'FLIGHT'}) };
    $@ ? undef : [@{$info->{'ROUTE'}}[$params{'START'}..$params{'END'}]]
}

sub adref { return ref $_ ? @$_ : $_ for shift }

sub _find_nearest {
    my ($p, $r) = @_;
    my ($dist, $near, $name, $pts) = $r;

    eval { $pts = db_read('POINTS') } || return undef;

    for my $n (keys %$pts) {
        for (adref($pts->{$n})) {
            my $d = _dist($p, $_);
            ($near, $dist, $name) = ($_, $d, $n) if $d < $dist;
        }
    }

    $near ? [$name, $near] : undef;
}

sub _round_coords {
    map { sprintf "%.4f", $_ } @_
}

sub make_route {
    my %params = @_;

    my @from = _unpack_point($params{'FROM'});
    $from[1] += 360 if $from[1] < 0;
    my @to = _unpack_point($params{'TO'});
    $to[1] += 360 if $to[1] < 0;

    my $dist = _dist(@params{'FROM', 'TO'});
    my $n = int($dist/1000) + 1;
    my $delta = $dist/$n/2.;

    my @imp;
    my ($dw, $dl) = (($to[0] - $from[0])/$n, ($to[1] - $from[1])/$n);
    for (1..($n - 1)) {
        my ($w, $l) = _round_coords($from[0] + $_*$dw, $from[1] + $_*$dl);
        $l -= 360 if $l > 180;

        my $p = _pack_point($w, $l);
        my $np = _find_nearest($p, $delta);

        if ($np) {
            push @imp, _pack_point(@$np);
        }
        else {
            my $name = join '', map { chr(65 + int(rand(25))) } 1..5;
            push @imp, _pack_point($name, $p);

            _store_pts([_pack_point($name, $p)]);
        }
    }

    \@imp
}

sub find_points {
    my $name = shift;
    my ($p, $r, $pts) = (&_pack_point, qr/^$name/i);

    eval { $pts = db_read('POINTS') } || return undef;

    [sort { _dist($p, $a->[1]) <=> _dist($p, $b->[1]) }
    grep { $_->[0] =~ $r }
    map {
        my $k = $_;
        map { [$k, $_] } adref($pts->{$k})
    } keys %$pts];
}

1;

