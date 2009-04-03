#!/usr/bin/perl
use strict;
use warnings;

use Test::More tests => 3;

local $/ = "\n\n";
my @states = qx( ./nim/trace ) 
  or die 'running trace helper failed';

is shift @states, $_ while <DATA>;

__DATA__
   leaf state: 1
    leaf state: 1
   a/b: -1/1
  a/b: 1/2147483638
 a/b: -1/2147483637 (visited: 4)
   leaf state: 1
    leaf state: 1
   a/b: -1/1
  a/b: 1/1 (1 branch skipped)
 a/b: -1/2147483637 (visited: 8)
    leaf state: 1
   a/b: -1/2147483637
  a/b: 1/1 (2 branches skipped)
 a/b: -1/2147483637 (visited: 11)
best branch: -1 (ply 3 search; 11 states visited)

  leaf state: 1
 a/b: -1/2147483637 (visited: 1)
   leaf state: 1
  a/b: -1/1
 a/b: 1/2147483637 (visited: 3)
best branch: 1 (ply 3 search; 3 states visited)

only one move possible (skipping search)

