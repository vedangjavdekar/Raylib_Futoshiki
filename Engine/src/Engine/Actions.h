#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "raylib.h"

#include "Events.h"
#include "ActionTypes.h"

namespace Engine
{
	struct Action
	{
		KeyboardKey KeyCode;
		InteractionType Interaction;
		MappingContext Context;

		inline bool Evaluate(MappingContext currentContext) const
		{
			if ((Context & currentContext) == MappingContext::UNDEFINED && 
				(Context & MappingContext::ALWAYS_ON) == MappingContext::UNDEFINED)
			{
				return false;
			}

			switch (Interaction)
			{
			case Engine::InteractionType::PRESSED:
				return IsKeyPressed(KeyCode);
			case Engine::InteractionType::REPEATED:
				return IsKeyPressedRepeat(KeyCode);
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

		void PushInputLayer(MappingContext context);
		void PopInputLayer();

		void SetCurrentMappingContext(MappingContext context); // keeps ALWAYS_ON irrespective of the setting
		void AddMappingContext(MappingContext context);
		void RemoveMappingContext(MappingContext context);
		void RemoveAllMappingContexts(); // Only keeps ALWAYS_ON

		void AddAction(ActionType actionType, const Action& action);
		void AddAction(ActionType actionType, const KeyboardKey& key, const InteractionType interactionType = InteractionType::PRESSED,const MappingContext context = MappingContext::ALWAYS_ON);

		void RemoveAction(ActionType actionType, const KeyboardKey& key);
		void RemoveAllActions(ActionType actionType);

	private:
		std::unordered_map <ActionType, std::vector<Action>> m_ActionMap;
		std::vector<MappingContext> m_MappingContexts = { MappingContext::ALWAYS_ON | MappingContext::GAME };
		size_t m_CurrentMappingContextIndex = 0;
		bool m_IsGuessMode = false;
	};
}