$(document).ready(function() {
	var options = { 
		success:   showResponse,
		dataType:  'json',
		clearForm: true
	};

	$("#putForm").ajaxForm(options);
});

function showResponse(responseText, statusText, xhr, $form)  {
	var result;
	if (responseText['status'] == "success")
		result = "Here's ID of your song:<p>" + responseText['uuid'] + "</p>";
	else
		result = 'Sorry, something has gone wrong =(';
	$("#uploadModalbody").html(result);
	$('#uploadModal').modal();
} 

function getHostname() {
	return window.location.hostname;
}

$(document).ready(function() {
	setInterval(function() {
		var curUrl = "http://" + getHostname() + ":17216/info.xsl";
		$.ajax({
			url : curUrl,
			dataType : "jsonp",
			jsonpCallback : "callback",
			success : function(data) {
				var title = data['/stream.ogg']['title'];
				$(".jp-title").text(title);
			},
			timeout : 1000
		});
	}, 1000);


	var stream = {
		oga: "http://" + getHostname() + ":17216/stream.ogg"
	};

	$("#jquery_jplayer_1").jPlayer({
		ready: function (event) {
			$(this).jPlayer("setMedia", stream);
		},
		swfPath: "static/js",
		supplied: "oga",
		preload: "none",
		wmode: "window",
		keyEnabled: true
	});
});
