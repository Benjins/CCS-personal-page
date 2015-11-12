/*
Author: Benji Smith
Licensed under MIT License. See License.txt for details
*/

#ifndef SIMPLE_XML_H
#define SIMPLE_XML_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using std::string; using std::vector; using std::ifstream;
using std::cout; using std::endl; using std::ofstream;
using std::map;

#define MAX_STACK_SIZE 8192

template<typename T>
struct Stack{
	T* values;//[MAX_STACK_SIZE];
	int count;

	Stack(){
		count = 0;
		values = new T[MAX_STACK_SIZE];
	}

	void Push(const T& newValue){ 
		values[count] = newValue;
		count++;
	}

	T Pop(){
		count--;
		return values[count];
	}

	T& Top(){
		return values[count-1];
	}

	~Stack(){
		delete[] values;
	}
};

struct XMLAttribute{
	string name;
	string data;

	XMLAttribute()
	: name(), data(){
	}

	XMLAttribute(const string& _name, const string& _data)
		: name(_name), data(_data){
	}

	XMLAttribute(const string&& _name, const string&& _data)
		: name(_name), data(_data){	
	}

	void Print() const{
		cout << name << ":" << data;
	}

	string ToString(){
		return name + "='" + data + "'";
	}
};

struct XMLElement{
	string name;
	vector<XMLAttribute> attributes;
	map<string, string> attributeMap;
	vector<XMLElement> children;

	void AddAttribute(const string& name, const string& data){
		attributes.emplace_back(name, data);
		attributeMap.insert(std::make_pair(name, data));
	}

	void AddAttribute(const string&& name, const string&& data){
		attributes.emplace_back(name, data);
		attributeMap.insert(std::make_pair(name, data));
	}

	void Print() const;

	string SaveElement(string tabbing = "");
};

struct XMLDocument{
	string name;
	vector<XMLElement> contents;

	void Print() const;
};

vector<string> Tokenize(const string& document);

XMLDocument ParseTokens(vector<string>& tokens);

void SaveXMLDoc(XMLDocument& doc, string fileName);

void LoadXMLDoc(XMLDocument& doc, string fileName);


#endif