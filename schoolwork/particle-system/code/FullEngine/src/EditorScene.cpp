#include "../header/int/EditorScene.h"
#include "../header/int/UserComps.h"
#include "../header/int/Input.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/Collider.h"
#include "../header/int/Texture.h"
#include "../header/ext/EasyBMP.h"
#include "../header/int/Material.h"
#include "../header/int/Vector4.h"
#include "../header/int/Mat4.h"

#include "../header/int/MetaTypeInfo.h"

extern MetaStructInfo genMetaStructInfo[];

#include <sstream>
#include <iomanip>
#include <cfloat>

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
#include <sys/time.h>
#endif 


EditorScene::EditorScene(int argc, char** argv) : Scene(argc, argv){
	glutMouseFunc(OnEditorMouseFunc);
	glutPassiveMotionFunc(OnEditorPassiveMouseFunc);
	glutMotionFunc(OnEditorPassiveMouseFunc);
	glutKeyboardFunc(OnEditorKeyFunc);
	glutKeyboardUpFunc(OnEditorKeyUpFunc);

	selectedObj = nullptr;
	selectedAxis = -1;
	globalManipulator = false;
	transformMode = TransformMode::Translation;
}

void EditorScene::Start(){
	
	timer.Reset();

	RecalculateSelectionSim();

	sceneCamera = camera;

	editorCamera.position = Vector3(3,3,-3);
	editorCamera.rotation = Quaternion(Y_AXIS, -45) * Quaternion(X_AXIS, 45);
	camera = &editorCamera;

	GuiElement* panel = new GuiElement(&resources);
	panel->name = "gui_editor_panel";
	panel->position = Vector2(0.85f,0.9f);
	panel->scale = Vector2(0.3f, 0.2f);
	panel->tex = new Texture(1,1);
	RGBApixel pix = {50,50,50,220};
	panel->tex->SetPixel(0,0,pix);
	panel->tex->Apply();

	editorGui.elements.push_back(panel);
	
	GuiText* posTxt = new GuiText(&resources, "data/arial_16.fuv");
	posTxt->position = Vector2(0.81f, 0.94f);
	posTxt->scale = Vector2(0.1f, 0.04f);
	posTxt->textScale = Vector2(1,1);
	posTxt->text = "Position: ";

	editorGui.elements.push_back(posTxt);

	GuiText* meshTxt = new GuiText(&resources, "data/arial_16.fuv");
	meshTxt->position = Vector2(0.81f, 0.9f);
	meshTxt->scale = Vector2(0.1f, 0.04f);
	meshTxt->text = "Mesh file: ";

	editorGui.elements.push_back(meshTxt);

	GuiText* nameTxt = new GuiText(&resources, "data/arial_16.fuv");
	nameTxt->position = Vector2(0.81f, 0.86f);
	nameTxt->scale = Vector2(0.1f, 0.04f);
	nameTxt->text = "Name: ";

	editorGui.elements.push_back(nameTxt);

	GuiText* rotTxt = new GuiText(&resources, "data/arial_16.fuv");
	rotTxt->position = Vector2(0.81f, 0.82f);
	rotTxt->scale = Vector2(0.1f, 0.04f);
	rotTxt->text = "Rotation: ";

	editorGui.elements.push_back(rotTxt);

	GuiButton* globalButton = new GuiButton(&resources, "data/arial_16.fuv");
	globalButton->name = "globalButton";
	globalButton->position = Vector2(0.5f, 0.95f);
	globalButton->scale = Vector2(0.1f, 0.04f);
	globalButton->text = "Global";
	globalButton->toggleOnClick = true;

	editorGui.elements.push_back(globalButton);

	GuiTextField* nameField = new GuiTextField(&resources, "data/arial_16.fuv");
	nameField->name = "nameField";
	nameField->position = Vector2(0.3f, 0.95f);
	nameField->scale = Vector2(0.1f, 0.04f);
	nameField->text = "...";

	editorGui.elements.push_back(nameField);

	GuiElement* componentPanel = new GuiElement(&resources);
	componentPanel->name = "components_panel";
	componentPanel->position = Vector2(0.85f,0.16f);
	componentPanel->scale = Vector2(0.3f, 0.3f);
	componentPanel->tex = new Texture(1,1);
	componentPanel->tex->SetPixel(0,0,pix);
	componentPanel->tex->Apply();

	editorGui.elements.push_back(componentPanel);

	running = true;
	while(running){
#ifdef __APPLE__
		glutCheckLoop();
#else
		glutMainLoopEvent();
#endif

		EditorUpdate();

		Render();
		EditorGUI();
		glutSwapBuffers();
		glutPostRedisplay();

		//These should be order-independent, just setting local states.
		editorGui.EndOfFrame();
		input.EndFrame();
	}
}

void EditorScene::RecalculateSelectionSim(){
	selectionSim.ResetSelectionBoxColliders();

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		Vector3 maxVert = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX), minVert = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		GameObject* obj = *iter;
		if(obj->mesh != NULL){
			for(auto iter = obj->mesh->vertices.begin(); iter != obj->mesh->vertices.end(); iter++){
				maxVert.x = max(maxVert.x, iter->position.x);
				minVert.x = min(minVert.x, iter->position.x);

				maxVert.y = max(maxVert.y, iter->position.y);
				minVert.y = min(minVert.y, iter->position.y);

				maxVert.z = max(maxVert.z, iter->position.z);
				minVert.z = min(minVert.z, iter->position.z);
			}
		}

		if(obj->mesh != NULL && obj->mesh->vertices.size() > 0){
			Vector3 center = (maxVert + minVert) / 2;
			Vector3 size   = (maxVert - minVert) / 2;

			BoxCollider* boundingBox = new BoxCollider(center, size);
			boundingBox->gameObject = obj;
			selectionSim.staticBoxBodies.push_back(boundingBox);
		}
		else{
			BoxCollider* boundingBox = new BoxCollider(Vector3(0,0,0), Vector3(0.4f, 0.4f, 0.4f));
			boundingBox->gameObject = obj;
			selectionSim.staticBoxBodies.push_back(boundingBox);
		}
	}
}

void EditorScene::EditorUpdate(){
	deltaTime = timer.GetTimeSince();
	timer.Reset();
	
	//cout << "EditorScene::EditorUpdate(): " << (deltaTime * 1000) << "  ms.\n";

	float windowWidth  = glutGet(GLUT_WINDOW_WIDTH);
	float windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	float mouseXNormalized = (input.mouseX -  windowWidth/2) / windowWidth  * 2;
	float mouseYNormalized = (input.mouseY - windowHeight/2) / windowHeight * 2;

	Vector2 uiMouseLoc = (Vector2(mouseXNormalized, mouseYNormalized) + Vector2(1,1)) / 2;
	uiMouseLoc.y = 1 - uiMouseLoc.y;

	GuiElement* hitElement = nullptr, *downElem=nullptr, *upElem=nullptr, *dragElem=nullptr;
	if(input.GetMouseDown(GLUT_LEFT_BUTTON)){
		hitElement = downElem = editorGui.OnMouseDown(uiMouseLoc);
	}
	else if(input.GetMouseUp(GLUT_LEFT_BUTTON)){
		hitElement = upElem = editorGui.OnMouseUp(uiMouseLoc);
	}
	else if(input.GetMouse(GLUT_LEFT_BUTTON)){
		hitElement = dragElem = editorGui.OnMouseDrag(uiMouseLoc);
	}

	if(input.GetKeyUp('x')){
		Stop();
	}

	if(input.GetKeyUp(127)){
		RemoveObject(selectedObj);
		DeselectObject();
	}
	
	if(input.GetKeyUp('p')){
		GameObject* obj = new GameObject();
		obj->scene = this;
		obj->transform.position = Vector3(0,1,0);
		obj->AddComponent<PathNodeComponent>();
		AddObject(obj);

		RecalculateSelectionSim();
	}

	if(input.GetKeyUp('j')){
		pathfinding.HookUpNodes();
	}

	if(input.GetKeyUp('f') && selectedObj != nullptr){
		GameObject* copy = Instantiate(selectedObj, selectedObj->transform.GlobalPosition() + editorCamera.Right(), selectedObj->transform.TotalRotation());
		SelectObject(copy);
	}

	const float speed = 2.0f;

	if(input.GetKey('w')){
		editorCamera.position = editorCamera.position + editorCamera.Forward() * speed * deltaTime;
	}
	if(input.GetKey('s')){
		editorCamera.position = editorCamera.position - editorCamera.Forward() * speed * deltaTime;
	}
	if(input.GetKey('a')){
		editorCamera.position = editorCamera.position - editorCamera.Right() * speed * deltaTime;
	}
	if(input.GetKey('d')){
		editorCamera.position = editorCamera.position + editorCamera.Right() * speed * deltaTime;
	}
	if(input.GetKey('q')){
		editorCamera.position = editorCamera.position + Y_AXIS * speed * deltaTime;
	}
	if(input.GetKey('z')){
		editorCamera.position = editorCamera.position - Y_AXIS * speed * deltaTime;
	}

	if(input.GetKeyDown('g')){
		globalManipulator = !globalManipulator;
	}

	GuiButton* globalButton = static_cast<GuiButton*>(editorGui.FindGUIElement("globalButton"));
	if(globalButton->isClicked){
		globalManipulator = !globalManipulator;
		globalButton->text = globalManipulator ? "Global" : "Local";
	}

	if(input.GetKeyUp('o')){
		camera = sceneCamera;
		SaveScene("Editor_Scene.xml");
		camera = &editorCamera;
	}
	if(input.GetKeyUp('l')){
		selectedObj=nullptr;
		for(BoxCollider* col : selectionSim.staticBoxBodies){
			delete col;
		}
		selectionSim.staticBoxBodies.clear();
		LoadScene("Editor_Scene.xml");
		RecalculateSelectionSim();
		sceneCamera = camera;
		camera = &editorCamera;
	}

	if(input.GetKeyUp('e')){
		transformMode = TransformMode::Translation;
	}
	if(input.GetKeyUp('r')){
		transformMode = TransformMode::Rotation;
	}
	
	if(hitElement == nullptr){
		Vector3 mouseWorldPos = editorCamera.position;
		mouseWorldPos = mouseWorldPos - (editorCamera.Up()    * mouseYNormalized);
		mouseWorldPos = mouseWorldPos + (editorCamera.Right() * mouseXNormalized * (windowWidth/windowHeight));
		mouseWorldPos = mouseWorldPos + (editorCamera.Forward() * 1.2f);

		Vector3 rayDirection = mouseWorldPos - editorCamera.position;

		if(input.GetMouseDown(GLUT_LEFT_BUTTON)){
			if(selectedObj != nullptr){
				Vector3 cameraToObj = selectedObj->transform.GlobalPosition() - editorCamera.position;
				float projectionAmount = DotProduct(cameraToObj, rayDirection)/DotProduct(rayDirection, rayDirection);
				Vector3 projectedCenter = rayDirection * projectionAmount;

				Vector3 orthoPart = projectedCenter - cameraToObj;
				if(orthoPart.MagnitudeSquared() < 1){
					Vector3 rightVector   = globalManipulator ?  X_AXIS : selectedObj->transform.Right();
					Vector3 upVector      = globalManipulator ?  Y_AXIS : selectedObj->transform.Up();
					Vector3 forwardVector = globalManipulator ?  Z_AXIS : selectedObj->transform.Forward();

					float xBit = DotProduct(orthoPart, rightVector);
					float yBit = DotProduct(orthoPart, upVector);
					float zBit = DotProduct(orthoPart, forwardVector);

					if(transformMode == TransformMode::Translation){
						if(xBit > yBit && xBit > zBit){
							selectedAxis = 0;
						}
						else if(yBit > xBit && yBit > zBit){
							selectedAxis = 1;
						}
						else if(zBit > yBit && zBit > xBit){
							selectedAxis = 2;
						}
					}
					else if(transformMode == TransformMode::Rotation){
						xBit = abs(xBit);
						yBit = abs(yBit);
						zBit = abs(zBit);
						printf("xBit: %f yBit: %f zBit: %f\n", xBit, yBit, zBit);
						if(xBit < yBit && xBit < zBit){
							selectedAxis = 0;
						}
						else if(yBit < xBit && yBit < zBit){
							selectedAxis = 1;
						}
						else if(zBit < yBit && zBit < xBit){
							selectedAxis = 2;
						}
					}
				}
			}
		}

		if(input.GetMouse(GLUT_LEFT_BUTTON) && selectedObj != nullptr && selectedAxis >= 0){
			float deltaX = prevX - input.mouseX;
			float deltaY = prevY - input.mouseY;

			Vector3 rightVector   = globalManipulator ?  X_AXIS : selectedObj->transform.Right();
			Vector3 upVector      = globalManipulator ?  Y_AXIS : selectedObj->transform.Up();
			Vector3 forwardVector = globalManipulator ?  Z_AXIS : selectedObj->transform.Forward();

			const Vector3 axes[3] = {rightVector, upVector, forwardVector};
			Vector3 axis = axes[selectedAxis];

			Vector3 cameraToObj = selectedObj->transform.GlobalPosition() - editorCamera.position;
			float projectionAmount = DotProduct(cameraToObj, rayDirection)/DotProduct(rayDirection, rayDirection);
			Vector3 projectedCenter = rayDirection * projectionAmount;
			Vector3 orthoPart = projectedCenter - cameraToObj;

			if(transformMode == TransformMode::Translation){
				float amount = DotProduct(orthoPart, axis);
				Vector3 delta = axis * amount;
				if(globalManipulator){
					//delta = selectedObj->transform.GlobalToLocal(delta);
				}
				selectedObj->transform.position = selectedObj->transform.position + delta;
			}
			else if(transformMode == TransformMode::Rotation){
				float amount = DotProduct(orthoPart, axes[(selectedAxis + 1) % 3]) * DotProduct(orthoPart, axes[(selectedAxis + 2) % 3]);
				//float currentAmount = Rotate(axis, selectedObj->transform.rotation)
				selectedObj->transform.rotation = Quaternion(axis, amount) * selectedObj->transform.rotation;
			}
		}

		if(input.GetMouse(GLUT_RIGHT_BUTTON)){
			float deltaX = prevX - input.mouseX;
			float deltaY = prevY - input.mouseY;

			xRot += deltaY/90;
			yRot += deltaX/90;

			editorCamera.rotation =  Quaternion(Y_AXIS, yRot) * Quaternion(X_AXIS, xRot);
		}
		else if(input.GetMouseUp(GLUT_LEFT_BUTTON)){
			RaycastHit testHit = selectionSim.Raycast(editorCamera.position, rayDirection);
			if(testHit.hit){
				SelectObject(testHit.col->gameObject);
			}
			else{
				DeselectObject();
			}

			selectedAxis = -1;
		}
	}
	
	if(selectedObj){
		Vector3 position = selectedObj->transform.position;
		static_cast<GuiText*>(editorGui.elements[1])->text = "Position: " + ToString(position.x, 4) + ", " + ToString(position.y, 4) + ", " + ToString(position.z, 4);

		Vector3 eulerAngles;
		Vector3 transformedAxes[3] = {selectedObj->transform.Right(), selectedObj->transform.Up(), selectedObj->transform.Forward()};
		const float rad2deg = 180/3.14159265f;
		eulerAngles.x = acos(transformedAxes[1].y) * ((transformedAxes[2].z > 0) ? 1 : -1) * rad2deg;
		eulerAngles.y = acos(transformedAxes[2].z) * ((transformedAxes[0].x > 0) ? 1 : -1) * rad2deg;
		eulerAngles.z = acos(transformedAxes[0].x) * ((transformedAxes[1].y > 0) ? 1 : -1) * rad2deg;
		static_cast<GuiText*>(editorGui.elements[4])->text = "Rotation: " + ToString(eulerAngles.x) + ", " + ToString(eulerAngles.y) + ", " + ToString(eulerAngles.z);

		static_cast<GuiText*>(editorGui.elements[3])->text = "Name: " + selectedObj->name;

		GuiText* meshTxt = static_cast<GuiText*>(editorGui.elements[2]);
		if(selectedObj->mesh != nullptr){
			meshTxt->text = "Mesh: " + selectedObj->mesh->fileName;
		}
		else{
			meshTxt->text = "Mesh: ";
		}

		for(EditorComponentGui& compGui : componentGui){
			compGui.Save();
		}
	}
	else{
		static_cast<GuiText*>(editorGui.elements[1])->text = "Position:";
		static_cast<GuiText*>(editorGui.elements[2])->text = "";
		static_cast<GuiText*>(editorGui.elements[3])->text = "Name:";
		static_cast<GuiText*>(editorGui.elements[4])->text = "Rotation:";
	}

	for(GameObject* obj : objects){
		for(Component* comp : obj->components){
			comp->OnEditorUpdate();
		}
	}

	for(GameObject* obj : spawnedObjects){
		AddObjectAndDescendants(obj);
	}

	for(GameObject* obj : destroyedObjects){
		DestroyObject(obj);
	}

	if(spawnedObjects.size() > 0 || destroyedObjects.size() > 0){
		RecalculateSelectionSim();
	}

	spawnedObjects.clear();
	destroyedObjects.clear();

	prevX = input.mouseX;
	prevY = input.mouseY;
}

void EditorScene::SelectObject(GameObject* obj){
	if(selectedObj != nullptr){
		DeselectObject();
	}

	selectedObj = obj;

	GuiElement* compPanel = editorGui.FindGUIElement("components_panel");

	int index = 0;
	for(Component* component : selectedObj->components){
		MetaStructInfo info = genMetaStructInfo[component->metaType];

		if(info.memberCount > 0){
			//info.name
			GuiText* guiTxt = new GuiText(&resources, "data/arial_16.fuv");
			guiTxt->position = Vector2(0.8f, 0.25f - 0.03f * index);
			guiTxt->scale = Vector2(0.1f, 0.04f);
			guiTxt->text = info.name;

			editorGui.elements.push_back(guiTxt);
			guiTxt->SetParent(compPanel);

			index++;

			for(int memIdx = 0; memIdx < info.memberCount; memIdx++){
				GuiText* fieldLabel = new GuiText(&resources, "data/arial_16.fuv");
				fieldLabel->position = Vector2(0.8f, 0.25f - 0.03f * index);
				fieldLabel->scale = Vector2(0.08f, 0.03f);
				fieldLabel->text = info.members[memIdx].name;

				editorGui.elements.push_back(fieldLabel);
				fieldLabel->SetParent(guiTxt);

				GuiTextField* inputField = new GuiTextField(&resources, "data/arial_16.fuv");
				inputField->position = Vector2(0.9f, 0.25f - 0.03f * index);
				inputField->scale = Vector2(0.1f, 0.03f);

				editorGui.elements.push_back(inputField);
				inputField->SetParent(guiTxt);

				EditorComponentGui compGui;
				compGui.type = static_cast<MetaType>(info.members[memIdx].type);
				compGui.memberPtr = ((char*)component) + info.members[memIdx].offset;
				compGui.fieldLabel = fieldLabel;
				compGui.inputText = inputField;
				compGui.Load();

				componentGui.push_back(compGui);

				index++;
			}
		}
	}
}

void EditorScene::DeselectObject(){
	selectedObj = nullptr;

	componentGui.clear();

	GuiElement* compPanel = editorGui.FindGUIElement("components_panel");
	editorGui.ClearElementChildren(compPanel);
}

void EditorScene::EditorGUI(){
	glDisable(GL_DEPTH_TEST);
	glLineWidth(8);
	Material* vertColMat = resources.GetMaterialByName("color");
	glUseProgram(vertColMat->shaderProgram);

	float width = glutGet(GLUT_WINDOW_WIDTH);
	float height = glutGet(GLUT_WINDOW_HEIGHT);
	float aspectRatio = width/height;
	float fov = 80;
	float nearClip = 0.01f;
	float farClip = 1000;

	Mat4x4 perspMatrix = GetPerspectiveMatrix(aspectRatio,fov,nearClip,farClip);
	Mat4x4 camMatrix = editorCamera.GetCameraMatrix();

	glUniformMatrix4fv(glGetUniformLocation(vertColMat->shaderProgram,  "_perspMatrix"), 1, GL_TRUE, &perspMatrix.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(vertColMat->shaderProgram,  "_cameraMatrix"), 1, GL_TRUE,  &camMatrix.m[0][0]);

	glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 1, 1, 0.8f, 1);

	for(GameObject* obj : objects){
		if(obj->GetComponent<LightComponent>() != nullptr){
			glBegin(GL_LINES);
			{
				Vector3 origin = obj->transform.GlobalPosition();// - Vector3(0.2f,0.2f,0.2f);
				Vector3 to = origin + obj->transform.Forward() * 0.4f;
				glVertex3f(origin.x, origin.y, origin.z);
				glVertex3f(to.x, to.y, to.z);
			}
			glEnd();
		}
	}

	glLineWidth(2);
	glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 1, 0.4f, 0.4f, 1);
	glEnable(GL_DEPTH_TEST);

	for(PathNode& node : pathfinding.nodes){
		Vector3 pos = node.position;
		Vector3 verts[4] = {pos + Vector3(0.2f,  0,  0.2f), pos + Vector3( 0.2f, 0, -0.2f), 
							pos + Vector3(-0.2f, 0, -0.2f), pos + Vector3(-0.2f, 0,  0.2f)};

		glBegin(GL_TRIANGLE_STRIP);
		for(int i = 0; i < 4; i++){
			glVertex3fv((GLfloat*)&verts[i]);
		}
		glEnd();

		for(PathNode* neigh : node.neighbors){
			glBegin(GL_LINES);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(neigh->position.x, neigh->position.y, neigh->position.z);
			glEnd();
		}
	}

	glLineWidth(5);
	glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0.4f, 0.6f, 0.6f, 1);

	vector<Vector3> path = pathfinding.FindPath(Vector3(-6, 1, 4), Vector3(5, 1, -6));
	Vector3 prevPos = Vector3(-4, 1, 4);
	for(auto iter = path.rbegin(); iter != path.rend(); ++iter){
		Vector3 pos = *iter;
		glBegin(GL_LINES);
		glVertex3f(prevPos.x, prevPos.y, prevPos.z);
		glVertex3f(pos.x,     pos.y,     pos.z);
		glEnd();
		prevPos = pos;
	}

	glDisable(GL_DEPTH_TEST);

	if(selectedObj != nullptr){

		Vector3 rightVector   = globalManipulator ?  X_AXIS : selectedObj->transform.Right();
		Vector3 upVector      = globalManipulator ?  Y_AXIS : selectedObj->transform.Up();
		Vector3 forwardVector = globalManipulator ?  Z_AXIS : selectedObj->transform.Forward();

		Vector3 origin = selectedObj->transform.GlobalPosition();

		if(transformMode == TransformMode::Translation){
			glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 1, 0, 0, 1);
			glBegin(GL_LINES);
			{
				Vector3 to = origin + rightVector/2;
				glVertex3f(origin.x, origin.y, origin.z);
				glVertex3f(to.x, to.y, to.z);
			}
			glEnd();

			glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0, 1, 0, 1);
			glBegin(GL_LINES);
			{
				Vector3 to = origin + upVector/2;
				glVertex3f(origin.x, origin.y, origin.z);
				glVertex3f(to.x, to.y, to.z);
			}
			glEnd();

			glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0, 0, 1, 1);
			glBegin(GL_LINES);
			{
				Vector3 to = origin + forwardVector/2;
				glVertex3f(origin.x, origin.y, origin.z);
				glVertex3f(to.x, to.y, to.z);
			}
			glEnd();
		}
		else if(transformMode == TransformMode::Rotation){
			const int precision = 24;

			float xRing[precision * 3];
			float yRing[precision * 3];
			float zRing[precision * 3];

			for(int i = 0; i < precision; i++){
				int xIndex = 3*i, yIndex = 3*i+1, zIndex = 3*i+2;
				float angle = ((float)i)/precision * 2 * 3.141592653589f;

				float sinAngle = sin(angle);
				float cosAngle = cos(angle);

				Vector3 xRingPos =  origin + (upVector * sinAngle + forwardVector * cosAngle)/2;
				xRing[xIndex] = xRingPos.x;
				xRing[yIndex] = xRingPos.y;
				xRing[zIndex] = xRingPos.z;

				Vector3 yRingPos = origin + (rightVector * sinAngle + forwardVector * cosAngle)/2;
				yRing[xIndex] = yRingPos.x;
				yRing[yIndex] = yRingPos.y;
				yRing[zIndex] = yRingPos.z;

				Vector3 zRingPos =  origin + (rightVector * sinAngle + upVector * cosAngle)/2;
				zRing[xIndex] = zRingPos.x;
				zRing[yIndex] = zRingPos.y;
				zRing[zIndex] = zRingPos.z;
			}

			glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 1, 0, 0, 1);
			glBegin(GL_LINE_LOOP);
			{
				for(int i = 0; i < precision*3; i += 3){
					glVertex3fv(&xRing[i]);
				}
			}
			glEnd();

			glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0, 1, 0, 1);
			glBegin(GL_LINE_LOOP);
			{
				for(int i = 0; i < precision*3; i += 3){
					glVertex3fv(&yRing[i]);
				}
			}
			glEnd();

			glUniform4f(glGetUniformLocation(vertColMat->shaderProgram, "_color"), 0, 0, 1, 1);
			glBegin(GL_LINE_LOOP);
			{
				for(int i = 0; i < precision*3; i += 3){
					glVertex3fv(&zRing[i]);
				}
			}
			glEnd();
		}
	}

	editorGui.RenderGui();

	glEnable(GL_DEPTH);
}

string ToString(float val, int precision /*= 2*/){
	std::ostringstream out;
    out << std::setprecision(precision) << std::fixed << val;
    return out.str();
}

static void OnEditorMouseFunc(int button, int state, int x, int y){
	EditorScene::getInstance().OnMouse(button, state, x, y);
}

static void OnEditorPassiveMouseFunc(int x, int y){
	EditorScene::getInstance().OnPassiveMouse(x, y);
}

static void OnEditorKeyFunc(unsigned char key, int x, int y){
	if(!EditorScene::getInstance().editorGui.OnKey(key)){
		EditorScene::getInstance().OnKey(key, x, y);
	}
}

static void OnEditorKeyUpFunc(unsigned char key, int x, int y){
	if(EditorScene::getInstance().editorGui.selectedElem == nullptr){
		EditorScene::getInstance().OnKeyUp(key, x, y);
	}
}

EditorScene::~EditorScene(){
	for(auto iter = selectionSim.staticBoxBodies.begin(); iter != selectionSim.staticBoxBodies.end(); iter++){
		delete (*iter);
	}
}