#!/bin/sh
if ! command -v 7z >/dev/null 2>&1 || ! command -v curl >/dev/null 2>&1
then
    echo "This script requires curl and 7z to function but they could not be found!"
    exit 1
fi

TMP_FILE=$(mktemp --suffix=.7z)

curl -L "https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/rubenvb/old/4.7.0-1/i686-w64-mingw32-gcc-4.7.0-1_rubenvb.7z/download" -o "$TMP_FILE"

7z x "$TMP_FILE"
rm "$TMP_FILE"
