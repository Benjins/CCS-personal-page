#include "../header/int/GuiElement.h"
#include "../header/int/Texture.h"
#include "../header/ext/EasyBMP.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Material.h"
#include "../header/int/FontBMPMaker.h"
#include "../header/ext/simple-xml.h"
#include "../header/int/LoadingUtilities.h"


GuiElement::GuiElement(){
	tex = NULL;
	parent = nullptr;
	isVisible = true;
	isPassive = false;
	guiProgram = 0;
	depth = 0;
	texLoc = 0;
	position = Vector2(0,0);
	scale = Vector2(0.2f,0.2f);
	name = "";
}

GuiElement::GuiElement(MaterialManager* resources){
	Material* mat = resources->GetMaterialByName("gui");
	guiProgram = mat->shaderProgram;
	texLoc = mat->GetUniformByName("_guiTex");
	parent = nullptr;

	isVisible = true;
	isPassive = false;

	name = "";
	tex = NULL;
	depth = 0;
	position = Vector2(0,0);
	scale = Vector2(0,0);
}

void GuiElement::SetParent(GuiElement* newParent){
		if(parent == newParent){
			return;
		}
		if(parent != nullptr){
			for(auto iter = parent->children.begin(); iter != parent->children.end(); ++iter){
				if(*iter == this){
					parent->children.erase(iter);
					break;
				}
			}
		}

		parent = newParent;
		if(newParent != nullptr){
			newParent->children.push_back(this);
		}
	}

void GuiElement::OnGui() const{
	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	glUseProgram(guiProgram);
	glActiveTexture(GL_TEXTURE0);
	if(tex != NULL){
		glBindTexture(tex->textureTarget, tex->textureObj);
	}
	glUniform1i(texLoc, 0);

	Vector2 corners[4] = { Vector2(position.x - scale.x/2, position.y - scale.y/2),
						   Vector2(position.x + scale.x/2, position.y - scale.y/2),
						   Vector2(position.x - scale.x/2, position.y + scale.y/2),
						   Vector2(position.x + scale.x/2, position.y + scale.y/2)};

	glBegin(GL_TRIANGLE_STRIP);
	for(int i = 0; i < 4; i++){
		Vector3 vert = Vector3(corners[i].x, corners[i].y, 0.5);
		vert = vert * 2 - Vector3(1,1,0);
		glVertex3f(vert.x, vert.y, vert.z);
	}
	glEnd();
	glUseProgram(currProgram);
}

XMLElement GuiElement::Serialize(){
	XMLElement elem;
	elem.name = "GuiElement";

	elem.attributes.push_back(XMLAttribute("name", name));
	elem.attributes.push_back(XMLAttribute("width", to_string(tex->width)));
	elem.attributes.push_back(XMLAttribute("height", to_string(tex->height)));
	elem.attributes.push_back(XMLAttribute("position", EncodeVector2(position)));
	elem.attributes.push_back(XMLAttribute("scale", EncodeVector2(scale)));

	return elem;
}

void GuiSetSliderValue(GuiElement* elem, float value){
	Texture* tex = elem->tex;
	RGBApixel col1 = {255,50,50,255}, col2 = {50,200,200,255};
	int width = tex->width, height = tex->height;
	int size = width * height;
	int border = value * width;

	RGBApixel* data = tex->pixelData;
	for(int i = 0; i < size; i++){
		int horiz = i % width;
		data[i] = (horiz > border ? col1 : col2);
	}

	tex->Apply();
}


GuiElement::~GuiElement(){
	if(tex != NULL){
		delete tex;
	}
}

GuiText::~GuiText(){
}

GuiText::GuiText(MaterialManager* resources, const string& _fuvFileName) 
	: GuiElement()
	, fuvFileName(_fuvFileName)
{
	textScale = Vector2(1,1);
	text = "";
	fuv = resources->LoadFUV(_fuvFileName);

	Material* textMat = resources->GetMaterialByName("gui-txt");
	guiProgram = textMat->shaderProgram;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texObj);
	glBindTexture(GL_TEXTURE_2D, texObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fuv->imageSize, fuv->imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, fuv->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(guiProgram, "_mainTex"), 0);
}

XMLElement GuiText::Serialize(){
	XMLElement elem;
	elem.name = "GuiText";

	elem.attributes.push_back(XMLAttribute("name", name));
	elem.attributes.push_back(XMLAttribute("fuv", fuvFileName));
	elem.attributes.push_back(XMLAttribute("text", text));
	elem.attributes.push_back(XMLAttribute("textScale", EncodeVector2(textScale)));
	elem.attributes.push_back(XMLAttribute("position", EncodeVector2(position)));
	elem.attributes.push_back(XMLAttribute("scale", EncodeVector2(scale)));

	return elem;
}

void GuiText::OnGui() const{
	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	glUseProgram(guiProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texObj);

	float sx = 2.0f / glutGet(GLUT_WINDOW_WIDTH)  * textScale.x;
	float sy = 2.0f / glutGet(GLUT_WINDOW_HEIGHT) * textScale.y; 

	//Left alignement by default
	float x = position.x - scale.x/2;
	float y = position.y;

	x = x * 2 - 1;
	y = 1 - (y * 2);

	for(int i = 0; i < text.size(); i++){
		char letter = text[i];
		if(letter == '\n' || letter == '\r'){
			x = position.x;
			y += sy * fuv->fontSize; 
			continue;
		}

		LetterUV uv = fuv->uvs[letter];
		float xSize = (uv.xMax - uv.xMin) * fuv->imageSize * sx;
		float ySize = (uv.yMax - uv.yMin) * fuv->imageSize * sy;

		float x2 = x + uv.left * sx;
		float y2 = y - uv.top * sy;
		
		glBegin(GL_TRIANGLE_STRIP);
		{
			glVertex4f(x2, -y2,                 uv.xMin, uv.yMin);
			glVertex4f(x2, -y2 - ySize,         uv.xMin, uv.yMax);
			glVertex4f(x2 + xSize, -y2,         uv.xMax, uv.yMin);
			glVertex4f(x2 + xSize, -y2 - ySize, uv.xMax, uv.yMax);
		}
		glEnd();

		x += uv.xAdvance * sx;
		y += uv.yAdvance * sy;
	}
	glUseProgram(currProgram);
}