function MyNashvilleFilter(srcImgData, destImgData, width, height){
	var size = width * height * 4;
	
	var colTrans = [
					[1.22,  -0.08,  -0.09],
					[-0.06,  0.88,  0.21],
					[-0.12,  0.25,  0.86]
				   ];
				   
	for(var idx = 0; idx < size; idx += 4){
	
		for(var pix = 0; pix < 3; pix++){
			var total = 0;
			
			for(var offset = 0; offset < 3; offset++){
				total += colTrans[pix][offset] * srcImgData.data[idx+offset];
			}
			
			destImgData.data[idx+pix] = total;
		}
		
		var brightness = srcImgData.data[idx] + srcImgData.data[idx+1] + srcImgData.data[idx+2];
		brightness /= 3;
		
		var lumShiftEffect = 0.05;
		destImgData.data[idx] += (brightness-0.3)*lumShiftEffect;
		destImgData.data[idx+1] -= (brightness-0.3)*lumShiftEffect;
		destImgData.data[idx+2] -= (brightness-0.3)*lumShiftEffect;
		
		var pow = 3;
		
		var remap = function(x){
			var coefficient = 0.5/Math.pow(0.5, pow);
			return 255*(coefficient*Math.pow(x/255 - 0.5,pow)+0.5);
		};
		
		var weight = -0.3;
		
		var lightnessFX = 0.1;
		
		for(var pix = 0; pix < 3; pix++){
			destImgData.data[idx+pix] = destImgData.data[idx+pix] * (1-weight) + weight*remap(destImgData.data[idx+pix]);
			destImgData.data[idx+pix] = destImgData.data[idx+pix]*(1-lightnessFX) + 255*lightnessFX;
		}
		
		var avg = 0;
		for(var pix = 0; pix < 3; pix++){
			avg += destImgData.data[idx+pix];
		}
		
		avg /= 3;
		
		var desat = 0.2;
		
		for(var pix = 0; pix < 3; pix++){
			destImgData.data[idx+pix] = destImgData.data[idx+pix] * (1-desat) + avg * desat;
		}
		
		for(var pix = 0; pix < 3; pix++){
			if(destImgData.data[idx+pix] < 50){
				destImgData.data[idx+pix] = (destImgData.data[idx+pix]/255)*(destImgData.data[idx+pix]/255)*255;
			}
		}
		
		destImgData.data[idx+3] = 255;
		
	}
}

function Saturation(srcImgData, destImgData, width, height){
	var size = width * height * 4;
	
	for(var idx = 0; idx < size; idx += 4){
		var avg = 0;
		for(var pix = 0; pix < 3; pix++){
			avg += srcImgData.data[idx+pix];
		}
		
		avg /= 3;
		
		var saturation = 0;
		for(var pix = 0; pix < 3; pix++){
			saturation += Math.abs(srcImgData.data[idx+pix] - avg);
		}
		
		for(var pix = 0; pix < 3; pix++){
			destImgData.data[idx+pix] = saturation;
		}
		
		destImgData.data[idx+3] = 255;
	}
}

function Histogram(srcImgData, destImgData, width, height){
	var size = width * height * 4;
	
	var histoR = [];
	var histoG = [];
	var histoB = [];
	
	var histos = [histoR, histoG, histoB];
	
	for(var i = 0; i < 3; i++){
		for(var idx = 0; idx < 256; idx++){
			histos[i][idx] = 0;
		}
	}
	
	for(var idx = 0; idx < size; idx += 4){
		for(var pix = 0; pix < 3; pix++){
			histos[pix][Math.floor(srcImgData.data[idx+pix])]++;
		}
	}
	
	var max = 0;
	for(var pix = 0; pix < 3; pix++){
		var histo = histos[pix];
		
		for(var i = 0; i < histo.length; i++){
			max = Math.max(max, histo[i]);
		}
	}
	
	
	for(var idx = 0; idx < 256; idx++){
		for(var pix = 0; pix < 3; pix++){
			var amt = histos[pix][idx];
			var norm = amt/max;
			
			var barHeight = norm*height;
			for(var y = 0; y < barHeight; y++){
				for(var x = idx * 2; x < idx*2+2; x++){
					var index = 4 * ((height-y) * width + x);
					destImgData.data[index+pix] = 255;
					destImgData.data[index+3] = 255;
				}
			}
		}
		
		
	}
}

function ProcessImages(){
	for(var i = 1; i <= 6; i++){
		var imageDiv = document.getElementById("image" + i);
		
		if(imageDiv !== null){
			var img = imageDiv.getElementsByTagName('img')[0];
			
			var imgCanvas = document.createElement('canvas');
			imgCanvas.width = img.width;
			imgCanvas.height = img.height;
			
			var ctx = imgCanvas.getContext('2d');
			ctx.drawImage(img, 0, 0, img.width, img.height);
			
			var destImgData = ctx.createImageData(img.width, img.height);
			var srcImgData = ctx.getImageData(0, 0, img.width, img.height);
			MyNashvilleFilter(srcImgData, destImgData, img.width, img.height);
			
			ctx.putImageData(destImgData, 0, 0);
			
			var histoCanvas = document.createElement('canvas');
			histoCanvas.width = img.width;
			histoCanvas.height = img.height;
			var ctx2 = histoCanvas.getContext('2d');
			var histImgData = ctx2.getImageData(0, 0, img.width, img.height);
			
			Histogram(srcImgData, histImgData, img.width, img.height);
			ctx2.putImageData(histImgData, 0, 0);
			
			var histoCanvas2 = document.createElement('canvas');
			histoCanvas2.width = img.width;
			histoCanvas2.height = img.height;
			var ctxNash = histoCanvas2.getContext('2d');
			var histImgData3 = ctxNash.getImageData(0, 0, img.width, img.height);
			
			var nashImg = imageDiv.getElementsByTagName('img')[1];
			ctxNash.drawImage(nashImg, 0, 0, nashImg.width, nashImg.height);
			var nashImgData = ctxNash.getImageData(0, 0, img.width, img.height);
			
			Histogram(nashImgData, histImgData3, img.width, img.height);
			ctxNash.putImageData(histImgData3, 0, 0);
			
			var histoCanvas3 = document.createElement('canvas');
			histoCanvas3.width = img.width;
			histoCanvas3.height = img.height;
			var ctx3 = histoCanvas3.getContext('2d');
			var histImgData3 = ctx3.getImageData(0, 0, img.width, img.height);
			
			Histogram(destImgData, histImgData3, img.width, img.height);
			ctx3.putImageData(histImgData3, 0, 0);
			
			var satCanvs = [];
			var srcDatas = [srcImgData, nashImgData, destImgData];
			for(var satIdx = 0; satIdx < 3; satIdx++){
				var satCanv = document.createElement('canvas');
				satCanv.width = img.width;
				satCanv.height = img.height;
				var ctxSat = satCanv.getContext('2d');
				var satImgData = ctxSat.getImageData(0, 0, img.width, img.height);
				
				Saturation(srcDatas[satIdx], satImgData, img.width, img.height);
				ctxSat.putImageData(satImgData, 0, 0);
				
				satCanvs.push(satCanv);
			}
			
			imageDiv.appendChild(imgCanvas);
			imageDiv.appendChild(document.createElement('br'));
			imageDiv.appendChild(histoCanvas);
			imageDiv.appendChild(histoCanvas2);
			imageDiv.appendChild(histoCanvas3);
			imageDiv.appendChild(document.createElement('br'));
			
			for(var satIdx = 0; satIdx < 3; satIdx++){
				imageDiv.appendChild(satCanvs[satIdx]);
			}
		}
	}
}

window.onload = function(){
	
	ProcessImages();
}