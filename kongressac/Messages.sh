#! /bin/sh
# SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
# SPDX-License-Identifier: BSD-2-Clause
$XGETTEXT `find . -name "*.cpp" -o -name "*.h" | grep -v '/tests/'` -o $podir/kongressac.pot
