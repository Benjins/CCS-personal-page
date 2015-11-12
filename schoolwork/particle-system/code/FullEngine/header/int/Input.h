#ifndef INPUT_H
#define INPUT_H

#pragma once

enum KeyState{
	PRESS,
	RELEASE,
	HOLD,
	OFF
};

struct Input{
	friend struct Scene;
	friend struct EditorScene;

protected:
	KeyState keys[255];

	KeyState mouseLeft;
	KeyState mouseMiddle;
	KeyState mouseRight;

	int mouseX;
	int mouseY;

	void PushKey(unsigned char key);
	void ReleaseKey(unsigned char key);

	void PushMouse(int button);
	void ReleaseMouse(int button);

	void EndFrame();

public:
	Input();

	bool GetKey(unsigned char key) const;
	bool GetKeyUp(unsigned char key) const;
	bool GetKeyDown(unsigned char key) const;

	bool GetMouse(int button) const;
	bool GetMouseUp(int button) const;
	bool GetMouseDown(int button) const;

	int inline GetMouseX() const{ return mouseX; }
	int inline GetMouseY() const{ return mouseY; }
};

#endif