from urllib.request import urlopen
import html.parser
import re
import subprocess

htmlParser = html.parser.HTMLParser()

OUTPUT_DIRECTORY = 'music/'

RESOURCE_URL = 'http://www.fakemusicgenerator.com/'

GRAB_URL_REGEX = r'(/download\.php\?artist=([+a-zA-Z]+)&track=([+a-zA-Z]+)&type=midi)'

def get_music_list():
    page = urlopen(RESOURCE_URL)
    text = page.read().decode('utf-8')
    text = htmlParser.unescape(text)
    return re.findall(GRAB_URL_REGEX, text)

playlist = get_music_list()
for song in playlist:
    print(song)
    response = urlopen(RESOURCE_URL + song[0])
    subprocess.getoutput("wget '%s' -O '%s%s.midi'"
                         % (RESOURCE_URL + song[0],
                            OUTPUT_DIRECTORY,
                            song[1].replace('+', '_') + song[2].replace('+', '_')))
    break

