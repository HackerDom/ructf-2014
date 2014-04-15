var http = require("http"),
    url = require("url"),
    router = require("./router"),
    data = require("./data"),
    fs = require('fs'),
    ejs = require('ejs');

String.prototype.endsWith = function(suffix) {
  return this.indexOf(suffix, this.length - suffix.length) !== -1;
};

function index(request, response) {
  data.get_data(0, function(rows){
    response.writeHead(200, {"Content-type": "text/html"});
    response.write(ejs.render(fs.readFileSync('templates/index.html', 'utf8'), {'rows': rows}));
    response.end();
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


router.add_route("/static/", static_files);
router.add_route("/", index);

data.init_db(function(){
  http.createServer(function(request, response) {
    var pathname = url.parse(request.url).pathname;
    console.log("Request for " + pathname + " received.");
  
    handler = router.route(pathname);
    handler(request, response); 
  }).listen(8888);
})
