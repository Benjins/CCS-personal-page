#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#pragma once

#include "GuiElement.h"

#include <vector>

using std::vector;

struct GuiSystem{
	vector<GuiElement*> elements;

	GuiElement* mouseDownElement;
	GuiElement* selectedElem;

	GuiSystem(){
		mouseDownElement = nullptr;
		selectedElem = nullptr;
	}

	void RenderGui();

	GuiElement* FindGUIElement(const string& name);
	void RemoveElement(GuiElement* elem);
	void ClearElementChildren(GuiElement* elem);

	GuiElement* AddGuiElement(MaterialManager* resources);
	void LoadGuiElement(const XMLElement& elem, MaterialManager* resources);
	void LoadGuiText(const XMLElement& elem, MaterialManager* resources);

	GuiElement* OnMouseDown(const Vector2& hitPoint);
	GuiElement* OnMouseDrag(const Vector2& hitPoint);
	GuiElement* OnMouseUp(const Vector2& hitPoint);

	//Returns true if we capture the keystroke
	bool OnKey(unsigned char key){
		if(selectedElem != nullptr){
			selectedElem->OnKey(key);
			return true;
		}
		else{
			return false;
		}
	}

	void EndOfFrame(){
		for(GuiElement* elem : elements){
			elem->EndOfFrame();
		}
	}

	~GuiSystem(){
		mouseDownElement = nullptr;

		for(GuiElement* elem : elements){
			delete elem;
		}

		elements.clear();
	}

};


#endif
