#!/usr/bin/python

import random
import socket
import re
import os
import string
from checker import *
from glob import glob
from uuid import UUID
from struct import pack, unpack

from mutagen.oggvorbis import OggVorbis

CHUNK_SIZE = 32 * 1024 - 1

REQUEST_PUT = 0
REQUEST_GET = 1

RESPONSE_OK    = 0
RESPONSE_ERROR = 1

SERVICE_PORT = 4242

SOCKET_TIMEOUT = 2

FLAG_TAG_NAME = 'FLAG'
CHECK_TTL = 60
FLAG_TTL = 90 * 60

MUSIC_DIRECTORY = 'music'

TEMP_FILE = '.musicbox.temp'

class MusicboxChecker(CheckerBase):
	def init_data(self):
		self.data = dict()
		self.data['played'] = []
		self.data['planted'] = dict()

	def get_avaliable_songs(self):
		return glob('%s/*' % MUSIC_DIRECTORY)

	def pick_song(self):
		songs = self.get_avaliable_songs()

		if len(songs) == 0:
			self.debug('No songs to use')
			exit(EXITCODE_CHECKER_ERROR)

		not_played_songs = [ track for track in songs if not track in self.data['played'] ]

		if len(not_played_songs) == 0:
			self.data['played'] = []
			not_played_songs = songs

		chosen = random.choice(not_played_songs)
		self.data['played'].append(chosen)

		return chosen

	def send_chunked_data(self, sock, data):
		for i in range((len(data) + CHUNK_SIZE - 1) / CHUNK_SIZE):
			chunk_offset = i * CHUNK_SIZE
			chunk_length = min(CHUNK_SIZE, len(data) - chunk_offset)
			sock.sendall(pack('h', chunk_length))
			sock.sendall(data[chunk_offset: chunk_offset + chunk_length])
		sock.sendall(pack('h', 0))

	def recv_chunked_data(self, sock):
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

	def put_song(self, sock, data):
		self.debug('Putting song...')

		sock.sendall(pack('B', REQUEST_PUT))
		sock.sendall(pack('h', FLAG_TTL))
		self.send_chunked_data(sock, data)

		self.debug('All data sent')

		status = unpack('B', sock.recv(1))[0]

		if status == RESPONSE_ERROR:
			return None

		uuid = UUID(bytes=sock.recv(16))

		self.debug('Uuid: %s' % str(uuid))

		return uuid

	def get_song(self, sock, uuid):
		self.debug('Getting song...')

		sock.sendall(pack('B', REQUEST_GET))
		sock.sendall(uuid.bytes)

		self.debug("All data sent")

		status = unpack('B', sock.recv(1))[0]

		self.debug("Response: %d" % status)

		if status == RESPONSE_ERROR:
			return None
		return self.recv_chunked_data(sock)

	def create_socket(self, addr, port):
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.settimeout(SOCKET_TIMEOUT)
		sock.connect((addr, port))
		return sock

	def check(self, addr):
		flag = ''.join([ random.choice(string.ascii_uppercase + string.digits) for e in range(31) ]) + '='
		global FLAG_TTL
		old_ttl = FLAG_TTL
		try:
			FLAG_TTL = CHECK_TTL

			if not self.put(addr, None, flag):
				return False
			return self.get(addr, None, flag)
		finally:
			FLAG_TTL = old_ttl

	def get(self, addr, flag_id, flag):
		sock = self.create_socket(addr, SERVICE_PORT)

		if not flag in self.data['planted']:
			self.debug("Didn't planted this flag: %s" % flag)
			exit(EXITCODE_CHECKER_ERROR)

		uuid = UUID(self.data['planted'][flag])
		song_data = self.get_song(sock, uuid)

		if not song_data:
			return False

		try:
			with open(TEMP_FILE, 'wb') as f:
				f.write(song_data)
			ogg_file = OggVorbis(TEMP_FILE)
			return flag in ogg_file[FLAG_TAG_NAME]
		finally:
			os.remove(TEMP_FILE)

	def put(self, addr, flag_id, flag):
		sock = self.create_socket(addr, SERVICE_PORT)

		song = self.pick_song()

		ogg_file = OggVorbis(song)
		ogg_file[FLAG_TAG_NAME] = flag
		ogg_file.save()

		try:
			with open(song, 'rb') as f:
				song_data = f.read()
			uuid = self.put_song(sock, song_data)
			if not uuid:
				return False
			self.data['planted'][flag] = str(uuid)
			return True
		finally:
			del ogg_file[FLAG_TAG_NAME]
			ogg_file.save()

MusicboxChecker().run()