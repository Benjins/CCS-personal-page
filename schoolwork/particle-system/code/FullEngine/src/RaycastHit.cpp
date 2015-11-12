#include "../header/int/RaycastHit.h"

RaycastHit::RaycastHit(){
	worldPos = Vector3(0,0,0);
	hit = false;
	texCoords = Vector2(0,0);
	col = NULL;
	depth = 0;
}