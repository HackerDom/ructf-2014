#!/usr/bin/python

import uuid

from httpchecker import *

GET = 'GET'
POST = 'POST'

class FeedbackChecker(HttpCheckerBase):
	def req(self, method, addr, url, data = None):
		conn = http.client.HTTPConnection(addr, 7654, 5)
		#conn.set_debuglevel(1)
		try:
			conn.request(method, url, data)
			response = conn.getresponse()
			result = response.read().decode('utf-8')
			self.debug('{} "{}" -> {} {}'.format(method, url, response.status, response.reason))
			return result
		finally:
			conn.close()

	def ajax(self, method, addr, url, data = None):
		return json.loads(self.req(method, addr, url, json.dumps(data)))

	def check(self, addr):
		result = self.ajax(GET, addr, '/search?query=')
		return result.get('hits') >= 0

	def get(self, addr, flag_id, flag):
		parts = flag_id.split(':', 3)

		user = {'login':parts[0], 'password':parts[1]}
		result = self.ajax(POST, addr, '/auth', user)
		if not result or result.get('error'):
			return False

		result = self.ajax(GET, addr, '/search?query=' + parts[2])
		if not result or result.get('error'):
			return False
		if result.get('hits') < 1:
			return False
		votes = result.get('votes')
		if not votes or len(votes) == 0:
			return False
		title = votes[0].get('title')
		if not title:
			return False
		return title.find(flag) >= 0

	def put(self, addr, flag_id, flag):
		user = {'login':uuid.uuid4().hex, 'password':uuid.uuid4().hex}
		self.debug(user)

		result = self.ajax(POST, addr, '/register', user)
		if not result or result.get('error'):
			return False

		vote = {'title':flag_id + ' ' + flag, 'text':'text', 'type':'invisible'}
		result = self.ajax(POST, addr, '/put', vote)
		if not result or result.get('error'):
			return False
		data = result.get('data')
		if not data:
			return False
		new_flag_id = data.strip()
		if not new_flag_id:
			return False
		print('{}:{}:{}'.format(user['login'], user['password'], new_flag_id))
		return True

FeedbackChecker().run()