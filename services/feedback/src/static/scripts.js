function $(selector, ctx) {
	ctx = ctx || document;
	return {
		all: function() {
			return ctx.querySelectorAll(selector);
		},
		first: function() {
			return ctx.querySelector(selector);
		},
		last: function() {
			var list = ctx.querySelectorAll(selector);
			return list.length > 0 ? list[list.length - 1] : null;
		}
	};
}

$.ajax = function(method, url, data, success, error) {
	var xhr = new XMLHttpRequest();
	xhr.open(method, url);
	xhr.onreadystatechange = function() {
		if(xhr.readyState == 4) {
			if(xhr.status >= 200 && xhr.status < 300 || xhr.status == 304) {
				if(success) { success(xhr.responseText); }
			} else {
				if(error) { error(); }
			}
		}
	};
	xhr.send(data);
};

$.get = function(url, data, success, error) {
	var query = [];
	if(data) {
		for(var key in data) {
			query.push(encodeURIComponent(key) + "=" + encodeURIComponent(data[key]));
		}
	}
	var queryString = query.length == 0 ? "" : "?" + query.join("&");
	$.ajax('GET', url + queryString, null, function(result) {
			if(success) {success(result ? JSON.parse(result) : null)}
		}, error);
};

$.post = function(url, data, success, error) {
	$.ajax('POST', url, JSON.stringify(data), success, error);
}