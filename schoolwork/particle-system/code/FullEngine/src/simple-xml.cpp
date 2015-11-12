/*
Author: Benji Smith
Licensed under MIT License. See License.txt for details
*/


#include "../header/ext/simple-xml.h"

void XMLElement::Print() const{
	cout << name << " ";
	for(auto iter = attributes.begin(); iter != attributes.end(); iter++){
		iter->Print();
		cout << " ";
	}
	for(auto iter = children.begin(); iter != children.end(); iter++){
		cout << endl;
		iter->Print();
	}
}

string XMLElement::SaveElement(string tabbing){
	string save = tabbing + "<" + name;
	for(auto iter = attributes.begin(); iter != attributes.end(); iter++){
		save = save + " " + iter->ToString(); 
	}

	if(children.size() == 0){
		save += "/>";
		return save;
	}
	else{
		save += ">\n";
			
		for(auto iter = children.begin(); iter != children.end(); iter++){
			save += (iter->SaveElement(tabbing + "  "));
			save += "\n";
		}

		save = save + (tabbing + "</" + name + ">");
		return save;
	}
}

void XMLDocument::Print() const{
	cout << "Document: " << name << endl;
	for(auto iter = contents.begin(); iter != contents.end(); iter++){
		iter->Print();
		cout << endl;
	}
}

vector<string> Tokenize(const string& document){
	vector<string> tokens;
	string memoryString = "";
	bool inString = false;
	bool inTag=false;
	bool inComment = false;

	for(int i = 0; i < document.size(); i++){
		char character = document[i];

		if(inComment){
			if(character == '?' && i < document.size() - 1 && document[i+1] == '>'){
				inComment = false;
				i++;
			}

			continue;
		}
		if(inString && character != '\''  && character != '"'){
			memoryString += character;
		}
		else if(character == '<' || character == '>' || character == '/' || character == '='){
			if(character == '<'){
				if(i < document.size() - 1 && document[i+1] == '?'){
					inComment = true;
					continue;
				}
				inTag = true;
			}
			else if(character == '>'){
				inTag = false;
				//cout << "inTag false.\n";
			}
			if(memoryString != ""){
				tokens.push_back(memoryString);
				memoryString = "";
			}
			string str = "-";
			str[0] = character;
			tokens.push_back(str);
		}
		else if(character == '\'' || character == '"'){
			if(!inTag){
				memoryString += character;
				inString = !inString;
			}
			else if(memoryString != "" || inString){
				tokens.push_back(memoryString);
				memoryString = "";
			}
			inString = !inString;
		}
		else if(character == ' ' || character == '\n' ||  character == '\t' || character == '\r'){
			if(character == ' ' && !inTag){
				bool justWhitespace = true;
				for(int i = 0; i < memoryString.size(); i++){
					if(memoryString[i] != ' '){
						justWhitespace = false;
						break;
					}
				}
				if(!justWhitespace){
					memoryString += character;
				}
			}
			else if(memoryString != ""){
				tokens.push_back(memoryString);
				memoryString = "";
			}
		}
		else{
			memoryString += character;
		}
	}

	return tokens;
}

XMLDocument ParseTokens(vector<string>& tokens){
	XMLDocument doc;

	Stack<string> tokenStack;
	Stack<XMLElement> elementStack;
	bool inTag = false;

	for(auto iter = tokens.begin(); iter != tokens.end(); iter++){
		string token = *iter;
		//cout << "Parsing token: |" << token << "|" << endl;
		if(token == "<"){
			inTag = true;
			tokenStack.Push(token);
		}
		else if(token == "/"){
			if(tokenStack.Top() == "<"){
				//Add the element to the doc
				XMLElement elem = elementStack.Pop();
				if(elementStack.count == 0){
					doc.contents.push_back(elem);
				}
				else{
					elementStack.Top().children.push_back(elem);
				}
				
				iter++;
				iter++;
			}
			else{
				tokenStack.Push(token);
			}
		}
		else if(token == ">"){
			inTag = false;
			bool startList = true;
			if(tokenStack.Top() == "/"){
				startList = false;
				tokenStack.Pop();
			}

			string currToken = tokenStack.Pop();
			XMLAttribute attr;
			attr.data = attr.name = "";
			bool gotData = false;
			while(currToken != "<"){
				if(currToken == "="){
					//do nothing
				}
				else if(!gotData){
					attr.data = currToken;
					gotData = true;
				}
				else{
					attr.name = currToken;
					elementStack.Top().AddAttribute(std::move(attr.name), std::move(attr.data));
					attr.data = attr.name = "";
					gotData = false;
				}
				currToken = tokenStack.Pop();
			}

			if(!startList){
				XMLElement elem = elementStack.Pop();
				if(elementStack.count == 0){
					doc.contents.push_back(elem);
				}
				else{
					elementStack.Top().children.push_back(elem);
				}
			}
		}
		else{
			if(!inTag){
				XMLElement elem;
				elem.name = "__plaintext__";
				elem.AddAttribute("val", std::move(token));
				if(elementStack.count == 0){
					doc.contents.push_back(elem);
				}
				else{
					elementStack.Top().children.push_back(elem);
				}
				continue;
			}
			else if(tokenStack.Top() == "<"){
				XMLElement elem;
				elem.name = token;
				//cout << "Pushing to element stack: " << token << endl;
				elementStack.Push(elem);
			}
			//cout << "Pushing token: "<< token << endl;
			tokenStack.Push(token);
		}
	}

	return doc;
}

void SaveXMLDoc(XMLDocument& doc, string fileName){
	ofstream fileOut;
	fileOut.open(fileName.c_str());

	if(!fileOut.good()){
		cout << "Failed to open file " << fileName << " for saving.\n";
		return;
	}

	string docContents = "";

	for(auto iter = doc.contents.begin(); iter != doc.contents.end(); iter++){
		docContents += iter->SaveElement();
		docContents += "\n";
	}

	fileOut << docContents;
}


void LoadXMLDoc(XMLDocument& doc, string fileName){
	ifstream file;
	string fileContents;

	file.open(fileName.c_str());

	if(!file.good()){
		cout << "Error opening file: " << fileName << endl;
		return;
	}

	while(!file.eof()){
		string line;
		std::getline(file,line,'\n');
		fileContents = fileContents + line + "\n";
	}

	vector<string> tokens = Tokenize(fileContents);
	for(auto iter = tokens.begin(); iter != tokens.end(); iter++){
		//cout << "|" << *iter << "|\n";
	}
	doc = ParseTokens(tokens);
}