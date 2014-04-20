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
  my $f = $self->app->fly;
  $f->{progress} = time() - localtime($self->app->round->{time})->epoch;
  $self->render(json => $f);
}

1;
