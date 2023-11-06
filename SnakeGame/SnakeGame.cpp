#include "SnakeGame.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

SnakeGame::SnakeGame(unsigned int width, unsigned int height)
	: m_Width(width), m_Height(height)
{
	if (m_Width == 0 || m_Height == 0) {
		m_Grid = nullptr;
		m_PixelData = nullptr;
		m_SnakeHeadPosX = 0;
		m_SnakeHeadPosY = 0;
	}
	else {
		m_Grid = new Tile[m_Width * m_Height] { Tile::Empty };
		m_PixelData = new unsigned int[m_Width * m_Height];

		m_SnakeHeadPosX = m_Width / 2;
		m_SnakeHeadPosY = m_Height / 2;
		m_Grid[m_SnakeHeadPosX * m_Width + m_SnakeHeadPosY] = Tile::FacingNowhere;
		srand((unsigned int)(time(nullptr)));
		SpawnRandomFruit();
	}

	m_SnakeTailPosX = m_SnakeHeadPosX;
	m_SnakeTailPosY = m_SnakeHeadPosY;


	for (unsigned int i = 0; i < m_Height; ++i) {
		for (unsigned int j = 0; j < m_Width; ++j) {
			switch (m_Grid[i * m_Width + j]) {
				case Tile::Empty: {
					m_PixelData[(m_Height - (i + 1)) * m_Width + j] = ConvertToColor(0.24f, 0.72f, 0.91f, 1.0f);
					break;
				}
				case Tile::Fruit: {
					m_PixelData[(m_Height - (i + 1)) * m_Width + j] = ConvertToColor(0.1f, 0.7f, 0.07f, 1.0f);
					break;
				}
				default: {
					m_PixelData[(m_Height - (i + 1)) * m_Width + j] = ConvertToColor(0.65f, 0.14f, 0.28f, 1.0f);
					break;
				}
			}
		}
	}
}

SnakeGame::~SnakeGame() {
	delete[] m_PixelData;
	delete[] m_Grid;
}

void SnakeGame::Update(float deltaTime) {
	m_InternalTimer += deltaTime;

	if (!m_MovementsQueue.empty()) {
		m_LastActiveMovement = m_MovementsQueue.front();
		SetTileAt(Tile(m_LastActiveMovement), m_SnakeHeadPosX, m_SnakeHeadPosY);
	}

	if (m_InternalTimer < m_TimePerFrame)
		return;

	m_InternalTimer -= m_InternalTimer;

	Tile headTile = m_Grid[m_SnakeHeadPosY * m_Width + m_SnakeHeadPosX];
	Tile tailTile = m_Grid[m_SnakeTailPosY * m_Width + m_SnakeTailPosX];

	switch (headTile) {
	case Tile::FacingUp: {
		--m_SnakeHeadPosY;
		break;
	}
	case Tile::FacingLeft: {
		--m_SnakeHeadPosX;
		break;
	}
	case Tile::FacingDown: {
		++m_SnakeHeadPosY;
		break;
	}
	case Tile::FacingRight: {
		++m_SnakeHeadPosX;
		break;
	}
	default:
		break;
	}
	if (m_SnakeHeadPosX >= m_Width || m_SnakeHeadPosY >= m_Height) {
		m_GameOver = true;
		std::cout << "You lost!\nScore: " << m_Score << "\n";
		return;
	}

	if (!m_MovementsQueue.empty()) {
		m_LastActiveMovement = m_MovementsQueue.front();
		m_MovementsQueue.pop();
	}

	if (m_LastActiveMovement != Movement::Unknown) {
		if (m_Grid[m_SnakeHeadPosY * m_Width + m_SnakeHeadPosX] == Tile::Fruit) {
			++m_Score;
			SpawnRandomFruit();
			SetTileAt(Tile(m_LastActiveMovement), m_SnakeHeadPosX, m_SnakeHeadPosY);

			const unsigned int maxScore = m_Width * m_Height - 1;
			if (m_Score == maxScore) {
				m_GameOver = true;
				std::cout << "You won! You got the maximum possible score!\nScore: " << m_Score << "\n";
			}

			return;
		}
		else if (m_Grid[m_SnakeHeadPosY * m_Width + m_SnakeHeadPosX] > Tile::Fruit) {
			m_GameOver = true;
			std::cout << "You lost!\nScore: " << m_Score << "\n";
			return;
		}
		SetTileAt(Tile(m_LastActiveMovement), m_SnakeHeadPosX, m_SnakeHeadPosY);
	}

	if (tailTile != Tile::FacingNowhere)
		SetTileAt(Tile::Empty, m_SnakeTailPosX, m_SnakeTailPosY);

	switch (tailTile) {
		case Tile::FacingUp: {
			--m_SnakeTailPosY;
			break;
		}
		case Tile::FacingLeft: {
			--m_SnakeTailPosX;
			break;
		}
		case Tile::FacingDown: {
			++m_SnakeTailPosY;
			break;
		}
		case Tile::FacingRight: {
			++m_SnakeTailPosX;
			break;
		}
		default:
			break;
	}
}

void SnakeGame::PushMovement(Movement movement) {
	if (((int(m_LastActiveMovement) + int(movement) == 12) || (int(m_LastActiveMovement) + int(movement) == 48)) && m_Score)
		return;

	if (m_MovementsQueue.empty() || m_MovementsQueue.back() != movement)
		m_MovementsQueue.push(movement);
}

void SnakeGame::Shutdown() {
	std::cout << "Game exited.\nYour score: " << m_Score << "\n";
	m_GameOver = true;
}

void SnakeGame::SetTileAt(Tile tile, unsigned int x, unsigned int y) {
	m_Grid[y * m_Width + x] = tile;
	switch (tile) {
		case Tile::Empty: {
			m_PixelData[(m_Height - (y + 1)) * m_Width + x] = ConvertToColor(0.24f, 0.72f, 0.91f, 1.0f);
			break;
		}
		case Tile::Fruit: {
			m_PixelData[(m_Height - (y + 1)) * m_Width + x] = ConvertToColor(0.1f, 0.7f, 0.07f, 1.0f);
			break;
		}
		default: {
			m_PixelData[(m_Height - (y + 1)) * m_Width + x] = ConvertToColor(0.65f, 0.14f, 0.28f, 1.0f);
			break;
		}
	}
}

void SnakeGame::SpawnRandomFruit() {
	unsigned int fruitX;
	unsigned int fruitY;

	do {
		fruitX = rand() % m_Width;
		fruitY = rand() % m_Height;
	}
	while (m_Grid[fruitY * m_Width + fruitX] != Tile::Empty);

	SetTileAt(Tile::Fruit, fruitX, fruitY);
}

unsigned int SnakeGame::ConvertToColor(float r, float g, float b, float a) {
	int R = (int)(r * 255.0f);
	int G = (int)(g * 255.0f);
	int B = (int)(b * 255.0f);
	int A = (int)(a * 255.0f);
	return (A << 24) | (B << 16) | (G << 8) | R;
}
