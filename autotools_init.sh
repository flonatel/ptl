
# This must be called in the source dir for the first time
libtoolize --copy

aclocal
autoheader
automake --add-missing --copy
automake
autoconf

