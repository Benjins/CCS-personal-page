var snapshots = [];

var SnapShot = function(changes, date, author, descr){
	this.changes = changes;
	this.date = date;
	this.author = author;
	this.description = descr;
}

var Change = function(file, line, before, after){
	this.file = file;
	this.line = line;
	this.before = before;
	this.after = after;
}

var changes = [ new Change("main.tex", 13, "{Intro}", "{\text Introduction}"),
				new Change("main.tex", 17, "{\em This is what we should be doing.}", "{\em This is not what we should not be doing.}")];

function CreateSnapshot(){
	var descrVal = document.getElementById('description-field').value;
	if(descrVal !== ""){
		var changesCopy = [];
		for(var idx in changes){
			changesCopy.push(changes[idx]);
		}
		snapshots.push(new SnapShot(changesCopy, new Date(), "Marek Potomek", descrVal));
		changes.length = 0;
		ReloadChangesUI();
		document.getElementById('description-field').value = "";
	}
	else{
		document.getElementById('snapshot-dialog').style.visibility = 'visible';
		document.getElementById('description-field-modal').focus();
	}
}

function ModalOnKey(evt){
	if(evt.keyCode === 13){
		DialogOK();
	}
}

function DialogOK(){
	var descrVal = document.getElementById('description-field-modal').value;
	document.getElementById('snapshot-dialog').style.visibility = 'hidden';
	var changesCopy = [];
		for(var idx in changes){
			changesCopy.push(changes[idx]);
		}
	snapshots.push(new SnapShot(changesCopy, new Date(), "Marek Potomek", descrVal));
	changes.length = 0;
	ReloadChangesUI();
}

function DialogCancel(){
	document.getElementById('snapshot-dialog').style.visibility = 'hidden';
}

function GoToHistory(){
	window.location.href = "history.html";
}

function GoToChanges(){
	window.location.href = "index.html";
}

function ReloadChangesUI(){
	var changeList = document.getElementById("change-list");
	if(changeList){
		while (changeList.firstChild) {
			changeList.removeChild(changeList.firstChild);
		}
		for(var idx = 0; idx < changes.length; idx++){
			var change = changes[idx];
			
			var changeElem = document.createElement('div');
			changeElem.className = "change-elem";
			changeList.appendChild(changeElem);
			
			var changeAuthor = document.createElement('p');
			var changeDate = document.createElement('p');
			var changeText = document.createElement('p');
			
			changeAuthor.innerHTML = "<b>File:</b> " + change.file;
			changeDate.innerHTML = "<b>Date:</b> " + change.date + "<br/>";
			changeText.innerHTML = "<b>Changes:</b> &nbsp;&nbsp;" 
										+ "<span style='background:#eaa;width:50%;overflow:hidden;'>" + change.before + " " + "</span>" 
										+ "<span style='background:#aea;width:50%;overflow:hidden;'>" + " " + change.after  + "</span>";
			
			changeElem.appendChild(changeAuthor);
			changeElem.appendChild(changeDate);
			changeElem.appendChild(changeText);
		}
	}
}

function ReloadHistoryUI(){
	var snapshotList = document.getElementById("snapshot-list");
	if(snapshotList){
		for(var idx in snapshots){
			var snapShot = snapshots[idx];
			var snapShotElem = document.createElement('div');
			snapShotElem.className = "snapshot-elem";
			
			snapShotElem.setAttribute("selected", "false");
			snapShotElem.onclick = function(){
				console.log("On Click:  " + this);
				if(this.getAttribute("selected") === "true"){
					this.className = "snapshot-elem";
					this.setAttribute("selected", "false");
				}
				else{
					
					this.className = "snapshot-elem snapshot-elem-select";
					this.setAttribute("selected", "true");
				}
			};
			
			var snapShotAuthor = document.createElement('p');
			var snapShotDate = document.createElement('p');
			var changesTitle = document.createElement('p');
			var snapShotDescription = document.createElement('p');
			
			snapShotAuthor.innerHTML = "Author: " + snapShot.author;
			snapShotDate.innerHTML = "Date: " + snapShot.date;
			snapShotDescription.innerHTML = "Description: " + snapShot.description + "<br/>";
			changesTitle.innerHTML = "Changes:";
			
			snapShotElem.appendChild(snapShotAuthor);
			snapShotElem.appendChild(snapShotDate);
			snapShotElem.appendChild(snapShotDescription);
			snapShotElem.appendChild(changesTitle);
			
			for(var i = 0; i < 3; i++){
				if(i < snapShot.changes.length){
					var snapShotChange = document.createElement('p');
					snapShotChange.innerHTML = "<span style='background:#eaa;width:50%;overflow:hidden;'>" + snapShot.changes[i].before + " " + "</span>" 
											 + "<span style='background:#aea;width:50%;overflow:hidden;'>" + " " + snapShot.changes[i].after + "</span>";
					snapShotElem.appendChild(snapShotChange);
				}
			}
			
			
			
			
			snapshotList.appendChild(snapShotElem);
		}
	}
}

window.onload = function(){
	if(localStorage.snapshots){
		snapshots = JSON.parse(localStorage.snapshots);
	}
	
	ReloadHistoryUI();
	ReloadChangesUI();
}

window.onunload = function(){
	localStorage.snapshots = JSON.stringify(snapshots);
}

