#!/bin/bash

getPaths() {
	if [ "$#" -lt 2 ]; then
		echo "Usage:" `basename "$0"` "<file_ext> <dir_names>"
		exit 1
	fi
	local extension=*."$1"

	shift

	local relative_wds=()

	for arg in "$@"; do

		if [ -d $arg ]; then
			local dirlist=`find -O3 $arg -type f -name $extension -printf "%h\n" | uniq`

			for dir in $dirlist; do
				relative_wds+="$dir "
			done
		fi
	done

	echo $relative_wds

	return 0
}

getPaths "$@"
