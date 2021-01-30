#pragma once
#include "FanMesh.h"

class ModelLoader {
public:
	void Load(std::string path, FanMesh* const fanMesh);
};

