var Node = function(name){this.name = name;};

var Edge = function(v1,v2, weight){this.v1 = v1; this.v2 = v2; this.weight = weight;};

var verts = [new Node("A"), new Node("B"), new Node("C"), new Node("D"), new Node("E"), new Node("F"), new Node("G"), new Node("H")];

var edges = [new Edge(verts[0], verts[1], 2), new Edge(verts[1], verts[2], 3), new Edge(verts[0], verts[2], 5), 
			 new Edge(verts[2], verts[4], 3), new Edge(verts[3], verts[5], 1), new Edge(verts[5], verts[6], 1),
			 new Edge(verts[2], verts[6], 1), new Edge(verts[3], verts[4], 1), new Edge(verts[7], verts[6], 1)];
			 
			 
var badEdges = [];
			 
var newVerts = [];

var newEdges = [];

function FindNode(name){
	for(var idx in verts){
		if(verts[idx].name === name){
			return verts[idx];
		}
	}
	
	return null;
}

function FindNewNode(name){
	for(var idx in newVerts){
		if(newVerts[idx].name === name){
			return newVerts[idx];
		}
	}
	
	return null;
}
			 
(function(){
	var x = 50;
	var y = 50;
	var offset = 100;
	var width = 300;
	
	for(var idx in verts){
		var newVert = new Node(verts[idx].name);
		
		newVert.x = x + (y % 37) + 300;
		newVert.y = y;
		newVert.forest = newVert;
		
		newVerts.push(newVert);
		
		verts[idx].x = x + (y % 37);
		verts[idx].y = y;
		
		x += offset;
		if(x >= width - offset/2){
			x = 70;
			y += offset;
		}
	}

})();		 
			 
function Render(ctx){
	ctx.clearRect(0,0,700,700);

	ctx.fillStyle = "#777";
	ctx.lineWidth = 2;
	
	ctx.font = "24px serif";
	
	ctx.strokeStyle = "#a33";
	ctx.setLineDash([5,5]);
	for(var idx in badEdges){
		ctx.beginPath();
		ctx.moveTo(badEdges[idx].v1.x,badEdges[idx].v1.y);
		ctx.lineTo(badEdges[idx].v2.x,badEdges[idx].v2.y);
		ctx.stroke();
		ctx.closePath();
		
		var midX = (badEdges[idx].v1.x + badEdges[idx].v2.x)/2;
		var midY = (badEdges[idx].v1.y + badEdges[idx].v2.y)/2;
		
		ctx.fillText("" + badEdges[idx].weight, midX, midY);
	}
	
	ctx.strokeStyle = "#222";
	ctx.setLineDash([5,0]);
	
	for(var idx in edges){
		ctx.beginPath();
		ctx.moveTo(edges[idx].v1.x,edges[idx].v1.y);
		ctx.lineTo(edges[idx].v2.x,edges[idx].v2.y);
		ctx.stroke();
		ctx.closePath();
		
		var midX = (edges[idx].v1.x + edges[idx].v2.x)/2;
		var midY = (edges[idx].v1.y + edges[idx].v2.y)/2;
		
		ctx.fillText("" + edges[idx].weight, midX, midY);
	}
	
	for(var idx in newEdges){
		ctx.beginPath();
		ctx.moveTo(newEdges[idx].v1.x,newEdges[idx].v1.y);
		ctx.lineTo(newEdges[idx].v2.x,newEdges[idx].v2.y);
		ctx.stroke();
		ctx.closePath();
		
		var midX = (newEdges[idx].v1.x + newEdges[idx].v2.x)/2;
		var midY = (newEdges[idx].v1.y + newEdges[idx].v2.y)/2;
		
		ctx.fillText("" + newEdges[idx].weight, midX, midY);
	}
	
	ctx.font = "12px serif";
	
	for(var idx in verts){
		ctx.fillStyle = "#777";
		
		ctx.beginPath();
		ctx.arc(verts[idx].x,verts[idx].y,30, 0, (Math.PI/180)*360, false);
		ctx.fill();
		ctx.stroke();
		ctx.closePath();
		
		ctx.fillStyle = "#111";
		ctx.fillText(verts[idx].name, verts[idx].x, verts[idx].y);
	}
	
	for(var idx in newVerts){
		ctx.fillStyle = "#777";
		
		ctx.beginPath();
		ctx.arc(newVerts[idx].x,newVerts[idx].y,30, 0, (Math.PI/180)*360, false);
		ctx.fill();
		ctx.stroke();
		ctx.closePath();
		
		ctx.fillStyle = "#111";
		ctx.fillText(newVerts[idx].name, newVerts[idx].x, newVerts[idx].y);
		ctx.fillStyle = "#228";
		ctx.fillText(newVerts[idx].forest.name, newVerts[idx].x, newVerts[idx].y+16);
	}
	
	var edgeCpy = [];
	for(var idx in edges){
		edgeCpy.push(edges[idx]);
	}
	edgeCpy.sort(function(a,b){return a.weight - b.weight;});
	
	var edgeInfo = "";
	
	ctx.fillStyle = "#111";
	
	for(var idx = 0; idx < edgeCpy.length; idx++){
		if(idx > 0){
			edgeInfo += ";";
		}
		edgeInfo += ("(" + edgeCpy[idx].v1.name + "," + edgeCpy[idx].v2.name + ")");
	}
	
	ctx.font = "24px serif";
	ctx.fillText(edgeInfo, 50,500);
	var cursorPos = 60 + 60*edgeIdx;
	ctx.fillText("^", cursorPos,524);
}

var edgeIdx = 0;

function NextEdge(){
	var description = document.getElementById('description-para');
	
	var forest = newVerts[0].forest;
	var done = true;
	for(var idx = 1; idx < newVerts.length; idx++){
		if(forest !== newVerts[idx].forest){
			done = false;
			break;
		}
	}
	
	if(done){
		description.innerHTML = "We're done adding new edges.";
		return;
	}
	
	var edgeCpy = [];
	for(var idx in edges){
		edgeCpy.push(edges[idx]);
	}
	
	edgeCpy.sort(function(a,b){return a.weight - b.weight;});
	var nextEdge = edgeCpy[edgeIdx];
	
	var v1 = FindNewNode(nextEdge.v1.name);
	var v2 = FindNewNode(nextEdge.v2.name);
	
	//Adding this node would create a cycle
	if(v1.forest === v2.forest){
		badEdges.push(new Edge(v1,v2,nextEdge.weight));
		description.innerHTML = "An edge between \"" + v1.name + "\" and \"" + v2.name + "\" would create a cycle, so we skip it.";
		edgeIdx++;
		return;
	}
	
	var forest1 = v1.forest;
	for(var idx in newVerts){
		if(newVerts[idx].forest === forest1){
			newVerts[idx].forest = v2.forest;
		}
	}
	
	description.innerHTML = "We added an edge between \"" + v1.name + "\" and \"" + v2.name + "\".";
	
	var newEdge = new Edge(v1,v2,nextEdge.weight);
	newEdges.push(newEdge);
	edgeIdx++;
}

function Update(){}

function StartOver(){
	document.getElementById('description-para').innerHTML = "Starting Kruskal's algorithm.";

	edgeIdx = 0;
	
	newEdges = [];
	badEdges = [];
	
	for(var idx in newVerts){
		newVerts[idx].forest = newVerts[idx];
	}
	
	for(var idx in edges){
		edges[idx].weight = Math.floor(Math.random() * 5 + 1);
	}
}

window.onload = function(){
	StartOver();
	
	var canvas = document.getElementById('main-canvas');
	var ctx = canvas.getContext('2d');
	setInterval(function(){Update();Render(ctx);}, 30);
};