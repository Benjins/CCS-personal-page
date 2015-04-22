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
		document.getElementById("volume").style.background = "url('img/volume-button-on.gif')";
		musicPlayer.volume = 1;
	}
	else{
		document.getElementById("volume").style.background = "url('img/volume-button-off.gif')";
		musicPlayer.volume = 0;
	}
}