var Entity = function(x, y, radius, color){
			this.x = x;
			this.y = y;
			this.r = radius;
			this.col = color;

			this.onTick = function(dt){};
			this.onClick = function(x, y, btn){};
			this.onMouse = function(x, y, btn){};
			this.render = function(ctx){
				ctx.beginPath();
			 	ctx.fillStyle = this.col;
			 	ctx.arc(this.x, this.y, this.r, 0, 2 * Math.PI, false);
			 	ctx.fill();
			};
		};		

		KeyState = {PRESS:0, HOLD:1, RELEASE:3, UP:4};

		var canvas;
		var context;
		var entities = [];
		var inputKeys = [];
		for(var i = 0; i < 255; i++){
			inputKeys[i] = KeyState.UP;
		}

		function OnKeyUp(key){
			return inputKeys[key.charCodeAt(0)] === KeyState.RELEASE;
		}

		function OnKeyDown(key){
			return inputKeys[key.charCodeAt(0)] === KeyState.PRESS;
		}

		function OnKey(key){
			//console.log(inputKeys[key]);
			return inputKeys[key.charCodeAt(0)] === KeyState.PRESS || inputKeys[key.charCodeAt(0)] === KeyState.HOLD;
		}

		function EndOfFrameKeys(){
			for(var i = 0; i < 255; i++){
				if(inputKeys[i] === KeyState.PRESS){
					inputKeys[i] = KeyState.HOLD;
				}
				if(inputKeys[i] === KeyState.RELEASE){
					inputKeys[i] = KeyState.UP;
					console.log(inputKeys[i]);
				}
			}
		}
		
		window.onbeforeunload = function(){
			localStorage["audio"]  = document.getElementById("music").currentTime;
			localStorage["volume"] = document.getElementById("music").volume;
		};
		
		window.onload = function(){
			if(localStorage["audio"]){
				document.getElementById("music").currentTime = localStorage["audio"];
			}
			if(localStorage["volume"]){
				document.getElementById("music").volume = localStorage["volume"];
				if(localStorage["volume"] < 1){
					document.getElementById("volume").style.background = "#faa";
				}
			}
			canvas = document.getElementById("main-canvas");
			context = canvas.getContext("2d");
			if(context === null || context === undefined){
				alert("Your browser does not support 2D canvas rendering.");
				return;
			}

			var newEnt = new Entity(window.innerWidth/2, window.innerHeight - 60, 20, "#e38");
			newEnt.pressed = false;
			var speed = 70;
			
			var spawnEntity = function(x, y, player){
				var entSpawn = new Entity(x, y, 16, "#ccc");
				entSpawn.player = player;
				entSpawn.runAway = false;
				entSpawn.onTick = function(dt){
					var playerDist = Math.sqrt(Math.pow(this.x - this.player.x, 2) + Math.pow(this.y - this.player.y, 2));
					if(playerDist < 80){
						this.runAway = true;
					}
					else if(playerDist > 120){
						this.runAway = false;
					}
					
					if(this.runAway){
						//console.log("Running away");
						var moveX = (this.x - this.player.x) / playerDist;
						var moveY = (this.y - this.player.y) / playerDist;
						this.x += moveX * dt * speed * 1.5;
						this.y += moveY * dt * speed * 1.5;
						//return;
					}
					
					for(var idx in entities){
						if(entities[idx] === this || entities[idx] === this.player){
							continue;
						}
						var dist = Math.sqrt(Math.pow(this.x - entities[idx].x, 2) + Math.pow(this.y - entities[idx].y, 2));
						if(dist < (this.r + entities[idx].r) && dist > 2){
							var moveX = (this.x - entities[idx].x) / dist;
							var moveY = (this.y - entities[idx].y) / dist;
							this.x += moveX;
							this.y += moveY;
						}	
					}
				};
				entities.push(entSpawn);
			};
			
			/*
			for(var i = 0; i < 20; i++){
				spawnEntity((0.4*Math.random() + 0.2) * window.innerWidth, (0.4*Math.random() + 0.2) * window.innerHeight, newEnt);
			}
			*/
			
			
			
			if(typeof entityTable !== 'undefined'){
				for(var idx in entityTable){
					spawnEntity(entityTable[idx].x * window.innerWidth, entityTable[idx].y * window.innerHeight, newEnt);
				}
			}
			
			if(typeof blockTable !== 'undefined'){
				for(var idx in blockTable){
					var blockEnt = new Entity(blockTable[idx].x * window.innerWidth, 
											  blockTable[idx].y * window.innerHeight, 
											  blockTable[idx].r, "rgba(120, 120, 120, 1.0)");
					entities.push(blockEnt);
					
				}
			}
			
			if(typeof doorTable !== 'undefined'){
				for(var idx in doorTable){
					var doorEnt = new Entity(doorTable[idx].x * window.innerWidth, 
											  doorTable[idx].y * window.innerHeight, 
											  0, "#eee");
					doorEnt.link = doorTable[idx].link;
					doorEnt.player = newEnt;
					doorEnt.w = 100;
					doorEnt.h = 40;
					doorEnt.render = function(ctx){
						//ctx.beginPath();
						//ctx.rect(this.x-this.w/2, this.y-this.h/2, this.w, this.h);
						ctx.drawImage(document.getElementById("door"), this.x-this.w/2, this.y-this.h/2, this.w, this.h);
						//ctx.fillStyle = this.col;
			 			//ctx.fill();
					};
					doorEnt.onTick = function(dt){
						if(Math.abs(this.x - this.player.x) < this.w/2 + this.player.r 
						&& Math.abs(this.y - this.player.y) < this.h/2 + this.player.r){
							window.location.href = this.link;
						}
					};
					entities.push(doorEnt);
					
				}
			}
			
			newEnt.onTick = function(dt){
				if(OnKey("W") || OnKey("&")){
					this.y -= dt * speed;
				}
				if(OnKey("S") || OnKey("(")){
					this.y += dt * speed;
				}
				if(OnKey("A") || OnKey("%")){
					this.x -= dt * speed;
				}
				if(OnKey("D") || OnKey("'")){
					this.x += dt * speed;
				}
				
				if(OnKey("T")){
					if(!pressed){
						spawnEntity(this.x, this.y+20, this);
						pressed = true;
					}
				}
				else{
					pressed = false;
				}
			};
			entities.push(newEnt);

			document.onkeydown = function(evt){
				var key = evt.which || evt.keyCode;
				inputKeys[key] = KeyState.PRESS;
			};

			document.onkeyup = function(evt){
				var key = evt.which || evt.keyCode;
				inputKeys[key] = KeyState.RELASE;
			};

			setInterval(function(){GameLoop(0.017);}, 17);
		};

		function GameLoop(dt){
			for(var idx in entities){
				entities[idx].onTick(dt);
			}

			RenderScene(context);

			EndOfFrameKeys();
		}

		function RenderScene(ctx){
			ctx.canvas.width  = window.innerWidth;
  			ctx.canvas.height = window.innerHeight;
			ctx.clearRect(0,0,window.innerWidth,window.innerHeight);
			for(var idx in entities){
				entities[idx].render(ctx);
			}
		}