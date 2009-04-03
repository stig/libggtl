#!perl
use strict;
use warnings;

use Test::More tests => 11;

local $/ = '';
my @states = qx( ./ttt-demo )
  or die 'running ttt-demo failed';

is shift @states, $_ while <DATA>;

__DATA__
...
...
... - x to move

...
...
..x - o to move

...
...
o.x - x to move

...
.x.
o.x - o to move

o..
.x.
o.x - x to move

o..
xx.
o.x - o to move

o..
xxo
o.x - x to move

o..
xxo
oxx - o to move

oo.
xxo
oxx - x to move

oox
xxo
oxx - o to move

The only winning move is not to play.
