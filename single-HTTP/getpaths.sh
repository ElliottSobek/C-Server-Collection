#!/bin/bash

getPaths() {
	if [ "$#" -lt 1 ]; then
		echo "Usage:" `basename "$0"` "<dir_names>"
		exit 1
	fi

	local relative_wds=()

	for arg in "$@"; do

		if [ -d $arg ]; then
			local dirlist=`find -O3 $arg -type f -name "*.c" -printf "%h\n" | uniq`

			for dir in $dirlist; do
				relative_wds+="$dir "
			done
		fi
	done

	echo $relative_wds

	return 0
}

getPaths "$@"
