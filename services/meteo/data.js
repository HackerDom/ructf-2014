var mysql = require('mysql');

var DB_HOST = 'localhost',
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
  })
}
