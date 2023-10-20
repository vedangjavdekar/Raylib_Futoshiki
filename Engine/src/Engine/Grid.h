#pragma once
#include "raylib.h"
#include <stdint.h>
#include <vector>
#include <bitset>
#include <unordered_set>

#include "Serialization/LevelData.h"

namespace Engine
{
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

		float CellSize = 108.0f;
		float BlockSize = 72.0f;
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
		};

	public:
		Grid(uint8_t rows, uint8_t cols, GridStyle style = GridStyle{}, bool isEditMode = false);

		void Update();
		void Draw();
		void Reset();

		void ToggleGuess(uint8_t guess);
		void ToggleNumber(uint8_t guess);

		void SetSelection(uint8_t x, uint8_t y);
		void GetSelection(uint8_t& x, uint8_t& y);

		void AddSelection(int x, int y);
		
		void LockCell(uint8_t x, uint8_t y, uint8_t number);
		
		void AddGreaterThanConstraint(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

		static Serialization::LevelData GetSaveData(const Grid& grid);
		void LoadFromData(const Serialization::LevelData& levelData);

		bool PlayerWon() const;
		bool IsEditMode() const;
	private:
		CellData& GetCellData(uint8_t x, uint8_t y);
		const CellData& GetCellData(uint8_t x, uint8_t y)const;

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
		uint8_t m_Rows, m_Cols;

		int m_SelectedRow, m_SelectedCol;
		Vector2 m_Origin;

		std::vector<CellData> m_CellData;
		std::vector<ConstraintData> m_Constraints;

		std::bitset<99> m_Errors;

		size_t m_TargetSum;
		bool m_IsEditMode;
		bool m_PlayerWon;
	};
}