package Monitor::Main;
use Mojo::Base 'Mojolicious::Controller';

sub index {
  my $self = shift;

  # $self->stash(services => $self->_service_status);
  $self->render;
}

sub flags {
  my $self = shift;

  $self->stash(flags    => $self->_flags);
  $self->stash(services => $self->_services);
  $self->stash(teams    => $self->_teams);
  $self->stash(round    => $self->_round);
  $self->render;
}

sub _service_status {
  my $self = shift;
  my $services;
  my $db = $self->db;

  my $ss = $self->app->cache->{ss};
  if ($ss && $ss->{expires} > time) {
    $services = $ss->{data};
  } else {
    my $stm = $db->prepare('SELECT team_id, service, status, fail_comment FROM service_status;');
    $stm->execute() or $self->app->log->warn("SQL error [$DBI::err]: $DBI::errstr");
    while (my $row = $stm->fetchrow_hashref()) {
      $services->{$row->{service}}{$row->{team_id}} = $row;
    }

    my $cache = $self->app->cache;
    $cache->{ss} = {expires => 60 + time, data => $services};
    $self->app->cache($cache);
  }

  return $services;
}

sub _flags {
  my $self = shift;
  my $flags;
  my $db = $self->db;

  my $f = $self->app->cache->{f};
  if ($f && $f->{expires} > time) {
    $flags = $f->{data};
  } else {
    my $stm = $db->prepare('SELECT * FROM services_flags_stolen score;');
    $stm->execute() or $self->app->log->warn("SQL error [$DBI::err]: $DBI::errstr");
    while (my $row = $stm->fetchrow_hashref()) {
      $flags->{$row->{team}}{$row->{service}} = $row->{flags};
    }

    my $cache = $self->app->cache;
    $cache->{f} = {expires => 60 + time, data => $flags};
    $self->app->cache($cache);
  }

  return $flags;
}

1;
