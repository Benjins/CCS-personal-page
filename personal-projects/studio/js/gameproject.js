function CreateProject(){
	var proj = {};
	proj.sprites = {};
	proj.scenes = {};
	proj.scripts = {};
	proj.prefabs = {};
	proj.currentScene = {};
	
	proj.Load = function(){
		for(var idx in this.sprites){
			var image = new Image();
			
			console.log(this.sprites[idx]);
			image.src = "data:image/png;base64," + this.sprites[idx].imageData;
			this.sprites[idx].image = image;
		}
	};
	
	return proj;
}

function CreateScene(name){
	var newScn = {};
	newScn.entities = [];
	newScn.prefabInstances = [];
	
	newScn.Update = function(dt){
		for(var idx in this.entities){
			this.entities[idx].Update(dt);
		}
	};
	
	newScn.Render = function(ctx){
		ctx.clearRect(0,0,ctx.canvas.width, ctx.canvas.height);
		for(var idx in this.entities){
			this.entities[idx].Render(ctx);
		}
	};
	
	newScn.AddPrefabInstance = function(prefab, x, y){
		var prefabInst = {prefab : prefab, x : x, y : y};
		this.prefabInstances.push(prefabInst);
	}
	
	newScn.Load = function(){
		window.project.Load();
		
		for(var idx in this.prefabInstances){
			var prefab = window.project.prefabs[this.prefabInstances[idx].prefab];
			var newEnt = prefab.Instance();
			newEnt.x = this.prefabInstances[idx].x;
			newEnt.y = this.prefabInstances[idx].y;
			this.entities.push(newEnt);
		}
	};
	
	window.project.scenes[name] = newScn;
	
	return newScn;
}

window.project = CreateProject();
window.project.currentScene = CreateScene("startup__scene");

function AddSprite(name, imgData){
	window.project.sprites[name] = {imageData : imgData.substr(22)};
	return window.project.sprites[name];
}

function AddPrefab(name, prefab){
	window.project.prefabs[name] = prefab;
	return window.project.prefabs[name];
}

function CreateEntity(name){
	var ent = {};
	ent.name = name;
	ent.comps = [];
	ent.sprite = null;
	
	ent.x = 0;
	ent.y = 0;
	ent.rotation = 0;
	ent.scale = {x:1, y:1};
	
	ent.Update = function(dt){
		for(var idx in this.comps){
			this.comps[idx].Update(dt);
		}
	};
	
	ent.Render = function(ctx){
		if(this.sprite !== null){
			ctx.setTransform(1, 0, 0, 1, 0, 0);
			ctx.translate(this.x, this.y);
			ctx.rotate(this.rotation*Math.PI/180);
			ctx.scale(this.scale.x, this.scale.y);
			
			var image = window.project.sprites[this.sprite].image;
			ctx.drawImage(image, 0, 0, 100, 100);
		}
	};
	
	ent.AddComponent = function(comp){
		comp.ent = this;
		this.comps.push(comp);
	};
	
	return ent;
}

function CreatePrefab(name){
	var prefab = CreateEntity(name);
	
	prefab.Instance = function(){
		var newEnt = CreateEntity(name + "(clone)");
		
		newEnt.sprite = this.sprite;
		newEnt.rotation = this.rotation;
		newEnt.scale.x = this.scale.x;
		newEnt.scale.y = this.scale.y;
		
		for(var idx in this.comps){
			newEnt.AddComponent(this.comps[idx]);
		}
		
		return newEnt;
	};
	
	return prefab;
}

function CreateInstance(prefabName){
	var prefab = window.project.prefabs[prefabName];
	if(prefab === null || prefab === undefined){
		console.error("Null or undefined prefab with name '" + prefabName + "'.");
	}
	else{
		window.project.currentScene.entities.push(prefab.Instance());
	}
}

function SaveProject(fileName){
	var projectObj = {};
	projectObj.sprites = {};
	for(var idx in window.project.sprites){
		projectObj.sprites[idx] = {};
		projectObj.sprites[idx].imageData = window.project.sprites[idx].imageData;
	}
	
	projectObj.scripts = window.project.scripts;
	
	projectObj.prefabs = {};
	for(var idx in window.project.prefabs){
		var prefSave = {};
		prefSave.sprite = window.project.prefabs[idx].sprite;
		prefSave.rotation = window.project.prefabs[idx].rotation;
		prefSave.scale = window.project.prefabs[idx].scale;
		projectObj.prefabs[idx] = prefSave;
	}
	
	projectObj.scenes = {};
	for(var idx in window.project.scenes){
		var sceneSave = {};
		sceneSave.prefabInstances = window.project.scenes[idx].prefabInstances;
		projectObj.scenes[idx] = sceneSave;
	}
	
	console.log(projectObj);
	var serialized = JSON.stringify(projectObj);
	
	window.projectSave = serialized;
}

function LoadProject(fileName){
	if(window.projectSave){
		var parsedSave = JSON.parse(window.projectSave);
		
		
		
		window.project.sprites = parsedSave.sprites;
		window.project.scripts = parsedSave.scripts;
		window.project.prefabs = parsedSave.prefabs;
		window.project.scenes = [];
		
		for(var idx in parsedSave.scenes){
			var scn = CreateScene(idx);
			scn.prefabInstances = parsedSave.scenes[idx].prefabInstances;
			window.project.scenes.push(scn);
		}
		
		console.log(window.project.scenes);
	}
}

