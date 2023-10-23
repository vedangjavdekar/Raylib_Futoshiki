#pragma once
#include <vector>
#include <string>

#include "Events.h"
#include "Actions.h"

#include "Serialization/LevelData.h"

namespace Engine
{
	struct ScrollSettings
	{
		int ItemHeight = 50;
		int Separation = 5;
		int FontSize = 30;
		float ScrollMultiplier = 0.5f;
		float ScrollTime = 0.5f;

		bool WrapAround = true;
	};

	struct ItemStyle
	{
		Color ItemText = WHITE;
		Color ItemBackground = BLACK;
	};

	class LevelSelection
	{
		const std::string LEVEL_NAME_FMT = "Level{:03}";

	public:
		void LoadLevelNames(const std::string& directoryPath);
		bool ParseLevel(size_t levelIndex, Serialization::LevelData& outLevelData);
		std::string GetLastLoadedLevelPath() const;
		std::string GetLastLoadedLevelName() const;

		void SaveLevel(const Serialization::LevelData& levelData, bool overwrite = false);
		void ShowMenu();

		void ProcessEvents(Event& event);
		void Update(const float deltaTime);
		void Draw(float widthPercent = 0.75f, float heightPercent = 0.75f, float padding = 10);

		void Close(bool commit);

		bool IsOpen() const;

		bool HasLevels() const;

	public:
		ScrollSettings Settings;
		ItemStyle NormalItemStyle{ WHITE, GRAY };
		ItemStyle SelectedItemStyle{ WHITE, ORANGE };
	private:
		bool m_IsOpen = false;

		std::string m_LoadedLevelName;
		std::string m_DirectoryPath;
		std::vector<std::string> m_LevelNames;

		int m_DrawOffsetIndex = 0;
		int m_PrevDrawOffsetIndex = 0;
		int m_SelectedIndex = 0;

		float m_Offset = 0.0f;
		float m_TweenTime = 0.0f;

		float m_TargetOffset = 0.0f;
		float m_StartOffset = 0.0f;
	};
}