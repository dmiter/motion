#!/bin/sh

VERSION=`git show -s --format=%H`
echo -n "Git-$VERSION"
