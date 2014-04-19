var mysql = require('mysql');

var DB_HOST = process.env.DB_PORT_3306_TCP_ADDR || 'localhost',
    DB_USER = 'meteo',
    DB_PASSWORD = 'DhBr1lONLzMetf7mhROc',
    DB_NAME = 'meteo';

var connection;

exports.init_db = function(callback){ 
  connection = mysql.createConnection({host: DB_HOST, user: DB_USER, password: DB_PASSWORD});
  connection.connect();
  connection.query('USE ' + DB_NAME, function(err, _, _) {
    if (err) throw err;
    callback();
  });
}

exports.get_data = function(start, callback){
  start = start ? (parseInt(start) || 0) : 0;
  if (! connection)
    throw 'Can\'t get data until connection established';
  connection.query('SELECT * FROM data ORDER BY id DESC LIMIT ' + start + ', 50', function(err, rows, fields){
    if (err) throw err;
    callback(rows);
  });
}

exports.get_by_hash = function(hash, callback) {
  if (! connection)
    throw 'Can\'t get data until connection established';
  connection.query('SELECT * FROM data WHERE hash = ?', [hash], function(err, rows) {
    if (err) throw err;
    callback(rows[0]);
  });
}

exports.add_filter = function(city, theme, callback) {
  if (! connection)
    throw 'Can\'t add filter until connection established';
  if (theme.indexOf('http') != -1 || theme.indexOf('//') != -1)
    throw 'Theme can\'t contains `http` or `//` as substring';
  connection.query('INSERT INTO filters (city, theme) VALUES (?, ?)', [city, theme], function(err) {
    if (err) throw err;
    callback();
  });
}

exports.get_filters = function(callback) {
  if (! connection)
    throw 'Can\'t get filters until connection established';
  connection.query('SELECT * FROM filters ORDER BY id DESC LIMIT 10', function(err, rows) {
    if (err) throw err;
    callback(rows);
  });
}

exports.get_filter = function(filter_id, callback) {
  if (! connection)
    throw 'Can\'t get filters until connection established';
  connection.query('SELECT * FROM filters WHERE id = ?', [filter_id], function(err, rows) {
    if (err) throw err;
    callback(rows[0]);
  });
}
