#!/bin/sh
TMP_FILE=$(mktemp --suffix=.7z)

curl -L "https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/rubenvb/old/4.7.0-1/i686-w64-mingw32-gcc-4.7.0-1_rubenvb.7z/download" -o "$TMP_FILE"

7z x "$TMP_FILE"

rm "$TMP_FILE"