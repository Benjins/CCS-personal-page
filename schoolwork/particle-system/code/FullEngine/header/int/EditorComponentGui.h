#ifndef EDITORCOMPONENTGUI_H
#define EDITORCOMPONENTGUI_H

#pragma once

#include "../ext/simple-xml.h"
#include "../int/MetaTypeInfo.h"

struct GuiText; struct GuiTextField; struct Component;

struct EditorComponentGui{
	MetaType type;
	void* memberPtr;
	GuiText* fieldLabel;
	GuiTextField* inputText;
	string cachedInputText;

	EditorComponentGui();

	void Load();
	void Save();
};

#endif