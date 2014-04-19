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

sub fly_data {
  my $self = shift;
  $self->render(json => $self->app->fly);
}

1;
