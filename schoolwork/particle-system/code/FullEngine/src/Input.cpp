#include "../header/int/Input.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

Input::Input(){
	for(int i = 0; i < 256; i++){
		keys[i] = KeyState::OFF;
	}
}

void Input::PushKey(unsigned char key){
	keys[key] = KeyState::PRESS;
}

void Input::ReleaseKey(unsigned char key){
	keys[key] = KeyState::RELEASE;
}

void Input::PushMouse(int button){
	if(button == GLUT_LEFT_BUTTON)  { mouseLeft   = KeyState::PRESS;}
	if(button == GLUT_RIGHT_BUTTON) { mouseRight  = KeyState::PRESS;}
	if(button == GLUT_MIDDLE_BUTTON){ mouseMiddle = KeyState::PRESS;}
}

void Input::ReleaseMouse(int button){
	if(button == GLUT_LEFT_BUTTON)  { mouseLeft   = KeyState::RELEASE;}
	if(button == GLUT_RIGHT_BUTTON) { mouseRight  = KeyState::RELEASE;}
	if(button == GLUT_MIDDLE_BUTTON){ mouseMiddle = KeyState::RELEASE;}
}

bool Input::GetKey(unsigned char key) const{
	return keys[key] == KeyState::PRESS || keys[key] == KeyState::HOLD;
}

bool Input::GetKeyUp(unsigned char key) const{
	return keys[key] == KeyState::RELEASE;
}

bool Input::GetKeyDown(unsigned char key) const{
	return keys[key] == KeyState::PRESS;
}

bool Input::GetMouse(int button) const{
	if(button == GLUT_LEFT_BUTTON)  { return mouseLeft   == KeyState::PRESS || mouseLeft   == KeyState::HOLD;}
	if(button == GLUT_RIGHT_BUTTON) { return mouseRight  == KeyState::PRESS || mouseRight  == KeyState::HOLD;}
	if(button == GLUT_MIDDLE_BUTTON){ return mouseMiddle == KeyState::PRESS || mouseMiddle == KeyState::HOLD;}
	return false;
}

bool Input::GetMouseUp(int button) const{
	if(button == GLUT_LEFT_BUTTON)  { return mouseLeft   == KeyState::RELEASE;}
	if(button == GLUT_RIGHT_BUTTON) { return mouseRight  == KeyState::RELEASE;}
	if(button == GLUT_MIDDLE_BUTTON){ return mouseMiddle == KeyState::RELEASE;}
	return false;
}

bool Input::GetMouseDown(int button) const{
	if(button == GLUT_LEFT_BUTTON)  { return mouseLeft   == KeyState::PRESS;}
	if(button == GLUT_RIGHT_BUTTON) { return mouseRight  == KeyState::PRESS;}
	if(button == GLUT_MIDDLE_BUTTON){ return mouseMiddle == KeyState::PRESS;}
	return false;
}

void Input::EndFrame(){
	for(int i = 0; i < 256; i++){
		if(keys[i] == KeyState::PRESS){
			keys[i] = KeyState::HOLD;
		}
		if(keys[i] == KeyState::RELEASE){
			keys[i] = KeyState::OFF;
		}
	}

	if(mouseLeft == KeyState::PRESS)  { mouseLeft = KeyState::HOLD;}
	if(mouseLeft == KeyState::RELEASE){ mouseLeft = KeyState::OFF; }

	if(mouseRight == KeyState::PRESS)  { mouseRight = KeyState::HOLD;}
	if(mouseRight == KeyState::RELEASE){ mouseRight = KeyState::OFF; }

	if(mouseMiddle == KeyState::PRESS)  { mouseMiddle = KeyState::HOLD;}
	if(mouseMiddle == KeyState::RELEASE){ mouseMiddle = KeyState::OFF; }
}
