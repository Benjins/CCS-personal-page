function sendUserComment(user, comment){
	var commentObj = {};
	commentObj['user'] = user;
	commentObj['comment'] = comment;

	var req = new XMLHttpRequest();
	req.open("POST", "http://45.55.247.145/comments/upload.php");
	req.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
	req.send(JSON.stringify(commentObj));
	console.log(JSON.stringify(commentObj));
}

function sendComment(){
	var nameField = document.getElementById('nameField');
	var commentField = document.getElementById('commentField');

	sendUserComment(nameField.value, commentField.value);
}

function getComments(){
	var req = new XMLHttpRequest();
	req.open("GET", "http://45.55.247.145/comments/upload.php", true);
	req.onreadystatechange = function() {
	    if (req.readyState == 4) {
		showComments(JSON.parse(req.responseText));
	    }
	};

	req.send();
}

function showComments(commentList){
	var commentListElem = document.getElementById('comments-list');
	
	var children = commentListElem.children;
	for(var idx = 0; idx < children.length; idx++){
		console.log(commentListElem);
		console.log(children[idx]);
		commentListElem.removeChild(children[idx]);
	}

	for(var idx in commentList){
		var cmmt = commentList[idx];
		var cmmtNode = document.createElement('li');
		cmmtNode.innerHTML = cmmt.user + ": '" + cmmt.comment + "'";
		commentListElem.appendChild(cmmtNode);
	}
}


