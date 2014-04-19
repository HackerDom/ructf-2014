var handlers = {};

var error404 = function(request, response)
{
  response.writeHead(404, {"Content-type": "text/html"});
  response.write("Page doesn't exists");
  response.end();
}

exports.add_route = function(pathname, handler) {
  console.log("add_route " + pathname);
  handlers[pathname] = handler;
}

exports.route = function(pathname) {
  console.log("Looking for route for " + pathname);
  for (var handler_path in handlers)
  {
    if (pathname.substr(0, handler_path.length) == handler_path)
      return handlers[handler_path];
  }
  return error404;
}
