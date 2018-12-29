#!/bin/bash

if [ -z "$1" ] || [ ! -f "$1" ]; then
	echo "Usage: `basename $0` /path/to/leap-motion.sdk.tgz"
	exit
fi

file="$1"

version="`echo "$file" | egrep -o '[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]*'`"
sdkos="Linux"
if [ -n "`echo "$file" | grep -i mac`" ]; then
	sdkos="Mac"
fi

echo "Extracting $sdkos sdk v${version}"

tmpdir=`mktemp -d`
echo -n "  Unpacking..."
tar xfz "$file" -C "$tmpdir"
echo "done."

rm -rf LeapSDK/$sdkos
mkdir -p LeapSDK/{$sdkos,include}
echo "$version" > LeapSDK/version.${sdkos}.txt
echo -n "  Updating includes..."
cp -r "$tmpdir"/*/LeapSDK/include/* LeapSDK/include
echo "done."
echo -n "  Adding sdk files..."
mv "$tmpdir"/*/LeapSDK/* LeapSDK/$sdkos/
mv "$tmpdir"/*/* LeapSDK/$sdkos/
echo "done."

rm -rf "$tmpdir"

echo "Extraction complete."