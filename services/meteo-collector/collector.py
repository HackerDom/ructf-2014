#!/usr/bin/env python3

import pymysql
import multiprocessing
import socket
import logging
import select
import random

def init_db():
  global db
  DB_HOST = 'localhost'
  DB_USER = 'meteo'
  DB_PASS = 'DhBr1lONLzMetf7mhROc'
  DB_NAME = 'meteo'
  db = pymysql.connect(host=DB_HOST, user=DB_USER, passwd=DB_PASS, db=DB_NAME, charset='utf8')

MODULUS = int('D12FBA8D63EA1F6ABFE3C989FEBD928361627ADE5AC6F878ED52613ED03076AFFA5E27BF7A0004DE149B7332D239737184ADA6B95F4C8454F4A293012C313C1C485A940CB60F5872809A613A328C729D2D7493DFC78D04BAE83E9ED6EB8624363A8F9B466A002F16E31989A581F2C638AB1668931126554FF45939441DDDAA9EB7A1D93FC771643E02936DCACE1B7AA56878818298B31ED37BF66147374DD49CBD0792EFEC4F0CAC129D4E41C5F804381A200F2A7CB6F140ABE15A97502F217D079BAE380E00C38953177DD76D6A7B7BC7EC15D05D201B5DF3BB4C1CF59C73D3C636FE8F55C603E7B853AD7741DCCC730B7A46AB519ADFF645DB8E57584AA95F', 16)

def socket_readline(connection, logger):
  TIMEOUT = 10

  data = b''

  while True:
    ready, _, _ = select.select([connection], [], [], TIMEOUT)
    if len(ready) == 0 :
      logger.debug('Timeout expired. Client is too slow')
      connection.sendall(b'Timeout expired. You are too slow\n')
      raise TimeoutError()
  
    char = connection.recv(1)
    if char == b'\n' or len(char) == 0:
      break

    data += char

  return data.decode()

def get_list():
  global db
  cur = db.cursor()
  cur.execute('SELECT * FROM data ORDER BY id DESC LIMIT 10')
  return [row for row in cur]
    
def add(what, place, lat, _long, metar, comment):
  global db
  unique_hash = '%032X' % random.getrandbits(128)
  cur = db.cursor()
  cur.execute('INSERT INTO `data` (`what`, `place`, `lat`, `long`, `metar`, `comment`, `hash`) VALUES (%s, %s, %d, %d, %s, %s, %s)' % (
        db.escape(what),
        db.escape(place),
        lat,
        _long,
        db.escape(metar),
        db.escape(comment),
        db.escape(unique_hash)
        ))
  db.commit()
  return unique_hash

def handle_command(connection, logger):
  command = socket_readline(connection, logger)
  if not command:
    logger.debug('Socket closed remotely')
    return False

  logger.debug('Received command `%s`' % command)

  if command == 'list':
    l = get_list()
    for el in l:
      _id, timestamp, lat, _long, what, place, metar, comment, _ = el
      connection.sendall(('%d. %s [%s] (%d, %d) %s $ %s\n' % (_id, place, timestamp, lat, _long, metar, comment)).encode())
  elif command == 'add':
    connection.sendall('What (city, airport)?\n'.encode())
    what = socket_readline(connection, logger)
    if what not in ['city', 'airport']:
      logger.debug('Invalid place id: %s' % what)
      connection.sendall(('Invalid place: %s\n' % what).encode())
      return True
    
    connection.sendall('Place of meteo-tower?\n'.encode())
    place = socket_readline(connection, logger)

    connection.sendall('Latitude?\n'.encode())
    lat = int(socket_readline(connection, logger))

    connection.sendall('Longitude?\n'.encode())
    _long = int(socket_readline(connection, logger))

    connection.sendall('Metar?\n'.encode())
    metar = socket_readline(connection, logger)

    connection.sendall('Comment [optional]?\n'.encode())
    comment = socket_readline(connection, logger)

    unique_hash = add(what, place, lat, _long, metar, comment)

    logger.debug('Record has been dumped to db. Hash = %s' % unique_hash)
    connection.sendall(('Hash for your record is %s\n' % unique_hash).encode())
  else:
    logger.debug('Unknown command: %s' % command)
    connection.sendall(('Unknown command: %s\n' % command).encode())

  return True


def handle_client(connection, logger):
  connection.sendall('This is automatic collector-interface for meteo-towers, project codename `RuCTF`, version 2.0.14.\n'.encode())
  connection.sendall('Please authenticate:\n'.encode())
  
  rnd = random.randint(1, 10000000000000000)
  connection.sendall(('My secret is %014X\n' % rnd).encode())

  connection.sendall('Send me your hash\n'.encode())

  logger.debug('My secret is %d = %014X' % (rnd, rnd))
  tower_hash = socket_readline(connection, logger)
  logger.debug('Client sent me %s' % tower_hash)
  try:
    tower_hash = int(tower_hash, 16)
  except:
    logger.debug('Invalid digits')
    connection.sendall('Your hash has invalid format\n'.encode())
    raise

  check = pow(tower_hash, 65537, MODULUS)
  if check != rnd:
    logger.debug('Can\'t authorize: invalid hash')
    connection.sendall('Invalid hash, I can\'t authenticate you\n'.encode())
    return

  connection.sendall('I am ready to execute your commands\n'.encode())

  while handle_command(connection, logger):
    pass

def handle(connection, address):
  logging.basicConfig(level=logging.DEBUG)
  logger = logging.getLogger('process-%r' % (address,))
  try:
    logger.debug('Connected %r at %r', connection, address)
    handle_client(connection, logger)
  except:
    logger.exception('Problem handling request')
  finally:
    logger.debug('Closing socket')
    connection.close()

class Server(object):
  def __init__(self, hostname, port):
    self.logger = logging.getLogger('server')
    self.hostname = hostname
    self.port = port

  def start(self):
    self.logger.debug('listening')
    self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.socket.bind((self.hostname, self.port))
    self.socket.listen(100)

    while True:
      conn, address = self.socket.accept()
      self.logger.debug('Got connection')
      process = multiprocessing.Process(target=handle, args=(conn, address))
      process.daemon = True
      process.start()
      self.logger.debug('Started process %r', process)

if __name__ == '__main__':
  init_db()

  logging.basicConfig(level=logging.DEBUG)
  PORT = 9000
  server = Server('0.0.0.0', PORT)
  try:
    logging.info('Listening on port %d' % PORT)
    server.start()
  except:
    logging.exception('Unexpected exception')
  finally:
    logging.info('Shutting down')
    for process in multiprocessing.active_children():
      logging.info('Shutting down process %r', process)
      process.terminate()
      process.join()
  logging.info('All done')

