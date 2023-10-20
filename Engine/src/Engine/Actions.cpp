#include "Actions.h"

namespace Engine
{
	static Event GenerateEventFromAction(ActionType actionType)
	{
		switch (actionType)
		{
		case Engine::ActionType::SELECT_LEFT:
			return Event{ EventType::CHANGE_SELECTION, { -1,  0} };
		case Engine::ActionType::SELECT_RIGHT:
			return Event{ EventType::CHANGE_SELECTION, {  1,  0} };
		case Engine::ActionType::SELECT_UP:
			return Event{ EventType::CHANGE_SELECTION, {  0, -1} };
		case Engine::ActionType::SELECT_DOWN:
			return Event{ EventType::CHANGE_SELECTION, {  0,  1} };
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
		case Engine::ActionType::REPARSE_LEVEL:
			return Event{ EventType::REPARSE_LEVEL, { 0,  0} };
		case Engine::ActionType::BOARD_RESET:
			return Event{ EventType::BOARD_RESET, { 0,  0} };
		case Engine::ActionType::EDITOR_MODE:
			return Event{ EventType::ENTER_EDIT_MODE, { 0,  0} };
		case Engine::ActionType::PLAY_MODE:
			return Event{ EventType::ENTER_PLAY_MODE, { 0,  0} };
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
				if (action.Evaluate(m_MappingContext))
				{
					if (actionType == ActionType::GUESS_MODE)
					{
						m_IsGuessMode = true;
					}
					else if (actionType == ActionType::NUMBER_MODE)
					{
						m_IsGuessMode = false;
					}
					else
					{
						Event currEvent = GenerateEventFromAction(actionType);
						if (currEvent.type == EventType::NUMBER_EVENT)
						{
							if (m_IsGuessMode)
							{
								currEvent.type = EventType::ENTER_GUESS;
							}
							else
							{
								currEvent.type = EventType::ENTER_NUMBER;
							}
						}

						events.push_back(currEvent);
					}
				}
			}
		}
	}

	void ActionMap::SetCurrentMappingContext(MappingContext context)
	{
		m_MappingContext = context;
	}

	void ActionMap::AddMappingContext(MappingContext context)
	{
		m_MappingContext |= context;
	}

	void ActionMap::RemoveMappingContext(MappingContext context)
	{
		m_MappingContext &= ~context;
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