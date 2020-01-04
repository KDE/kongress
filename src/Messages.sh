#! /usr/bin/env bash
$XGETTEXT `find . -name \*.qml -o -name \*.cpp -o -name \*.h` -o $podir/kdefosdem.pot
rm -f rc.cpp

