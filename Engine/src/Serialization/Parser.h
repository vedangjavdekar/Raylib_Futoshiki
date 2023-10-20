#pragma once
#include <string>
#include "LevelData.h"

namespace Serialization
{
	class Parser
	{
	public:
		LevelData Parse(const std::string& filepath);

		// TODO: Serialize to file.
		bool Write(const LevelData& levelData, const std::string& filepath);
	};
}