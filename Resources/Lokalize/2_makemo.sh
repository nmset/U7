#!/bin/bash
if [ ! -x 2_makemo.sh ]
then
    echo "Wrong working directory. Please cd to the directory containing this script."
    exit 0
fi

if [ -z $1 ];
then
    echo "\$1: domain; example: fr"
    exit 0
fi

APPNAME=utf8util
DOMAIN="$1"

msgfmt $DOMAIN/$APPNAME.po -o $DOMAIN/$APPNAME.mo

# cp $DOMAIN/$APPNAME.mo /usr/local/share/locale/$DOMAIN/LC_MESSAGES/

exit 0

