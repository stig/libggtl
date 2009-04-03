#!/usr/bin/perl
use strict;
use warnings;

use Test::More tests => 9;

my $state = `./30reversi_move`;
my @states = map { "$_\n" } split /\n\n/, $state;

is( shift @states, <<"EOF" );
......
......
..ox..
..xo..
......
...... - x to move
EOF

is( shift @states, <<"EOF", "flip up" );
......
..x...
..xx..
..xo..
......
...... - o to move
EOF


is( shift @states, <<"EOF", "flip left" );
......
..x...
..xx..
.ooo..
......
...... - x to move
EOF

is( shift @states, <<"EOF", "flip down" );
......
..x...
..xx..
.oox..
...x..
...... - o to move
EOF

is( shift @states, <<"EOF", "flip right" );
......
..x...
..xx..
.oooo.
...x..
...... - x to move
EOF

# now for diagonals

is( shift @states, <<"EOF", "flip down & left" );
......
..x...
..xx..
.xooo.
x..x..
...... - o to move
EOF

is( shift @states, <<"EOF", "flip up & right" );
......
..x.o.
..xo..
.xooo.
x..x..
...... - x to move
EOF

is( shift @states, <<"EOF", "flip down & right" );
......
..x.o.
..xo..
.xoxo.
x..xx.
...... - o to move
EOF

is( shift @states, <<"EOF", "flip up & left" );
.o....
..o.o.
..xo..
.xoxo.
x..xx.
...... - x to move
EOF

