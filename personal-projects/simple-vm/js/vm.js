window.onload = function(){
	
}

var lastEntered = "";

function enteredText(ele) {
    if(event.keyCode == 13) {
    	lastEntered = ele.value;
        console.log(VMEval(ele.value));
	ele.value = "";        
    }
    else if(event.keyCode == 38) {
    	ele.value = lastEntered;
    }
}

function VMEval(input){
	var tokens = Tokenise(input);
	var root = ShuntingYard(tokens);
	return root.eval();
}

function Tokenise(input){
	var tokenList = [];
	var tokenString = "";
	var length = input.length;
	for(var i = 0; i < length; i++){
		var currChar = input.charAt(i);
		if(currChar === ' ' || currChar === '\n' || currChar === '\t' || currChar === '\r'){
			if(tokenString !== ""){
				tokenList.push(tokenString);
				tokenString = "";
			}
		}
		else if(currChar === '+' || currChar === '*' || currChar === '(' || currChar === ')' || currChar === ';'){
			if(tokenString !== ""){
				tokenList.push(tokenString);
				tokenString = "";
			}
			var str = "" + currChar;
			tokenList.push(str);
		}
		else{
			tokenString += currChar;
		}
	}
	
	if(tokenString !== ""){
		tokenList.push(tokenString);
	}
	
	return tokenList;
}

function ShuntingYard(tokens){
	var outputStack = [];
	var operatorStack = [];
	var precedence = {"+":2, "*":3};
	var length = tokens.length;
	for(var i = 0; i < length; i++){
		var token = tokens[i];
		if(token === "("){
			operatorStack.push(token);
		}
		else if(token === ")"){
			
		}
		else if(token === "*" || token === "+"){
			if(operatorStack.length > 0 && precedence[operatorStack[operatorStack.length - 1]] < precedence[token]){
				 if(token === "*"){
				 	 console.log("Ouput operator: " + token);
				 	 outputStack.push(new Mult(outputStack.pop(), outputStack.pop()));
				 }
				 else if(token === "+"){
				 	 console.log("Ouput operator: " + token);
				 	 outputStack.push(new Add(outputStack.pop(), outputStack.pop()));
				 }
			}
			else{
				console.log("Push operator: " + token);
				operatorStack.push(token);
			}
		}
		else if(token === ";"){
			
		}
		else if(parseInt(token) !== NaN){
			outputStack.push(new Literal(parseInt(token)));
		}
		else{
			console.log("Odd token: " + token);
		}
	}
	
	while(operatorStack.length > 0){
		var op = operatorStack.pop();
		if(op === "*"){
			console.log("Ouput end operator: " + op);
			outputStack.push(new Mult(outputStack.pop(), outputStack.pop()));
		}
		else if(op === "+"){
			console.log("Ouput end operator: " + op);
			outputStack.push(new Add(outputStack.pop(), outputStack.pop()));
		}
	}
	
	return outputStack[0];
}


var Mult = function(left, right){
	this.left = left;
	this.right = right;
}

var Add = function(left, right){
	this.left = left;
	this.right = right;
}

var Literal = function(value){
	this.value = value;
}

Mult.prototype.eval = function(){
	return this.left.eval() * this.right.eval();
}

Add.prototype.eval = function(){
	return this.left.eval() + this.right.eval();
}

Literal.prototype.eval = function(){
	return this.value;
}


