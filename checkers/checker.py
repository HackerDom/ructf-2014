#!/usr/bin/python

import sys
import socket
import json
import os.path
import traceback

EXITCODE_OK            = 101
EXITCODE_CORRUPT       = 102
EXITCODE_MUMBLE        = 103
EXITCODE_DOWN          = 104
EXITCODE_CHECKER_ERROR = 110

class CheckerBase(object):
	def check(self, addr):
		pass

	def get(self, addr, flag_id, flag):
		pass

	def put(self, addr, flag_id, flag):
		pass

	def debug(self, msg):
		sys.stderr.write('%s\n' % msg)

	def run(self):
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
		except socket.error as e:
			self.debug(e)
			if 'errno' in dir(e) and e.errno == 111:
				exit(EXITCODE_DOWN)
			exit(EXITCODE_MUMBLE)
		except Exception as e:
			self.debug('Error during execution')
			traceback.print_exc(100, sys.stderr)
			self.debug(e)
			exit(EXITCODE_CHECKER_ERROR)

