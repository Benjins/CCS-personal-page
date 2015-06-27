//coverage-parser

var Coverage = function(file, percent, total){
	this.file = file;
	this.percent = percent;
	this.total = total;
}

function GetCoverageResultsFromFile(fileName){
	var request = new XMLHttpRequest();
	request.open("GET", fileName);
	request.onreadystatechange = function(){
		if (request.readyState===4 && request.status===200){
			var coverage = GetCoverageResultsFromContents(request.responseText);
			console.log(errors);

			var container = document.getElementById("coverage-container");
			for(var idx in coverage){
				var cover = coverage[idx];
				AddCoverageDisplayToContainer(container, cover);
			}
		}
		else if(request.readyState===4){
			console.log("GetAllErrorsFromFile: Error, server responded with error code: '" + request.status + "'.");
		}
	};

	request.send();
}

function GetCoverageResultsFromContents(fileContents){
	var lines = fileContents.split("\n");

	var covers = [];

	var fileName = "";
	for(var idx in lines){
		var line = lines[idx];
		if(line.trim){ //Some older browsers may not support this
			line = line.trim();
		}
		else{
			line = line.replace(/^\s+|\s+$/gm,'');
		}
		
		if(line.length > 0){
			if(line.indexOf("File") == 0){
				console.log(line.split("'")[1]);
				fileName = line.split("'")[1];
			}
			else if(line.indexOf("Lines") == 0){
				var percentStr = line.substring(line.indexOf(":")+1, line.indexOf("%"));
				console.log(percentStr);
				var spaceSplit = line.split(" ");
				var totalStr = spaceSplit[spaceSplit.length-1];
				covers.push(new Coverage(fileName, parseFloat(percentStr), parseInt(totalStr)));
			}
		}		
	}

	return covers;
}

function AddCoverageDisplayToContainer(container, coverage){
	var errorDiv = document.createElement('div');
	errorDiv.className = "coverage";

	var fileInfo = document.createElement('p');

	fileInfo.innerHTML = coverage.file + " had " + coverage.percent + "% of " + coverage.total + " lines covered.";
	errorDiv.appendChild(fileInfo);

	container.appendChild(errorDiv);
}


var testString = "File 'src/Vector4.cpp'\n" +
"Lines executed:85.71% of 21\n"+
"Creating 'Vector4.cpp.gcov'\n"+
"\n"+
"File 'src/main.cpp'\n"+
"Lines executed:100.00% of 4\n"+
"Creating 'main.cpp.gcov'\n"+
"\n"+
"File 'src/simple-xml.cpp'\n"+
"Lines executed:0.71% of 140\n"+
"Creating 'simple-xml.cpp.gcov'\n"+
"\n"+
"Lines executed:15.37% of 3253\n";