package Monitor::Main;
use Mojo::Base 'Mojolicious::Controller';

sub index {
  shift->render;
}

sub flags {
  my $self = shift;

  $self->render(json => $self->app->flags);
}

sub history {
  my $self = shift;

  $self->render(json => $self->app->history);
}

1;
