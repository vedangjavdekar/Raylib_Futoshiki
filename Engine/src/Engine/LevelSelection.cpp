#include "LevelSelection.h"

#include <sstream>
#include <filesystem>
#include <iostream>
#include <fmt/core.h>
#include "raymath.h"

#include "Serialization/Parser.h"
#include "Animations/Easings.h"
#include "Application.h"
#include "Actions.h"

#include <iostream>

namespace Engine
{
	void LevelSelection::LoadLevelNames(const std::string& directoryPath)
	{
		namespace fs = std::filesystem;

		const std::string extensionFilter = ".data";

		m_LevelNames.clear();
		m_DirectoryPath = directoryPath;

		int currentLevelIndex = 1;
		bool shouldRenameRest = false;
		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			if (entry.path().extension().string() == extensionFilter)
			{
				const std::string levelName = fmt::format(LEVEL_NAME_FMT, currentLevelIndex);
				if (shouldRenameRest || (entry.path().stem().string() != levelName))
				{
					fs::rename(entry.path(), fs::path(directoryPath + levelName + extensionFilter));
					shouldRenameRest = true;
				}

				m_LevelNames.push_back(levelName);
				currentLevelIndex++;
			}
		}
	}

	bool LevelSelection::ParseLevel(size_t levelIndex, Serialization::LevelData& outLevelData)
	{
		if (levelIndex >= m_LevelNames.size())
		{
			TraceLog(LOG_ERROR, "Level Index out of bounds");
			outLevelData = Serialization::LevelData();
			return false;
		}

		m_LoadedLevelName = m_LevelNames[levelIndex];
		outLevelData = Serialization::Parse(m_DirectoryPath + m_LevelNames[levelIndex] + ".data");
		if (outLevelData.GridSize == 0)
		{
			Application::Get().GetNotifications().AddNotification(LOG_ERROR, "Error: Level Data has invalid grid size.");
			outLevelData = Serialization::LevelData();
			return false;
		}

		return true;
	}

	std::string LevelSelection::GetLastLoadedLevelName() const
	{
		return m_LoadedLevelName;
	}

	std::string LevelSelection::GetLastLoadedLevelPath() const
	{
		return m_DirectoryPath + m_LoadedLevelName + ".data";
	}

	void LevelSelection::SaveLevel(const Serialization::LevelData& levelData, bool overwrite)
	{
		if (m_LoadedLevelName == "")
		{
			overwrite = false;
		}

		std::string levelName = m_LoadedLevelName;
		std::stringstream ss;
		if (overwrite)
		{
			ss << m_DirectoryPath << m_LoadedLevelName << ".data";
		}
		else
		{
			levelName = fmt::format(LEVEL_NAME_FMT, (m_LevelNames.size() + 1));
			ss << m_DirectoryPath << levelName << ".data";
		}

		Notifications& notifications = Application::Get().GetNotifications();
		if (Serialization::Write(levelData, ss.str()))
		{
			notifications.AddNotification(LOG_INFO, fmt::format("{} saved successfully!", levelName));
			m_LoadedLevelName = levelName;
		}
		else
		{
			notifications.AddNotification(LOG_ERROR, "Something went wrong");
		}

		if (!overwrite)
		{
			LoadLevelNames(m_DirectoryPath);
		}
	}

	void LevelSelection::ShowMenu()
	{
		m_SlideTime = 0.0f;
		m_AnimationDirection = 1;
		m_IsOpen = true;
		Application::Get().AddEvent(Event{ EventType::INPUT_LAYER_OPERATION, {(int)InputLayerOperation::PUSH, (int)MappingContext::LEVEL_SELECTION} });
	}

	void LevelSelection::ProcessEvents(Event& event)
	{
		if (m_IsOpen)
		{
			switch (event.type)
			{
			case EventType::CHANGE_SELECTION:
			{
				m_SelectedIndex += event.data[1];
				if (Settings.WrapAround)
				{
					m_SelectedIndex += m_LevelNames.size();
					m_SelectedIndex %= m_LevelNames.size();
				}
				else
				{
					m_SelectedIndex = std::min(std::max(m_SelectedIndex, 0), (int)(m_LevelNames.size() - 1));
				}
				event.handled = true;
				break;
			}
			case EventType::COMMIT:
			{
				Application::Get().AddEvent(Event{ EventType::TOGGLE_LEVEL_MENU,{1, 0} });
				event.handled = true;
				break;
			}
			case EventType::CANCEL:
			{
				Application::Get().AddEvent(Event{ EventType::TOGGLE_LEVEL_MENU,{0, 0} });
				event.handled = true;
				break;
			}
			default:
				break;
			}
		}
	}

	void LevelSelection::Update(const float deltaTime)
	{
		if (!m_IsOpen && (m_AnimationDirection == 0))
		{
			return;
		}

		if (m_AnimationDirection != 0)
		{
			m_SlideTime += m_AnimationDirection * deltaTime;
			if (m_SlideTime < 0.0f || m_SlideTime > Settings.SlideTime)
			{
				m_SlideTime = Clamp(m_SlideTime, 0.0f, Settings.SlideTime);
				m_AnimationDirection = 0;
			}
		}

		if (m_Offset != m_TargetOffset)
		{
			m_TweenTime += deltaTime;

			float timePercent = Clamp((m_TweenTime / Settings.ScrollTime), 0.0f, 1.0f);
			float easedTimePercent = Easings::EaseOutCubic(timePercent);

			m_Offset = Lerp(m_StartOffset, m_TargetOffset, easedTimePercent);
			if (m_Offset == m_TargetOffset)
			{
				m_PrevDrawOffsetIndex = m_DrawOffsetIndex;
				m_StartOffset = 0.0f;
				m_TargetOffset = 0.0f;
				m_Offset = 0.0f;
			}
		}
	}

	void LevelSelection::Draw(float widthPercent, float heightPercent, float padding)
	{
		if (!m_IsOpen && (m_AnimationDirection == 0))
		{
			return;
		}

		const float absWidth = widthPercent * GetScreenWidth();
		const float absHeight = heightPercent * GetScreenHeight();

		const float targetX = 0.5f * (GetScreenWidth() - absWidth);
		const float currentX = Lerp(GetScreenWidth(), targetX, Easings::EaseInOutCubic(m_SlideTime / Settings.SlideTime));

		Rectangle ClientArea = {
			currentX,
			0.5f * (GetScreenHeight() - absHeight),
			absWidth,
			absHeight
		};

		DrawRectangleRec(ClientArea, DARKGRAY);
		ClientArea.x += padding;
		ClientArea.y += padding;
		ClientArea.width -= 2 * padding;
		ClientArea.height -= 2 * padding;

		Vector2 nextDrawPosition = Vector2{ ClientArea.x, ClientArea.y };
		DrawText("Select Level", (int)nextDrawPosition.x, (int)nextDrawPosition.y, 36, WHITE);
		nextDrawPosition.y += 50;
		ClientArea.x = nextDrawPosition.x;
		ClientArea.y = nextDrawPosition.y;
		ClientArea.height -= 50;

		const int displayCount = ((int)ClientArea.height / Settings.ItemHeight);
		if (m_SelectedIndex < m_DrawOffsetIndex || m_SelectedIndex >= (m_DrawOffsetIndex + displayCount - 1))
		{
			m_DrawOffsetIndex = m_SelectedIndex - (displayCount / 2);

			m_DrawOffsetIndex = std::max(m_DrawOffsetIndex, 0);
			m_DrawOffsetIndex = std::min(m_DrawOffsetIndex, (int)m_LevelNames.size() - 1);

			const int deltaOffset = std::abs(m_DrawOffsetIndex - m_PrevDrawOffsetIndex);
			if (m_DrawOffsetIndex > m_PrevDrawOffsetIndex)
			{
				m_StartOffset = 0.0f;
				m_TargetOffset = -(deltaOffset * Settings.ItemHeight + (deltaOffset - 1) * Settings.Separation);
			}
			else
			{
				m_StartOffset = -(deltaOffset * Settings.ItemHeight + (deltaOffset - 1) * Settings.Separation);
				m_TargetOffset = 0.0f;
			}

			m_Offset = m_StartOffset;
			m_TweenTime = 0.0f;
		}

		BeginScissorMode((int)ClientArea.x, (int)ClientArea.y, (int)ClientArea.width, (int)ClientArea.height);
		for (int i = std::min(m_PrevDrawOffsetIndex, m_DrawOffsetIndex); i < std::min(std::max(m_PrevDrawOffsetIndex, m_DrawOffsetIndex) + displayCount, (int)m_LevelNames.size()); ++i)
		{
			float baseY = nextDrawPosition.y + m_Offset;

			ItemStyle style = (i == m_SelectedIndex) ? SelectedItemStyle : NormalItemStyle;
			DrawRectangle((int)nextDrawPosition.x, (int)baseY, (int)ClientArea.width, Settings.ItemHeight, style.ItemBackground);
			DrawText(m_LevelNames[i].c_str(),
				(int)(nextDrawPosition.x + 10),
				(int)(baseY + 0.5f * (Settings.ItemHeight - Settings.FontSize)), Settings.FontSize, style.ItemText);
			nextDrawPosition.y += Settings.ItemHeight + Settings.Separation;
		}
		EndScissorMode();
	}

	void LevelSelection::Close(bool commit)
	{
		Application::Get().AddEvent(Event{ EventType::INPUT_LAYER_OPERATION, {(int)InputLayerOperation::POP, 0} });

		if (commit)
		{
			Application::Get().AddEvent(Event{ EventType::SELECT_LEVEL,{m_SelectedIndex, 0} });
		}

		m_IsOpen = false;
		m_AnimationDirection = -1;
	}

	bool LevelSelection::IsOpen() const
	{
		return m_IsOpen;
	}

	bool LevelSelection::HasLevels() const
	{
		return m_LevelNames.size();
	}
}