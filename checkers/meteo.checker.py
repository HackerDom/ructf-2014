#!/usr/bin/env python3

import sys
import socket
import requests
import random
import logging
import select
import time
from bs4 import BeautifulSoup

HTTP_PORT = 8888
TOWER_PORT = 9000

EXPONENTA = int('00c9412e2e11ce91e356328517eaa9e067de2dad20781dadb84a3b53abe7087967dd6cf0b8db5b61537868067102a89b54bfdd81e61eb5b32b3921a065246b49f5bbf6c93161876a04df8da7b52224d5b03aa74a0da6231a45e86336d6774a5b54810ae0cce1f6c35735aec02b27d6b6b69683bca0a1834bd8dcb16ad30a18d5a8976b82be0bb21f24d100da9d35716d374aaa92168bfe8683f1d855737aff5a07945424d4fa9e9ee9dde34c0b2f603c7c74105f83baa2f0decf6ef42d2246fe9857f29f9be4e52e20862f5f95e0b7251107339cdbf09ee530a3d6ecac375352d6c7d3af75a176248ef0b249ce7b06c66dac723fde75b6aad9540004f7f03b4fe1', 16)
MODULUS =  int('D12FBA8D63EA1F6ABFE3C989FEBD928361627ADE5AC6F878ED52613ED03076AFFA5E27BF7A0004DE149B7332D239737184ADA6B95F4C8454F4A293012C313C1C485A940CB60F5872809A613A328C729D2D7493DFC78D04BAE83E9ED6EB8624363A8F9B466A002F16E31989A581F2C638AB1668931126554FF45939441DDDAA9EB7A1D93FC771643E02936DCACE1B7AA56878818298B31ED37BF66147374DD49CBD0792EFEC4F0CAC129D4E41C5F804381A200F2A7CB6F140ABE15A97502F217D079BAE380E00C38953177DD76D6A7B7BC7EC15D05D201B5DF3BB4C1CF59C73D3C636FE8F55C603E7B853AD7741DCCC730B7A46AB519ADFF645DB8E57584AA95F', 16)

def OK(message=''):
  if message:
    logging.debug(message)
    print(message)
  sys.exit(101)

def CORRUPT(message=''):
  if message:
    logging.debug(message)
    print(message)
  sys.exit(102)

def MUMBLE(message=''):
  if message:
    logging.debug(message)
    print(message)
  sys.exit(103)

def DOWN(message=''):
  if message:
    logging.debug(message)
    print(message)
  sys.exit(104)

def ERR(message=''):
  if message:
    logging.error(message)
    print(message)
  sys.exit(105)

def get_or_die(url):
  r = requests.get(url)
  if r.status_code != 200:
    ERR('Expected status code 200 on page: %s' % url)
  return r

def post_or_die(url, **kwargs):
  r = requests.post(url, **kwargs)
  if r.status_code != 200:
    ERR('Expected status code 200 on page: %s' % url)
  return r

def check_substring_or_die(string, pattern):
  logging.debug('Checking substring `%s`..' % pattern)
  if pattern not in string:
    logging.debug('Can\'t find substring `%s`' % pattern)
    MUMBLE('Page (header, file, message from server etc) doesn\'t contain needed substring')

def random_city():
  return 'Yekaterinburg'

def random_theme():
  return '/static/dark'

def socket_readline(connection):
  TIMEOUT = 3

  data = b''

  while True:
    ready, _, _ = select.select([connection], [], [], TIMEOUT)
    if len(ready) == 0 :
      logging.debug('Timeout expired. Server is too slow')
      connection.sendall(b'Timeout expired. You are too slow\n')
      DOWN('Timeout expired. Server is too slow')
  
    char = connection.recv(1)
    if char == b'\n' or len(char) == 0:
      break

    data += char

  data = data.decode()
  logging.debug('Received line from server: %s' % data)
  return data

def correct_hash(x):
  return '%X' % pow(x, EXPONENTA, MODULUS)

def random_what():
  return random.choice(['city', 'airport'])

def random_city():
  return random.choice(['Yekaterinburg', 'Moscow', 'Samara', 'Saint-Petersburg', 'Krasnodar', ])

def random_airport():
  return random.choice(['Koltsovo', 'Domodedovo', 'Sheremetevo'])

def random_place(what):
  if what == 'city':
    return random_city()
  else:
    return random_airport()

def random_metar():
  city = random.choice(['UEAA','UEEE','UELL','UERP','UERR','UESK','UESO','UESS','UEST','UHBB','UHBI','UHHH','UHMA','UHMD','UHMM','UHMP','UHPP','UHSS','UHSH','UHWW','UIAA','UIAE','UIBB','UIBS','UIBV','UIII','UIKB','UIKE','UIKG','UIKK','UIKM','UINN','UITK','UITT','UIUB','UIUN','UIUU','ULAA','ULAM','ULDD','ULDR','ULKK','ULLI','ULLP','ULMM','ULOL','ULOO','ULPB','ULPM','ULSS','ULWC','ULWR','ULWW','ULAH','UMKK','UNAA','UNBB','UNBI','UNCC','UNEE','UNKL','UNKY','UNNT','UNOO','UNOS','UNTT','UNWW','UOHH','UOII','UODD','UOOD','UOOO','UOOW','UOTT','URFF','URKA','URKK','URKM','URKT','URKW','URMG','URML','URMM','URMN','URMO','URMS','URMT','URRR','URRT','URSS','URWA','URWI','URWW','USCC','USCM','USDD','USKK','USMM','USMU','USNN','USNR','USII','USPP','USRK','USRN','USRO','USRR','USSE','USSI','USSK','USSS','USTM','USTO','USTR','USTL','USUU','UUBA','UUBB','UUBP','UUBR','UUDD','UUDL','UUEE','UUEM','UUMO','UUOB','UUOK','UUOO','UUUS','UUWW','UUYH','UUYP','UUYW','UUYY','UWGG','UWKB','UWKD','UWKE','UWKO','UWKS','UWLL','UWLN','UWLS','UWLW','UWOO','UWOR','UWPP','UWPS','UWSS','UWSK','UWUB','UWUF','UWUK','UWUS','UWUU','UWWB','UWWZ','UWWS','UWWG','UWWW','UAAA','UAAH','UAAR','UACC','UACP','UADD','UAII','UAIT','UAKD','UAKK','UAOL','UAON','UAOO','UARR','UASB','UASK','UASP','UASS','UATA','UATE','UATG','UATT','UATR','UAUR','UAUU','UBBB','UBBG','UBBN','UBBY','UBBL'])
  utc_time = time.strftime('%H%M00Z', time.gmtime())
  wind = '%03d%02dMPS' % (random.randint(0, 359), random.randint(0, 99))
  visibility = '%04d' % random.choice([50, 100, 1000, 9999])
  weather = ' '.join(random.sample(['DZ','RA','SN','SG','PL','GS','RASN','SNRA','SHSN','SHRA','SHGR','FZRA','FZDZ','TSRA','TSGR','TSGS','TSSN','DS','SS'], random.randint(0, 4)))
  q = 'Q%04d' % random.randint(800, 1200)
  additional = random.choice(['NOSIG', 'BECMG', 'TEMPO'])

  metar = ' '.join([city, utc_time, wind, visibility, weather, q, additional])
  logging.debug('Generated metar: %s' % metar)

  return metar

########################### MODES ###############################

def check(hostname):
  root = 'http://%s:%d' % (hostname, HTTP_PORT)
  
  r = get_or_die(root)
  check_substring_or_die(r.text, 'Andrew Gein, Hackerdom, RuCTF 2014')
  check_substring_or_die(r.text, 'This service collects meteo data from meteorological towers')
  check_substring_or_die(r.text, 'Create your own filter')

  theme = random_theme()
  r = post_or_die(root + '/create_filter', data={'city': random_city(), 'theme': theme})
  check_substring_or_die(r.text, 'New filter has been added')

  soup = BeautifulSoup(r.text)
  menus = soup.find_all(class_='dropdown-menu')
  if len(menus) != 2:
    MUMBLE('Invalid markup on page: %s/create_filter' % root)

  last_href = menus[1].li.a['href']

  r = requests.get(root + last_href)
  if 'link' not in r.headers:
    MUMBLE('Invalid headers on page: %s%s' % (root, last_href))
  link_header = r.headers['link']

  check_substring_or_die(link_header, '<%s.css>' % theme)
   

def put(hostname, flag_id, flag):
  client = socket.socket()
  try:
    client.connect((hostname, TOWER_PORT))
    hello1 = socket_readline(client)
    check_substring_or_die(hello1, 'This is automatic collector-interface for meteo-towers')
    hello2 = socket_readline(client)
    check_substring_or_die(hello2, 'Please authenticate:')

    hello3 = socket_readline(client)
    check_substring_or_die(hello3, 'My secret is')

    secret = hello3[-14:]
    logging.debug('Received secret from server: `%s`' % secret)
    try:
      secret = int(secret, 16)
    except:
      MUMBLE('Server secret should be correct hex number')

    socket_readline(client)

    chash = correct_hash(secret)

    logging.debug('Sending %s as hash' % chash)

    client.sendall((chash + '\n').encode())

    answer = socket_readline(client)
    if 'I am ready to execute your commands' not in answer:
      CORRUPT('Can\'t authenticate into service')

    client.sendall(b'add\n')
    what = random_what()
    place = random_place(what)
    lat = random.randint(-180, 180)
    _long = random.randint(0, 360)
    metar = random_metar()
    comment = flag

    socket_readline(client); client.sendall((what + '\n').encode())
    socket_readline(client); client.sendall((place + '\n').encode())
    socket_readline(client); client.sendall((str(lat) + '\n').encode())
    socket_readline(client); client.sendall((str(_long) + '\n').encode())
    socket_readline(client); client.sendall((metar + '\n').encode())
    socket_readline(client); client.sendall((comment + '\n').encode())

    answer = socket_readline(client)
    check_substring_or_die(answer, 'Hash for your record is')

    unique_hash = answer[-32:]
    logging.debug('Uniqiue hash is %s', unique_hash)
    
    print(unique_hash)   
  finally:
    client.close()

def get(hostname, flag_id, flag):
  root = 'http://%s:%d' % (hostname, HTTP_PORT)

  flag_url = root + '/hash/%s' % flag_id
  r = get_or_die(flag_url)
  if flag not in r.text:
    CORRUPT('Can\'t find my flag')

  soup = BeautifulSoup(r.text)
  menus = soup.find_all(class_='dropdown-menu')
  if len(menus) != 2:
    MUMBLE('Invalid markup on page: /hash/%s' % (root, '?' * len(flag_id)))

  menu_elements = menus[1].find_all('li')
  for menu_element in menu_elements[:-2][:5]:
    filter_href = menu_element.find_all('a')[0]['href']
    logging.debug('Get one filter page `%s%s`, set referer `%s`' % (root, filter_href, flag_url))
    requests.get(root + filter_href, headers={'Referer': flag_url})  

########################### -MODES ###############################

logging.basicConfig(level=logging.DEBUG)
logging.debug('Checker started with parameters: `%s`' % ' '.join(sys.argv))

if len(sys.argv) < 2:
  ERR('Invalid count of parameters')

mode = sys.argv[1]
MODES = {'check': check, 'put': put, 'get': get}

if mode not in MODES.keys():
  ERR('Invalid mode: %s' % mode)

try:
  MODES[mode](*(sys.argv[2:]))
except Exception as e:
  ERR('Invalid arguments: %s' % e)

OK()