var PieceTypes = {
	PAWN : 0,
	BISHOP : 1,
	KNIGHT : 2,
	ROOK : 3,
	KING : 4,
	QUEEN : 5
}

var chessPieceImages = {};

chessPieceImages[PieceTypes.PAWN]   = "#512";
chessPieceImages[PieceTypes.BISHOP] = "#E9E";
chessPieceImages[PieceTypes.KNIGHT] = "#AEA";
chessPieceImages[PieceTypes.ROOK]   = "#D12";
chessPieceImages[PieceTypes.KING]   = "#5D2";
chessPieceImages[PieceTypes.QUEEN]  = "#5DE";

var chessPieces = [];

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
	}
	
	chessPieces.push(newPiece);
	
	/*
	if(type == PieceTypes.PAWN){
		newPiece.canMove = function(){};
	}
	*/
}

function DrawChessBoard(ctx){
	var squareWidth = ctx.canvas.width / 8;
	var squareHeight = ctx.canvas.height / 8;
	
	ctx.fillStyle = "#111";
	
	for(var x = 0; x < 8; x += 2){
		for(var y = 0; y < 8; y++){
			ctx.fillRect(squareWidth * (x + (1 - (y % 2))), squareHeight * y, squareWidth, squareHeight);
		}
	}
}

function Update(){
	
}

function Render(ctx){
	ctx.clearRect(0,0,ctx.canvas.width,ctx.canvas.height);
	DrawChessBoard(ctx);
	
	for(var idx in chessPieces){
		chessPieces[idx].Render(ctx);
	}
}

window.onload = function(){
	var canvas = document.getElementById("main-canvas");
	var canvasCtx = canvas.getContext("2d");
	
	for(var i = 0; i < 8; i++){
		CreateChessPiece(i, 1, PieceTypes.PAWN, 0, canvasCtx.canvas.height/8);
		CreateChessPiece(i, 6, PieceTypes.PAWN, 1, canvasCtx.canvas.height/8);
	}
	
	CreateChessPiece(0, 0, PieceTypes.ROOK, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(0, 7, PieceTypes.ROOK, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(7, 0, PieceTypes.ROOK, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(7, 7, PieceTypes.ROOK, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(1, 0, PieceTypes.KNIGHT, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(1, 7, PieceTypes.KNIGHT, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(6, 0, PieceTypes.KNIGHT, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(6, 7, PieceTypes.KNIGHT, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(2, 0, PieceTypes.BISHOP, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(2, 7, PieceTypes.BISHOP, 0, canvasCtx.canvas.height/8);
	CreateChessPiece(5, 0, PieceTypes.BISHOP, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(5, 7, PieceTypes.BISHOP, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(4, 0, PieceTypes.QUEEN, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(4, 7, PieceTypes.QUEEN, 1, canvasCtx.canvas.height/8);
	
	CreateChessPiece(3, 0, PieceTypes.KING, 1, canvasCtx.canvas.height/8);
	CreateChessPiece(3, 7, PieceTypes.KING, 1, canvasCtx.canvas.height/8);
	
	setInterval(function(){Update();Render(canvasCtx);}, 200);
}