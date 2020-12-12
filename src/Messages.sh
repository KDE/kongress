#! /usr/bin/env bash
# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
# SPDX-License-Identifier: BSD-2-Clause

$XGETTEXT `find . -name \*.qml -o -name \*.cpp -o -name \*.h` -o $podir/kongress.pot
rm -f rc.cpp

