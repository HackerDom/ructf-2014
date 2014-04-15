var http = require("http"),
    url = require("url"),
    router = require("./router"),
    data = require("./data"),
    fs = require('fs'),
    ejs = require('ejs'),
    querystring = require('querystring');

String.prototype.endsWith = function(suffix) {
  return this.indexOf(suffix, this.length - suffix.length) !== -1;
};

var countries = {'Russia': ['Yekaterinburg', 'Moscow', 'Saint-Petersburg', 'Samara', 'Krasnodar'], 'USA': ['New York City', 'Washington', 'Las Vegas', 'Los Angeles', 'Chicago']};

function show_filter_index(response, filter) {
  data.get_data(0, function(rows){
    data.get_filters(function(filters) {
      response.writeHead(200, {'Content-type': 'text/html'});
      response.write(ejs.render(fs.readFileSync('templates/index.html', 'utf8'),
        {'rows': rows,
         'countries': countries,
         'filters': filters,
         'city': filter.city
        }));
      response.end();
    });
  }); 
}

function index(request, response) {
  show_filter_index(response, {});
}

function create_filter(request, response, post) {
  if (post) {
    response.writeHead(200, {'Content-type': 'text/html'});
    post_data = querystring.parse(post);
    if (!('city' in post_data) || !('theme' in post_data)) {
      error500(response, 'invalid request');
      return;
    }
    city = post_data.city;
    theme = post_data.theme;
    data.add_filter(city, theme, function() {
      data.get_filters(function(filters) {
        response.write(ejs.render(fs.readFileSync('templates/create_filter.html', 'utf8'), 
          {'countries': countries,
           'message': 'New filter added',
           'filters': filters
          }));
        response.end();
      });
    });
  } else {
    response.writeHead(200, {'Content-type': 'text/html'});
    data.get_filters(function(filters) {
      response.write(ejs.render(fs.readFileSync('templates/create_filter.html', 'utf8'),
        {'countries': countries,
         'message': '',
         'filters': filters
        }));
      response.end();
    });
  }
}

function city(request, response) {
  var city = url.parse(request.url).pathname.split("/")
  city = city[city.length - 1];
  show_filter_index(response, {'city': city});
}

function filter(request, response) {
  var filter_id = parseInt(url.parse(request.url).pathname.substr(8)) || 0;
  console.log("Get filter: " + filter_id);
  data.get_filter(filter_id, function(filter) {
    if (! filter || !('theme' in filter) || !('city' in filter)) {
      error500(response, 'invalid request');
      return;
    }
    response.writeHead(200, {'Content-Type': 'text/html', 'Link': filter.theme});
    show_filter_index(response, {'city': filter.city});
  });
}


function get_mime_type(filename) {
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".js")) return "text/javascript";
  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
  if (filename.endsWith(".png")) return "image/png";
  return "text/html";
}

function static_files(request, response) {
  var pathname = url.parse(request.url).pathname.substr(1);
  if (pathname.indexOf("..") != -1) {
    response.writeHead(403, {"Content-Type": "text/html"});
    response.write("Access denied");
    response.end();
    return;
  }
  fs.readFile(pathname, function(err, content){
    if (err) {
      console.log(err);
      // TODO: call error404()
      response.writeHead(404, {"Content-Type": "text/html"});
      response.end();
      return;
    }
    console.log("Get static file: " + pathname);
    // TODO: correct Content-Type
    response.writeHead(200, {"Content-Type": get_mime_type(pathname)});
    response.write(content);
    response.end();
  });
}

function error500(response, message) {
  response.writeHead(500, {'Content-Type': 'text/html'});
  response.write('Error occured: ' + message);
  response.end();
}

router.add_route("/static/", static_files);
router.add_route("/create_filter", create_filter);
router.add_route("/filter/", filter);
router.add_route("/city/", city);
router.add_route("/", index);

data.init_db(function(){
  http.createServer(function(request, response) {
    var pathname = url.parse(request.url).pathname;
    var post_data = '';
    console.log("Request for " + pathname + " received.");

    request.addListener('data', function(chunk) { post_data += chunk; });
  
    request.addListener('end', function(){
      handler = router.route(pathname);
      try {
        handler(request, response, post_data);
      } catch (msg) {
        error500(response, msg);
      }
    });
  }).listen(8888);
})
