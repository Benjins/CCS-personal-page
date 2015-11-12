#include "../header/int/EditorComponentGui.h"
#include "../header/int/GuiElement.h"
#include "../header/int/Component.h"
#include "../header/int/Collider.h"
#include "../header/int/LoadingUtilities.h"

#include <algorithm>

EditorComponentGui::EditorComponentGui(){
	memberPtr = nullptr;
	fieldLabel = nullptr;
	inputText = nullptr;
}

void EditorComponentGui::Save(){
	if(cachedInputText != inputText->text){
		cachedInputText = inputText->text;
		if(type == MetaType_bool){
			*((bool*)(memberPtr)) = (inputText->text == "T");
		}
		else if(type == MetaType_int){
			*((int*)(memberPtr)) = atoi(inputText->text.c_str());
		}
		else if(type == MetaType_float){
			*((float*)(memberPtr)) = atof(inputText->text.c_str());
		}
		else if(type == MetaType_string){
			*((string*)(memberPtr)) = inputText->text;
		}
		else if(type == MetaType_Vector2){
			if(std::count(inputText->text.begin(), inputText->text.end(), ',') == 1){
				*((Vector2*)(memberPtr)) = ParseVector2(inputText->text);
			}
		}
		else if(type == MetaType_Vector3){
			if(std::count(inputText->text.begin(), inputText->text.end(), ',') == 2){
				*((Vector3*)(memberPtr)) = ParseVector3(inputText->text);
			}
		}
	}
}

void EditorComponentGui::Load(){
	if(type == MetaType_bool){
		inputText->text = (*(bool*)memberPtr) ? "T" : "F";
	}
	else if(type == MetaType_int){
		inputText->text = to_string(*(int*)memberPtr);
	}
	else if(type == MetaType_float){
		inputText->text = to_string(*(float*)memberPtr);
	}
	else if(type == MetaType_string){
			inputText->text = *((string*)memberPtr);
	}
	else if(type == MetaType_Vector2){
		inputText->text = EncodeVector2(*(Vector2*)memberPtr);
	}
	else if(type == MetaType_Vector3){
		inputText->text = EncodeVector3(*(Vector3*)memberPtr);
	}

	cachedInputText = inputText->text;
	
}

