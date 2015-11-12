function RunScene(scnName){
	var scn = window.project.scenes[scnName];
	
	var pref1 = CreatePrefab();
	pref1.sprite = "spr1";
	pref1.AddComponent({time : 0, Update : function(dt){this.time += dt;this.ent.x = Math.cos(this.time)*50+50}});
	pref1.rotation = 20;
	AddPrefab("pref1", pref1);
	scn.AddPrefabInstance("pref1", 60, 40);
	
	scn.Load();
	scn.running = true;
	
	/*
	var ent1 = CreateEntity("ent1");
	ent1.sprite = "spr1";
	window.project.currentScene.entities.push(ent1);
	ent1.AddComponent({time : 0, Update : function(dt){this.time += dt;this.ent.x = Math.cos(this.time)*50+50}});
	*/
	
	var canv = document.getElementById('scene-canvas');
	var ctx = canv.getContext('2d');
	
	var intervalId = setInterval(function(){
		ctx.setTransform(1, 0, 0, 1, 0, 0);
		scn.Update(0.03);
		scn.Render(ctx);
		
		if(!scn.running){
			clearInterval(intervalId);
		}
	}, 30);
}