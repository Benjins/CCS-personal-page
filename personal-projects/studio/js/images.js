document.getElementById('imgUp').onchange = function (evt) {
    var tgt = evt.target || window.event.srcElement,
        files = tgt.files;

    // FileReader support
    if (FileReader && files && files.length) {
        var fr = new FileReader();
        fr.onload = function () {
			AddSprite("spr1", fr.result);
        }
        fr.readAsDataURL(files[0]);
    }

    // Not supported
    else {
        // fallback -- perhaps submit the input to an iframe and temporarily store
        // them on the server until the user's session ends.
    }
}

function downloadImages(){
	var imgList = document.getElementById('imgList');
	var tempCanv = document.createElement('canvas');
	var ctx = tempCanv.getContext('2d');
	
	var zip = new JSZip();
	var images = zip.folder("images");
	
	for(var i = 0; i < imgList.children.length; i++){
		var imgElem = imgList.children[i];
		tempCanv.width = imgElem.naturalWidth;
		tempCanv.height = imgElem.naturalHeight;
		ctx.drawImage(imgElem, 0, 0, tempCanv.width, tempCanv.height);
		
		var imgData = tempCanv.toDataURL().substr(22); //Get just the base64 string, not the url bit.
		var imgZipFile = images.file("img" + i + ".png", imgData, {base64: true});
	}
	
	var content = zip.generate({type:"blob"});
	console.log(content);
	saveAs(content, "example.zip");
}