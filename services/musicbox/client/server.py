#!/usr/bin/python

import json
from io import BytesIO
from subprocess import Popen, PIPE
from bottle import get, post, request, run, static_file, response
from client import *
from uuid import UUID

main_page = open('templates/index.html', 'rt').read()
fail_page = open('templates/fail.html', 'rt').read()
project_path = '.'

musicbox_location = '127.0.0.1'
musicbox_port = 4242

@get('/static/<path:path>')
def callback(path):
	return static_file(path, root=project_path + '/static/')

@get('/')
def index():
	return main_page

@post('/get')
def get_song_form_handler():
	try:
		data = get_song(musicbox_location, musicbox_port, UUID(request.forms.get('uuid').strip()))
		response.content_type = "audio/ogg"
		response.add_header('Content-Disposition', 'attachment; filename=song.ogg;')
		return data
	except Exception as e:
		print(e)
		return fail_page

@post('/put')
def put_song_form_handler():
	try:
		song_data = request.files.get('input_file')
		uuid = put_song(musicbox_location, musicbox_port, song_data.file.read())
		return json.dumps({ 'status' : 'success', 'uuid' : str(uuid) })
	except Exception as e:
		print(e)
		return json.dumps({ 'status' : 'fail' })


run(host='localhost', port=8080)