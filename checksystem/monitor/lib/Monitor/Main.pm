package Monitor::Main;
use Mojo::Base 'Mojolicious::Controller';

sub index {
  shift->render;
}

sub flags {
  shift->render;
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
