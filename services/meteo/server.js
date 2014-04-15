var http = require("http"),
    url = require("url"),
    router = require("./router"),
    data = require("./data"),
    fs = require('fs'),
    ejs = require('ejs');

function index(request, response) {
  data.get_data(0, function(raws){
    response.writeHead(200, {"Content-type": "text/html"});
    response.write("Hello world! You are on the index page");
    response.write(ejs.render(fs.readFileSync('templates/index.html', 'utf8'), {}));
    response.write(JSON.stringify(raws));
    response.end();
  });
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
    response.writeHead(202, {"Content-Type": "text/html"});
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
