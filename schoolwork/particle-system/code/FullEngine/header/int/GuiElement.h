#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#pragma once

#include "../ext/Vector2.h"
#include "FontBMPMaker.h"
#include "Texture.h"
#include "../ext/EasyBMP.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include <string>
#include <iostream>
#include <vector>

using std::string; using std::vector; using std::cout;

struct Texture; struct MaterialManager; struct Scene; struct FUV; struct XMLElement;

enum struct GuiType{
	RawTexture,
	Text,
	Slider
};

struct GuiElement{
	
	Texture* tex;
	Vector2 position;
	Vector2 scale;
	int depth; //Higher depths drawn on top of lower depths

protected:
	GuiElement* parent;
public:
	vector<GuiElement*> children;

	bool isVisible;
	bool isPassive;

	GLint guiProgram;
	GLuint texLoc;
	string name;

	GuiElement();
	GuiElement(MaterialManager* resources);

	void SetParent(GuiElement* newParent);

	virtual void EndOfFrame(){}

	virtual void OnKey(unsigned char key){}

	virtual void OnMouseDown(const Vector2& hitPoint){
	}

	virtual void OnMouseUp(const Vector2& hitPoint){
	}

	virtual void OnMouseDrag(const Vector2& hitPoint){}
	virtual void OnClicked(const Vector2& hitPoint){}

	bool WasHit(const Vector2& hitPoint){
		if(isPassive){
			return false;
		}

		return hitPoint.x >= (position.x - scale.x/2) && hitPoint.x <= (position.x + scale.x/2)
			&& hitPoint.y >= (position.y - scale.y/2) && hitPoint.y <= (position.y + scale.y/2);
	}

	GuiElement* GetParent() const { return parent; }

	virtual void OnGui() const;

	virtual XMLElement Serialize();

	virtual ~GuiElement();
};

struct GuiSlider : GuiElement{

};

struct GuiText : GuiElement{
	string text;
	FUV* fuv;
	GLuint texObj;
	string fuvFileName;
	Vector2 textScale;

	GuiText(MaterialManager* resources, const string& _fuvFileName);

	virtual void OnGui() const;

	virtual XMLElement Serialize();

	virtual ~GuiText();
};

struct GuiTextField : GuiText{
	GuiTextField(MaterialManager* resources, const string& _fuvFileName)
		: GuiText(resources, _fuvFileName){
		tex = new Texture(1,1);
		RGBApixel col = {20,20,20,20};
		tex->SetPixel(0,0,col);
		tex->Apply();
	}

	virtual void OnGui() const{
		GuiElement::OnGui();
		GuiText::OnGui();
	}

	virtual void OnKey(unsigned char key){
		if(key == '\n' || key == '\r'){
			//deselect ourselves?
		}
		else if(key == '\b'){
			//std::string.pop_back() would be preferable, but it seems clang sometimes lacks support for it
			text = text.substr(0, text.size()-1);
		}
		else if(key == '\t'){
			text.append("  ");
		}
		else{
			text += (char)key;
		}
	}
};

struct GuiButton : GuiText{
	bool toggleOnClick;
	bool isClicked;
	bool isDown;
	
	GuiButton(MaterialManager* resources, const string& _fuvFileName)
		: GuiText(resources, _fuvFileName){
		tex = new Texture(1,1);
		RGBApixel col = {20,20,20,20};
		tex->SetPixel(0,0,col);
		tex->Apply();

		toggleOnClick = false;
		isDown = false;
		isClicked = false;
	}

	virtual void OnGui() const{
		GuiElement::OnGui();
		GuiText::OnGui();
	}

	virtual void EndOfFrame(){
		isClicked = false;
		if(!toggleOnClick){
			isDown = false;
		}
	}

	virtual void OnMouseDown(const Vector2& hitPoint){
		GuiElement::OnMouseDown(hitPoint);
	}

	virtual void OnMouseUp(const Vector2& hitPoint){
		GuiElement::OnMouseUp(hitPoint);

		isClicked = true;

		if(toggleOnClick){
			isDown = !isDown;
		}
	}

	virtual ~GuiButton(){}
};

void GuiSetSliderValue(GuiElement* elem, float value);

#endif