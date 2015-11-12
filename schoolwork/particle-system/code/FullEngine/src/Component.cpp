#include "../header/int/Component.h"
#include "../header/int/GameObject.h"
#include "../header/ext/simple-xml.h"

Component::Component(){
}

void Component::OnAwake(){
}
	
void Component::OnUpdate(){
}

void Component::OnCollision(Collider* col){
}

XMLElement Component::Serialize(){
	XMLElement elem;
	elem.name = "";
	return elem;
}

void Component::Deserialize(const XMLElement& data){
	
}

Component::~Component(){
	
}