#pragma once
#include <stdint.h>
#include "raylib.h"

namespace Engine
{
    enum class EventType : uint8_t
    {   
        //Common
        COMMIT,
        CANCEL,

        // Input Layers
        INPUT_LAYER_OPERATION,

        // Grid Interaction
        CHANGE_GRID_STATE,
        CHANGE_SELECTION,
        NUMBER_EVENT,
        
        // Game State
        PLAYER_WON,

        // Level Events
        BOARD_RESET,
        SAVE_LEVEL,
        TOGGLE_LEVEL_MENU,
        SELECT_LEVEL,
    };

    struct Event
    {
        EventType type;
        int data[2];
        bool handled = false;
    };

}