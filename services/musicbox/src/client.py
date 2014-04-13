#!/usr/bin/python

from socket import *
import sys
import struct
from uuid import UUID

READING_CHUNK_SIZE = 32 * 1024

CHUNK_SIZE = 32 * 1024

REQUEST_DIRECTION = 0

GET_TYPE = 0
PUT_TYPE = 1

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

def encode_chunk(data, uuid, direction, chunk_type):
	assert len(data) <= CHUNK_SIZE
	return struct.pack("bbh", direction, chunk_type, len(data)) + uuid.bytes + data

def data_to_chunk_sequenece(data, uuid, direction, chunk_type):
	result = b''
	for i in range((len(data) + CHUNK_SIZE - 1) / CHUNK_SIZE):
		result += encode_chunk(data[ i * CHUNK_SIZE : i * CHUNK_SIZE + CHUNK_SIZE], uuid, direction, chunk_type)
	return result + encode_chunk(b'', uuid, direction, chunk_type)

def data_from_chunk_sequence(chunks):
	decoded_len = 0
	result = b''
	while decoded_len < len(chunks):
		chunk_len = struct.unpack("h", chunks[decoded_len + 2 : decoded_len + 4])[0]
		result += chunks[decoded_len + 20 : decoded_len + 20 + chunk_len]
		decoded_len += 20 + chunk_len
	return result

def decode_chunk(chunk):
	directon, chunk_type, chunk_len = struct.unpack("bbh", chunk[:4])
	uuid = UUID(bytes=chunk[4:20])
	return { 'directon' : directon, 'type' : chunk_type, 'len' : chunk_len, 'uuid' : uuid, 'data' : chunk[20:] }

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

host, port, action, arg = sys.argv
port = int(port)

print("Connecting to %s:%d" % (host, port))

s = socket(AF_INET, SOCK_STREAM)
s.connect((host,port))

if action == "get":
	request_chunk = encode_chunk(b'', uuid.UUID(arg), REQUEST_DIRECTION, GET_TYPE)
	s.sendall(request_chunk)
	chunks = get_response_from_socket(s)
	data = data_from_chunk_sequence(chunks)
	sys.stdout.write(data)
elif action == "put":
	data = read_binary_file(arg)
	chunks = data_to_chunk_sequenece(data, UUID(int = 0), REQUEST_DIRECTION, PUT_TYPE)
	s.sendall(chunks)
	response = get_response_from_socket(s)
	print(decode_chunk(response))
else:
	show_help()
	exit(1)

s.close()