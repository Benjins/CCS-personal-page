//Expects --template '{file},{line},{severity},{id},{message}'

var Error = function(file, line, id, severity, message){
	this.file = file;
	this.line = line;
	this.id = id;
	this.severity = severity;
	this.message = message;
}


function GetAllErrorsFromFile(fileName){
	var request = new XMLHttpRequest();
	request.open("GET", fileName);
	request.onreadystatechange = function(){
		if (request.readyState===4 && request.status===200){
			var errors = GetErrorsFromContents(request.responseText);
			console.log(errors);

			var container = document.getElementById("error-container");
			for(var idx in errors){
				var error = errors[idx];
				AddErrorDisplayToContainer(container, error);
			}
		}
		else if(request.readyState===4){
			console.log("GetAllErrorsFromFile: Error, server responded with error code: '" + request.status + "'.");
		}
	};

	request.send();
}

function GetErrorsFromContents(fileContents){
	var lines = fileContents.split("\n");

	var errors = [];

	for(var idx in lines){
		var line = lines[idx];
		if(line.trim){ //Some older browsers may not support this
			line = line.trim();
		}
		else{
			line = line.replace(/^\s+|\s+$/gm,'');
		}

		if(line.length > 0){
			errors.push(ParseError(line));
		}		
	}

	return errors;
}

function ParseError(line){
	var lineSplit = line.split(",");

	var file = lineSplit[0];
	var line = parseInt(lineSplit[1]);
	var id = lineSplit[2];
	var severity = lineSplit[3];
	var message = lineSplit[4];

	return new Error(file, line, id, severity, message);
}


function AddErrorDisplayToContainer(container, error){
	var errorDiv = document.createElement('div');
	errorDiv.className = "error";

	var fileInfo = document.createElement('p');

	fileInfo.innerHTML = "<span style='background:#afa;'>" + error.file + ":" + error.line + "</span>" + "<span style='float:right;'>" + error.message + "</span><span style='clear:both;'></span>";

	errorDiv.appendChild(fileInfo);

	container.appendChild(errorDiv);
}
