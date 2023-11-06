#pragma once

#include <queue>

class SnakeGame {
public:
	enum class Movement : char {
		Unknown = 0,

		Up = 4,
		Left = 16,
		Down = 8,
		Right = 32
	};

	SnakeGame(unsigned int width = 32, unsigned int height = 32);
	~SnakeGame();

	void Update(float deltaTime);
	void* GetPixelData() { return m_PixelData; }
	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }
	bool IsGameOver() const { return m_GameOver; }

	void PushMovement(Movement movement);
	void Shutdown();

private:
	enum class Tile : char {
		Empty = 0,
		Fruit = 1,
		FacingNowhere = 2,
		FacingUp = 4,
		FacingLeft = 16,
		FacingDown = 8,
		FacingRight = 32
	};

	void SetTileAt(Tile tile, unsigned int x, unsigned int y);
	void SpawnRandomFruit();
	unsigned int ConvertToColor(float r, float g, float b, float a);

private:
	unsigned int m_Width;
	unsigned int m_Height;

	unsigned int m_SnakeHeadPosX;
	unsigned int m_SnakeHeadPosY;
	unsigned int m_SnakeTailPosX;
	unsigned int m_SnakeTailPosY;

	bool m_GameOver = false;
	unsigned int m_Score = 0;
	float m_InternalTimer = 0;

	const float m_TimePerFrame = 0.1f;

	Tile* m_Grid;
	unsigned int* m_PixelData;

	std::queue<Movement> m_MovementsQueue;
	Movement m_LastActiveMovement = Movement::Unknown;
};
