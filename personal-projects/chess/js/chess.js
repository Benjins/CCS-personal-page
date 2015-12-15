var PieceTypes = {
	PAWN : 0,
	BISHOP : 1,
	KNIGHT : 2,
	ROOK : 3,
	KING : 4,
	QUEEN : 5
}

var chessPieceImages = {};

var selectedPiece = null;
var rightClickedPiece = null;

chessPieceImages[PieceTypes.PAWN]   = "#552";
chessPieceImages[PieceTypes.BISHOP] = "#E9E";
chessPieceImages[PieceTypes.KNIGHT] = "#AEA";
chessPieceImages[PieceTypes.ROOK]   = "#D12";
chessPieceImages[PieceTypes.KING]   = "#5D2";
chessPieceImages[PieceTypes.QUEEN]  = "#5DE";

var chessPieceValues = {};

chessPieceValues[PieceTypes.PAWN]   = 1;
chessPieceValues[PieceTypes.BISHOP] = 3;
chessPieceValues[PieceTypes.KNIGHT] = 2;
chessPieceValues[PieceTypes.ROOK]   = 4;
chessPieceValues[PieceTypes.KING]   = 15;
chessPieceValues[PieceTypes.QUEEN]  = 12;

var chessPiecePics = {};

chessPiecePics[PieceTypes.PAWN]   = [];
chessPiecePics[PieceTypes.BISHOP] = [];
chessPiecePics[PieceTypes.KNIGHT] = [];
chessPiecePics[PieceTypes.ROOK]   = [];
chessPiecePics[PieceTypes.KING]   = [];
chessPiecePics[PieceTypes.QUEEN]  = [];

var chessPieces = [];

var canvas;

function RemovePiece(piece){
	chessPieces.splice(chessPieces.indexOf(piece),1);
	console.log("Removed piece: " + piece);
}

function FindPiece(x, y){
	var foundPiece = null;
	for(var idx in chessPieces){
		if(chessPieces[idx].gridX === x && chessPieces[idx].gridY === y){
			foundPiece = chessPieces[idx];
			break;
		}
	}
	
	return foundPiece;
}

function pieceIsThreatened(pc){
	var defenders = [];
	var attackers = [];
	
	for(var idx = 0; idx < chessPieces.length; idx++){
		var moves = chessPieces[idx].GetMoves();
		for(var mi in moves){
			if(moves[mi].x === pc.gridX && moves[mi].y === pc.gridY){
				if(chessPieces[idx].color === pc.color){
					defenders.push(chessPieces[idx]);
					//console.log("Found defender");
				}
				else{
					attackers.push(chessPieces[idx]);
					//console.log("Found attacker");
				}
			}
		}
	}
	
	defenders.sort(function(a,b){return chessPieceValues[a.type] - chessPieceValues[b.type]});
	attackers.sort(function(a,b){return chessPieceValues[a.type] - chessPieceValues[b.type]});
	
	var atkVal = chessPieceValues[pc.type];
	for(var idx = 0; idx < attackers.length && idx < defenders.length; idx++){
		atkVal -= chessPieceValues[attackers[idx].type];
		atkVal += chessPieceValues[defenders[idx].type];
	}
	
	return atkVal;
}

function AnalyzeBoard(col){
	var captureMultiplier = 5;

	var benefit = 0;
	for(var idx in chessPieces){
		if(chessPieces.color == col){
			benefit -= chessPieceValues[chessPieces[idx].type] * captureMultiplier;
		}
		else{
			benefit += chessPieceValues[chessPieces[idx].type] * captureMultiplier;
		}
		
		var atkVal = pieceIsThreatened(chessPieces[idx]);
		if(chessPieces.color === col){
			benefit -= atkVal;
		}
		else{
			benefit += atkVal;
		}
	}
	
	return benefit;
}

function CreateChessPiece(startX, startY, type, color, gridSize){
	var newPiece = {};
	
	newPiece.gridX = startX;
	newPiece.gridY = startY;
	
	newPiece.x = startX * gridSize;
	newPiece.y = startY * gridSize;
	
	newPiece.gridSize = gridSize;
	
	newPiece.type = type;
	newPiece.color = color;
	
	newPiece.Render = function(ctx){
		ctx.fillStyle = chessPieceImages[type];
		
		ctx.fillRect(this.x + this.gridSize/4, this.y + this.gridSize/4, this.gridSize/2, this.gridSize/2);
		
		ctx.fillStyle = (this.color === 0) ? "#222" : "#ddd";
		ctx.fillRect(this.x + this.gridSize*3/8, this.y + this.gridSize*3/8, this.gridSize/8, this.gridSize/8);
	}
	
	chessPieces.push(newPiece);
	
	newPiece.Contains = function(x, y){
		return ((x - this.x) < this.gridSize && (y - this.y) < this.gridSize
			 && (x - this.x)  > 0&& (y - this.y) > 0);
	}
	
	newPiece.GetMoves = function(){
		var moves = [];
		
		if(this.type === PieceTypes.PAWN){
			var nextY = this.gridY + (this.color === 0 ? 1 : -1);
			var afterY = this.gridY + (this.color === 0 ? 2 : -2);
			
			var foundPiece  = FindPiece(this.gridX, nextY);
			var foundPiece2 = FindPiece(this.gridX, afterY);
			var foundPieceL = FindPiece(this.gridX - 1, nextY);
			var foundPieceR = FindPiece(this.gridX + 1, nextY);
					
			if(nextY >= 0 && nextY < 8 && foundPiece === null){
				moves.push({x: this.gridX, y: nextY});
			}
					
			if(this.gridY === (this.color === 0 ? 1 : 6) && foundPiece === null && foundPiece2 === null){
				moves.push({x: this.gridX, y: afterY});
			}
			
			if(foundPieceL !== null && foundPieceL.color !== this.color){
				moves.push({x: this.gridX - 1, y: nextY});
			}
			
			if(foundPieceR !== null && foundPieceR.color !== this.color){
				moves.push({x: this.gridX + 1, y: nextY});
			}
		}
		else if(this.type === PieceTypes.BISHOP){
			var dirs = [true, true, true, true];
			for(var mv = 1; mv < 8; mv++){
				var newX = [this.gridX + mv,this.gridX + mv,this.gridX - mv,this.gridX - mv];
				var newY = [this.gridY + mv,this.gridY - mv,this.gridY + mv,this.gridY - mv];
				
				for(var idx = 0; idx < 4; idx++){
					if(dirs[idx]){
						if(newX[idx] >= 0 && newX[idx] < 8 && newY[idx] >= 0 && newY[idx] < 8){
							var foundPiece = FindPiece(newX[idx], newY[idx]);
							if(foundPiece === null){
								moves.push({x: newX[idx], y: newY[idx]});
							}
							else if(foundPiece.color !== this.color){
								moves.push({x: newX[idx], y: newY[idx]});
								dirs[idx] = false;
							}
							else{
								dirs[idx] = false;
							}
						}
						else{
							dirs[idx] = false;
						}
					}
				}
			}
		}
		else if(this.type === PieceTypes.KNIGHT){
			var potentialMoves = [{x: this.gridX + 1, y: this.gridY+2},{x: this.gridX + 2, y: this.gridY+1},
								  {x: this.gridX + 1, y: this.gridY-2},{x: this.gridX + 2, y: this.gridY-1},
								  {x: this.gridX - 1, y: this.gridY+2},{x: this.gridX - 2, y: this.gridY+1},
								  {x: this.gridX - 1, y: this.gridY-2},{x: this.gridX - 2, y: this.gridY-1}];
			
			for(var idx = 0; idx < potentialMoves.length; idx++){
				var x = potentialMoves[idx].x;
				var y = potentialMoves[idx].y;
				
				if(x >= 0 && x < 8 && y >= 0 && y < 8){
					var foundPiece = FindPiece(x,y);
					if(foundPiece === null || foundPiece.color !== this.color){
						moves.push(potentialMoves[idx]);
					}
				}
			}
		}
		else if(this.type === PieceTypes.ROOK){
			var dirs = [true, true, true, true];
			for(var mv = 1; mv < 8; mv++){
				var newX = [this.gridX + mv,this.gridX - mv,this.gridX,     this.gridX];
				var newY = [this.gridY,     this.gridY,     this.gridY + mv,this.gridY - mv];
				
				for(var idx = 0; idx < 4; idx++){
					if(dirs[idx]){
						if(newX[idx] >= 0 && newX[idx] < 8 && newY[idx] >= 0 && newY[idx] < 8){
							var foundPiece = FindPiece(newX[idx], newY[idx]);
							if(foundPiece === null){
								moves.push({x: newX[idx], y: newY[idx]});
							}
							else if(foundPiece.color !== this.color){
								moves.push({x: newX[idx], y: newY[idx]});
								dirs[idx] = false;
							}
							else{
								dirs[idx] = false;
							}
						}
						else{
							dirs[idx] = false;
						}
					}
				}
			}
		}
		else if(this.type === PieceTypes.QUEEN){
			var dirs = [true, true, true, true, true, true, true, true];
			for(var mv = 1; mv < 8; mv++){
				var newX = [this.gridX + mv,this.gridX - mv,this.gridX,     this.gridX,      this.gridX + mv,this.gridX + mv,this.gridX - mv,this.gridX - mv];
				var newY = [this.gridY,     this.gridY,     this.gridY + mv,this.gridY - mv, this.gridY + mv,this.gridY - mv,this.gridY + mv,this.gridY - mv];
				
				for(var idx = 0; idx < 8; idx++){
					if(dirs[idx]){
						if(newX[idx] >= 0 && newX[idx] < 8 && newY[idx] >= 0 && newY[idx] < 8){
							var foundPiece = FindPiece(newX[idx], newY[idx]);
							if(foundPiece === null){
								moves.push({x: newX[idx], y: newY[idx]});
							}
							else if(foundPiece.color !== this.color){
								moves.push({x: newX[idx], y: newY[idx]});
								dirs[idx] = false;
							}
							else{
								dirs[idx] = false;
							}
						}
						else{
							dirs[idx] = false;
						}
					}
				}
			}
		}
		
		
		return moves;
	}
	
	newPiece.tryMove = function(newX, newY){
		//console.log(this.GetMoves());
		
		if(this.type === PieceTypes.PAWN){
			//console.log("before: " + this.gridY);
			//console.log("Pawn of col: " + this.color + " try move from (" + this.gridX + ", " + this.gridY + ") to (" + newX +  ", " + newY + ")" );
			if(newX === this.gridX){
				if(newY === this.gridY + (this.color === 0 ? 1 : -1)){
					var foundPiece = FindPiece(newX, newY);
					if(foundPiece === null){
						this.gridX = newX;
						this.gridY = newY;
						//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
					}
					//console.log(this);
				}
				else if(this.gridY === (this.color === 0 ? 1 : 6)){
					if(newY === this.gridY + (this.color === 0 ? 2 : -2)){
						var foundPiece = FindPiece(newX, newY);
						var foundPiece2 = FindPiece(newX, newY + (this.color === 0 ? -1 : 1));
						if(foundPiece === null && foundPiece2 === null){
							this.gridX = newX;
							this.gridY = newY;
							//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
						}
					}
				}
			}
			else if(newX === this.gridX - 1 || newX === this.gridX + 1){
				if(newY === this.gridY + (this.color === 0 ? 1 : -1)){
					var foundPiece = FindPiece(newX, newY);
					
					if(foundPiece !== null && foundPiece.color !== this.color){
						this.gridX = newX;
						this.gridY = newY;
						//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
						RemovePiece(foundPiece);
					}
				}
			}
			
			//console.log("after: " + this.gridY);
		}
		else if(this.type === PieceTypes.BISHOP){
			var xDiff = newX - this.gridX;
			var yDiff = newY - this.gridY;
			
			if(Math.abs(xDiff) === Math.abs(yDiff)){
				var xSign = Math.sign(xDiff);
				var ySign = Math.sign(yDiff);
				var valid = true;
				for(var i = 1; i < Math.abs(xDiff); i++){
					var isEmpty = (FindPiece(this.gridX + i*xSign, this.gridY + i*ySign) === null);
					
					if(!isEmpty){
						valid = false;
						break;
					}
				}
				
				var destPiece = FindPiece(newX, newY);
				valid = valid && (destPiece === null || destPiece.color != this.color);
				
				if(valid){
					this.gridX = newX;
					this.gridY = newY;
					//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
					
					if(destPiece){
						RemovePiece(destPiece);
					}
				}
			}
		}
		else if(this.type === PieceTypes.KNIGHT){
			var xDiff = newX - this.gridX;
			var yDiff = newY - this.gridY;
			
			var mult = xDiff * yDiff;
			if(Math.abs(mult) === 2){
				var destPiece = FindPiece(newX, newY);
				if(destPiece === null || destPiece.color !== this.color){
					this.gridX = newX;
					this.gridY = newY;
					
					if(destPiece){
						RemovePiece(destPiece);
					}
				}
			}
		}
		else if(this.type === PieceTypes.ROOK){
			var xDiff = newX - this.gridX;
			var yDiff = newY - this.gridY;
			
			if(xDiff === 0 || yDiff == 0){
				var isValid = true;
				if(xDiff !== 0){
					var xSign = Math.sign(xDiff);
					for(var i = 1; i < Math.abs(xDiff); i++){
						var pc = FindPiece(this.gridX + i*xSign, this.gridY);
						var isEmpty = (pc === null);
						
						if(!isEmpty){
							isValid = false;
							break;
						}
					}
				}
				else{
					var ySign = Math.sign(yDiff);
					for(var i = 1; i < Math.abs(yDiff); i++){
						var pc = FindPiece(this.gridX, this.gridY + i*ySign);
						var isEmpty = (pc === null);
						
						if(!isEmpty){
							isValid = false;
							break;
						}
					}
				}
				
				if(isValid){
					var destPiece = FindPiece(newX, newY);
					if(destPiece === null || destPiece.color !== this.color){
						this.gridX = newX;
						this.gridY = newY;
						//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
						
						if(destPiece){
							RemovePiece(destPiece);
						}
					}
				}

			}
		}  
		else if(this.type === PieceTypes.KING){
			var xDiff = newX - this.gridX;
			var yDiff = newY - this.gridY;
			
			if(Math.abs(xDiff) <= 1 && Math.abs(yDiff) <= 1){
				var destPiece = FindPiece(newX, newY);
				if(destPiece === null || destPiece.color !== this.color){
					this.gridX = newX;
					this.gridY = newY;
					//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
					
					if(destPiece){
						RemovePiece(destPiece);
					}
				}
			}
		} 
		else if(this.type === PieceTypes.QUEEN){
			var xDiff = newX - this.gridX;
			var yDiff = newY - this.gridY;
			
			if(xDiff === 0 || yDiff === 0 || Math.abs(xDiff) === Math.abs(yDiff)){
				var xSign = Math.sign(xDiff);
				var ySign = Math.sign(yDiff);
				var valid = true;
				for(var i = 1; i < Math.max(Math.abs(xDiff),Math.abs(yDiff)) ; i++){
					var pc = FindPiece(this.gridX + i*xSign, this.gridY + i*ySign);
					var isEmpty = (pc === null);
					
					if(!isEmpty){
						valid = false;
						break;
					}
				}
				
				var destPiece = FindPiece(newX, newY);
				valid = valid && (destPiece === null || destPiece.color != this.color);
				
				if(valid){
					this.gridX = newX;
					this.gridY = newY;
					//console.log("Accept move to: (" + newX + ", " + newY + ") on: ");
					
					if(destPiece){
						RemovePiece(destPiece);
					}
				}
			}
		}
		
	};
}

function DrawChessBoard(ctx){
	var squareWidth = ctx.canvas.width / 8;
	var squareHeight = ctx.canvas.height / 8;
	
	ctx.fillStyle = "#222";
	
	for(var x = 0; x < 8; x += 2){
		for(var y = 0; y < 8; y++){
			ctx.fillRect(squareWidth * (x + (1 - (y % 2))), squareHeight * y, squareWidth, squareHeight);
		}
	}
}

function Update(){
	for(var idx in chessPieces){
		if(chessPieces[idx] !== selectedPiece){
			chessPieces[idx].x = chessPieces[idx].gridX * chessPieces[idx].gridSize;
			chessPieces[idx].y = chessPieces[idx].gridY * chessPieces[idx].gridSize;
		} 
	}
}

function Render(ctx){
	var squareWidth = ctx.canvas.width / 8;
	var squareHeight = ctx.canvas.height / 8;
	
	ctx.clearRect(0,0,ctx.canvas.width,ctx.canvas.height);
	DrawChessBoard(ctx);
	
	if(selectedPiece !== null){
		var moves = selectedPiece.GetMoves();
		
		ctx.fillStyle = "#666";
	
		for(var idx in moves){
			var x = moves[idx].x;
			var y = moves[idx].y;
			ctx.fillRect(squareWidth * x, squareHeight * y, squareWidth, squareHeight);
		}
	}
	
	for(var idx in chessPieces){
		chessPieces[idx].Render(ctx);
	}
}

function MakeAIMove(col){
	var pieceMovesList = [];

	for(var idx in chessPieces){
		if(chessPieces[idx].color === col){
			var moves = chessPieces[idx].GetMoves();
			
			if(moves.length > 0){
				var pieceMoves = {piece: chessPieces[idx], moveList: moves};
				pieceMovesList.push(pieceMoves);
			}
		}
	}
	
	var bestMove = null;
	var bestMovePiece = null;
	var bestMoveBenefit = -10000;
	
	//console.log(pieceMovesList);
	
	for(var pcMoveIdx in pieceMovesList){
		var pcMove = pieceMovesList[pcMoveIdx];
		//console.log(pcMove);
		for(var mvIdx in pcMove.moveList){
			var mv = pcMove.moveList[mvIdx];
			
			var prevX = pcMove.piece.gridX;
			var prevY = pcMove.piece.gridY;
			
			var foundPiece = FindPiece(mv.x, mv.y);
			if(foundPiece !== null){
				chessPieces.splice(chessPieces.indexOf(foundPiece), 1);
			}
			
			pcMove.piece.gridX = mv.x;
			pcMove.piece.gridY = mv.y;
			
			var benefit = AnalyzeBoard(pcMove.piece.col);
			
			if(benefit > bestMoveBenefit){
				console.log("Found better move: " + bestMoveBenefit + " < " + benefit);
				bestMoveBenefit = benefit;
				bestMovePiece = pcMove.piece;
				bestMove = mv;
			}
			else if(benefit === bestMoveBenefit){
				if(Math.random() < 0.1){
					console.log("Random move of equal benefit: " + benefit);
					benefit = bestMoveBenefit;
					bestMovePiece = pcMove.piece;
					bestMove = mv;
				}
			}
			
			if(foundPiece !== null){
				chessPieces.push(foundPiece);
			}
			
			pcMove.piece.gridX = prevX;
			pcMove.piece.gridY = prevY;
		}
	}
	
	//console.log(bestMovePiece);
	//console.log(bestMove);
	bestMovePiece.tryMove(bestMove.x, bestMove.y);
}

window.onload = function(){
	canvas = document.getElementById("main-canvas");
	var canvasCtx = canvas.getContext("2d");
	
	for(var i = 0; i < 8; i++){
		CreateChessPiece(i, 1, PieceTypes.PAWN, 0, canvasCtx.canvas.height/8);
		CreateChessPiece(i, 6, PieceTypes.PAWN, 1, canvasCtx.canvas.height/8);
	}
	
	CreateChessPiece(0, 0, PieceTypes.ROOK, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(0, 7, PieceTypes.ROOK, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(7, 0, PieceTypes.ROOK, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(7, 7, PieceTypes.ROOK, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(1, 0, PieceTypes.KNIGHT, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(1, 7, PieceTypes.KNIGHT, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(6, 0, PieceTypes.KNIGHT, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(6, 7, PieceTypes.KNIGHT, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(2, 0, PieceTypes.BISHOP, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(2, 7, PieceTypes.BISHOP, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(5, 0, PieceTypes.BISHOP, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(5, 7, PieceTypes.BISHOP, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(4, 0, PieceTypes.QUEEN, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(4, 7, PieceTypes.QUEEN, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(3, 0, PieceTypes.KING, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(3, 7, PieceTypes.KING, 1, canvasCtx.canvas.height/8);
	
	window.onContextMenu = function(evt){
		var x = evt.pageX || evt.clientX;
		var y = evt.pageY || evt.clientY;
		x -= canvas.offsetLeft;
		y -= canvas.offsetTop;
		
		if(x < 800 && y < 800){
			return false;
		}
		
		return true;
	}
	
	window.onmousedown = function(evt){
		var x = evt.pageX || evt.clientX;
		var y = evt.pageY || evt.clientY;
		x -= canvas.offsetLeft;
		y -= canvas.offsetTop;
		
		var button = evt.which || evt.button;
		rightClickedPiece = null;
		
		for(var idx in chessPieces){
			if(chessPieces[idx].Contains(x, y)){
				if(button === 1){
					selectedPiece = chessPieces[idx];
				}
				else if(button === 3){
					rightClickedPiece = chessPieces[idx];
					evt.preventDefault();
				}
				break;
			}
		}
	};
	
	window.onmousemove = function(evt){
		var x = evt.pageX || evt.clientX;
		var y = evt.pageY || evt.clientY;
		x -= canvas.offsetLeft;
		y -= canvas.offsetTop;
		
		if(selectedPiece !== null){
			selectedPiece.x = x - selectedPiece.gridSize/2;
			selectedPiece.y = y - selectedPiece.gridSize/2;
		}
	};
	
	window.onmouseup = function(evt){
		if(selectedPiece !== null){
			var newGridX = Math.floor(selectedPiece.x / selectedPiece.gridSize + 0.5);
			var newGridY = Math.floor(selectedPiece.y / selectedPiece.gridSize + 0.5);
			
			selectedPiece.tryMove(newGridX, newGridY);
		}
		
		selectedPiece = null;
	};
	
	setInterval(function(){Update();Render(canvasCtx);}, 200);
}

function StartAutoPlay(){
	var turn = 0;
	window.autoPlayId = setInterval(function(){MakeAIMove(turn); turn = 1 - turn;}, 1200);
}

function StopAutoPlay(){
	clearInterval(window.autoPlayId);
}