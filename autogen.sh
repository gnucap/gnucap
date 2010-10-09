#! /bin/sh
#
# $Id$
#
# Run the various GNU autotools to bootstrap the build
# system.  Should only need to be done once.

# for now avoid using bash as not everyone has that installed
CONFIG_SHELL=/bin/sh
export CONFIG_SHELL

echo "Running aclocal..."
aclocal $ACLOCAL_FLAGS || exit 1

echo "Running autoheader..."
autoheader || exit 1

echo "Running automake..."
automake -a -c --gnu || exit 1

echo "Running autoconf..."
autoconf || exit 1

echo "not Running configure..."
##./configure $@ || exit 1


