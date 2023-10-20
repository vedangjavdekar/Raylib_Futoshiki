#pragma once
#include <stdint.h>
#include "raylib.h"

namespace Engine
{
    enum class EventType : uint8_t
    {
        BOARD_RESET,
        ENTER_EDIT_MODE,
        ENTER_PLAY_MODE,
        CHANGE_SELECTION,
        NUMBER_EVENT,
        ENTER_GUESS,
        ENTER_NUMBER,
        PLAYER_WON,
        REPARSE_LEVEL
    };

    struct Event
    {
        EventType type;
        int data[2];
    };

}