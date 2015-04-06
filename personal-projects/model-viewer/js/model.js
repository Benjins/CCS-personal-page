var Model = function(verts, faces){
	this.verts = verts;
	this.faces = faces;
}

var Vector3 = function(x,y,z){
	this.x = x;
	this.y = y;
	this.z = z;
}

var Vector2 = function(x,y){
	this.x = x;
	this.y = y;
}

var canvasMouseCoords = {"x":0, "y":0};

function SetUpScroll(){
	var myCanv = document.getElementById("model-canvas");
	if (myCanv.addEventListener) {
		// IE9, Chrome, Safari, Opera
		myCanv.addEventListener("mousewheel", onCanvasScroll, false);
		// Firefox
		myCanv.addEventListener("DOMMouseScroll", onCanvasScroll, false);
	}
	// IE 6/7/8
	else{ 
		myCanv.attachEvent("onmousewheel", onCanvasScroll);
	}
}

function ParseOBJFile(file){
	var vertices = [];
	var faces = [];
	var uvs = [];
	var lines = file.split("\n");
	for(var i in lines){
		var line = lines[i];
		if(line.length < 2){
			continue;
		}
		else if(line[0] === 'v' && line[1] === 't'){
			var data = line.split(" ");
			var uv = new Vector2(parseFloat(data[1]), parseFloat(data[2]));
			uvs.push(uv);
		}
		else if(line[0] === 'v'){
			var data = line.split(" ");
			var vertPos = new Vector3(parseFloat(data[1]), parseFloat(data[2]), parseFloat(data[3]));
			vertices.push(vertPos);
		}
		else if(line[0] === 'f'){
			var data = line.split(" ");
			if(data[1].split("/").size > 0){
				var verts = [0,0,0];
				var uvs = [0,0,0]
				for(var i = 0; i < 3; i++){
					var vertData = data[i+1].split("/");
					verts[i] = parseInt(vertData[0]) - 1;
					uvs[i] = parseInt(vertData[1]) - 1;
					
					var face;
					face.verts = verts;
					face.uvs = uvs;
					faces.push(face);
				}
				
				var face= {};
				face.verts = verts;
				faces.push(face);
			}
			else{
				var verts = [0,0,0];
				for(var i = 0; i < 3; i++){
					verts[i] = parseInt(data[i+1]) - 1;
				}
				
				var face = {};
				face.verts = verts;
				faces.push(face);
			}
		}
	}
	
	return new Model(vertices, faces);
}

var gl;
    function initGL(canvas) {
    	SetUpScroll();
        try {
            gl = canvas.getContext("experimental-webgl");
            gl.viewportWidth = canvas.width;
            gl.viewportHeight = canvas.height;
        } catch (e) {
        }
        if (!gl) {
            alert("Could not initialise WebGL, sorry :-(");
        }
    }
    function getShader(gl, id) {
        var shaderScript = document.getElementById(id);
        if (!shaderScript) {
            return null;
        }
        var str = "";
        var k = shaderScript.firstChild;
        while (k) {
            if (k.nodeType == 3) {
                str += k.textContent;
            }
            k = k.nextSibling;
        }
        var shader;
        if (shaderScript.type == "x-shader/x-fragment") {
            shader = gl.createShader(gl.FRAGMENT_SHADER);
        } else if (shaderScript.type == "x-shader/x-vertex") {
            shader = gl.createShader(gl.VERTEX_SHADER);
        } else {
            return null;
        }
        gl.shaderSource(shader, str);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            alert(gl.getShaderInfoLog(shader));
            return null;
        }
        return shader;
    }
    
    var shaderProgram;
    function initShaders() {
        var fragmentShader = getShader(gl, "shader-fs");
        var vertexShader = getShader(gl, "shader-vs");
        shaderProgram = gl.createProgram();
        gl.attachShader(shaderProgram, vertexShader);
        gl.attachShader(shaderProgram, fragmentShader);
        gl.linkProgram(shaderProgram);
        if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
            alert("Could not initialise shaders");
        }
        gl.useProgram(shaderProgram);
        shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "vertexPos");
        gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);
        shaderProgram.uvAttribute = gl.getAttribLocation(shaderProgram, "uv");
        //gl.enableVertexAttribArray(shaderProgram.uvAttribute);
        shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
        shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    }
    
    var mvMatrix = mat4.create();
    var pMatrix = mat4.create();
    function setMatrixUniforms() {
        gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
        gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
    }
   
    var triangleVertexPositionBuffer;
    function initBuffers() {
    	    
    	triangleVertexPositionBuffer = gl.createBuffer();
        
    	
    	var modelFile = new XMLHttpRequest();
    	modelFile.open("GET", "data/monkey.obj", true);
    	modelFile.onreadystatechange = function(){
		if (modelFile.readyState==4 && (modelFile.status==200 || modelFile.status==0)){
			
			var model = ParseOBJFile(modelFile.responseText);
			
			gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
			var vertices = [
			     0.0,  1.0,  0.0,
			    -1.0, -1.0,  0.0,
			     1.0, -1.0,  0.0
			];
			
			for(var i = 0; i < model.faces.length; i++){
				for(var j = 0; j < 3; j++){
					vertices.push(model.verts[model.faces[i].verts[j]].x);
					vertices.push(model.verts[model.faces[i].verts[j]].y);
					vertices.push(model.verts[model.faces[i].verts[j]].z);
				}
			}
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
			triangleVertexPositionBuffer.itemSize = 3;
			triangleVertexPositionBuffer.numItems = Math.round(vertices.length / 3);
			console.log(vertices.length / 3);
		}
		else if(modelFile.readyState==4){
			console.log("Error: Server repsonded with error code: " + modelFile.status);
		}
	}
	
	modelFile.send();
	
        
    }
    var rotX = 0;
    var rotY = 0;
    
    var currX = 0;
    var currY = 0;
    
	var mouseDown = false;

    function onCanvasDragStart(evt){
		//console.log(evt);
    	evt.dataTransfer.setData('text/plain', 'This text may be dragged');
		//console.log(evt);
		var x, y;
		if(evt.x && evt.y){
			x = evt.x;
			y = evt.y;
		}
		else{
			x = evt.pageX;
			y = evt.pageY;
		}
    	currX = x;
    	currY = y;
		evt.preventDefault();

		mouseDown = true;
		
		return false;
    }
    
    function onCanvasDrag(evt){
		//evt.dataTransfer.setData('text/plain', 'This text may be dragged');
    	var x = evt.x;
        var y = evt.y;
		//console.log(evt);
    	if(navigator.userAgent.toLowerCase().indexOf('firefox') > -1){
    		x = canvasMouseCoords.x;
    		y = canvasMouseCoords.y;
    	}
    	    
    	if(evt.x != 0 || evt.y != 0){
    		rotY += currY - y;
    		rotX -= currX - x;
    	
    		currX = x;
    		currY = y;
    	}
    }
    
	function OnCanvasMouseOver(evt){
		//console.log("OnCanvasMouseOver");
		if(mouseDown){
			var x, y;
			if(evt.x && evt.y){
				x = evt.x;
				y = evt.y;
			}
			else{
				x = evt.pageX;
				y = evt.pageY;
			}
			
			rotY += currY - y;
    		rotX -= currX - x;
    	
    		currX = x;
    		currY = y;
		}
	}

	function OnCanvasDragEnd(evt){
		//console.log("OnCanvasDragEnd");
		mouseDown = false;
	}

    var zoom = -7;
    
    function onCanvasScroll(evt){
		var delta = evt.wheelDeltaY ? evt.wheelDeltaY : evt.detail * -20;
    	if(zoom < -0.2 || delta/120 < 0){
    		zoom +=  delta/250;
    	}
    }
    
    function drawScene() {
    	//rot = rot + 0.1;
        gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        mat4.perspective(45, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0, pMatrix);
        mat4.identity(mvMatrix);
        mat4.translate(mvMatrix, [-1, 0.0, zoom]);
        mat4.rotate(mvMatrix, Math.PI/90*rotY, [1, 0, 0]); 
        mat4.rotate(mvMatrix, Math.PI/40*rotX, [0, 1, 0]);
        gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
        gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, triangleVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
        setMatrixUniforms();
        gl.drawArrays(gl.TRIANGLES, 0, triangleVertexPositionBuffer.numItems);
    }
    function webGLStart() {
        var canvas = document.getElementById("model-canvas");
        initGL(canvas);
        initShaders();
        initBuffers();
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.enable(gl.DEPTH_TEST);
        setInterval(drawScene, 15);
    }
