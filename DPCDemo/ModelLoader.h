#pragma once
#include <string>
#include "FanMesh.h"

class ModelLoader {
public:
	bool Load(std::string path, FanMesh* const fanMesh);
};

