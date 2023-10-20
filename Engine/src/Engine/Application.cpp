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
		, m_Grid(6, 6)
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
		m_EventQueue.push_back(customEvent);
	}

	void Application::Init()
	{
		SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
		InitWindow(m_ApplicationProps.Width, m_ApplicationProps.Height, m_ApplicationProps.Title.c_str());
		SetTargetFPS(144);

		m_Grid.Center = Vector2{ 0.5f * GetScreenWidth(), 0.5f * GetScreenHeight() };

		SetupKeybindings();

		Serialization::Parser parser;
		Serialization::LevelData data = parser.Parse("./data/Level.data");
		m_Grid.LoadFromData(data);

		m_IsRunning = true;
	}

	void Application::Update(const float deltaTime)
	{
		if (IsWindowResized())
		{
			m_Grid.Center = Vector2{ 0.5f * GetScreenWidth(), 0.5f * GetScreenHeight() };
		}

		ProcessEvents();

		m_Grid.Update();
	}

	void Application::Draw()
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("Futoshiki", 10, 10, 36, DARKGRAY);
		DrawText("CTRL + Num = Guess", 10, 50, 16, GRAY);
		DrawText("Num = Number", 10, 75, 16, GRAY);
		DrawText("WASD/Arrow Keys = Navigation", 10, 100, 16, GRAY);
		DrawText("R = Reset", 10, 125, 16, RED);

		if (m_Grid.PlayerWon())
		{
			const float width = MeasureText("You won!", 64);
			DrawText("You won!", 0.5f * (GetScreenWidth() - width), 10, 64, GREEN);
		}

		m_Grid.Draw();

		EndDrawing();
	}

	void Application::ProcessEvents()
	{
		m_ActionMap.GenerateEvents(m_EventQueue);

		for (const auto& event : m_EventQueue)
		{
			switch (event.type)
			{
			case EventType::REPARSE_LEVEL:
			{
				Serialization::Parser parser;
				Serialization::LevelData data = parser.Parse("./data/Level.data");
				m_Grid.LoadFromData(data);
				break;
			}
			case EventType::BOARD_RESET:
			{
				m_Grid.Reset();
				m_ActionMap.AddMappingContext(MappingContext::GAME);
				break;
			}
			case EventType::PLAYER_WON:
			{
				m_ActionMap.RemoveMappingContext(MappingContext::GAME);
				break;
			}
			case EventType::CHANGE_SELECTION:
			{
				m_Grid.AddSelection(event.data[0], event.data[1]);
				break;
			}
			case EventType::ENTER_GUESS:
			{
				m_Grid.ToggleGuess(event.data[0]);
				break;
			}
			case EventType::ENTER_NUMBER:
			{
				m_Grid.ToggleNumber(event.data[0]);
				break;
			}
			default:
			{
				break;
			}
			}
		}

		m_EventQueue.clear();
	}

	void Application::SetupKeybindings()
	{
		m_ActionMap.AddAction(ActionType::SELECT_UP, KEY_UP, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::SELECT_UP, KEY_W, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::SELECT_DOWN, KEY_DOWN, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::SELECT_DOWN, KEY_S, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::SELECT_LEFT, KEY_LEFT, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::SELECT_LEFT, KEY_A, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::SELECT_RIGHT, KEY_RIGHT, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::SELECT_RIGHT, KEY_D, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::ONE, KEY_ONE, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::ONE, KEY_KP_1, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::TWO, KEY_TWO, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::TWO, KEY_KP_2, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::THREE, KEY_THREE, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::THREE, KEY_KP_3, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::FOUR, KEY_FOUR, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::FOUR, KEY_KP_4, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::FIVE, KEY_FIVE, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::FIVE, KEY_KP_5, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::SIX, KEY_SIX, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::SIX, KEY_KP_6, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::SEVEN, KEY_SEVEN, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::SEVEN, KEY_KP_7, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::EIGHT, KEY_EIGHT, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::EIGHT, KEY_KP_8, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::NINE, KEY_NINE, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::NINE, KEY_KP_9, InteractionType::PRESSED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::GUESS_MODE, KEY_LEFT_CONTROL, InteractionType::PRESSED, MappingContext::GAME);
		m_ActionMap.AddAction(ActionType::NUMBER_MODE, KEY_LEFT_CONTROL, InteractionType::RELEASED, MappingContext::GAME);

		m_ActionMap.AddAction(ActionType::BOARD_RESET, KEY_R, InteractionType::PRESSED, MappingContext::ALWAYS_ON);
		m_ActionMap.AddAction(ActionType::REPARSE_LEVEL, KEY_O, InteractionType::PRESSED, MappingContext::ALWAYS_ON);
	}

}