#!/bin/sh

if [ "$#" -eq 0 ]; then
	echo "usage: rpath.sh executable" 1>&2
	exit 1
fi

otool -L "$1" | grep -i "SDL2" | while read -r l; do
	lib=$(echo $l | awk '{ print $1 }')

	cp "$lib" $(dirname "$1")/../Frameworks
	install_name_tool -change \
		"$lib" \
		"@executable_path/../Frameworks/$(basename $lib)" \
		"$1"
done
