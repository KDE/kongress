<!--
    SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
    SPDX-License-Identifier: CC-BY-SA-4.0
-->
# Kongress

Companion application for conference attendees

## Features

Kongress provides practical information about conferences. It supports conferences that offer their schedule in [iCalendar](https://tools.ietf.org/html/rfc5545) format. In Kongress, the data of the talks are shown in various ways, e.g. in daily views, by talk category, etc. The users can also create a list of favorite conference talks/events as well as they can navigate to the web page of each talk. A map of the conference venue, location information and link to OpenStreetMap can also be added.

![Desktop screenshot](https://cdn.kde.org/screenshots/kongress/desktop.png)

A set of conferences have been preloaded and are available after installation. Users may add extra conferences, providing a link to their iCalendar schedule (under development).

## Setup

### Build

```
git clone https://invent.kde.org/utilities/kongress
cd kongress
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Run

```
bin/kongress
```

*To simulate Plasma Mobile user experience:*

```
QT_QUICK_CONTROLS_MOBILE=true QT_QUICK_CONTROLS_STYLE=Plasma bin/kongress
```

### Install

```
sudo make install
```
