var svgElements = [];
var svgContainerOffset;
var shapeType = "Rect"
var shapeFill = "#ff0000";

function changeShapeType(newType){
	shapeType = newType;
}

window.onload = function(){
	svgContainerOffset = document.getElementById("svg-container").getBoundingClientRect();
}

function pickColor(){
	
}

var SVGCirc = function(x, y, r, col){
	this.x = x;
	this.y = y;
	this.r = r;
	this.color = col;
	this.id = "svgElem_" + svgElements.length;
}

function selectElement(elem){
	var bounds = document.getElementById(elem.id).getBoundingClientRect();
	console.log(bounds);
}

SVGCirc.prototype.Add = function(){
	var circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
	circle.ondragstart = function() { return false; };
	circle.onclick = function(){selectElement(circle);}
	circle.setAttributeNS(null, "cx",this.x);
	circle.setAttributeNS(null, "cy",this.y);
	circle.setAttributeNS(null, "r", this.r);
	circle.setAttributeNS(null, "fill", "#ff0000");
	circle.setAttributeNS(null, "id", this.id);
	document.getElementById("svg-container").appendChild(circle);
}

SVGCirc.prototype.Remove = function(){
	var elem = document.getElementById(this.id);
	elem.parentElement.removeChild(elem);
}

SVGCirc.prototype.Update = function(x, y){
	//console.log("this.x: " + this.x + "  x: " + x + "  this:y: " + this.y + "  y: " + y);
	this.r = Math.max(Math.abs(x - this.x), Math.abs(y - this.y));
	var circle = document.getElementById(this.id);
	circle.setAttribute("r", this.r);
}

var SVGRect = function(x, y, w, h, col){
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
	this.color = col;
	this.id = "svgElem_" + svgElements.length;
}

SVGRect.prototype.Add = function(){
	var rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
	rect.ondragstart = function() { return false; };
	rect.onclick = function(){selectElement(rect);}
	rect.setAttributeNS(null, "x",this.x);
	rect.setAttributeNS(null, "y",this.y);
	rect.setAttributeNS(null, "width",this.w);
	rect.setAttributeNS(null, "height",this.h);
	rect.setAttributeNS(null, "fill", "#00ff00");
	rect.setAttributeNS(null, "id", this.id);
	document.getElementById("svg-container").appendChild(rect);
}

SVGRect.prototype.Remove = function(){
	var elem = document.getElementById(this.id);
	elem.parentElement.removeChild(elem);
}

SVGRect.prototype.Update = function(x, y){
	this.w = x - this.x;
	this.h = y - this.y;
	var rect = document.getElementById(this.id);
	rect.setAttribute("width",  this.w);
	rect.setAttribute("height", this.h);
}

function AddElem(x, y){
	if(shapeType == "Circ"){
		var newCirc = new SVGCirc(x, y, 0, "red");
		svgElements.push(newCirc);
		newCirc.Add();
	}
	else if(shapeType == "Rect"){
		var newRect = new SVGRect(x, y, 0, 0, "red");
		svgElements.push(newRect);
		newRect.Add();
	}
}

function TrueX(e){
	var x;
	if (e.pageX && e.pageX !== NaN) { 
	  x = e.pageX;
	}
	else { 
	  x = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft;
	} 
	x -= svgContainerOffset.left;
	
	return x;
}

function TrueY(e){
	var y;
	if (e.pageY && e.pageY !== NaN) { 
	  y = e.pageY;
	}
	else { 
	  y = e.clientY + document.body.scrollTop + document.documentElement.scrollTop; 
	} 
	y -= svgContainerOffset.top;
	
	return y;
}

var isMouseDown = false;
document.onkeyup     = function(event) { 
								var evtobj = event? event : e; 
								if (evtobj.keyCode == 90 && evtobj.ctrlKey && svgElements.length > 0){
									svgElements.pop().Remove();
								} 
							};
document.onmousedown = function(e) { if(e.which == 1){isMouseDown = true; AddElem(TrueX(e), TrueY(e));}};
document.onmouseup   = function(e) { 
							if(e.which == 1){
								isMouseDown = false;
								if(svgElements.length > 0 && Math.pow(TrueX(e) - svgElements[svgElements.length-1].x, 2)
														   + Math.pow(TrueY(e) - svgElements[svgElements.length-1].y, 2) < 2){
									svgElements[svgElements.length-1].Remove();
									svgElements.pop();
								}
							}
						};
document.onmousemove = function(e) { if(e.which == 1){if(isMouseDown && svgElements.length > 0) {svgElements[svgElements.length-1].Update(TrueX(e), TrueY(e));} }};


