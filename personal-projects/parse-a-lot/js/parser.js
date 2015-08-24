/*
string split
merge
trim
get nth of list
parse number
fid / substr
foreach in list
find replace
to lower case
to upper case
if then else
compare
*/

/*
multiple outputs for same node

sub nodes, i.e. foreach
*/

var NODE_TYPES = {
	Split : 0,
	Merge : 1,
	Trim : 2,
	Compare : 3,
	ForEach : 4,
	IfElse : 5,
	Constant : 6,
	ParserIn : 7,
	ParserOut : 8,
	Fork : 9
};

var DATA_TYPES = {
	String : 1,
	StringList : 2,
	Bool : 4,
	StringOrStringList : 3,
	Any : 7
};

var dataTypeColors = {};

dataTypeColors[DATA_TYPES.String] = "#c22";
dataTypeColors[DATA_TYPES.StringList] = "#2c2";
dataTypeColors[DATA_TYPES.Bool] = "#22c";
dataTypeColors[DATA_TYPES.StringOrStringList] = "#cc2";
dataTypeColors[DATA_TYPES.Any] = "#ccc";

var Vector2 = function(x,y){
	this.x = x;
	this.y = y;
};

var InOutNode = function(node, isInput, index){
	this.node = node;
	this.isInput = isInput;
	this.index = index;
	
	this.GetPosition = function(){
		var xPos = this.node.position.x + (this.node.width/2 * (this.isInput ? -1 : 1));
		var numPuts = (this.isInput ? this.node.inputs.length : this.node.outputs.length);
		var yPos = this.node.position.y - this.node.height/2 + ((this.index + 1)/(numPuts + 1) * this.node.height);
		
		return new Vector2(xPos, yPos);
	}
	
};

function VecSub(a,b){
	return new Vector2(a.x - b.x, a.y - b.y);
}

function VecAdd(a,b){
	return new Vector2(a.x + b.x, a.y + b.y);
}

var CanvasNode = function(width, height){
	this.width = width;
	this.height = height;
	
	this.name = "";
	
	this.inputs = [null];
	this.outputs = [null];
	
	this.textField = null;
	
	this.inputTypes = [DATA_TYPES.String];
	this.outputTypes = [DATA_TYPES.String];
	
	this.position = new Vector2(100,100);
	this.mouseOffset = new Vector2(0,0);
};

CanvasNode.prototype.Render = function(ctx){
	var x = this.position.x - this.width/2;
	var y = this.position.y - this.height/2;
	
	this.inOutRadius = 8;
	
	ctx.fillStyle="#bbbbbb";
	ctx.strokeStyle = "#333333";
	
	ctx.beginPath();
	ctx.rect(x, y, this.width, this.height);
	ctx.fill();
	
	ctx.beginPath();
	ctx.lineWidth = 2;
	ctx.rect(x, y, this.width, this.height);
	ctx.stroke();
	
	var numInputs = this.inputs.length;
	var numOutputs = this.outputs.length;
	
	var inXPos  = this.position.x - this.width/2;
	var outXPos = this.position.x + this.width/2;
	
	for(var i = 0; i < numInputs; i++){
		var yPos = this.position.y - this.height/2 + ((i+1)/(numInputs+1) * this.height);
		
		if(this.inputs[i] !== null){
			ctx.beginPath();
			ctx.fillStyle = "#ddd";
			ctx.arc(inXPos, yPos, this.inOutRadius, 0, 2 * Math.PI, false);
			ctx.fill();
		}
		
		ctx.beginPath();
		ctx.strokeStyle = dataTypeColors[this.inputTypes[i]];
		ctx.arc(inXPos, yPos, this.inOutRadius, 0, 2 * Math.PI, false);
		ctx.lineWidth = 4;
		ctx.stroke();
	}
	
	for(var i = 0; i < numOutputs; i++){
		var yPos = this.position.y - this.height/2 + ((i+1)/(numOutputs+1) * this.height);
		
		if(this.outputs[i] !== null){
			ctx.beginPath();
			ctx.fillStyle = "#ddd";
			ctx.arc(outXPos, yPos, this.inOutRadius, 0, 2 * Math.PI, false);
			ctx.fill();
			
			var toPos = this.outputs[i].GetPosition();
			
			ctx.strokeStyle = "#222";
			ctx.beginPath();
			ctx.moveTo(outXPos, yPos);
			ctx.lineTo(toPos.x,toPos.y);
			ctx.lineWidth = 2;
			ctx.stroke();
		}
		
		ctx.beginPath();
		ctx.strokeStyle = dataTypeColors[this.outputTypes[i]];
		ctx.arc(outXPos, yPos, this.inOutRadius, 0, 2 * Math.PI, false);
		ctx.lineWidth = 4;
		ctx.stroke();
	}
	
	ctx.fillStyle = "#222";
	ctx.font="18px Georgia";
	ctx.fillText(this.name,x + 10, y + 18);
};

CanvasNode.prototype.Contains = function(point){
	return point.x > (this.position.x -  this.width/2) && point.x < (this.position.x +  this.width/2)
		&& point.y > (this.position.y - this.height/2) && point.y < (this.position.y + this.height/2);
};

CanvasNode.prototype.WithinInputOrOutputCircle = function(point){
	var numOutputs = this.outputs.length;
	var numInputs = this.inputs.length;
	var radSqr = Math.pow(this.inOutRadius, 2);
	
	var outXPos = this.position.x + this.width/2;
	for(var i = 0; i < numOutputs; i++){
		var yPos = this.position.y - this.height/2 + ((i+1)/(numOutputs+1) * this.height);
		
		if(Math.pow(point.x - outXPos, 2) + Math.pow(point.y - yPos, 2) < radSqr){
			return new InOutNode(this, false, i);
		}
	}
	
	var inXPos = this.position.x - this.width/2;
	for(var i = 0; i < numInputs; i++){
		var yPos = this.position.y - this.height/2 + ((i+1)/(numInputs+1) * this.height);
		
		if(Math.pow(point.x - inXPos, 2) + Math.pow(point.y - yPos, 2) < radSqr){
			return new InOutNode(this, true, i);
		}
	}
	
	return null;
};

CanvasNode.prototype.MouseDown = function(point){
	this.mouseOffset = VecSub(point, this.position);
}

CanvasNode.prototype.MouseMove = function(point){
	this.position = VecSub(point, this.mouseOffset);
}

CanvasNode.prototype.MouseUp = function(point, selection){
	
}

var ParseCanvas = function(){
	this.nodes = [];
	this.clickedNode = null;
	this.clickedInOut = null;
	this.mousePos = new Vector2(0,0);
	
	this.MouseDown = function(point){
		var foundIdx = -1;
		
		for(var idx = this.nodes.length - 1; idx >= 0; idx--){
			var node = this.nodes[idx];
			var inOutCircle = node.WithinInputOrOutputCircle(point);
			if(inOutCircle !== null){
				this.clickedInOut = inOutCircle;
				foundIdx = idx;
			}
			else if(node.Contains(point)){
				this.clickedNode = node;
				node.MouseDown(point);
				foundIdx = idx;
				break;
			}
		}
		
		this.mousePos = point;
		
		if(this.clickedNode !== null || this.clickedInOut !== null){
			var foundNode = this.nodes[foundIdx];
			for(var i = foundIdx + 1; i < this.nodes.length; i++){
				this.nodes[i-1] = this.nodes[i];
			}
			
			this.nodes[this.nodes.length - 1] = foundNode;
		}
		
	}
	
	this.MouseMove = function(point){
		this.mousePos = point;
		if(this.clickedNode !== null){
			this.clickedNode.MouseMove(point);
		}
		else if(this.clickedInOut !== null){
			//Nothing?
		}
	}
	
	this.MouseUp = function(point){
		if(this.clickedInOut !== null){
			var foundNode = false;
			for(var idx = this.nodes.length - 1; idx >= 0; idx--){
				var node = this.nodes[idx];
				var inOutCircle = node.WithinInputOrOutputCircle(point);
				if(inOutCircle !== null && inOutCircle.node !== this.clickedInOut.node 
					&& this.clickedInOut.isInput !== inOutCircle.isInput){
					if(inOutCircle.isInput){
						if((inOutCircle.node.inputTypes[inOutCircle.index] & this.clickedInOut.node.outputTypes[this.clickedInOut.index]) !== 0){
							inOutCircle.node.inputs[inOutCircle.index] = this.clickedInOut;
							this.clickedInOut.node.outputs[this.clickedInOut.index] = inOutCircle;
						}
					}
					else{
						if((inOutCircle.node.outputTypes[inOutCircle.index] & this.clickedInOut.node.inputTypes[this.clickedInOut.index]) !== 0){
							inOutCircle.node.outputs[inOutCircle.index] = this.clickedInOut;
							this.clickedInOut.node.inputs[this.clickedInOut.index] = inOutCircle;
						}
					}
					
					foundNode = true;
					break;
				}
			}
			
			if(!foundNode){
				if(this.clickedInOut.isInput){
					if(this.clickedInOut.node.inputs[this.clickedInOut.index] !== null){
						if(this.clickedInOut.node.inputs[this.clickedInOut.index].node !== null){
							this.clickedInOut.node.inputs[this.clickedInOut.index].node = null
						}
						this.clickedInOut.node.inputs[this.clickedInOut.index] = null;
					}
				}
				else{
					if(this.clickedInOut.node.outputs[this.clickedInOut.index] !== null){
						if(this.clickedInOut.node.outputs[this.clickedInOut.index].node !== null){
							this.clickedInOut.node.outputs[this.clickedInOut.index].node = null
						}
						this.clickedInOut.node.outputs[this.clickedInOut.index] = null;
					}
				}
			}
			
			this.clickedInOut = null;
		}
		
		if(this.clickedNode !== null){
			for(var idx = this.nodes.length - 1; idx >= 0; idx--){
				if(this.nodes[idx].Contains(point)){
					this.nodes[idx].MouseUp(point, this.clickedNode);
					break;
				}
			}
		}
		
		this.clickedNode = null;
	}
	
	this.Render = function(ctx){
		ctx.clearRect(0,0,600,600);
		for(var idx in this.nodes){
			this.nodes[idx].Render(ctx);
			if(this.nodes[idx].textField !== null){
				this.nodes[idx].textField.style.left = (mainCanvas.offsetLeft + this.nodes[idx].position.x - this.nodes[idx].width/3) + "px";
				this.nodes[idx].textField.style.top = (mainCanvas.offsetTop + this.nodes[idx].position.y) + "px";
				this.nodes[idx].textField.style.zIndex = 110 + idx;
			}
		}
		
		if(this.clickedInOut !== null){
			var pos = this.clickedInOut.GetPosition();
			
			ctx.stokeStyle = "#222";
			ctx.beginPath();
			ctx.moveTo(pos.x,pos.y);
			ctx.lineTo(this.mousePos.x,this.mousePos.y);
			ctx.lineWidth = 2;
			ctx.stroke();
		}
	}
	
	this.AddNode = function(type){
		if(type === NODE_TYPES.Split){
			var node = new CanvasNode(200,80);
			node.inputTypes = [DATA_TYPES.String];
			node.outputTypes = [DATA_TYPES.StringList];
			node.name = "Split";
			node.textField = document.createElement('input');
			document.body.appendChild(node.textField);
			node.textField.type = 'text';
			node.textField.style.position = 'absolute';
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.Merge){
			var node = new CanvasNode(200,80);
			node.inputTypes = [DATA_TYPES.StringList];
			node.outputTypes = [DATA_TYPES.String];
			node.name = "Merge";
			node.textField = document.createElement('input');
			document.body.appendChild(node.textField);
			node.textField.type = 'text';
			node.textField.style.position = 'absolute';
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.Trim){
			var node = new CanvasNode(200,80);
			node.inputTypes = [DATA_TYPES.String];
			node.outputTypes = [DATA_TYPES.String];
			node.name = "Trim";
			node.textField = document.createElement('input');
			document.body.appendChild(node.textField);
			node.textField.type = 'text';
			node.textField.style.position = 'absolute';
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.Compare){
			var node = new CanvasNode(200,150);
			node.inputTypes = [DATA_TYPES.String, DATA_TYPES.String];
			node.outputTypes = [DATA_TYPES.Bool];
			node.name = "Compare";
			node.inputs = [null, null];
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.ForEach){
			var node = new CanvasNode(200,150);
			node.inputs = [null, null];
			node.outputs = [null, null];
			node.inputTypes = [DATA_TYPES.StringList, DATA_TYPES.String];
			node.outputTypes = [DATA_TYPES.StringList, DATA_TYPES.String];
			node.name = "For Each__";
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.IfElse){
			var node = new CanvasNode(160,180);
			node.inputs = [null, null, null];
			node.outputs = [null];
			node.inputTypes = [DATA_TYPES.Bool, DATA_TYPES.StringOrStringList, DATA_TYPES.StringOrStringList];
			node.outputTypes = [DATA_TYPES.StringOrStringList];
			node.name = "If Else";
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.Constant){
			var node = new CanvasNode(180,80);
			node.inputs = [];
			node.outputs = [null];
			node.inputTypes = [];
			node.outputTypes = [DATA_TYPES.String];
			node.textField = document.createElement('input');
			document.body.appendChild(node.textField);
			node.textField.type = 'text';
			node.textField.style.position = 'absolute';
			node.name = "Constant";
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.ParserIn){
			var node = new CanvasNode(120,80);
			node.inputs = [];
			node.outputs = [null];
			node.inputTypes = [];
			node.outputTypes = [DATA_TYPES.String];
			node.name = "Parser In";
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.ParserOut){
			var node = new CanvasNode(120,80);
			node.inputs = [null];
			node.outputs = [];
			node.inputTypes = [DATA_TYPES.String];
			node.outputTypes = [];
			node.name = "Parser Out";
			this.nodes.push(node);
		}
		else if(type === NODE_TYPES.Fork){
			var node = new CanvasNode(140,120);
			node.inputs = [null];
			node.outputs = [null, null];
			node.inputTypes = [DATA_TYPES.Any];
			node.outputTypes = [DATA_TYPES.Any, DATA_TYPES.Any];
			node.name = "Fork";
			this.nodes.push(node);
		}
		else{
			console.log("Unkown node type: '" + type + "'");
		}
	}
};

window.onload = function(){
	var mainCanvas = document.getElementById('mainCanvas');
	var ctx = mainCanvas.getContext("2d");
	var canvasObj = new ParseCanvas();
	window.canvasObj = canvasObj;
	
	mainCanvas.onmousedown = function(event){
		var x = event.offsetX?(event.offsetX):event.pageX-mainCanvas.offsetLeft;
		var y = event.offsetY?(event.offsetY):event.pageY-mainCanvas.offsetTop;
		canvasObj.MouseDown(new Vector2(x,y));
	};
	
	mainCanvas.onmousemove = function(event){
		var x = event.offsetX?(event.offsetX):event.pageX-mainCanvas.offsetLeft;
		var y = event.offsetY?(event.offsetY):event.pageY-mainCanvas.offsetTop;
		canvasObj.MouseMove(new Vector2(x,y));
	};
	
	mainCanvas.onmouseup = function(event){
		var x = event.offsetX?(event.offsetX):event.pageX-mainCanvas.offsetLeft;
		var y = event.offsetY?(event.offsetY):event.pageY-mainCanvas.offsetTop;
		canvasObj.MouseUp(new Vector2(x,y));
	};
	
	setInterval(function(){
		ctx.canvas.width = mainCanvas.offsetWidth;
		ctx.canvas.height = mainCanvas.offsetHeight;
		canvasObj.Render(ctx);
	}, 
	17);
};

