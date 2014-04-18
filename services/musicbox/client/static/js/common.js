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
		result = 'Sorry, something gone wrong =(';
	$("#uploadModalbody").html(result);
	$('#uploadModal').modal();
} 