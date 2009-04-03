#!/usr/bin/perl
use strict;
use warnings;

use Test::More tests => 34;

local $/ = '';
my @states = qx( ./reversi-demo )
  or die 'running reversi-demo failed';

is shift @states, $_ while <DATA>;

__DATA__
......
......
..ox..
..xo..
......
...... - x to move

best branch: 6 (ply 3 search; 48 states visited)
......
......
..ox..
..xx..
...x..
...... - o to move

best branch: 1 (ply 3 search; 45 states visited)
......
......
..ox..
..ox..
..ox..
...... - x to move

best branch: 7 (ply 3 search; 89 states visited)
......
......
..ox..
.xxx..
..ox..
...... - o to move

best branch: 0 (ply 3 search; 87 states visited)
......
......
..ooo.
.xxo..
..ox..
...... - x to move

best branch: 8 (ply 3 search; 135 states visited)
......
...x..
..xxo.
.xxx..
..ox..
...... - o to move

best branch: 3 (ply 3 search; 122 states visited)
......
...x..
.oooo.
.xxx..
..ox..
...... - x to move

best branch: 6 (ply 3 search; 257 states visited)
......
...xx.
.ooxo.
.xxx..
..ox..
...... - o to move

best branch: 5 (ply 3 search; 123 states visited)
......
...xx.
.ooxo.
.xox..
..oo..
....o. - x to move

best branch: 4 (ply 3 search; 253 states visited)
......
...xx.
.ooxo.
.xox..
..xx..
...xo. - o to move

best branch: 8 (ply 3 search; 163 states visited)
......
...xx.
.ooxo.
.xox..
..ox..
..ooo. - x to move

best branch: 0 (ply 3 search; 251 states visited)
......
...xx.
.ooxo.
.xox..
..xx..
.xooo. - o to move

best branch: 8 (ply 3 search; 155 states visited)
......
...xx.
.ooxo.
.xox..
..xx..
ooooo. - x to move

best branch: -1 (ply 3 search; 330 states visited)
......
...xx.
.ooxxx
.xox..
..xx..
ooooo. - o to move

best branch: 11 (ply 3 search; 223 states visited)
.....o
...xo.
.oooxx
.xox..
..xx..
ooooo. - x to move

best branch: -2 (ply 3 search; 198 states visited)
.....o
..xxo.
.oxoxx
.xxx..
..xx..
ooooo. - o to move

best branch: 12 (ply 3 search; 261 states visited)
.....o
..xxoo
.oxoox
.xxo..
..ox..
ooooo. - x to move

best branch: -4 (ply 3 search; 217 states visited)
...x.o
..xxxo
.oxoox
.xxo..
..ox..
ooooo. - o to move

best branch: 14 (ply 3 search; 258 states visited)
...xoo
..xxoo
.oxoox
.xxo..
..ox..
ooooo. - x to move

best branch: -7 (ply 3 search; 181 states visited)
...xoo
..xxoo
.oxoxx
.xxo.x
..ox..
ooooo. - o to move

best branch: 16 (ply 3 search; 118 states visited)
...xoo
..xxoo
.oxoxo
.xxo.o
..ox.o
ooooo. - x to move

best branch: -11 (ply 3 search; 141 states visited)
...xoo
..xxoo
.oxxxo
.xxxxo
..ox.o
ooooo. - o to move

best branch: 18 (ply 3 search; 94 states visited)
...xoo
..xxoo
.oxxoo
.xxxoo
..oooo
ooooo. - x to move

best branch: -10 (ply 3 search; 36 states visited)
...xoo
..xxoo
.oxxoo
.xxxoo
..ooxo
ooooox - o to move

best branch: 17 (ply 3 search; 70 states visited)
...xoo
..xxoo
.oxxoo
oooooo
..ooxo
ooooox - x to move

best branch: -11 (ply 3 search; 34 states visited)
...xoo
..xxoo
xxxxoo
oooooo
..ooxo
ooooox - o to move

best branch: 20 (ply 3 search; 52 states visited)
.o.xoo
..oxoo
xxxooo
oooooo
..ooxo
ooooox - x to move

best branch: -12 (ply 3 search; 34 states visited)
.oxxoo
..xxoo
xxxooo
oooooo
..ooxo
ooooox - o to move

best branch: 21 (ply 3 search; 17 states visited)
.oxxoo
o.xxoo
ooxooo
oooooo
..ooxo
ooooox - x to move

best branch: -9 (ply 3 search; 15 states visited)
.oxxoo
o.xxoo
ooxooo
oxoooo
x.ooxo
ooooox - o to move

best branch: 2147483637 (ply 3 search; 4 states visited)
.oxxoo
o.xxoo
ooxooo
oooooo
xoooxo
ooooox - x to move

only one move possible (skipping search)
xxxxoo
x.xxoo
xoxooo
xooooo
xoooxo
ooooox - o to move

only one move possible (skipping search)
xxxxoo
xooooo
xooooo
xooooo
xoooxo
ooooox - x to move

player 1 lost
