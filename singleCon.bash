function singleConnectionTest() {
	if [ $# -ne 2 ]; then
		echo "Usage: . singleConnectionTest <dir_name> <integer>"
		return 1
	fi
	local cdw=$PWD
	cd $1
	for ((i = 0; i < $2; i++)); do
		nc localhost 8888 < getQuery.txt
	done
	cd $cdw
	return 0
}

singleConnectionTest "$@"
