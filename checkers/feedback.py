#!/usr/bin/python3

import uuid
import requests as r

from httpchecker import *

GET = 'GET'
POST = 'POST'
PORT = 7654

class FeedbackChecker(HttpCheckerBase):
	def session(self, addr):
		s = r.Session()
		s.headers['User-Agent'] = 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:28.0) Gecko/20100101 Firefox/28.0'
		s.headers['Accept'] = '*/*'
		s.headers['Accept-Language'] = 'en-US,en;q=0.5'
		return s

	def url(self, addr, suffix):
		return 'http://{}:{}/{}'.format(addr, PORT, suffix)

	def parseresponse(self, response):
		try:
			if response.status_code != 200:
				raise r.exceptions.HTTPError('status code {}'.format(response.status_code))
			try:
				result = response.json()
				#self.debug(result)
				return result
			except ValueError:
				raise r.exceptions.HTTPError('failed to parse response')
		finally:
			response.close()

	def spost(self, s, addr, suffix, data = None):
		response = s.post(self.url(addr, suffix), json.dumps(data), timeout=5)
		return self.parseresponse(response)

	def sget(self, s, addr, suffix):
		response = s.get(self.url(addr, suffix), timeout=5)
		return self.parseresponse(response)

	def check(self, addr):
		s = self.session(addr)

		result = self.sget(s, addr, 'search?query=')
		return result and result.get('hits') >= 0

	def get(self, addr, flag_id, flag):
		s = self.session(addr)

		parts = flag_id.split(':', 3)

		user = {'login':parts[0], 'password':parts[1]}
		result = self.spost(s, addr, 'auth', user)
		if not result or result.get('error'):
			self.debug('login failed')
			return False

		self.debug(parts[2])
		result = self.sget(s, addr, 'search?query=' + parts[2])
		if not result or result.get('error'):
			self.debug('search failed')
			return False
		if result.get('hits') < 1:
			self.debug('too few "hits"')
			return False
		votes = result.get('votes')
		if not votes or len(votes) == 0:
			self.debug('votes is empty')
			return False
		title = votes[0].get('title')
		if not title:
			self.debug('no "title" field')
			return False
		if title.find(flag) < 0:
			self.debug('flag not found in "title"')
			return False
		return True

	def put(self, addr, flag_id, flag):
		s = self.session(addr)

		user = {'login':uuid.uuid4().hex, 'password':uuid.uuid4().hex}
		self.debug(user)

		result = self.spost(s, addr, 'register', user)
		if not result or result.get('error'):
			self.debug('registration failed')
			return False

		vote = {'title':flag_id + ' ' + flag, 'text':'text', 'type':'invisible'}
		result = self.spost(s, addr, 'put', vote)
		if not result or result.get('error'):
			self.debug('put failed')
			return False
		data = result.get('data')
		if not data:
			self.debug('no "data" field')
			return False
		new_flag_id = data.strip()
		if not new_flag_id:
			self.debug('flag_id is empty')
			return False
		print('{}:{}:{}'.format(user['login'], user['password'], new_flag_id))
		return True

FeedbackChecker().run()