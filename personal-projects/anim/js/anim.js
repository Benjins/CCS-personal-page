var rotKeys1 = [{t: 0, val: 0}, {t: 1, val: 1}, {t: 2, val: 1.5}, {t: 2.5, val: 0.4}, {t: 3, val: 0}];
var scaleKeys1 = [{t: 0, val: 1}, {t: 1, val: 1.4}, {t: 2, val: 0.8}, {t: 2.5, val: 1.1}, {t: 3, val: 1}];
var xKeys1 = [{t: 0, val: 100}, {t: 0.6, val: 180}, {t: 3, val: 100}];
var yKeys1 = [{t: 0, val: 100}, {t: 1.5, val: 120}, {t: 3, val: 90}, {t: 3.6, val: 100}];

var xCameraKeys = [{t: 0, val: 0}, {t: 2, val: -30}, {t: 4, val: 0}];
var yCameraKeys = [{t: 0, val: 0}, {t: 5, val: -60}, {t: 7, val: 0}];


var scaleKeys2 = [{t: 0, val: 1}, {t: 2, val: 1.3}, {t: 2.4, val: 2}, {t: 2.5, val: 1.4}, {t: 3, val: 1}];

var scene = {};
scene.camera = {x: -100, y: -100};
scene.objects = [{x: 0, y: 0, rot: 0, scale: 1, rotKeys: rotKeys1, scaleKeys: scaleKeys1, xKeys : xKeys1, yKeys: yKeys1},
				 {x: 220, y: 405, rot: 40, scale: 2.4, scaleKeys: scaleKeys2},
				 {x: 200, y: 100, rot: 0, scale: 4}];

var time = 0;

function Evaluate(keys, time){
	var length = keys[keys.length - 1].t;
	time = time % length;
	
	for(var idx = 0; idx < keys.length; idx++){
		if(keys[idx].t >= time){
			if(idx == 0){
				return keys[idx].val;
			}
			else{
				var fromVal = keys[idx-1].val;
				var toVal = keys[idx].val;
				
				var weight = (time - keys[idx-1].t)/(keys[idx].t - keys[idx-1].t);
				
				return fromVal * (1 - weight) + toVal * weight;
			}
		}
	}
	
	return null;
}

function Render(ctx){
	//console.log("Render time: " + time);
	ctx.clearRect(0,0,600,600);
	ctx.save();
	
	scene.camera.x = Evaluate(xCameraKeys, time);
	scene.camera.y = Evaluate(yCameraKeys, time);
	
	ctx.fillstyle = '#444';
	
	ctx.translate(-scene.camera.x, -scene.camera.y);
	
	for(var idx in scene.objects){
	
		var obj = scene.objects[idx];
		
		if(obj.xKeys){
			obj.x = Evaluate(obj.xKeys, time);
		}
		
		if(obj.yKeys){
			obj.y = Evaluate(obj.yKeys, time);
		}
		
		if(obj.rotKeys){
			obj.rot = Evaluate(obj.rotKeys, time);
		}
		
		if(obj.scaleKeys){
			obj.scale = Evaluate(obj.scaleKeys, time);
		}
	
		ctx.save();
		
		ctx.translate(obj.x, obj.y);
		ctx.rotate(obj.rot);
		ctx.scale(obj.scale, obj.scale);
		
		ctx.fillRect(0,0,20,20);
		
		ctx.restore();
	}
	
	ctx.restore();
}

window.onload = function(){
	var canvas = document.getElementById('main-canvas');
	var ctx = canvas.getContext('2d');
	setInterval(function(){time += 0.03;Render(ctx);}, 30);
};