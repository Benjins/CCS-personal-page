/*
Circles
Rectangles

methods:
updateSVG()
onDragStart()
onDrag()
onDragEnd()

*/

var svgElements = [];

var SVGCirc = function(x, y, r, col){
	this.x = x;
	this.y = y;
	this.r = r;
	this.color = col;
	this.id = "svgElem_" + svgElements.length;
}

SVGCirc.prototype.Add = function(){
	var circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
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
	this.r = Math.max(Math.abs(x - this.x), Math.abs(y - this.y));
	var circle = document.getElementById(this.id);
	circle.setAttribute("r", this.r);
}

//Remove
//Add
//Update

var type = "Circ"

function KeyPress(e) {
      var evtobj = window.event? event : e
      if (evtobj.keyCode == 90 && evtobj.ctrlKey){
		
	  };
}

function AddElem(x, y){
	if(type == "Circ"){
		var newCirc = new SVGCirc(x, y, 2, "red");
		svgElements.push(newCirc);
		newCirc.Add();
	}
}

function TrueX(e){
	var x;
	if (e.pageX) { 
	  x = e.pageX;
	}
	else { 
	  x = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft; 
	} 
	x -= document.getElementById("svg-container").offsetLeft;
	
	return x;
}

function TrueY(e){
	var y;
	if ( e.pageY) { 
	  y = e.pageY;
	}
	else { 
	  y = e.clientY + document.body.scrollTop + document.documentElement.scrollTop; 
	} 
	y -= document.getElementById("svg-container").offsetTop;
	
	return y;
}

var isMouseDown = false;
document.onkeyup     = function(event) { var evtobj = window.event? event : e; if (evtobj.keyCode == 90 && evtobj.ctrlKey){svgElements.pop().Remove();} };
document.onmousedown = function(e) { if(e.which == 1){isMouseDown = true; AddElem(TrueX(e), TrueY(e));}};
document.onmouseup   = function(e) { if(e.which == 1){isMouseDown = false; }};
document.onmousemove = function(e) { if(e.which == 1){if(isMouseDown) {svgElements[svgElements.length-1].Update(TrueX(e), TrueY(e));} }};