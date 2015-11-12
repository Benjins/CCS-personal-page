#include "../header/int/SourceParser.h"
#include "../header/int/Macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ParseSourceFiles(char** sourceFileNames, int sourceFileCount){
	
	char* generatedSource = (char*)malloc(1024*1024*4);
	char* genCursor = generatedSource;
	
	genCursor += sprintf(genCursor, "#ifndef META_TYPES_H\n");
	genCursor += sprintf(genCursor, "#define META_TYPES_H\n");
	genCursor += sprintf(genCursor, "#include \"../header/int/Macros.h\"\n");
	genCursor += sprintf(genCursor, "#include \"../header/int/MetaTypeInfo.h\"\n");

	genCursor += sprintf(genCursor, "#if defined(_MSC_VER)\n");
	genCursor += sprintf(genCursor, "#define OFFSET_OF(type,field) static_cast<int>((size_t)&(((type*)0)->field))\n");
	genCursor += sprintf(genCursor, "#else\n");
	genCursor += sprintf(genCursor, "#include <stddef.h>\n");
	genCursor += sprintf(genCursor, "#define OFFSET_OF offsetof\n");
	genCursor += sprintf(genCursor, "#endif\n");

	vector<Token> additionalMetaTypes;
	vector<Token> additionalMetaTypesWithoutFields;

	char* fundamentalTypes[] = {"int", "float", "Vector2", "Vector3", "string", "bool"};

	vector<char*> fileStrings;

	for(int fileIdx = 0; fileIdx < sourceFileCount; fileIdx++){
		FILE* sourceFile = fopen(sourceFileNames[fileIdx], "rb");

		fseek(sourceFile, 0, SEEK_END);
		int fileSize = (int)ftell(sourceFile);
		fseek(sourceFile, 0, SEEK_SET);

		char* fileBuffer = (char*)malloc(fileSize+1);
		fread(fileBuffer, 1, fileSize, sourceFile);
		fileBuffer[fileSize] = '\0';

		fclose(sourceFile);

		vector<Token> tokens = LexFileContents(fileBuffer, fileSize);

		for(int i = 0; i < tokens.size(); i++){
			Token token = tokens[i];

			if(memcmp(token.start, "struct", 6) == 0){
				if(memcmp(tokens[i+2].start, ":", 1) == 0){
					bool inheritsFromComponent = false;

					for(Token additionalType : additionalMetaTypes){
						if(tokens[i+3].length == additionalType.length
						&& memcmp(tokens[i+3].start, additionalType.start, additionalType.length) == 0){
							inheritsFromComponent = true;
							break;
						}
					}

					if(!inheritsFromComponent){
						for(Token additionalType : additionalMetaTypesWithoutFields){
							if(tokens[i+3].length == additionalType.length
							&& memcmp(tokens[i+3].start, additionalType.start, additionalType.length) == 0){
								inheritsFromComponent = true;
								break;
							}
						}
					}

					if(memcmp(tokens[i+3].start, "Component", 9) == 0 || inheritsFromComponent){
						if(tokens[i+4].type == SourceTokenType::OPERATOR && memcmp(tokens[i+4].start, "{", 1) == 0){
							Token compTypeToken = tokens[i+1];

							printf("Found component: '%.*s'\n", compTypeToken.length, compTypeToken.start);

							char* oldCursor = genCursor;
							bool hasFields = false;
							genCursor += sprintf(genCursor, "MetaMemberInfo memberInfo_%.*s[] = {\n", compTypeToken.length, compTypeToken.start);

							int braceCount = 1;
							for(i = i + 5; braceCount > 0 ; i++){
								if(tokens[i].type == SourceTokenType::OPERATOR){
									if(memcmp(tokens[i].start, "{", 1) == 0){
										braceCount++;
									}
									else if(memcmp(tokens[i].start, "}", 1) == 0){
										braceCount--;
									}
								}
								else if(braceCount == 1){
									bool isFundamentalType = false;
									for(int idx = 0; idx < ARRAY_COUNT(fundamentalTypes); idx++){
										if(strncmp(tokens[i].start, fundamentalTypes[idx], tokens[i].length) == 0){
											isFundamentalType = true;
											break;
										}
									}

									if(isFundamentalType && memcmp(tokens[i-1].start, "DontSerialize", 13) != 0){
										if(memcmp(tokens[i+2].start, ";", tokens[i+2].length) == 0){
											printf("Found member field: '%.*s' of type '%.*s'\n", tokens[i+1].length, tokens[i+1].start, tokens[i].length, tokens[i].start);
											
											Token fieldName = tokens[i+1];
											Token typeName = tokens[i];

											hasFields = true;

											genCursor += sprintf(genCursor, "\t{\"%.*s\", OFFSET_OF(%.*s, %.*s), MetaType_%.*s},\n",
												fieldName.length, fieldName.start, 
												compTypeToken.length, compTypeToken.start,
												fieldName.length, fieldName.start,
												typeName.length, typeName.start);
										}
									}
								}
							}

							if(hasFields){
								genCursor += sprintf(genCursor, "};\n");
								additionalMetaTypes.push_back(compTypeToken);
							}
							else{
								genCursor = oldCursor;
								additionalMetaTypesWithoutFields.push_back(compTypeToken);
							}


						}
					}
				}
			}
		}

		int xxy = 0;

		fileStrings.push_back(fileBuffer);
	}


	genCursor += sprintf(genCursor, "enum MetaStructTypes {\n");
	for(Token token : additionalMetaTypes){
		genCursor += sprintf(genCursor, "metaStruct_%.*s,\n", token.length, token.start);
	}
	for(Token token : additionalMetaTypesWithoutFields){
		genCursor += sprintf(genCursor, "metaStruct_%.*s,\n", token.length, token.start);
	}
	genCursor += sprintf(genCursor, "};\n");

	genCursor += sprintf(genCursor, "MetaStructInfo genMetaStructInfo[] = {\n");
	for(Token token : additionalMetaTypes){
		genCursor += sprintf(genCursor, "{\"%.*s\", memberInfo_%.*s, ARRAY_COUNT(memberInfo_%.*s)},\n",
			token.length, token.start, token.length, token.start, token.length, token.start);
	}
	for(Token token : additionalMetaTypesWithoutFields){
		genCursor += sprintf(genCursor, "{\"%.*s\", nullptr, 0},\n", token.length, token.start);
	}
	genCursor += sprintf(genCursor, "};\n");

	genCursor += sprintf(genCursor, "\n#endif\n");


	char* generatedSourceFileName = "gen/MetaTypes.h";
	FILE* generateSourceFile = fopen(generatedSourceFileName, "wb");
	fwrite(generatedSource, 1, genCursor - generatedSource, generateSourceFile);

	fclose(generateSourceFile);
	free(generatedSource);

	for(char* fileStr : fileStrings){
		free(fileStr);
	}
}

inline bool IsWhitespace(char c){
	return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

inline bool IsOperator(char c){
	return (c == '+') || (c == '-') || (c == '*') || (c == '/') || (c == '(') || (c == ')')
		|| (c == '&') || (c == '|') || (c == '=') || (c == '!') || (c == ';') || (c == ':')
		|| (c == '<') || (c == '>') || (c == ',') || (c == '{') || (c == '}') || (c == '[')
		|| (c == ']');
}

inline bool IsNumber(char c){
	return (c >= '0') && (c <= '9') || (c == '.');
}

vector<Token> LexFileContents(char* fileContents, int fileLength){
	Token currToken;
	currToken.start = fileContents;
	currToken.length = 0;
	currToken.type = SourceTokenType::EMPTY;

	vector<Token> tokens;

	for(int i = 0; i < fileLength; i++){

		char currChar = fileContents[i];

		bool isWhitespace = IsWhitespace(currChar);
		bool isQuote = (currChar == '"');
		bool isOperator = IsOperator(currChar);
		bool isNumber = IsNumber(currChar);

		switch (currToken.type){
		case SourceTokenType::EMPTY:{
			if(!isWhitespace){
				currToken.start = &fileContents[i];
				currToken.length = 1;

				if(isQuote){
					currToken.start++;
					currToken.length = 0;
					currToken.type = SourceTokenType::STRING;
				}
				else if(isOperator){
					currToken.type = SourceTokenType::OPERATOR;
				}
				else if(isNumber){
					currToken.type = SourceTokenType::NUMBER;
				}
				else{
					currToken.type = SourceTokenType::IDENTIFIER;
				}
			}
		}break;

		case SourceTokenType::NUMBER:
		case SourceTokenType::IDENTIFIER:{
			if(isWhitespace){
				tokens.push_back(currToken);
				currToken.length = 0;
				currToken.type = SourceTokenType::EMPTY;
			}
			else if(isOperator){
				tokens.push_back(currToken);
				currToken.start = &fileContents[i];
				currToken.length = 1;
				currToken.type = SourceTokenType::OPERATOR;
			}
			else{
				currToken.length++;
			}
		}break;

		case SourceTokenType::STRING:{
			if(isQuote){
				tokens.push_back(currToken);
				currToken.length = 0;
				currToken.type = SourceTokenType::EMPTY;
			}
			else{
				currToken.length++;
			}
		}break;

		case SourceTokenType::OPERATOR:{
			tokens.push_back(currToken);

			currToken.length = 1;
			currToken.start = &fileContents[i];

			if(isWhitespace){
				currToken.length = 0;
				currToken.type = SourceTokenType::EMPTY;
			}
			else if(isNumber){
				currToken.type = SourceTokenType::NUMBER;
			}
			else if(isOperator){
				currToken.type = SourceTokenType::OPERATOR;
			}
			else if(isQuote){
				currToken.start++;
				currToken.length = 0;
				currToken.type = SourceTokenType::STRING;
			}
			else{
				currToken.type = SourceTokenType::IDENTIFIER;
			}
		}break;

		default:
			break;
		}
	}

	return tokens;
}