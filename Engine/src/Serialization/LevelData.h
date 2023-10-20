#pragma once
#include <vector>

namespace Serialization
{
    struct LockedNumber
    {
        uint8_t X;
        uint8_t Y;
        uint8_t Val;
    };

    struct GreaterThanConstraint
    {
        uint8_t X1;
        uint8_t Y1;
        uint8_t X2;
        uint8_t Y2;
    };

    struct LevelData
    {
        uint8_t GridSize;
        std::vector<LockedNumber> LockedCells;
        std::vector<GreaterThanConstraint> GreaterThanConstraints;
    };
}