#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "raylib.h"
#include "Events.h"


#define BIT(x) 1 << x 

namespace Engine
{
	enum class ActionType : uint8_t
	{
		BOARD_RESET,
		REPARSE_LEVEL,
		EDITOR_MODE,
		PLAY_MODE,
		GUESS_MODE,
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
	};

	enum class MappingContext : uint8_t
	{
		UNDEFINED = 0,
		ALWAYS_ON = BIT(0),
		GAME = BIT(1),
		EDITOR = BIT(2),
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
		DOWN,
		RELEASED,
	};

	struct Action
	{
		KeyboardKey KeyCode;
		InteractionType Interaction;
		MappingContext Context;

		inline bool Evaluate(MappingContext currentContext) const
		{
			if ((Context & currentContext) == MappingContext::UNDEFINED && (Context & MappingContext::ALWAYS_ON) == MappingContext::UNDEFINED)
			{
				return false;
			}

			switch (Interaction)
			{
			case Engine::InteractionType::PRESSED:
				return IsKeyPressed(KeyCode);
			case Engine::InteractionType::DOWN:
				return IsKeyDown(KeyCode);
			case Engine::InteractionType::RELEASED:
				return IsKeyReleased(KeyCode);
			default:
				return false;
			}
		}
	};

	class ActionMap
	{
	public:
		void GenerateEvents(std::vector<Event>& events);
		void SetCurrentMappingContext(MappingContext context);
		void AddMappingContext(MappingContext context);
		void RemoveMappingContext(MappingContext context);

		void AddAction(ActionType actionType, const Action& action);
		void AddAction(ActionType actionType, const KeyboardKey& key, const InteractionType interactionType = InteractionType::PRESSED,const MappingContext context = MappingContext::ALWAYS_ON);

		void RemoveAction(ActionType actionType, const KeyboardKey& key);
		void RemoveAllActions(ActionType actionType);
	private:
		std::unordered_map <ActionType, std::vector<Action>> m_ActionMap;
		MappingContext m_MappingContext = MappingContext::ALWAYS_ON | MappingContext::GAME;
		bool m_IsGuessMode = false;
	};
}