#!/usr/bin/python3

import socket
import time

from random import randint

# how many total connections?
HOW_MANY = 100
port = 8091
NUM_CONCUR = 10

# you can modify this list with pairs of request/response pairs
# (request, response)
pairs = [
    ("GET /nothere", "404 Not Found"),
    ("GET /little.txt", "200 OK"),
    ("GET /big.txt", "403 Access Denied"),
    ("GT BENT BUEHLER", "400 Bad Request")
]

my_sockets = []

total_good = 0
total_bad = 0

red = "\033[91m"
gre = "\033[92m"
yel = "\033[93m"
c = "\033[0m"

def test_socket(sock, atuple):
    global total_good, total_bad
    sent = sock.send(bytes(atuple[0] + " HTTP/1.1", "utf-8"))

    for i in range(100):
        try:
            chunk = sock.recv(15)
            astr = chunk.decode("utf-8")

            if astr[:16] == "HTTP/1.1 " + atuple[1][:6]:
                print(gre + "good: " + c, end='')
                total_good += 1
            elif astr[:16] == "HTTP/1.0 " + atuple[1][:6]:
                print(gre + "good: " + c, end='')
                total_good += 1
            else:
                print(red + "bad -> " + c, end='')
                total_bad += 1
            print(yel + "requested:" + c, atuple[0], yel + "wanted: " + c, "'HTTP/1.* " + atuple[1][:6] + "'", yel + "got: " + c, "'" + astr + "'")
            return
        except socket.error as e:
            if e.args[0] == 11:
                time.sleep(0.05)
    print(red + "bad -> " + c, end='')
    print(yel + "requested: " + c, atuple[0], yel + "wanted: " + c, "'HTTP/1.* " + atuple[1][:6] + "'", yel + "got no reply." + c)
    total_bad += 1


def make_new_conn():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    s.connect(("localhost", port))
    s.setblocking(0)
    return s


def setup(count: int) -> None:
    for i in range(count):
       my_sockets.append(make_new_conn())

# let's pick a random socket

setup(NUM_CONCUR) # start with 10 processes

for i in range(HOW_MANY):
    rando = randint(0, len(my_sockets) - 1)

    test_socket(my_sockets[rando],pairs[randint(0, 3)])
    my_sockets[rando].close()
    my_sockets.pop(rando)
    my_sockets.append(make_new_conn())

while len(my_sockets) > 0:
    rando = randint(0, len(my_sockets) - 1)

    test_socket(my_sockets[rando],pairs[randint(0, 3)])
    my_sockets[rando].close()
    my_sockets.pop(rando)

print("\n" + gre + "good: ", total_good, c, red + "bad: ", total_bad,c);
print("Success %: ", total_good / (total_good + total_bad));
