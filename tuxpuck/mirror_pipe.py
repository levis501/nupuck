#!/usr/bin/env python

"""Dummy socket server.  repeats input to output"""

import socket
import os

NAME="/tmp/nupuck.socket"
sock = socket.socket( socket.AF_UNIX, socket.SOCK_STREAM)
try:
  os.remove(NAME)
except OSError:
  pass
sock.bind(NAME)
sock.listen(1)
while True:
  conn, addr = sock.accept()
  while 1:
    data = conn.recv(1024)
    if not data:
      break
    conn.send(data)
