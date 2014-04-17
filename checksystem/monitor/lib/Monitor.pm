package Monitor;
use Mojo::Base 'Mojolicious';

has services   => sub { {} };
has teams      => sub { {} };
has scoreboard => sub { [] };
has round      => sub { {} };
has status     => sub { {} };
has flags      => sub { {} };
has history    => sub { [] };

sub startup {
  my $self = shift;

  my $mode = $self->mode;
  $self->plugin('Config', file => "monitor.$mode.conf");

  $self->plugin(
    'PgAsync',
    dbi => [
      $self->config->{db}{source}, $self->config->{db}{user},
      $self->config->{db}{pass}, {AutoCommit => 1, RaiseError => 1, pg_enable_utf8 => 1}]);

  my $r = $self->routes;
  $r->get('/')->to('main#index')->name('index');
  $r->get('/flags')->to('main#flags')->name('flags');
  $r->get('/history')->to('main#history')->name('history');

  $self->pg(
    'SELECT id, name FROM services;',
    sub {
      my $db = shift;

      $self->log->info('Fetch services at startup');
      while (my $row = $db->sth->fetchrow_hashref()) {
        $self->services->{$row->{id}} = $row->{name};
      }
    });

  $self->pg(
    'SELECT id, name, vuln_box FROM teams;',
    sub {
      my $db = shift;

      $self->log->info('Fetch teams at startup');
      while (my $row = $db->sth->fetchrow_hashref()) {
        $self->teams->{$row->{id}} = $row;
      }
    });

  Mojo::IOLoop->recurring(
    5 => sub {
      $self->log->info('Update scoreboard');
      Mojo::IOLoop->delay(
        sub {
          my $delay = shift;

          $self->log->info('Fetch SLA and FP for all team');
          $self->pg(
            'SELECT DISTINCT ON (team_id, service_id) team_id, service_id, score
            FROM score ORDER BY team_id, service_id, time DESC;'
              => $delay->begin
          );
          $self->pg(
            'SELECT DISTINCT ON (team_id, service_id) team_id, service_id, successed, failed
            FROM sla ORDER BY team_id, service_id, time DESC;'
              => $delay->begin
          );
          $self->pg(
            'SELECT n, EXTRACT(EPOCH FROM time) AS time
            FROM rounds ORDER BY n DESC LIMIT 1;'
              => $delay->begin
          );
          $self->pg(
            'SELECT team_id, service_id, status FROM service_status;'
              => $delay->begin
          );
          $self->pg(
            'SELECT * FROM services_flags_stolen;'
              => $delay->begin
          );
          $self->pg(
            'SELECT * FROM points_history
            ORDER BY team_id, round'
              => $delay->begin
          );
        },
        sub {
          my ($delay, $fh, $sh, $rh, $ssh, $flh, $ph) = @_;
          my ($flag_points, $sla_points);

          while (my $row = $sh->sth->fetchrow_hashref()) {
            my ($sla, $sum) = (1, $row->{successed} + $row->{failed});
            $sla = $row->{successed} / $sum if $sum > 0;
            $sla_points->{$row->{team_id}}{$row->{service_id}} = $sla;
          }
          while (my $row = $fh->sth->fetchrow_hashref) {
            $flag_points->{$row->{team_id}}{$row->{service_id}} = $row->{score};
          }

          my @data;
          for my $tid (keys %{$self->teams}) {

            my $score = 0;
            $score += $sla_points->{$tid}{$_} * $flag_points->{$tid}{$_}
              for keys %{$self->services};

            push @data, {
              team => {
                id       => $tid,
                name     => $self->teams->{$tid}{name},
                vuln_box => $self->teams->{$tid}{vuln_box}
              },
              sla   => $sla_points->{$tid},
              fp    => $flag_points->{$tid},
              score => $score
              };
          }

          @data = sort { $b->{score} <=> $a->{score} } @data;
          $self->scoreboard(\@data);

          my $row = $rh->sth->fetchrow_hashref;
          $self->round({n => $row->{n}, time => scalar localtime int $row->{time}});

          while (my $row = $ssh->sth->fetchrow_hashref()) {
            $self->app->status->{$row->{service_id}}{$row->{team_id}} = $row;
          }

          while (my $row = $flh->sth->fetchrow_hashref()) {
            $self->app->flags->{$row->{team_id}}{$row->{service}} = $row->{flags};
          }

          my ($h, $nh);
          while (my $row = $ph->sth->fetchrow_hashref()) {
            push @{$h->{$row->{name}}}, {x => $row->{round}, y => 0 + $row->{points}};
          }
          for (keys %$h) {
            push @$nh, {name => $_, data => $h->{$_}};
          }
          $self->app->history($nh);
        });
    });
}

1;
