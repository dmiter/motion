#!/bin/sh

VERSION="3.2.12"
COMMIT="0fb31d66c9bb5032403f831c7f3b6ae241938bcb"
CCOUNT=$(git rev-list $COMMIT..HEAD --count)
echo -n "$VERSION-dmiter-$CCOUNT"
