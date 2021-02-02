#pragma once
#include "FanMesh.h"

class ModelLoader {
public:
	bool Load(std::string path, FanMesh* const fanMesh);
};

