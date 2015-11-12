#ifndef SOURCE_PARSER_H
#define SOURCE_PARSRE_H

#include <vector>

using std::vector;

#pragma once

enum struct SourceTokenType{
	IDENTIFIER,
	OPERATOR,
	STRING,
	NUMBER,
	EMPTY
};

struct Token{
	char* start;
	int length;
	SourceTokenType type;
};

void ParseSourceFiles(char** sourceFileNames, int sourceFileCount);

vector<Token> LexFileContents(char* fileContents, int fileLength);



#endif