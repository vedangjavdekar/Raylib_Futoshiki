#pragma once
#include <string>
#include "LevelData.h"

namespace Serialization
{
	LevelData Parse(const std::string& filepath);
	bool Write(const LevelData& levelData, const std::string& filepath);
}