#include "Grid.h"
#include <string>
#include <unordered_map>


#include "raymath.h"

#include "ConstraintArrowVectors.h"
#include "Application.h"

namespace Engine
{
	Grid::Grid(uint8_t rows, uint8_t cols, GridStyle style, bool isEditMode)
		:m_Rows(rows)
		, m_Cols(cols)
		, Style(style)
		, Center({ 0.0f })
		, m_Origin({ 0.0f })
		, m_SelectedRow(0)
		, m_SelectedCol(0)
		, m_IsEditMode(isEditMode)
		, m_TargetSum(0)
		, m_PlayerWon(false)

	{
		if (rows <= 0 && cols <= 0)
		{
			TraceLog(LOG_FATAL, "Grid Data Not Valid");
		}

		for (size_t i = 0; i < rows * cols; ++i)
		{
			m_CellData.emplace_back();
		}
	}

	void Grid::Update()
	{
		if (!m_PlayerWon)
		{
			ClearAllErrors();

			CheckConstraints();
		}
	}

	void Grid::Draw()
	{
		m_Origin = Vector2Subtract(Center, Vector2{ 0.5f * Style.CellSize * m_Rows, 0.5f * Style.CellSize * m_Cols });

		// Draw blocks and numbers
		for (size_t y = 0; y < m_Rows; ++y)
		{
			for (size_t x = 0; x < m_Cols; ++x)
			{
				Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ Style.CellSize * x, Style.CellSize * y });
				Vector2 blockPosition = Vector2Add(cellPosition, Vector2{ 0.5f * (Style.CellSize - Style.BlockSize), 0.5f * (Style.CellSize - Style.BlockSize) });

				CellData& cell = GetCellData(x, y);
				if (!cell.Locked)
				{
					if (m_SelectedRow == y && m_SelectedCol == x && !m_PlayerWon)
					{
						DrawRectangle(blockPosition.x, blockPosition.y, Style.BlockSize, Style.BlockSize, Style.SelectionColor);
					}

					DrawRectangleLines(blockPosition.x, blockPosition.y, Style.BlockSize, Style.BlockSize, Style.BlockBorderColor);
				}
				else
				{
					DrawRectangle(blockPosition.x, blockPosition.y, Style.BlockSize, Style.BlockSize, Style.LockedBlockColor);
				}

				// Draw Guesses
				if (cell.Guesses.any())
				{
					const float offset = Style.BlockSize / 3;
					for (size_t i = 0; i < 9; ++i)
					{
						if (cell.Guesses[i] != 0)
						{
							const std::string text = std::to_string(i + 1);
							Vector2 textDims = MeasureTextEx(GetFontDefault(), text.c_str(), Style.GuessFontSize, offset);

							DrawText(text.c_str(),
								blockPosition.x + (i % 3) * offset + 0.5f * (offset - textDims.x),
								blockPosition.y + (i / 3) * offset + 0.5f * (offset - textDims.y),
								Style.GuessFontSize, Style.GuessFontColor);
						}
					}
				}
				// Draw Number
				else if (cell.Number)
				{
					const std::string text = std::to_string(cell.Number);
					Vector2 textDims = MeasureTextEx(GetFontDefault(), text.c_str(), Style.NumberFontSize, Style.NumberFontSize);

					Color fontColor = Style.NumberFontColor;
					if (cell.Locked)
					{
						fontColor = Style.LockedFontColor;
					}
					// If not satisfying constraints mark as wrong
					if (CheckCellHasError(x, y))
					{
						fontColor = Style.WrongFontColor;
					}

					DrawText(text.c_str(),
						blockPosition.x + 0.5f * (Style.BlockSize - textDims.x),
						blockPosition.y + 0.5f * (Style.BlockSize - textDims.y),
						Style.NumberFontSize, fontColor);
				}


			}
		}

		for (const auto& constraint : m_Constraints)
		{
			Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ Style.CellSize * constraint.X1, Style.CellSize * constraint.Y1 });
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

		// Draw Col Errors
		for (size_t x = 0; x < m_Cols; ++x)
		{
			Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ Style.CellSize * x, 0 });
			Vector2 blockPosition = Vector2Add(cellPosition, Vector2{ 0.5f * (Style.CellSize - Style.BlockSize), 0.5f * (Style.CellSize - Style.BlockSize) });

			if (CheckColHasError(x))
			{
				DrawRectangleLinesEx(
					Rectangle{
						blockPosition.x,
						blockPosition.y,
						Style.BlockSize,
						(m_Rows - 1) * Style.CellSize + Style.BlockSize
					},
					5.0f,
					Style.WrongFontColor
				);
			}
		}

		// Draw Row Errors
		for (size_t y = 0; y < m_Rows; ++y)
		{
			Vector2 cellPosition = Vector2Add(m_Origin, Vector2{ 0, Style.CellSize * y });
			Vector2 blockPosition = Vector2Add(cellPosition, Vector2{ 0.5f * (Style.CellSize - Style.BlockSize), 0.5f * (Style.CellSize - Style.BlockSize) });

			if (CheckRowHasError(y))
			{
				DrawRectangleLinesEx(
					Rectangle{
						blockPosition.x,
						blockPosition.y,
						(m_Cols - 1) * Style.CellSize + Style.BlockSize,
						Style.BlockSize,
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

	void Grid::ToggleGuess(uint8_t guess)
	{
		CellData& cell = GetCellData(m_SelectedCol, m_SelectedRow);
		if (cell.Number)
		{
			cell.Number = 0;
		}

		if (guess <= m_Rows)
		{
			cell.Guesses.flip(guess - 1);
		}
	}

	void Grid::ToggleNumber(uint8_t guess)
	{
		CellData& cell = GetCellData(m_SelectedCol, m_SelectedRow);
		if (cell.Guesses.any())
		{
			cell.Guesses.reset();
		}

		if (cell.Number == guess)
		{
			cell.Number = 0;
		}
		else if (guess <= m_Rows)
		{
			cell.Number = guess;
		}
	}

	void Grid::SetSelection(uint8_t x, uint8_t y)
	{
		if (!(x < 0 || x >= m_Cols))
		{
			m_SelectedCol = x;
		}

		if (!(y < 0 || y >= m_Rows))
		{
			m_SelectedRow = y;
		}
	}

	void Grid::GetSelection(uint8_t& x, uint8_t& y)
	{
		x = m_SelectedRow;
		y = m_SelectedRow;
	}

	void Grid::AddSelection(int x, int y)
	{
		if ((m_SelectedCol + x < 0 || m_SelectedCol + x >= m_Cols))
		{
			return;
		}

		if ((m_SelectedRow + y < 0 || m_SelectedRow + y >= m_Rows))
		{
			return;
		}

		CellData& cell = GetCellData(m_SelectedCol + x, m_SelectedRow + y);
		if (cell.Locked)
		{
			AddSelection(2 * x, 2 * y);
		}
		else
		{
			m_SelectedCol += x;
			m_SelectedRow += y;
		}
	}

	void Grid::LockCell(uint8_t x, uint8_t y, uint8_t number)
	{
		CellData& cell = GetCellData(x, y);
		cell.Locked = true;
		cell.Number = number;
	}

	void Grid::AddGreaterThanConstraint(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
	{
		if (x1 == x2 && y1 == y2)
		{
			return;
		}

		ConstraintData& data = m_Constraints.emplace_back();
		data.X1 = x1;
		data.Y1 = y1;
		data.X2 = x2;
		data.Y2 = y2;
	}

	Serialization::LevelData Grid::GetSaveData(const Grid& grid)
	{
		Serialization::LevelData data;
		data.GridSize = grid.m_Rows;
		for (size_t y = 0; y < grid.m_Rows; ++y)
		{
			for (size_t x = 0; x < grid.m_Cols; ++x)
			{
				const CellData& cell = grid.m_CellData[y * grid.m_Cols + x];
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
			return;
		}

		m_Rows = levelData.GridSize;
		m_Cols = levelData.GridSize;
		m_TargetSum = levelData.GridSize * (levelData.GridSize * (levelData.GridSize + 1) / 2);

		m_CellData.resize(levelData.GridSize * levelData.GridSize);
		m_Constraints.clear();

		for (auto& cell : m_CellData)
		{
			cell.Guesses.reset();
			cell.Locked = false;
			cell.Number = 0;
		}

		for (const auto& lockedCell : levelData.LockedCells)
		{
			LockCell(lockedCell.X, lockedCell.Y, lockedCell.Val);
		}

		for (size_t i=0;i<m_CellData.size();++i)
		{
			if (!m_CellData[i].Locked)
			{
				m_SelectedCol = i % m_Rows;
				m_SelectedRow = i / m_Rows;
				break;
			}
		}

		for (const auto& constraint : levelData.GreaterThanConstraints)
		{
			AddGreaterThanConstraint(constraint.X1, constraint.Y1, constraint.X2, constraint.Y2);
		}
	}

	bool Grid::PlayerWon() const
	{
		return m_PlayerWon;
	}

	bool Grid::IsEditMode() const
	{
		return m_IsEditMode;
	}

	Grid::CellData& Grid::GetCellData(uint8_t x, uint8_t y)
	{
		return m_CellData[y * m_Cols + x];
	}

	const Grid::CellData& Grid::GetCellData(uint8_t x, uint8_t y)const
	{
		return m_CellData[y * m_Cols + x];
	}

	bool Grid::CheckColHasError(uint8_t col) const
	{
		const size_t offset = 9;
		return m_Errors[offset + col];
	}

	bool Grid::CheckRowHasError(uint8_t row) const
	{
		return m_Errors[row];
	}

	bool Grid::CheckCellHasError(uint8_t x, uint8_t y) const
	{
		const size_t offset = 18;
		return m_Errors[offset + 9 * y + x];
	}

	void Grid::MarkErrorColumn(uint8_t col)
	{
		const size_t offset = 9;
		m_Errors.set(offset + col);
	}

	void Grid::MarkErrorRow(uint8_t row)
	{
		m_Errors.set(row);
	}

	void Grid::MarkErrorCell(uint8_t x, uint8_t y)
	{
		const size_t offset = 18;
		m_Errors.set(offset + 9 * y + x);
	}

	void Grid::ClearAllErrors()
	{
		m_Errors.reset();
	}

	void Grid::CheckConstraints()
	{
		bool allConstraintsSatisfied = true;
		size_t currentSum = 0;
		// Row Constraints
		{
			std::unordered_map<size_t, size_t> rowState;

			for (size_t y = 0; y < m_Rows; ++y)
			{
				rowState.clear();
				for (size_t x = 0; x < m_Cols; ++x)
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
			std::unordered_map<size_t, size_t> colState;
			for (size_t x = 0; x < m_Cols; ++x)
			{
				colState.clear();
				for (size_t y = 0; y < m_Rows; ++y)
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
}