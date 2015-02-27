//Global array of language stats for all repos
var languageStats = [];

function hello_alert(){
	var req = new XMLHttpRequest();
	req.onreadystatechange = function(){
		if (req.readyState==4 && req.status==200){
			DisplayRepos(JSON.parse(req.responseText));
			//document.getElementById("stat-body").innerHTML=req.responseText;
		}
		else if(req.readyState==4){
			console.log("Error: Github repsonded with error code: " + req.status);
		}
	}
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

	console.log("Repo count: " + repoJson.length);
	
	var repoNames = [];
	
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
	}

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

				var langData = listItem.appendChild(document.createElement("td"));
				langData.id = "repo_" + repoName;
				langData.innerHTML = commits.length;
			}
			
			if(commits.length === 100){
				UpdateCommitCount(repoName, 2);
			}
		}
	}
	
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
	}
	
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
}
