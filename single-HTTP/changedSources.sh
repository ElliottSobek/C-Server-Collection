#!/bin/bash

changedSources() {
	if [ "$#" -ne 4 ]; then
		echo "Usage:" `basename "$0"` "<src_dir> <src_ext> <other_dir> <other_ext>"
		exit 1
	fi
	local pathed_src_files=`find -O3 "$1" -type f -name *.$2 -printf "%p\n"`

	for pathed_src_file in $pathed_src_files; do
		local src_file=`basename $pathed_src_file`
		local pathed_obj_file=$3/${src_file%.$2}.$4

		if [ ! -f $pathed_obj_file ]; then
			echo $src_file
		elif [ $pathed_src_file -nt $pathed_obj_file ]; then
			echo $src_file
		fi
	done

	return 0
}

changedSources "$@"
