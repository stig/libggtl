GGTL exists to make it simple to create strategic games in C. The AI provided by GGTL can play any two-player, zero-sum game with perfect information.


# Changelog #

## Version 2.1.3 ##

Released: Mar 26, 2006

The new getter/setter interface I introduced in 2.1.2 was, quite frankly, horrible. I've resorted to having explicit ```
ggtl_set_float()``` and ```
ggtl_get_float()``` functions for now. Maybe someday in the future I can think of a nicer interface for both. Sorry about the  interface breakage but since the interface I'm breaking has been out only for a week, I hope you'll forgive me.

Available for download <a href='/files/ggtl/'>here</a>.

## Version 2.1.2 ##

Released: Mar 20, 2006

After getting some input on <a href='/software/ggtl/'>GGTL</a> 2.x from <a href='http://www.bluedust.com/'>Steven Goodwin</a> I've ended up hacking on it a bit this weekend. He reported some minor stylistic and distribution issues and asked some interesting questions. One of them was if I had thought about using a floating-point value in seconds for the time to search in seconds.

I have indeed thought about this without getting around to it, but his prodding gave me the little push I needed. The old code (which is still supported) took an integer argument representing the number of milliseconds to search for. The new option uses a double-precision floating point value for the number of <em>seconds</em> to search for thus allowing both longer and shorter search times.

I (as did Steve) expect that the ability to do sub-millisecond granularity searches is a lot more useful than searches that last longer than ```
INT_MAX``` milliseconds...

Here's the relevant ChangeLog entry:

<ul>
<li>Allow sub-millisecond precision for iterative searches</li>
<li>Deprecated MSEC option in favour of the new TIME option; this<br>
takes a double-precision value in seconds, allowing for<br>
sub-millisecond precision.</li>
<li>Introduced new <code>ggtl_getval()</code> & <code>ggtl_setval()</code> functions capable<br>
of setting/getting double-precision values (needed for TIME).</li>
<li>Bumped library version number to 1:0:1</li>
<li>Don't require a config.h header; pass in options on commandline</li>
</ul>


## Version 1.99.2 ##

Released: Apr 30, 2005

I have made another pre-release of GGTL 2.

The main focus of this release is to correct the oversight that prohibited re-use of cached moves in the reversi extension. This caused an enormous memory usage, but has now been corrected. Other changes were:

<ul>
<li>Do not link ggtl with tap; use it only in the tests</li>
<li>improve reversi-demo and add manpage for it</li>
<li>make AI skip search when only 1 move is available</li>
<li>kill duplication across the various AIs</li>
<li>added tests for the tracing output</li>
<li>clean up tests a bit</li>
</ul>


## Version 1.99 ##

Released: Apr 3, 2005

This GGTL 2.0 preview release has been rewritten from the ground up to be more flexible, more extendible, and easier to use.

The provided Reversi and Nim extensions implement all the game-specific callback functions GGTL's AI needs. Using one of these extensions means you can have a game with a capable AI up and running in next to no time. Doing so incurs no penalty in flexibility, however--you can override any provided callback functions with your own.

Regretfully, backward compatibility with previous versions had to be sacrificed in order to meet the goals for this version. Versions 1.x and 2.x can both be installed at the same time, however, so updating programs to use the new version should be relatively simple.

Find more information at the <a href='http://brautaset.org/software/ggtl/'>GGTL homepage</a>.
