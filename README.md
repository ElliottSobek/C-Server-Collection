[![Generic badge](https://img.shields.io/badge/development%20status-in%20development-red.svg "Development Status")](https://shields.io/)

# C-Server-Collection

A collection of Servers written in C

Single threaded non concurrent: single-HTTP
Multithreaded concurrent: threaded-HTTP
Selecting concurrent: select-HTTP
Forking concurrent: fork-HTTP

# Project Note

Runs in a unix like environment.

These servers are build as a proof of concept and a challenge to myself. They can be used as a testing or production server with the appropriate modifications for your needs. An individual with intermediate knowledge of C and SQL is recommended.

## Development Requirements

* gcc
* libsqlite3-dev

## Building

If you are adding features or debugging a server run: `make [debug]`

If optimizing run: `make profile`

* An alternative would be to compile the source code using: `make [debug]` and use the OProfile program.

If using for production run: `make production`

### Options

* Dump the entire database or a specified table (-d)[table_name]
* Load a database fixture (-l) <filepath>
* Print help menu (-h)
* Print version number (-V)
* Print verbose output (-v)
* Set a configuration file to be used (-s) <filepath>
* Run the server as a specific effective user id (-u) <unsigned int>
* Run the server as a specific effective group id (-g) <unsigned int>

### Logging

All servers follow a rolling log file implementation where logs follow the structure of .../logs/year/month/week/day.log.

### Limitations

1. Given the servers are written in C, adding a path to the URL routing list structure requires the server to be recompiled and restarted.

2. Given the C languange is a non object oriented language the use of a true database ORM is not possible. An intermediate knowledge if the SQL language is recommended when using the database API.

3. These servers are custom build from the ground up and, as such, are not HTTP 1.1 or HTTP 1.0 compliant. With this said, the servers follow closely the HTTP 1.0 specification.

4. None of the servers are load balancing capable or implement connection timeouts.

## Contribution

Anyone can contribute and work on any available issues.

Keep the same format as is used. After, simply make a pull request and have
patience.
