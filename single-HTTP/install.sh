#!/bin/bash

install () {
	if [ "$#" -ne 0 ]; then
		echo "Must be root to run"
		exit 1
	fi
	install -m 0755 single-HTTP /usr/bin/
	install -g 0 -o 0 -m 0644 single-HTTP.man /usr/share/man/man8/
	gzip --synchronous --best /usr/share/man/man8/single-HTTP.man
	mandb

	return 0
}

install "$@"
