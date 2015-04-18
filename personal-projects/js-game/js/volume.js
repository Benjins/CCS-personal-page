var toggle = true;
if(localStorage["volume"] && localStorage["volume"] < 1){
	toggle = false;
}
var musicPlayer = undefined;

function VolumeOnClick(vlm){
	if(!musicPlayer){
		musicPlayer = document.getElementById("music");
	}

	toggle = !toggle;
	if(toggle){
		vlm.style.background = "#ddd";
		musicPlayer.volume = 1;
	}
	else{
		vlm.style.background = "#faa";
		musicPlayer.volume = 0;
	}
}