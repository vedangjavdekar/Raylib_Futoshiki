#include "Actions.h"

namespace Engine
{
	static Event GenerateEventFromAction(ActionType actionType)
	{
		switch (actionType)
		{
			// Common Events
		case Engine::ActionType::COMMIT:
			return Event{ EventType::COMMIT, { 0, 0} };
		case Engine::ActionType::CANCEL:
			return Event{ EventType::CANCEL, { 0, 0} };

			// Selection Events
		case Engine::ActionType::SELECT_LEFT:
			return Event{ EventType::CHANGE_SELECTION, { -1,  0} };
		case Engine::ActionType::SELECT_RIGHT:
			return Event{ EventType::CHANGE_SELECTION, {  1,  0} };
		case Engine::ActionType::SELECT_UP:
			return Event{ EventType::CHANGE_SELECTION, {  0, -1} };
		case Engine::ActionType::SELECT_DOWN:
			return Event{ EventType::CHANGE_SELECTION, {  0,  1} };

			// Number Events
		case Engine::ActionType::ONE:
			return Event{ EventType::NUMBER_EVENT, { 1,  0} };
		case Engine::ActionType::TWO:
			return Event{ EventType::NUMBER_EVENT, { 2,  0} };
		case Engine::ActionType::THREE:
			return Event{ EventType::NUMBER_EVENT, { 3,  0} };
		case Engine::ActionType::FOUR:
			return Event{ EventType::NUMBER_EVENT, { 4,  0} };
		case Engine::ActionType::FIVE:
			return Event{ EventType::NUMBER_EVENT, { 5,  0} };
		case Engine::ActionType::SIX:
			return Event{ EventType::NUMBER_EVENT, { 6,  0} };
		case Engine::ActionType::SEVEN:
			return Event{ EventType::NUMBER_EVENT, { 7,  0} };
		case Engine::ActionType::EIGHT:
			return Event{ EventType::NUMBER_EVENT, { 8,  0} };
		case Engine::ActionType::NINE:
			return Event{ EventType::NUMBER_EVENT, { 9,  0} };

			// Grid Modes
		case Engine::ActionType::ALT_MODE:
			return Event{ EventType::CHANGE_GRID_STATE, { 1,  -1} };
		case Engine::ActionType::NUMBER_MODE:
			return Event{ EventType::CHANGE_GRID_STATE, { 0,  -1} };
		case Engine::ActionType::EDITOR_MODE:
			return Event{ EventType::CHANGE_GRID_STATE, { -1, 1} };
		case Engine::ActionType::PLAY_MODE:
			return Event{ EventType::CHANGE_GRID_STATE, { -1, 0} };

			// Level Events
		case Engine::ActionType::BOARD_RESET:
			return Event{ EventType::BOARD_RESET, { 0,  0} };
		case Engine::ActionType::SAVE_LEVEL:
			return Event{ EventType::SAVE_LEVEL, { 0,  0} };
		case Engine::ActionType::TOGGLE_LEVEL_MENU:
			return Event{ EventType::TOGGLE_LEVEL_MENU, { 0,  0} };

		default:
			return Event{ EventType::CHANGE_SELECTION, { 0,  0} };
		}
	}
	void ActionMap::GenerateEvents(std::vector<Event>& events)
	{
		for (const auto& [actionType, actions] : m_ActionMap)
		{
			for (const auto& action : actions)
			{
				if (action.Evaluate(m_MappingContexts[m_CurrentMappingContextIndex]))
				{
					Event currEvent = GenerateEventFromAction(actionType);
					events.push_back(currEvent);
				}
			}
		}
	}

	void ActionMap::PushInputLayer(MappingContext context)
	{
		MappingContext& newContext = m_MappingContexts.emplace_back();
		newContext = MappingContext::ALWAYS_ON | context;
		m_CurrentMappingContextIndex++;
	}

	void ActionMap::PopInputLayer()
	{
		if (m_MappingContexts.size() > 1)
		{
			m_MappingContexts.pop_back();
			m_CurrentMappingContextIndex--;
		}
	}

	void ActionMap::SetCurrentMappingContext(MappingContext context)
	{
		m_MappingContexts[m_CurrentMappingContextIndex] = context | MappingContext::ALWAYS_ON;
	}

	void ActionMap::AddMappingContext(MappingContext context)
	{
		m_MappingContexts[m_CurrentMappingContextIndex] |= context;
	}

	void ActionMap::RemoveMappingContext(MappingContext context)
	{
		m_MappingContexts[m_CurrentMappingContextIndex] &= ~context;
	}

	void ActionMap::RemoveAllMappingContexts()
	{
		m_MappingContexts[m_CurrentMappingContextIndex] = MappingContext::ALWAYS_ON;
	}

	void ActionMap::AddAction(ActionType actionType, const Action& action)
	{
		if (m_ActionMap.find(actionType) == m_ActionMap.end())
		{
			m_ActionMap.emplace(actionType, std::vector<Action>{ action });
		}
		else
		{
			bool needsAdding = true;
			for (auto& _action : m_ActionMap.at(actionType))
			{
				if (_action.Interaction == action.Interaction && _action.KeyCode == action.KeyCode)
				{
					needsAdding = false;
					break;
				}
			}

			if (needsAdding)
			{
				m_ActionMap.at(actionType).emplace_back(action);
			}
		}
	}

	void ActionMap::AddAction(ActionType actionType, const KeyboardKey& key, const InteractionType interactionType, const MappingContext context)
	{
		AddAction(actionType, Action{ key, interactionType, context });
	}

	void ActionMap::RemoveAction(ActionType actionType, const KeyboardKey& key)
	{
		if (m_ActionMap.find(actionType) == m_ActionMap.end())
		{
			return;
		}

		std::vector<Action>& actions = m_ActionMap.at(actionType);
		for (auto itr = actions.begin(); itr != actions.end();)
		{
			if (itr->KeyCode == key)
			{
				itr = actions.erase(itr);
			}
			else
			{
				itr = std::next(itr);
			}
		}
	}

	void ActionMap::RemoveAllActions(ActionType actionType)
	{
		if (m_ActionMap.find(actionType) == m_ActionMap.end())
		{
			return;
		}

		m_ActionMap.erase(actionType);
	}

}