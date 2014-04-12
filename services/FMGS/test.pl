#!/usr/bin/perl

use MCDU qw/set_title set_status put_line draw/;

set_title('A320-211');
set_status('SCRATCHPAD');
put_line(1, 'L', 'PROSTO', '20.84 31.99');
put_line(2, 'L', 'ACTIVE#AKIDU', '30.84 31');
put_line(3, 'L', 'WEFIKA', '9.84 32.08');
put_line(5, 'L', 'SELECTABLE#<ERASE');
put_line(6, 'L', 'SELECTABLE#<BACK');
put_line($_, 'R', 'SELECTABLE#*CHANGE') for 1..3;

system "clear";
print draw;
