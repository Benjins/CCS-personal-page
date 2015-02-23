if (typeof String.prototype.endsWith !== 'function') {
    String.prototype.endsWith = function(suffix) {
        return this.indexOf(suffix, this.length - suffix.length) !== -1;
    };
}

function detectSVG(){
	return document.implementation.hasFeature("http://www.w3.org/TR/SVG11/feature#BasicStructure", "1.1");
}

function checkSVG(){
	if(detectSVG()){
		console.log("SVG's working fine.");
		return;
	}
	var objects = document.getElementsByTagName("object");
	for(var i in objects){
		var obj = objects[i];
		if(obj !== null && obj !== undefined && obj.getAttribute !== undefined){
			var dataVal = obj.getAttribute('data');
			if(dataVal.endsWith(".svg")){
				var newDataVal = dataVal.substring(0,dataVal.length - 4);
				newDataVal = newDataVal + ".gif";
				obj.style.display = 'none';
				obj.setAttribute('data', newDataVal);
				obj.style.display = '';
			}
		}
	}
}

window.onload = checkSVG;