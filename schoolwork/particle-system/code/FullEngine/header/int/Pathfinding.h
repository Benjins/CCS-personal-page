#ifndef PATHFINDING_H
#define PATHFINDING_H

#pragma once

#include <vector>
#include <map>
#include "../ext/Vector3.h"

using std::vector;
using std::map;

struct Scene;

struct PathNode{
	vector<PathNode*> neighbors;
	Vector3 position;
	int id;
};

struct Pathfinding{
	vector<PathNode> nodes;
	Scene* scene;

	void HookUpNodes(float maxDistance = 20);

	int AddNode(Vector3 pos);

	vector<Vector3> FindPath(Vector3 source, Vector3 destination);
	vector<Vector3> FindPath(PathNode* source, PathNode* destination);
};

#endif