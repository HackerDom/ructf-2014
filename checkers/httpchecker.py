#!/usr/bin/python3

import sys
import socket
import json
import os.path
import traceback

import requests as r

EXITCODE_OK            = 101
EXITCODE_CORRUPT       = 102
EXITCODE_MUMBLE        = 103
EXITCODE_DOWN          = 104
EXITCODE_CHECKER_ERROR = 110

class HttpWebException(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

class HttpCheckerBase(object):
	def check(self, addr):
		pass

	def get(self, addr, flag_id, flag):
		pass

	def put(self, addr, flag_id, flag):
		pass

	def debug(self, msg):
		sys.stderr.write('%s\n' % msg)

	def run(self):
		self.debug(' '.join(sys.argv))

		if len(sys.argv) < 3:
			self.debug('Not enough arguments')
			exit(EXITCODE_CHECKER_ERROR)
		script_name = os.path.basename(os.sys.argv[0])
		command, addr = sys.argv[1:3]


		try:
			if command == 'check':
				if self.check(addr):
					exit(EXITCODE_OK)
				else:
					exit(EXITCODE_MUMBLE)

			if len(sys.argv) < 5:
				self.debug('Not enough arguments')
				exit(EXITCODE_CHECKER_ERROR)

			flag_id, flag = sys.argv[3:5]

			if command == 'get':
				if self.get(addr, flag_id, flag):
					exit(EXITCODE_OK)
				else:
					exit(EXITCODE_CORRUPT)

			if command == 'put':
				if self.put(addr, flag_id, flag):
					exit(EXITCODE_OK)
				else:
					exit(EXITCODE_MUMBLE)

			self.debug('Invalid command')
			exit(EXITCODE_CHECKER_ERROR)
		except HttpWebException as e:
			print('http status code {}'.format(e.value))
			exit(EXITCODE_MUMBLE)
		except (r.exceptions.ConnectionError, r.exceptions.Timeout) as e:
			self.debug(e)
			print('connection problem')
			exit(EXITCODE_DOWN)
		except (r.exceptions.HTTPError, r.exceptions.TooManyRedirects) as e:
			self.debug(e)
			print('invalid http response')
			exit(EXITCODE_MUMBLE)
		except socket.error as e:
			self.debug(e)
			if isinstance(e, socket.timeout) or 'errno' in dir(e) and e.errno == 111:
				print('connection problem')
				exit(EXITCODE_DOWN)
			print('socket error')
			exit(EXITCODE_MUMBLE)
		except Exception as e:
			self.debug('Error during execution')
			traceback.print_exc(100, sys.stderr)
			self.debug(e)
			exit(EXITCODE_CHECKER_ERROR)
