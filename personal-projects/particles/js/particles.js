var Vector2 = function(x,y){this.x = x; this.y = y;};

var particles = [];
var particleCount = 0;

var particleSystem = {};

particleSystem.startPosition = new Vector2(400,400);
particleSystem.startVelocty = new Vector2(0,100);
particleSystem.startSize = 10;
particleSystem.endSize = 0;
particleSystem.startCol = {"r":220,"g":100,"b":100,"a":1};
particleSystem.endCol = {"r":20,"g":80,"b":200,"a":0};
particleSystem.gravityFactor = -50;
particleSystem.particlesPerSecond = 20;
particleSystem.mass = 10;
particleSystem.maxLifetime = 10;
particleSystem.timeSinceSpawn = 0;
particleSystem.time = 0;

function Reset(){
	particleCount = 0;
	particleSystem.timeSinceSpawn = 0;
	particleSystem.time = 0;
}

function AddParticle(){
	particleCount++;
	
	var newParticle = {};
	newParticle.position = new Vector2(particleSystem.startPosition.x, particleSystem.startPosition.y);
	newParticle.velocity = new Vector2(particleSystem.startVelocty.x, particleSystem.startVelocty.y);
	newParticle.velocity.x += (Math.random()*50-25);
	newParticle.velocity.y += Math.random(20);
	newParticle.col = particleSystem.startCol;
	newParticle.size = particleSystem.startSize;
	newParticle.age = 0;
	
	particles[particleCount-1] = newParticle;
}

function RemoveParticle(idx){
	particles[idx] = particles[particleCount-1];
	particleCount--;
}

function LerpRGB(c1,c2,t){
	return {r: c1.r*(1-t)+c2.r*t, g: c1.g*(1-t)+c2.g*t, b: c1.b*(1-t)+c2.b*t, a: c1.a*(1-t)+c2.a*t};
}

function Update(dt){
	particleSystem.time += dt;
	particleSystem.timeSinceSpawn += dt;
	//console.log(particleSystem.timeSinceSpawn);
	//console.log(particleCount);

	while(particleSystem.timeSinceSpawn >= (1/particleSystem.particlesPerSecond)){
		particleSystem.timeSinceSpawn -= (1/particleSystem.particlesPerSecond);
		AddParticle();
	}

	for(var i = 0; i < particleCount; i++){
		particles[i].age += dt;
		if(particles[i].age >= particleSystem.maxLifetime){
			//console.log("remove particle");
			RemoveParticle(i);
			i--;
			continue;
		}
		
		var ageRatio = particles[i].age / particleSystem.maxLifetime;
		//console.log(ageRatio);
		
		particles[i].size = particleSystem.startSize * (1 - ageRatio) + particleSystem.endSize * ageRatio;
		particles[i].col = LerpRGB(particleSystem.startCol, particleSystem.endCol, ageRatio);
		
		particles[i].velocity.y += particleSystem.gravityFactor*dt;
		
		particles[i].position.x += particles[i].velocity.x*dt;
		particles[i].position.y += particles[i].velocity.y*dt;
		//console.log( particles[i].velocity.y*dt);
	}
	
	
	for(var i = 0; i < particleCount; i++){
		for(var j = i+1; j < particleCount; j++){
			var xDiff = particles[i].position.x - particles[j].position.x;
			var yDiff = particles[i].position.y - particles[j].position.y;
			
			var dist = Math.sqrt(xDiff*xDiff+yDiff*yDiff);
			var radii = particles[i].size + particles[j].size;
			var overlap = dist - radii;
			
			if(overlap < 0){
				xDiff = xDiff / dist * overlap/2;
				yDiff = yDiff / dist * overlap/2;
				
				particles[i].position.x -= xDiff;
				particles[i].position.y -= yDiff;
				
				particles[i].velocity.x -= xDiff;
				particles[i].velocity.y -= yDiff;
				                                
				particles[j].position.x += xDiff;
				particles[j].position.y += yDiff;
				                                
				particles[j].velocity.x += xDiff;
				particles[j].velocity.y += yDiff;
			}
		}
	}
	
}

function Render(ctx){
	ctx.clearRect(0,0,800,800);
	
	ctx.fillStyle = "#222";

	for(var i = 0; i < particleCount; i++){
		ctx.fillStyle = "rgba(" + Math.floor(particles[i].col.r) + "," + Math.floor(particles[i].col.g) + "," + Math.floor(particles[i].col.b) + "," + particles[i].col.a + ")";
		ctx.beginPath();
		ctx.arc(particles[i].position.x, 800 - particles[i].position.y, particles[i].size,0,2*Math.PI);
		ctx.fill();
	}
}

window.onload = function(){
	var canvas = document.getElementById('main-canvas');
	var ctx = canvas.getContext('2d');
	window.updateParticles = true;
	setInterval(function(){if(window.updateParticles){Update(0.03);}Render(ctx);}, 30);
};