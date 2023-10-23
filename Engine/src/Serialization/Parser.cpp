#include "Parser.h"
#include <fstream>
#include <sstream>

#include <fmt/core.h>

namespace Serialization
{
	LevelData Parse(const std::string& filepath)
	{
		LevelData data;
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			return LevelData();
		}

		std::string line;

		while (getline(file, line))
		{
			std::stringstream ss(line);
			std::string word;

			bool parseSize = false;
			bool parseNumber = false;
			bool parseConstraint = false;

			size_t numberCounter = 0;
			size_t constraintCounter = 0;

			while (ss >> word)
			{
				if (parseSize)
				{
					data.GridSize = std::stoi(word);
					parseSize = false;
				}
				else if (parseNumber)
				{
					switch (numberCounter)
					{
					case 0:
						data.LockedCells.back().X = std::stoi(word);
						break;
					case 1:
						data.LockedCells.back().Y = std::stoi(word);
						break;
					case 2:
						data.LockedCells.back().Val = std::stoi(word);
						parseNumber = false;
						break;
					}
					numberCounter++;
				}
				else if (parseConstraint)
				{
					switch (constraintCounter)
					{
					case 0:
						data.GreaterThanConstraints.back().X1 = std::stoi(word);
						break;
					case 1:
						data.GreaterThanConstraints.back().Y1 = std::stoi(word);
						break;
					case 2:
						data.GreaterThanConstraints.back().X2 = std::stoi(word);
						break;
					case 3:
						data.GreaterThanConstraints.back().Y2 = std::stoi(word);
						parseConstraint = false;
						break;
					}
					constraintCounter++;
				}

				if (word == "S" || word == "s")
				{
					parseSize = true;
				}
				else if (word == "N" || word == "n")
				{
					parseNumber = true;
					data.LockedCells.emplace_back();
					numberCounter = 0;
				}
				else if (word == "C" || word == "c")
				{
					parseConstraint = true;
					data.GreaterThanConstraints.emplace_back();
					constraintCounter = 0;
				}
			}
		}

		return data;
	}

	bool Write(const LevelData& levelData, const std::string& filepath)
	{
		std::ofstream file(filepath, std::ofstream::out);

		if (!file.is_open())
		{
			return false;
		}

		std::stringstream ss;
		ss << "S " << std::to_string(levelData.GridSize) << "\n";
		for (auto& lockedCell : levelData.LockedCells)
		{
			ss << "N " 
				<< std::to_string(lockedCell.X) << " " 
				<< std::to_string(lockedCell.Y) << " " 
				<< std::to_string(lockedCell.Val) << "\n";
		}

		for (auto& constraint : levelData.GreaterThanConstraints)
		{
			ss << "C " 
				<< std::to_string(constraint.X1) << " " 
				<< std::to_string(constraint.Y1) << " " 
				<< std::to_string(constraint.X2) << " "
				<< std::to_string(constraint.Y2) << "\n";
		}

		file << ss.str();

		file.close();
		return true;
	}
}