#!/usr/bin/python

from socket import *
import sys
import struct
from uuid import UUID

READING_CHUNK_SIZE = 32 * 1024 - 1

CHUNK_SIZE = 32 * 1024 - 1

PUT_TYPE = 0
GET_TYPE = 1

def show_help():
	print("%s <host> <port> put <filename>" % sys.argv[0])
	print("%s <host> <port> get <uuid>"     % sys.argv[0])

def read_binary_file(filename):
	result = b''
	with open(filename, "rb") as f:
		while True:
			next_bytes = f.read(READING_CHUNK_SIZE)
			if not next_bytes:
				break
			result += next_bytes
	return result

def encode_chunk(data):
	assert len(data) <= CHUNK_SIZE
	return struct.pack("h", len(data)) + data

def data_to_chunk_sequenece(data):
	result = b''
	for i in range((len(data) + CHUNK_SIZE - 1) // CHUNK_SIZE):
		result += encode_chunk(data[ i * CHUNK_SIZE : i * CHUNK_SIZE + CHUNK_SIZE])
	return result + encode_chunk(b'')

def data_from_chunk_sequence(chunks):
	decoded_len = 0
	result = b''
	while decoded_len < len(chunks):
		chunk_len = struct.unpack("h", chunks[decoded_len : decoded_len + 2])[0]
		result += chunks[decoded_len + 2 : decoded_len + 2 + chunk_len]
		decoded_len += 2 + chunk_len
	return result

def get_response_from_socket(sock):
	result = b''
	while True:
		next_bytes = sock.read(READING_CHUNK_SIZE)
		if not next_bytes:
			break
		result += next_bytes
	return result

if len(sys.argv) < 5:
	show_help()
	exit(1)

progname, host, port, action, arg = sys.argv
port = int(port)

print("Connecting to %s:%d" % (host, port))

s = socket(AF_INET, SOCK_STREAM)
s.connect((host,port))

if action == "get":
	s.sendall(struct.pack('B', GET_TYPE))
	uuid = UUID(arg)
	s.sendall(uuid.bytes)
	status = struct.unpack('B', s.recv(1))[0]
	if status == 0:
		chunks = get_response_from_socket(s)
		data = data_from_chunk_sequence(chunks)
		sys.stdout.write(data)
	else:
		print('Error =(')
elif action == "put":
	s.sendall(struct.pack('B', PUT_TYPE))
	s.sendall(struct.pack('H', 60 * 10))
	data = read_binary_file(arg)
	chunks = data_to_chunk_sequenece(data)
	s.sendall(chunks)
	response = struct.unpack('B', s.recv(1))[0]
	if response == 0:
		print('Success!')
		uuid = UUID(bytes=s.recv(16))
		print('UUID: %s' % str(uuid))
	else:
		print('Error =(')
else:
	show_help()
	exit(1)

s.close()