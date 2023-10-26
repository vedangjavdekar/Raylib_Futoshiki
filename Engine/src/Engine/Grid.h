#pragma once
#include "raylib.h"
#include <stdint.h>
#include <vector>
#include <bitset>
#include <unordered_set>

#include "Serialization/LevelData.h"
#include "Events.h"

#define ALT_MODE_ON 1
#define ALT_MODE_OFF 0
#define ALT_MODE_NC -1

#define EDIT_MODE_ON 1
#define EDIT_MODE_OFF 0
#define EDIT_MODE_NC -1

namespace Engine
{
	Event CreateGridStateChangeEvent(int altMode, int editMode);

	struct GridStyle
	{
		Color SelectionColor = Color{ 240, 229, 77, 255 };
		Color GuessFontColor = DARKGRAY;
		Color NumberFontColor = BLACK;
		Color LockedFontColor = DARKGRAY;
		Color WrongFontColor = RED;
		Color LockedBlockColor = LIGHTGRAY;
		Color BlockBorderColor = BLACK;
		Color ConstraintColor = BLACK;
		
		float TriangleWidthPercent = 0.7f;
		float TriangleHeightPercent = 0.6f;

		int GuessFontSize = 20;
		int NumberFontSize = 48;

		int CellSize = 108;
		int BlockSize = 72;
	};

	struct GridState
	{
		bool AltMode = false;
		bool EditMode = false;
	};

	class Grid
	{
	private:
		struct CellData
		{
			std::bitset<9> Guesses{ 0 };
			uint8_t Number{ 0 };
			bool Locked = false;
		};

		struct ConstraintData
		{
			uint8_t X1, Y1;
			uint8_t X2, Y2;

			bool IsSatisfied(const Grid& grid) const;
			bool IsRowConstraint() const;
			bool IsColConstraint() const;
			bool FaceLeft() const;
			bool FaceUp() const;
			bool IsViolated(uint8_t gridSize) const;
		};

	public:
		static const uint8_t DEFAULT_GRID_SIZE = 4;


		Grid(GridStyle style = GridStyle{});

		void Update();
		void Draw();
		void Reset();
		void NewBoard(bool useDefaultSize = false, bool showNotification = true);

		void OnHandleNumber(uint8_t number);
		void OnChangeSelection(int x, int y);

		const GridState& GetGridState()const;
		void SetAltMode(bool altMode);
		void SetEditMode(bool editMode);
		
		// Will result in loss of data when changing the grid size to a lower one.
		void ChangeGridSize(uint8_t gridSize,bool retainData = true);

		void LockCell(uint8_t x, uint8_t y, uint8_t number, bool suppressNotifications = true);
		void UnlockCell(uint8_t x, uint8_t y);
		
		void AddGreaterThanConstraint(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool suppressNotifications = true);
		void RemoveGreaterThanConstraint(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

		static Serialization::LevelData GetSaveData(const Grid& grid);
		void LoadFromData(const Serialization::LevelData& levelData);

		bool PlayerWon() const;
		
		void DrawHelpText(int x, int y);

		bool HasValidData() const;

	protected:
		void ToggleGuess(uint8_t guess);
		void ToggleNumber(uint8_t number);
		void ToggleLock(uint8_t number);
		void ToggleConstraint(int x,int y);

		void FlipGreaterThanConstraint(int index);
		int GetConstraintIndex(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool& isFlipped) const;


		void DrawBlock(const CellData& cell, uint8_t x, uint8_t y, const Vector2& blockPosition);
		void DrawGuess(const CellData& cell, uint8_t x, uint8_t y, const Vector2& blockPosition);
		void DrawNumber(const CellData& cell, uint8_t x, uint8_t y, const Vector2& blockPosition);
	
		CellData& GetCellData(uint8_t x, uint8_t y);
		const CellData& GetCellData(uint8_t x, uint8_t y)const;
		const bool IsCellLocked(uint8_t x, uint8_t y)const;
		const bool IsCellValid(uint8_t x, uint8_t y)const;

		bool CheckCellHasError(uint8_t x, uint8_t y) const;
		bool CheckColHasError(uint8_t col) const;
		bool CheckRowHasError(uint8_t row) const;
		
		void MarkErrorColumn(uint8_t col);
		void MarkErrorRow(uint8_t row);
		void MarkErrorCell(uint8_t x, uint8_t y);

		void ClearAllErrors();
		void CheckConstraints();

	public:
		Vector2 Center;
		GridStyle Style;

	private:
		GridState m_State;
		uint8_t m_GridSize;

		int m_SelectedRow, m_SelectedCol;
		Vector2 m_Origin;

		std::vector<CellData> m_CellData;
		std::vector<ConstraintData> m_Constraints;

		std::bitset<99> m_Errors;

		size_t m_TargetSum;
		bool m_PlayerWon;
	};
}