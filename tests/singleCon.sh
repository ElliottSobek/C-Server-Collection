#!/bin/bash

singleConnectionTest() {
	if [ $# -ne 2 ]; then
		echo "Usage:" `basename "$0"` "<dir_name> <integer>"
		exit 1
	fi
	local cdw=$PWD

	cd $1
	for ((i = 0; i < $2; i++)); do
		while IFS='' read -r line || [[ -n "$line" ]]; do
    		echo $line | nc localhost 8888
		done < getQuery.txt
	done
	IFS=' '

	cd $cdw
	return 0
}

singleConnectionTest "$@"
