#include "Application.h"
#include "Serialization/Parser.h"
#include "Serialization/LevelData.h"

#include "raylib.h"

namespace Engine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(ApplicationProps props)
		:m_ApplicationProps(props)
		, m_IsRunning(false)
		, m_Grid()
	{
		s_Instance = this;
	}

	Application::~Application()
	{
		s_Instance = nullptr;
	}

	void Application::Run()
	{
		if (m_IsRunning)
		{
			TraceLog(LOG_FATAL, "Only one instance of the application can be running at any time.\n");
		}

		Init();

		while ((!WindowShouldClose()) && m_IsRunning)
		{
			const float deltaTime = GetFrameTime();

			Update(deltaTime);

			Draw();
		}

		CloseWindow();
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}

	Application& Application::Get()
	{
		return *s_Instance;
	}

	void Application::AddEvent(const Event& customEvent)
	{
		m_PropagatedEventQueue.push_back(customEvent);
	}

	Notifications& Application::GetNotifications()
	{
		return m_Notifications;
	}

	void Application::Init()
	{
		SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
		InitWindow(m_ApplicationProps.Width, m_ApplicationProps.Height, m_ApplicationProps.Title.c_str());
		SetTargetFPS(144);
		SetExitKey(0);

		m_Grid.Center = Vector2{ 0.5f * GetScreenWidth(), 0.5f * GetScreenHeight() };

		SetupKeybindings();

		m_LevelSelection.LoadLevelNames("./data/");
		if (m_LevelSelection.HasLevels())
		{
			AddEvent(Event{ EventType::TOGGLE_LEVEL_MENU,{0,0} });
		}
		else
		{
			m_Notifications.AddNotification(LOG_INFO, "No levels were found. Starting Editor!");
			m_Grid.NewBoard(true, false);

			// Change to Edit mode by default
			AddEvent(CreateGridStateChangeEvent(ALT_MODE_NC, EDIT_MODE_ON));
		}

		m_IsRunning = true;
	}

	void Application::Update(const float deltaTime)
	{
		if (IsWindowResized())
		{
			m_Grid.Center = Vector2{ 0.5f * GetScreenWidth(), 0.5f * GetScreenHeight() };
		}


		if (IsKeyPressed(KEY_B))
		{
			m_Notifications.AddNotification(LOG_INFO, "Game Loaded successfully!");
		}

		if (IsKeyPressed(KEY_N))
		{
			m_Notifications.AddNotification(LOG_WARNING, "Game Loaded partially!");
		}

		if (IsKeyPressed(KEY_M))
		{
			m_Notifications.AddNotification(LOG_ERROR, "Game Loaded with Errors!");
		}

		m_ActionMap.GenerateEvents(m_EventQueue);

		do
		{
			ProcessEvents();
		} while (m_EventQueue.size());

		m_LevelSelection.Update(deltaTime);

		m_Grid.Update();

		m_Notifications.Update(deltaTime);
	}

	void Application::Draw()
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawText("Futoshiki", 10, 10, 36, DARKGRAY);
		const uint8_t fontSize = 20;
		if (!m_Grid.GetGridState().EditMode)
		{
			const char* levelSelectionText = "L = Toggle Level Selection";
			const int levelSelectionHelpTextWidth = MeasureText(levelSelectionText, fontSize);
			DrawText(levelSelectionText, (GetScreenWidth() - levelSelectionHelpTextWidth - 10), 10, fontSize, DARKGRAY);
		}

		const int levelNameWidth = MeasureText(m_LevelSelection.GetLastLoadedLevelName().c_str(), fontSize);
		DrawText(m_LevelSelection.GetLastLoadedLevelName().c_str(), (GetScreenWidth() - levelNameWidth) / 2, 10, fontSize, DARKGRAY);

		m_Grid.Draw();

		m_LevelSelection.Draw();

		const float offsetPercentX = 0.5f;
		const float offsetPercentY = 0.99f;
		m_Notifications.Draw(offsetPercentX, offsetPercentY);

		EndDrawing();
	}

	void Application::ProcessEvents()
	{
		for (auto& event: m_EventQueue)
		{
			m_LevelSelection.ProcessEvents(event);

			if (event.handled)
			{
				continue;
			}

			switch (event.type)
			{
			case EventType::INPUT_LAYER_OPERATION:
			{
				if (event.data[0] == (int)InputLayerOperation::PUSH)
				{
					m_ActionMap.PushInputLayer((MappingContext)event.data[1]);
				}
				else if (event.data[0] == (int)InputLayerOperation::POP)
				{
					m_ActionMap.PopInputLayer();
				}

				break;
			}
			case EventType::BOARD_RESET:
			{
				if (m_Grid.GetGridState().EditMode)
				{
					if (m_Grid.GetGridState().AltMode)
					{
						m_Grid.NewBoard();
					}
					else
					{ 
						m_Grid.LoadFromData(Serialization::Parse(m_LevelSelection.GetLastLoadedLevelPath()));
					}
				}
				else
				{
					m_Grid.Reset();
					m_ActionMap.RemoveAllMappingContexts();
					m_ActionMap.AddMappingContext(MappingContext::GAME);
				}
				break;
			}
			case EventType::PLAYER_WON:
			{
				m_ActionMap.RemoveAllMappingContexts();
				m_ActionMap.AddMappingContext(MappingContext::POST_GAME);
				break;
			}
			case EventType::CHANGE_SELECTION:
			{
				m_Grid.OnChangeSelection(event.data[0], event.data[1]);
				break;
			}
			case EventType::NUMBER_EVENT:
			{
				m_Grid.OnHandleNumber(event.data[0]);
				break;
			}
			case EventType::CHANGE_GRID_STATE:
			{
				if (event.data[0] != ALT_MODE_NC)
				{
					m_Grid.SetAltMode(event.data[0]);
				}

				if (event.data[1] != EDIT_MODE_NC)
				{
					m_Grid.SetEditMode(event.data[1]);

					m_ActionMap.RemoveAllMappingContexts();

					if (event.data[1])
					{
						m_ActionMap.AddMappingContext(MappingContext::EDITOR);
					}
					else
					{
						m_ActionMap.AddMappingContext(MappingContext::GAME);
					}
				}
				break;
			}
			case EventType::TOGGLE_LEVEL_MENU:
			{
				if (m_LevelSelection.IsOpen())
				{
					const bool shouldCommit = event.data[0];
					m_LevelSelection.Close(shouldCommit);
					if (!shouldCommit && !m_Grid.HasValidData())
					{
						AddEvent(CreateGridStateChangeEvent(ALT_MODE_NC, EDIT_MODE_ON));
						m_Grid.NewBoard();
					}
				}
				else
				{
					m_LevelSelection.ShowMenu();
				}
				break;
			}
			case EventType::SAVE_LEVEL:
			{
				m_LevelSelection.SaveLevel(Grid::GetSaveData(m_Grid), m_Grid.GetGridState().AltMode);
				break;
			}
			case EventType::SELECT_LEVEL:
			{
				Serialization::LevelData levelData;
				if (m_LevelSelection.ParseLevel(event.data[0], levelData))
				{
					m_Grid.LoadFromData(levelData);
					m_ActionMap.RemoveAllMappingContexts();
					m_ActionMap.AddMappingContext(MappingContext::GAME);
					AddEvent(CreateGridStateChangeEvent(ALT_MODE_NC, EDIT_MODE_OFF));
				}
				else
				{
					AddEvent(CreateGridStateChangeEvent(ALT_MODE_NC, EDIT_MODE_ON));
					m_Grid.NewBoard();
				}
				break;
			}
			case EventType::CANCEL:
			{
				if (m_Grid.GetGridState().EditMode)
				{
					AddEvent(CreateGridStateChangeEvent(ALT_MODE_NC, EDIT_MODE_OFF));
				}
#ifdef BUILD_DEBUG
				else
				{
					Close();
				}
#endif
				break;
			}
			default:
			{
				break;
			}
			}
		}

		m_EventQueue.clear();

		if (!m_PropagatedEventQueue.empty())
		{
			m_EventQueue.insert(m_EventQueue.end(), m_PropagatedEventQueue.begin(), m_PropagatedEventQueue.end());
			m_PropagatedEventQueue.clear();
		}
	}

	void Application::SetupKeybindings()
	{
		m_ActionMap.AddAction(ActionType::COMMIT, KEY_ENTER, InteractionType::PRESSED, MappingContext::ALWAYS_ON);
		m_ActionMap.AddAction(ActionType::CANCEL, KEY_ESCAPE, InteractionType::PRESSED, MappingContext::ALWAYS_ON);

		m_ActionMap.AddAction(ActionType::SELECT_UP, KEY_UP, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR | MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_UP, KEY_UP, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_UP, KEY_W, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR | MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_UP, KEY_W, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);

		m_ActionMap.AddAction(ActionType::SELECT_DOWN, KEY_DOWN, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR | MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_DOWN, KEY_DOWN, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_DOWN, KEY_S, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR | MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_DOWN, KEY_S, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);

		m_ActionMap.AddAction(ActionType::SELECT_LEFT, KEY_LEFT, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::SELECT_LEFT, KEY_LEFT, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_LEFT, KEY_A, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::SELECT_LEFT, KEY_A, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);

		m_ActionMap.AddAction(ActionType::SELECT_RIGHT, KEY_RIGHT, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::SELECT_RIGHT, KEY_RIGHT, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);
		m_ActionMap.AddAction(ActionType::SELECT_RIGHT, KEY_D, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::SELECT_RIGHT, KEY_D, InteractionType::REPEATED, MappingContext::LEVEL_SELECTION);

		m_ActionMap.AddAction(ActionType::ONE, KEY_ONE, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::ONE, KEY_KP_1, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::TWO, KEY_TWO, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::TWO, KEY_KP_2, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::THREE, KEY_THREE, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::THREE, KEY_KP_3, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::FOUR, KEY_FOUR, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::FOUR, KEY_KP_4, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::FIVE, KEY_FIVE, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::FIVE, KEY_KP_5, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::SIX, KEY_SIX, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::SIX, KEY_KP_6, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::SEVEN, KEY_SEVEN, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::SEVEN, KEY_KP_7, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::EIGHT, KEY_EIGHT, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::EIGHT, KEY_KP_8, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::NINE, KEY_NINE, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::NINE, KEY_KP_9, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::ALT_MODE, KEY_LEFT_CONTROL, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::NUMBER_MODE, KEY_LEFT_CONTROL, InteractionType::RELEASED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::ALT_MODE, KEY_RIGHT_CONTROL, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::NUMBER_MODE, KEY_RIGHT_CONTROL, InteractionType::RELEASED, MappingContext::GAME | MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::BOARD_RESET, KEY_R, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR | MappingContext::POST_GAME);
		m_ActionMap.AddAction(ActionType::SAVE_LEVEL, KEY_F, InteractionType::PRESSED, MappingContext::EDITOR);

		m_ActionMap.AddAction(ActionType::TOGGLE_LEVEL_MENU, KEY_L, InteractionType::PRESSED, MappingContext::ALWAYS_ON);

		m_ActionMap.AddAction(ActionType::EDITOR_MODE, KEY_E, InteractionType::PRESSED, MappingContext::GAME | MappingContext::POST_GAME | MappingContext::EDITOR);
		m_ActionMap.AddAction(ActionType::PLAY_MODE, KEY_P, InteractionType::PRESSED, MappingContext::GAME | MappingContext::EDITOR);
	}

}