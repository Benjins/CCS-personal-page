"use strict";

//Global array of language stats for all repos
var languageStats = [];

window.onload = function(){
	DisplayGraph(document.getElementById("lang-graph").getContext("2d"), {});
};

function DisplayGraph(ctx, numbers){
	var maxHeight = 0;
	var columnCount = 0;

	for(var name in numbers){
		maxHeight = Math.max(maxHeight, numbers[name]);
		columnCount++;
	}

	var columnWidth = (ctx.canvas.width / columnCount) * 0.5;
	var canvasHeight = ctx.canvas.height;
	var index = 0;
	
	ctx.font = "18px sans-serif";

	ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

	var rectData = [];

	for(var name in numbers){
		var percentHeight = numbers[name]/maxHeight * 0.8;
		var columnHeight = percentHeight * canvasHeight;
		var columnStartX = index * columnWidth * 2 + 10;
		var columnStartY = canvasHeight - columnHeight-1;
		
		ctx.fillStyle = "rgb(80,80,250)";
		ctx.fillRect(columnStartX, columnStartY, columnWidth, columnHeight);
		rectData[name] = {"x":columnStartX, "y":columnStartY,"width":columnWidth, "height":columnHeight};

		ctx.translate(columnStartX, columnStartY - 10);
		ctx.rotate(-0.1);
		ctx.fillStyle = "black";
		ctx.fillText(name, 6, 0, columnWidth * 1.5);

		ctx.setTransform(1,0,0,1,0,0);
		index++;
	}

	ctx.canvas.onmousemove = function(evt){
		var x = evt.clientX;
		var y = evt.clientY;
		var bounds = ctx.canvas.getBoundingClientRect();
		x -= bounds.left;
		y -= bounds.top;
	
		for(var name in rectData){
			var rect = rectData[name];
			var isHovered = x > rect.x 
						 && x < rect.x + rect.width 
						 && y > rect.y 
						 && y < rect.y + rect.height;

			if(isHovered){
				ctx.fillStyle = "rgb(20,20,220)";
			}
			else{
				ctx.fillStyle = "rgb(80,80,250)";
			}

			ctx.fillRect(rect.x, rect.y, rect.width, rect.height);
		}
	}

	ctx.beginPath(); 
	ctx.lineWidth="3";
	ctx.strokeStyle="black";
	ctx.moveTo(0,0);
	ctx.lineTo(0,ctx.canvas.height);
	ctx.lineTo(ctx.canvas.width, ctx.canvas.height);
	ctx.stroke();
}

function FetchAndDisplayRepoData(){
	var req = new XMLHttpRequest();
	req.onreadystatechange = function(){
		if (req.readyState==4 && req.status==200){
			DisplayRepos(JSON.parse(req.responseText));
		}
		else if(req.readyState==4){
			console.log("Error: Github repsonded with error code: " + req.status);
		}
	};
	req.open("GET", "https://api.github.com/users/benjins/repos", true);
	req.send();
}

function DisplayRepos(repoJson){
	var mainDiv = document.getElementById("stat-body");
	while (mainDiv.firstChild) {
		mainDiv.removeChild(mainDiv.firstChild);
	}
	mainDiv.appendChild(document.createElement("table"));
	mainDiv.firstChild.className = "gridtable";
	var firstRow = document.createElement("tr");
	mainDiv.firstChild.appendChild(firstRow);

	var header = document.createElement("th");
	header.innerHTML = "Repo Name";
	firstRow.appendChild(header);

	header = document.createElement("th");
	header.innerHTML = "Language";
	firstRow.appendChild(header);
	
	header = document.createElement("th");
	header.innerHTML = "# of Commits";
	firstRow.appendChild(header);
	
	for(var i in repoJson){
		var repo = repoJson[i];
		AddRepository(mainDiv.firstChild, repo.name, repo.html_url, repo.language);
	}
}

function AddRepository(tableElem, repoName, repoUrl, repoLanguage){
	var req = new XMLHttpRequest();
	req.open("GET", "https://api.github.com/repos/benjins/" + repoName + "/commits?author=benjins&per_page=100", true);

	var req2 = new XMLHttpRequest();
	req2.open("GET", "https://api.github.com/repos/benjins/" + repoName + "/languages", true);
	req2.onreadystatechange = function(){
		if (req2.readyState==4 && req2.status==200){
			var languages = JSON.parse(req2.responseText);
			for(var lang in languages){
				if(languageStats[lang] === null || languageStats[lang] === undefined){
					languageStats[lang] = languages[lang];
				}
				else{
					languageStats[lang] += languages[lang];
				}
			}
			
			UpdateLanguageStats();		
		}
	};

	req.onreadystatechange = function(){
		if (req.readyState==4 && req.status==200){
			var commits = JSON.parse(req.responseText);

			if(commits.length > 5){
				var listItem = document.createElement("tr");
				tableElem.appendChild(listItem);

				var nameData = listItem.appendChild(document.createElement("td"));
				nameData.innerHTML = "<a class='table-link' href='" + repoUrl + "'>" + repoName + "</a>";		

				var langData = listItem.appendChild(document.createElement("td"));
				langData.innerHTML = repoLanguage;	

				langData = listItem.appendChild(document.createElement("td"));
				langData.id = "repo_" + repoName;
				langData.innerHTML = commits.length;
			}
			
			if(commits.length === 100){
				UpdateCommitCount(repoName, 2);
			}
		}
	};
	
	req.send();
	req2.send();
}

function UpdateCommitCount(repoName, pageNum){
	var req2 = new XMLHttpRequest();
	req2.open("GET", "https://api.github.com/repos/benjins/" + repoName + "/commits?author=benjins&page=" + pageNum + "&per_page=100", true);
	
	req2.onreadystatechange = function(){
		if (req2.readyState==4 && req2.status==200){
			var commits = JSON.parse(req2.responseText);
			if(commits.length > 0){
				var elem = document.getElementById("repo_" + repoName);
				var commitCount = parseInt(elem.innerHTML) + commits.length;
				elem.innerHTML = commitCount;
			}
		
			if(commits.length === 100){
				UpdateCommitCount(repoName, pageNum + 1);
			}
		}
	};
	
	req2.send();
}

//TODO: create generic function that turns 2D array into table?

function UpdateLanguageStats(){
	var langTable = document.getElementById("lang-body").children[0];
	while (langTable.firstChild) {
		langTable.removeChild(langTable.firstChild);
	}

	var firstRow = document.createElement("tr");
	langTable.appendChild(firstRow);

	var header = document.createElement("th");
	header.innerHTML = "Language";
	firstRow.appendChild(header);

	header = document.createElement("th");
	header.innerHTML = "Volume (bytes)";
	firstRow.appendChild(header);

	for(var lang in languageStats){
		var row = document.createElement("tr");
		langTable.appendChild(row);

		var item = document.createElement("td");
		item.innerHTML = lang;
		row.appendChild(item);

		item = document.createElement("td");
		item.innerHTML = languageStats[lang];
		row.appendChild(item);
		//console.log(lang + languageStats[lang]);
	}

	DisplayGraph(document.getElementById("lang-graph").getContext("2d"), languageStats);
}
