#!/bin/sh

set -x
aclocal || exit 1
libtoolize --force || glibtoolize --force || exit 1
automake --add-missing || exit 1
autoconf || exit 1
