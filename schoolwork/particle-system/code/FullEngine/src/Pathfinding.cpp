#include "../header/int/Pathfinding.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/Scene.h"
#include <assert.h>

#include <cfloat>

void Pathfinding::HookUpNodes(float maxDistance /*= 20*/){
	for(auto iter = nodes.begin(); iter != nodes.end(); iter++){
		iter->neighbors.clear();
		for(auto neighborIter = nodes.begin(); neighborIter != nodes.end(); neighborIter++){
			if (neighborIter != iter){
				RaycastHit testHit = scene->physicsSim->Raycast(iter->position, neighborIter->position - iter->position);
				if(!testHit.hit || testHit.depth > (neighborIter->position - iter->position).Magnitude()){
					iter->neighbors.push_back(&(*neighborIter));
				}
			}
		}
	}
}

int Pathfinding::AddNode(Vector3 pos){
	PathNode newNode;
	newNode.position = pos;
	int newId = 0;
	if(nodes.size() > 0){
		newId = nodes[nodes.size() - 1].id + 1;
	}
	newNode.id = newId;
	nodes.push_back(newNode);

	HookUpNodes();
	return newId;
}

vector<Vector3> Pathfinding::FindPath(Vector3 source, Vector3 destination){
	float closestSourceDist = FLT_MAX;
	float closestDestDist = FLT_MAX;

	PathNode* sourceNode = nullptr;
	PathNode* destNode   = nullptr;

	for(auto iter = nodes.begin(); iter != nodes.end(); iter++){
		float sourceDist = (iter->position - source).MagnitudeSquared();
		float destDist   = (iter->position - destination).MagnitudeSquared();

		if(sourceDist < closestSourceDist){
			closestSourceDist = sourceDist;
			sourceNode = &(*iter);
		}

		if(destDist < closestDestDist){
			closestDestDist = destDist;
			destNode = &(*iter);
		}
	}

	vector<Vector3> path = FindPath(sourceNode, destNode);
	path.insert(path.begin(), destination);
	return path;
}

vector<Vector3> Pathfinding::FindPath(PathNode* source, PathNode* destination){
	map<PathNode*, float> dist;
	map<PathNode*, PathNode*> prev;
	vector<PathNode*> Q;

	for(PathNode& node : nodes){
		dist[&node] = FLT_MAX;
		prev[&node] = nullptr;
		Q.push_back(&node);
	}

	dist[source] = 0;

	while(Q.size() > 0){
		float minDist = FLT_MAX;
		PathNode* minDistNode = nullptr;
		int minDistNodeIndex=-1;
		int index=0;
		for(PathNode* node : Q){
			if(dist[node] < minDist){
				minDist = dist[node];
				minDistNode = node;
				minDistNodeIndex = index;
			}
			index++;
		}

		if(minDistNode == nullptr){
			break;
		}
		//assert(minDistNode != nullptr);

		if(minDistNode == destination){
			vector<Vector3> path;
			PathNode* current = destination;
			while(current != nullptr){
				path.push_back(current->position);
				current = prev[current];
			}

			return path;
		}

		Q.erase(Q.begin() + minDistNodeIndex);

		for(PathNode* neigh : minDistNode->neighbors){
			float alt = dist[minDistNode] + (minDistNode->position - neigh->position).Magnitude();
			if(alt < dist[neigh]){
				dist[neigh] = alt;
				prev[neigh] = minDistNode;
			}
		}
	}

	cout << "\nWarning: could not find path in pathfinding.\n";
	return vector<Vector3>();
}

/*
1  function Dijkstra(Graph, source):
13      
27      return dist[], prev[]
28
29  end function
*/
