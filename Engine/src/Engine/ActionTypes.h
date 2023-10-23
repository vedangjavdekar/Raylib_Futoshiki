#pragma once
#include <stdint.h>
#define BIT(x) 1 << x 

namespace Engine
{
	enum class ActionType : uint8_t
	{
		BOARD_RESET,
		SAVE_LEVEL,
		TOGGLE_LEVEL_MENU,
		EDITOR_MODE,
		PLAY_MODE,
		ALT_MODE,
		NUMBER_MODE,
		SELECT_LEFT,
		SELECT_RIGHT,
		SELECT_UP,
		SELECT_DOWN,
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		COMMIT,
		CANCEL
	};

	enum class MappingContext : uint8_t
	{
		UNDEFINED = 0,
		ALWAYS_ON = BIT(0),
		GAME = BIT(1),
		POST_GAME = BIT(2),
		EDITOR = BIT(3),
		LEVEL_SELECTION = BIT(4),
	};

	inline constexpr MappingContext operator|(MappingContext a, MappingContext b)
	{
		return static_cast<MappingContext>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	inline constexpr MappingContext operator&(MappingContext a, MappingContext b)
	{
		return static_cast<MappingContext>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	inline constexpr MappingContext& operator|=(MappingContext& a, MappingContext b)
	{
		return a = a | b;
	}

	inline constexpr MappingContext& operator&=(MappingContext& a, MappingContext b)
	{
		return a = a & b;
	}

	inline constexpr MappingContext operator ~(MappingContext a)
	{
		return static_cast<MappingContext>(~static_cast<uint8_t>(a));
	}

	enum class InteractionType : uint8_t
	{
		PRESSED,
		REPEATED,
		DOWN,
		RELEASED,
	};

	enum class InputLayerOperation : int
	{
		POP = -1,
		PUSH = 1
	};
}