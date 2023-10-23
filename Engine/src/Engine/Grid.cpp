#include "Grid.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fmt/core.h>

#include "raymath.h"

#include "ConstraintArrowVectors.h"
#include "Application.h"

namespace Engine
{
	Event CreateGridStateChangeEvent(int altMode, int editMode)
	{
		return Event{ EventType::CHANGE_GRID_STATE,{altMode,editMode} };
	}


	Grid::Grid(GridStyle style)
		: m_GridSize(0)
		, Style(style)
		, Center({ 0.0f })
		, m_Origin({ 0.0f })
		, m_SelectedRow(0)
		, m_SelectedCol(0)
		, m_TargetSum(0)
		, m_PlayerWon(false)

	{
	}

	void Grid::Update()
	{
		if (!HasValidData())
		{
			return;
		}

		if (!m_PlayerWon)
		{
			if (!m_State.EditMode)
			{
				ClearAllErrors();
				CheckConstraints();
			}
			else if (m_State.EditMode)
			{
				ClearAllErrors();
			}
		}
	}

	void Grid::Draw()
	{
		if (!HasValidData())
		{
			return;
		}

		DrawHelpText(10, 50);

		m_Origin = Vector2Subtract(Center, Vector2{ 0.5f * Style.CellSize * m_GridSize, 0.5f * Style.CellSize * m_GridSize });

		// Draw blocks and numbers
		for (uint8_t y = 0; y < m_GridSize; ++y)
		{
			for (uint8_t x = 0; x < m_GridSize; ++x)
			{
				Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ (float)Style.CellSize * x, (float)Style.CellSize * y });
				Vector2 blockPosition = Vector2Add(cellPosition, Vector2{ 0.5f * (Style.CellSize - Style.BlockSize), 0.5f * (Style.CellSize - Style.BlockSize) });
				CellData& cell = GetCellData(x, y);

				DrawBlock(cell, x, y, blockPosition);

				// All this functions are mutually exclusive, and are handled how they have been set.
				// Draw Guesses
				DrawGuess(cell, x, y, blockPosition);

				// Draw Number
				DrawNumber(cell, x, y, blockPosition);
			}
		}

		for (const auto& constraint : m_Constraints)
		{
			Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ (float)Style.CellSize * constraint.X1, (float)Style.CellSize * constraint.Y1 });
			Vector2 v1{ 0.0f }, v2{ 0.0f }, v3{ 0.0f };
			if (constraint.IsRowConstraint())
			{
				ComputeHorizontalArrow(cellPosition,
					Style.CellSize,
					Style.BlockSize,
					Style.TriangleWidthPercent,
					Style.TriangleHeightPercent,
					v1, v2, v3,
					constraint.FaceLeft());
			}

			if (constraint.IsColConstraint())
			{
				ComputeVerticalArrow(cellPosition,
					Style.CellSize,
					Style.BlockSize,
					Style.TriangleWidthPercent,
					Style.TriangleHeightPercent,
					v1, v2, v3,
					constraint.FaceUp());
			}

			DrawTriangle(v1, v2, v3, Style.ConstraintColor);

		}


		const Vector2 blockOffset{ 0.5f * (Style.CellSize - Style.BlockSize),0.5f * (Style.CellSize - Style.BlockSize) };

		// Draw Col Errors
		for (uint8_t x = 0; x < m_GridSize; ++x)
		{
			Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ (float)Style.CellSize * x, 0 });
			Vector2 blockPosition = Vector2Add(cellPosition, blockOffset);

			if (CheckColHasError(x))
			{
				DrawRectangleLinesEx(
					Rectangle{
						blockPosition.x,
						blockPosition.y,
						(float)Style.BlockSize,
						(float)(m_GridSize - 1) * Style.CellSize + Style.BlockSize
					},
					5.0f,
					Style.WrongFontColor
				);
			}
		}

		// Draw Row Errors
		for (uint8_t y = 0; y < m_GridSize; ++y)
		{
			Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ 0, (float)Style.CellSize * y });
			Vector2 blockPosition = Vector2Add(cellPosition, blockOffset);

			if (CheckRowHasError(y))
			{
				DrawRectangleLinesEx(
					Rectangle{
						blockPosition.x,
						blockPosition.y,
						(float)(m_GridSize - 1) * Style.CellSize + Style.BlockSize,
						(float)Style.BlockSize,
					},
					5.0f,
					Style.WrongFontColor
					);
			}
		}
	}

	void Grid::Reset()
	{
		for (auto& cell : m_CellData)
		{
			cell.Guesses.reset();
			if (!cell.Locked)
			{
				cell.Number = 0;
			}
		}
		m_PlayerWon = false;
	}

	void Grid::NewBoard(bool useDefaultSize, bool showNotification)
	{
		ChangeGridSize(useDefaultSize || (m_GridSize == 0) ? DEFAULT_GRID_SIZE : m_GridSize, false);

		if (showNotification)
		{
			Application::Get().GetNotifications().AddNotification(LOG_INFO, fmt::format("Starting Editor {0}x{0}", m_GridSize));
		}
	}

	void Grid::OnHandleNumber(uint8_t number)
	{
		if (!HasValidData())
		{
			return;
		}

		if (m_State.EditMode)
		{
			if (m_State.AltMode)
			{
				ChangeGridSize(number);
			}
			else if (!(number < 0 || number > m_GridSize))
			{
				ToggleLock(number);
			}
		}
		else
		{
			if (number < 0 || number > m_GridSize)
			{
				return;
			}

			if (m_State.AltMode)
			{
				ToggleGuess(number);
			}
			else
			{
				ToggleNumber(number);
			}
		}
	}

	void Grid::ToggleGuess(uint8_t guess)
	{
		CellData& cell = GetCellData(m_SelectedCol, m_SelectedRow);
		if (cell.Number)
		{
			cell.Number = 0;
		}

		cell.Guesses.flip(guess - 1);
	}

	void Grid::ToggleNumber(uint8_t number)
	{
		CellData& cell = GetCellData(m_SelectedCol, m_SelectedRow);
		if (cell.Guesses.any())
		{
			cell.Guesses.reset();
		}

		if (cell.Number == number)
		{
			cell.Number = 0;
		}
		else
		{
			cell.Number = number;
		}
	}

	void Grid::ToggleLock(uint8_t number)
	{
		CellData& cell = GetCellData(m_SelectedCol, m_SelectedRow);
		if (cell.Locked && cell.Number == number)
		{
			UnlockCell(m_SelectedCol, m_SelectedRow);
		}
		else
		{
			LockCell(m_SelectedCol, m_SelectedRow, number);
		}
	}

	void Grid::ToggleConstraint(int x, int y)
	{
		bool isFlipped = false;
		const int offset = GetConstraintIndex(m_SelectedCol, m_SelectedRow, m_SelectedCol + x, m_SelectedRow + y, isFlipped);
		if (offset == -1)
		{
			AddGreaterThanConstraint(m_SelectedCol, m_SelectedRow, m_SelectedCol + x, m_SelectedRow + y);
		}
		else
		{
			if (isFlipped)
			{
				FlipGreaterThanConstraint(offset);
			}
			else
			{
				RemoveGreaterThanConstraint(m_SelectedCol, m_SelectedRow, m_SelectedCol + x, m_SelectedRow + y);
			}
		}
	}

	void Grid::OnChangeSelection(int x, int y)
	{
		if (!HasValidData())
		{
			return;
		}

		

		if (m_State.EditMode)
		{
			if (m_State.AltMode)
			{
				ToggleConstraint(x, y);
			}
			else
			{
				m_SelectedCol += x;
				m_SelectedRow += y;

				m_SelectedCol = (m_SelectedCol + m_GridSize) % m_GridSize;
				m_SelectedRow = (m_SelectedRow + m_GridSize) % m_GridSize;
			}
		}
		else
		{
			if ((m_SelectedCol + x < 0 || m_SelectedCol + x >= m_GridSize)
				|| (m_SelectedRow + y < 0 || m_SelectedRow + y >= m_GridSize))
			{
				return;
			}

			CellData& cell = GetCellData(m_SelectedCol + x, m_SelectedRow + y);
			if (cell.Locked)
			{
				OnChangeSelection(2 * x, 2 * y);
			}
			else
			{
				m_SelectedCol += x;
				m_SelectedRow += y;
			}
		}
	}

	void Grid::ChangeGridSize(uint8_t gridSize, bool retainData)
	{
		if (!retainData)
		{
			for (uint8_t y = 0; y < m_GridSize; ++y)
			{
				for (uint8_t x = 0; x < m_GridSize; ++x)
				{
					CellData& cell = GetCellData(x, y);
					cell.Guesses.reset();
					cell.Number = 0;
					cell.Locked = false;
				}
			}

			m_GridSize = gridSize;
			m_GridSize = gridSize;
			m_TargetSum = gridSize * (gridSize * (gridSize + 1) / 2);

			m_CellData.resize(gridSize * gridSize);

			m_Constraints.clear();
		}
		else
		{
			struct TempLockedCell
			{
				uint8_t x;
				uint8_t y;
				int value;
			};

			std::vector<TempLockedCell> lockedCells;
			for (uint8_t y = 0; y < m_GridSize; ++y)
			{
				for (uint8_t x = 0; x < m_GridSize; ++x)
				{
					CellData& cell = GetCellData(x, y);
					if (cell.Locked)
					{
						TempLockedCell& lockedCell = lockedCells.emplace_back();
						lockedCell.x = x;
						lockedCell.y = y;
						lockedCell.value = cell.Number;
					}

					cell.Guesses.reset();
					cell.Number = 0;
					cell.Locked = false;
				}
			}

			m_GridSize = gridSize;
			m_GridSize = gridSize;
			m_TargetSum = gridSize * (gridSize * (gridSize + 1) / 2);

			m_CellData.resize(gridSize * gridSize);

			for (auto& itr = m_Constraints.begin(); itr != m_Constraints.end();)
			{
				if (itr->IsViolated(gridSize))
				{
					itr = m_Constraints.erase(itr);
				}
				else
				{
					itr = std::next(itr);
				}
			}

			for (const auto& lockedCell : lockedCells)
			{
				LockCell(lockedCell.x, lockedCell.y, lockedCell.value);
			}
		}
	}

	void Grid::LockCell(uint8_t x, uint8_t y, uint8_t number, bool suppressNotifications)
	{
		if ((x < 0 || x >= m_GridSize)
			|| (y < 0 || y >= m_GridSize) || (number > m_GridSize))
		{
			if (!suppressNotifications)
			{
				Application::Get().GetNotifications().AddNotification(LOG_WARNING, "Cell position out of grid.");
			}
			return;
		}

		CellData& cell = GetCellData(x, y);
		cell.Guesses.reset();
		cell.Locked = true;
		cell.Number = number;
	}

	void Grid::UnlockCell(uint8_t x, uint8_t y)
	{
		if ((x < 0 || x >= m_GridSize)
			|| (y < 0 || y >= m_GridSize))
		{
			return;
		}

		CellData& cell = GetCellData(x, y);
		cell.Locked = false;
		cell.Number = 0;
	}

	void Grid::AddGreaterThanConstraint(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool suppressNotifications)
	{
		if (x1 == x2 && y1 == y2)
		{
			if (!suppressNotifications)
			{
				Application::Get().GetNotifications().AddNotification(LOG_ERROR, "Constraint: same cell.");
			}
			return;
		}

		if ((x1 < 0 || x1 >= m_GridSize)
			|| (x2 < 0 || x2 >= m_GridSize)
			|| (y1 < 0 || y1 >= m_GridSize)
			|| (y2 < 0 || y2 >= m_GridSize))
		{
			if (!suppressNotifications)
			{
				Application::Get().GetNotifications().AddNotification(LOG_WARNING, "Constraint: out of grid.");
			}
			return;
		}

		if ((std::abs(x1 - x2) > 1) || (std::abs(y1 - y2) > 1))
		{
			if (!suppressNotifications)
			{
				Application::Get().GetNotifications().AddNotification(LOG_ERROR, "Constraint: not in adjacent cells.");
			}
			return;
		}

		ConstraintData& data = m_Constraints.emplace_back();
		data.X1 = x1;
		data.Y1 = y1;
		data.X2 = x2;
		data.Y2 = y2;
	}

	void Grid::RemoveGreaterThanConstraint(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
	{
		for (auto& itr = m_Constraints.begin(); itr != m_Constraints.end();)
		{
			if (itr->X1 == x1
				&& itr->Y1 == y1
				&& itr->X2 == x2
				&& itr->Y2 == y2)
			{
				m_Constraints.erase(itr);
				break;
			}
			else
			{
				itr = std::next(itr);
			}
		}
	}

	void Grid::FlipGreaterThanConstraint(int index)
	{
		std::swap(m_Constraints[index].X1, m_Constraints[index].X2);
		std::swap(m_Constraints[index].Y1, m_Constraints[index].Y2);
	}

	int Grid::GetConstraintIndex(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool& isFlipped) const
	{
		for (int i = 0; i < m_Constraints.size(); ++i)
		{
			if (m_Constraints[i].X1 == x1
				&& m_Constraints[i].Y1 == y1
				&& m_Constraints[i].X2 == x2
				&& m_Constraints[i].Y2 == y2)
			{
				return i;
			}
			else if (m_Constraints[i].X1 == x2
				&& m_Constraints[i].Y1 == y2
				&& m_Constraints[i].X2 == x1
				&& m_Constraints[i].Y2 == y1)
			{
				isFlipped = true;
				return i;
			}

		}

		return -1;
	}

	Serialization::LevelData Grid::GetSaveData(const Grid& grid)
	{
		Serialization::LevelData data;
		data.GridSize = grid.m_GridSize;
		for (uint8_t y = 0; y < grid.m_GridSize; ++y)
		{
			for (uint8_t x = 0; x < grid.m_GridSize; ++x)
			{
				const CellData& cell = grid.m_CellData[y * grid.m_GridSize + x];
				if (cell.Locked)
				{
					Serialization::LockedNumber& lockedCell = data.LockedCells.emplace_back();
					lockedCell.X = x;
					lockedCell.Y = y;
					lockedCell.Val = cell.Number;
				}
			}
		}

		for (const auto& constraint : grid.m_Constraints)
		{
			Serialization::GreaterThanConstraint& gtConstraint = data.GreaterThanConstraints.emplace_back();
			gtConstraint.X1 = constraint.X1;
			gtConstraint.Y1 = constraint.Y1;
			gtConstraint.X2 = constraint.X2;
			gtConstraint.Y2 = constraint.Y2;
		}

		return data;
	}

	void Grid::LoadFromData(const Serialization::LevelData& levelData)
	{
		if (levelData.GridSize == 0)
		{
			m_GridSize = levelData.GridSize;
			m_GridSize = levelData.GridSize;
			m_TargetSum = levelData.GridSize * (levelData.GridSize * (levelData.GridSize + 1) / 2);

			m_CellData.clear();
			m_Constraints.clear();
			return;
		}

		m_GridSize = levelData.GridSize;
		m_GridSize = levelData.GridSize;
		m_TargetSum = levelData.GridSize * (levelData.GridSize * (levelData.GridSize + 1) / 2);

		m_CellData.resize(levelData.GridSize * levelData.GridSize);
		m_Constraints.clear();

		for (auto& cell : m_CellData)
		{
			cell.Guesses.reset();
			cell.Locked = false;
			cell.Number = 0;
		}

		const bool suppressNotifications = false;

		for (const auto& lockedCell : levelData.LockedCells)
		{
			LockCell(lockedCell.X, lockedCell.Y, lockedCell.Val, suppressNotifications);
		}

		for (uint8_t i = 0; i < m_CellData.size(); ++i)
		{
			if (!m_CellData[i].Locked)
			{
				m_SelectedCol = i % m_GridSize;
				m_SelectedRow = i / m_GridSize;
				break;
			}
		}

		for (const auto& constraint : levelData.GreaterThanConstraints)
		{
			AddGreaterThanConstraint(constraint.X1, constraint.Y1, constraint.X2, constraint.Y2, suppressNotifications);
		}

		m_PlayerWon = false;
	}

	bool Grid::PlayerWon() const
	{
		return m_PlayerWon;
	}

	void Grid::DrawHelpText(int x, int y)
	{
		const float startX = x;
		float startY = y;
		const float  fontSize = 20;
		const float  lineSpacing = fontSize * 1.1f;

		if (m_State.EditMode)
		{
			DrawText("Ctrl + WASD/Arrow Keys = Toggle Constraint", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("Num = Toggle Locked Number", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("Ctrl + Num = Set Grid Size", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("F = Save As New Level", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("Ctrl + F = Overwrite Current Level", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("R = Reset", startX, startY, fontSize, RED);
			startY += lineSpacing;

			DrawText("Ctrl + R = New Level", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("MODE: EDIT", 10, GetScreenHeight() - 50, 40, BLUE);
		}
		else
		{
			DrawText("CTRL + Num = Guess", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("Num = Number", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("WASD/Arrow Keys = Navigation", startX, startY, fontSize, GRAY);
			startY += lineSpacing;

			DrawText("R = Reset", startX, startY, fontSize, RED);
			startY += lineSpacing;

			DrawText("MODE: PLAY", 10, GetScreenHeight() - 50, 40, GRAY);
		}

		DrawText("E = Edit", startX, startY, fontSize, DARKBLUE);
		startY += lineSpacing;

		DrawText("P = Play", startX, startY, fontSize, GRAY);
		startY += lineSpacing;

		if (m_PlayerWon)
		{
			const float width = MeasureText("You won!", 64);
			DrawText("You won!", 0.5f * (GetScreenWidth() - width), GetScreenHeight() - 80, 64, BLACK);
		}
	}

	void Grid::DrawBlock(const CellData& cell, uint8_t x, uint8_t y, const Vector2& blockPosition)
	{
		if (!cell.Locked || m_State.EditMode)
		{
			if (m_SelectedRow == y && m_SelectedCol == x && !m_PlayerWon)
			{
				DrawRectangle((int)blockPosition.x, (int)blockPosition.y, Style.BlockSize, Style.BlockSize, Style.SelectionColor);
			}

			DrawRectangleLines((int)blockPosition.x, (int)blockPosition.y, Style.BlockSize, Style.BlockSize, Style.BlockBorderColor);
		}
		else
		{
			DrawRectangle((int)blockPosition.x, (int)blockPosition.y, Style.BlockSize, Style.BlockSize, Style.LockedBlockColor);
		}
	}

	void Grid::DrawGuess(const CellData& cell, uint8_t x, uint8_t y, const Vector2& blockPosition)
	{
		if (m_State.EditMode || !cell.Guesses.any())
		{
			return;
		}

		const float offset = Style.BlockSize / 3.0f;
		for (uint8_t i = 0; i < 9; ++i)
		{
			if (cell.Guesses[i] != 0)
			{
				const std::string text = std::to_string(i + 1);
				Vector2 textDims = MeasureTextEx(GetFontDefault(), text.c_str(), (float)Style.GuessFontSize, offset);

				DrawText(text.c_str(),
					(int)(blockPosition.x + (i % 3) * offset + 0.5f * (offset - textDims.x)),
					(int)(blockPosition.y + (i / 3) * offset + 0.5f * (offset - textDims.y)),
					Style.GuessFontSize, Style.GuessFontColor);
			}
		}
	}

	void Grid::DrawNumber(const CellData& cell, uint8_t x, uint8_t y, const Vector2& blockPosition)
	{
		if (!cell.Number)
		{
			return;
		}

		if (m_State.EditMode && !cell.Locked)
		{
			return;
		}

		const std::string text = std::to_string(cell.Number);
		Vector2 textDims = MeasureTextEx(GetFontDefault(), text.c_str(), (float)Style.NumberFontSize, (float)Style.NumberFontSize);

		Color fontColor = Style.NumberFontColor;
		if (!m_State.EditMode && cell.Locked)
		{
			fontColor = Style.LockedFontColor;
		}

		// If not satisfying constraints mark as wrong
		if (CheckCellHasError(x, y))
		{
			fontColor = Style.WrongFontColor;
		}


		DrawText(text.c_str(),
			(int)(blockPosition.x + 0.5f * (Style.BlockSize - textDims.x)),
			(int)(blockPosition.y + 0.5f * (Style.BlockSize - textDims.y)),
			Style.NumberFontSize, fontColor);

	}

	Grid::CellData& Grid::GetCellData(uint8_t x, uint8_t y)
	{
		return m_CellData[y * m_GridSize + x];
	}

	const Grid::CellData& Grid::GetCellData(uint8_t x, uint8_t y)const
	{
		return m_CellData[y * m_GridSize + x];
	}

	bool Grid::CheckColHasError(uint8_t col) const
	{
		const uint8_t offset = 9;
		return m_Errors[offset + col];
	}

	bool Grid::CheckRowHasError(uint8_t row) const
	{
		return m_Errors[row];
	}

	bool Grid::CheckCellHasError(uint8_t x, uint8_t y) const
	{
		const uint8_t offset = 18;
		return m_Errors[offset + 9 * y + x];
	}

	void Grid::MarkErrorColumn(uint8_t col)
	{
		const uint8_t offset = 9;
		m_Errors.set(offset + col);
	}

	void Grid::MarkErrorRow(uint8_t row)
	{
		m_Errors.set(row);
	}

	void Grid::MarkErrorCell(uint8_t x, uint8_t y)
	{
		const uint8_t offset = 18;
		m_Errors.set(offset + 9 * y + x);
	}

	void Grid::ClearAllErrors()
	{
		m_Errors.reset();
	}

	void Grid::CheckConstraints()
	{
		bool allConstraintsSatisfied = true;
		uint8_t currentSum = 0;
		// Row Constraints
		{
			std::unordered_map<uint8_t, uint8_t> rowState;

			for (uint8_t y = 0; y < m_GridSize; ++y)
			{
				rowState.clear();
				for (uint8_t x = 0; x < m_GridSize; ++x)
				{
					CellData& cell = GetCellData(x, y);
					currentSum += cell.Number;
					if (cell.Number == 0)
					{
						continue;
					}

					if (rowState.find(cell.Number) != rowState.end())
					{
						MarkErrorRow(y);
						MarkErrorCell(rowState.at(cell.Number), y);
						MarkErrorCell(x, y);

						allConstraintsSatisfied &= false;
					}

					rowState.emplace(cell.Number, x);
				}
			}
		}

		// Col Constraints
		{
			std::unordered_map<uint8_t, uint8_t> colState;
			for (uint8_t x = 0; x < m_GridSize; ++x)
			{
				colState.clear();
				for (uint8_t y = 0; y < m_GridSize; ++y)
				{
					CellData& cell = GetCellData(x, y);
					if (cell.Number == 0)
					{
						continue;
					}

					if (colState.find(cell.Number) != colState.end())
					{
						MarkErrorColumn(x);
						MarkErrorCell(x, colState.at(cell.Number));
						MarkErrorCell(x, y);
						allConstraintsSatisfied &= false;
					}

					colState.emplace(cell.Number, y);
				}
			}
		}

		if (m_Constraints.size())
		{
			for (const auto& constraint : m_Constraints)
			{
				if (!constraint.IsSatisfied(*this))
				{
					MarkErrorCell(constraint.X1, constraint.Y1);
					MarkErrorCell(constraint.X2, constraint.Y2);

					if (constraint.IsRowConstraint())
					{
						MarkErrorRow(constraint.Y1);
					}

					if (constraint.IsColConstraint())
					{
						MarkErrorColumn(constraint.X1);
					}

					allConstraintsSatisfied &= false;
				}
			}
		}

		if (allConstraintsSatisfied)
		{
			if (currentSum == m_TargetSum)
			{
				m_PlayerWon = true;
				Application::Get().AddEvent(Event{ EventType::PLAYER_WON, {0,0} });
			}
		}
	}

	const GridState& Grid::GetGridState() const
	{
		return m_State;
	}

	void Grid::SetAltMode(bool altMode)
	{
		m_State.AltMode = altMode;
	}

	void Grid::SetEditMode(bool editMode)
	{
		m_State.EditMode = editMode;
		if (m_State.EditMode)
		{
			m_PlayerWon = false;
		}
	}

	bool Grid::HasValidData() const
	{
		return m_GridSize > 0;
	}

	bool Grid::ConstraintData::IsSatisfied(const Grid& grid) const
	{
		const Grid::CellData& cell1 = grid.GetCellData(X1, Y1);
		const Grid::CellData& cell2 = grid.GetCellData(X2, Y2);

		if (cell1.Number == 0 || cell2.Number == 0)
		{
			return true;
		}

		return cell1.Number > cell2.Number;
	}

	bool Grid::ConstraintData::IsRowConstraint() const
	{
		return Y1 == Y2;
	}

	bool Grid::ConstraintData::IsColConstraint() const
	{
		return X1 == X2;
	}
	bool Grid::ConstraintData::FaceLeft() const
	{
		return X1 > X2;
	}

	bool Grid::ConstraintData::FaceUp() const
	{
		return Y1 > Y2;
	}

	bool Grid::ConstraintData::IsViolated(uint8_t gridSize) const
	{
		return ((X1 < 0 || X1 >= gridSize)
			|| (X2 < 0 || X2 >= gridSize)
			|| (Y1 < 0 || Y1 >= gridSize)
			|| (Y2 < 0 || Y2 >= gridSize));
	}
}