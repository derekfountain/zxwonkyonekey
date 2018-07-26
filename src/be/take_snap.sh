#!/bin/sh

# Collect trace information - the dump, assumed to be dump.ss, plus the
# sym and berc files. I need to do better than this, but for now this
# is better than working out the archive line each time. :)

tar zcf issue.tar.gz dump.ss be/*.inc be/*.berc wonky.sym

# Unpack with:
#  cd tmp
#  mkdir issue
#  cd issue
# tar xf issue.tar.gz
#
# Then:
#  newbe -i be/wonkyonekey.berc -y wonky.sym dump.ss@0