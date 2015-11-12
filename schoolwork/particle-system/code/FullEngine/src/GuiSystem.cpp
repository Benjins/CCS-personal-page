#include "../header/int/GuiSystem.h"
#include "../header/ext/simple-xml.h"
#include "../header/int/LoadingUtilities.h"
#include <iostream>

using std::cout; using std::endl;

void GuiSystem::RenderGui(){
	for(GuiElement* elem : elements){
		elem->OnGui();
	}
}

GuiElement* GuiSystem::AddGuiElement(MaterialManager* resources){
	GuiElement* elem = new GuiElement(resources);
	elements.push_back(elem);

	return elem;
}

GuiElement* GuiSystem::FindGUIElement(const string& name){
	for(auto iter = elements.begin(); iter != elements.end(); iter++){
		GuiElement* elem = *iter;
		if(elem->name == name){
			return elem;
		}
	}

	return NULL;
}

void GuiSystem::RemoveElement(GuiElement* elem){
	for(auto iter = elements.begin(); iter != elements.end(); ++iter){
		if(*iter == elem){
			elements.erase(iter);
			break;
		}
	}

	delete elem;
}

void GuiSystem::ClearElementChildren(GuiElement* elem){
	for(GuiElement* child : elem->children){
		ClearElementChildren(child);
		RemoveElement(child);
	}

	elem->children.clear();
}

GuiElement* GuiSystem::OnMouseDown(const Vector2& hitPoint){
	for(auto iter = elements.rbegin(); iter != elements.rend(); ++iter){
		GuiElement* elem = *iter;
		if(elem->WasHit(hitPoint)){
			elem->OnMouseDown(hitPoint);
			mouseDownElement = elem;
			return elem;
		}
	}

	mouseDownElement = nullptr;

	return nullptr;
}

GuiElement* GuiSystem::OnMouseDrag(const Vector2& hitPoint){
	for(auto iter = elements.rbegin(); iter != elements.rend(); ++iter){
		GuiElement* elem = *iter;
		if(elem->WasHit(hitPoint)){
			elem->OnMouseDrag(hitPoint);
			return elem;
		}
	}

	return nullptr;
}

GuiElement* GuiSystem::OnMouseUp(const Vector2& hitPoint){
	if(mouseDownElement != nullptr){
		mouseDownElement->OnMouseUp(hitPoint);
	}

	for(auto iter = elements.rbegin(); iter != elements.rend(); ++iter){
		GuiElement* elem = *iter;
		if(elem->WasHit(hitPoint)){
			if(elem != mouseDownElement){
				elem->OnMouseUp(hitPoint);
			}

			mouseDownElement = nullptr;
			selectedElem = elem;
			return elem;
		}
	}

	selectedElem = nullptr;
	mouseDownElement = nullptr;
	return nullptr;
}

void GuiSystem::LoadGuiElement(const XMLElement& elem, MaterialManager* resources){
	GuiElement* guiElem = AddGuiElement(resources);
	int width=0,height=0;
	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "width"){
			width = atoi(iter->data.c_str());
		}
		else if (iter->name == "height"){
			height = atoi(iter->data.c_str());
		}
		else if (iter->name == "position"){
			guiElem->position = ParseVector2(iter->data);
		}
		else if (iter->name == "scale"){
			guiElem->scale = ParseVector2(iter->data);
		}
		else if (iter->name == "name"){
			guiElem->name = iter->data;
		}
	}

	guiElem->tex = new Texture(width, height);
}

void GuiSystem::LoadGuiText(const XMLElement& elem, MaterialManager* resources){
	string fuv;
	Vector2 position;
	Vector2 scale;
	string name;
	string text;
	Vector2 textScale;

	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "fuv"){
			fuv = iter->data;
		}
		else if(iter->name == "position"){
			position = ParseVector2(iter->data);
		}
		else if(iter->name == "scale"){
			scale = ParseVector2(iter->data);
		}
		else if (iter->name == "name"){
			name = iter->data;
		}
		else if (iter->name == "text"){
			text = iter->data;
		}
		else if (iter->name == "textScale"){
			textScale = ParseVector2(iter->data);
		}
	}

	GuiText* guiText = new GuiText(resources, fuv);
	guiText->position = position;
	guiText->scale = scale;
	guiText->name = name;
	guiText->text = text;
	guiText->textScale = textScale;

	elements.push_back(guiText);
}