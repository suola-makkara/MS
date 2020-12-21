#include "SFML/Graphics.hpp"

#include <iostream>

class Board
{
public:
	enum class State
	{
		NONE,
		WON,
		LOST
	};

private:
	enum Flag
	{
		OPEN = 0x1 << 5,
		FLAGGED = 0x1 << 6,
		MINE = 0x1 << 7
	};

	int width;
	int height;
	int depth;
	int mines;

	int hx = -1, hy = -1, hz = -1;

	State state = State::NONE;
	int tilesOpened = 0;

	unsigned int*** grid;

	bool checkFlag(int x, int y, int z, Flag flag)
	{
		return grid[x][y][z] & flag;
	}

	void toggleFlagged(int x, int y, int z)
	{
		grid[x][y][z] ^= Flag::FLAGGED;
	}

	void setOpen(int x, int y, int z)
	{
		grid[x][y][z] |= Flag::OPEN;
	}

	int getNB(int x, int y, int z)
	{
		return grid[x][y][z] & 0xF;
	}

	int countNBFlags(int x, int y, int z, Flag flag)
	{
		int count = 0;
		for (int xx = x - 1; xx <= x + 1; xx++)
		{
			if (xx < 0 || xx >= width) continue;
			for (int yy = y - 1; yy <= y + 1; yy++)
			{
				if (yy < 0 || yy >= height) continue;
				for (int zz = z - 1; zz <= z + 1; zz++)
				{
					if (zz < 0 || zz >= depth) continue;

					if (checkFlag(xx, yy, zz, flag))
						count++;
				}
			}
		}
		return count;
	}

	bool open(int x, int y, int z, bool initial = true)
	{
		if (checkFlag(x, y, z, Flag::OPEN)) return true;

		setOpen(x, y, z);

		if (checkFlag(x, y, z, Flag::MINE)) return false;

		if (getNB(x, y, z) == 0)
		{
			for (int xx = x - 1; xx <= x + 1; xx++)
			{
				if (xx < 0 || xx >= width) continue;
				for (int yy = y - 1; yy <= y + 1; yy++)
				{
					if (yy < 0 || yy >= height) continue;
					for (int zz = z - 1; zz <= z + 1; zz++)
					{
						if (zz < 0 || zz >= depth) continue;

						if (xx != x || yy != y || zz != z)
							open(xx, yy, zz, false);
					}
				}
			}
		}
		tilesOpened++;
		if (tilesOpened == width * height * depth - mines)
		{
			state = State::WON;
			std::cout << "You won\n";
		}
		//else if (getNB(x, y, z) == countNBFlags(x, y, z, Flag::FLAGGED))
		//{
		//	for (int xx = x - 1; xx <= x + 1; xx++)
		//	{
		//		if (xx < 0 || xx >= width) continue;
		//		for (int yy = y - 1; yy <= y + 1; yy++)
		//		{
		//			if (yy < 0 || yy >= height) continue;
		//			for (int zz = z - 1; zz <= z + 1; zz++)
		//			{
		//				if (zz < 0 || zz >= depth) continue;

		//				if ((xx != x || yy != y || zz != z) && !checkFlag(xx, yy, zz, Flag::FLAGGED))
		//					open(xx, yy, zz, false);
		//			}
		//		}
		//	}
		//}

		return true;
	}

public:
	Board(int width, int height, int depth, int mines)
		: width(width), height(height), depth(depth), mines(mines)
	{
		grid = new unsigned int** [width];

		for (int x = 0; x < width; x++)
		{
			grid[x] = new unsigned int* [height];
			for (int y = 0; y < height; y++)
			{
				grid[x][y] = new unsigned int[depth];
				for (int z = 0; z < depth; z++)
				{
					grid[x][y][z] = 0;
				}
			}
		}

		int minesAdded = 0;
		while (minesAdded < mines)
		{
			int x = rand() % width;
			int y = rand() % height;
			int z = rand() % depth;

			if (grid[x][y][z] == 0)
			{
				grid[x][y][z] = Flag::MINE;
				minesAdded++;
			}
		}

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < depth; z++)
				{
					if (!checkFlag(x, y, z, Flag::MINE))
						grid[x][y][z] = countNBFlags(x, y, z, Flag::MINE);
				}
			}
		}
	}

	~Board()
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
				delete[] grid[x][y];
			delete[] grid[x];
		}
		delete[] grid;
	}

	State getState() { return state; }

	void update(int sx, int sy, const sf::Event& e)
	{
		int x = sx / 20;
		int y = sy / 20;
		int z = x / 10 + (y / 10) * 4;
		x = x % 10;
		y = y % 10;

		if (x < 0 || y < 0 || z < 0
			|| x >= width || y >= height || z >= depth) return;

		switch (e.type)
		{
		case sf::Event::MouseMoved:
			hx = x;
			hy = y;
			hz = z;
			break;
		case sf::Event::MouseButtonPressed:
			switch (e.mouseButton.button)
			{
			case sf::Mouse::Button::Left:
				if (!checkFlag(x, y, z, Flag::FLAGGED))
					if (!open(x, y, z))
					{
						state = State::LOST;
						std::cout << "You lost\n";
					}
				break;
			case sf::Mouse::Button::Right:
				toggleFlagged(x, y, z);
				break;
			}
			break;
		}
	}

	void draw(sf::RenderWindow& window, std::vector<sf::Sprite>& numberVector)
	{
		window.clear(sf::Color(0x33, 0x33, 0x33));
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				for (int z = 0; z < depth; z++)
				{
					sf::Color lineColor;
					if (checkFlag(x, y, z, Flag::OPEN))
					{
						if (getNB(x, y, z) > 0)
						{
							sf::Sprite& numberSprite = numberVector[getNB(x, y, z) - 1];
							numberSprite.setPosition(20 * x + 200 * (z % 4), 20 * y + 200 * (z / 4));
							window.draw(numberSprite);
						}
						else if (checkFlag(x, y, z, Flag::MINE))
						{
							sf::Sprite& mineSprite = numberVector[26];
							mineSprite.setPosition(20 * x + 200 * (z % 4), 20 * y + 200 * (z / 4));
							window.draw(mineSprite);
						}
						lineColor = sf::Color::White;
					}
					else
					{
						sf::RectangleShape cover(sf::Vector2f(20, 20));
						cover.setPosition(20 * x + 200 * (z % 4), 20 * y + 200 * (z / 4));
						window.draw(cover);
						if (checkFlag(x, y, z, Flag::FLAGGED))
						{
							sf::Sprite& flagSprite = numberVector[27];
							flagSprite.setPosition(20 * x + 200 * (z % 4), 20 * y + 200 * (z / 4));
							window.draw(flagSprite);
						}
						lineColor = sf::Color(0x33, 0x33, 0x33);
					}
					if (x == 0)
					{
						sf::Vertex line[2]{ sf::Vector2f(20 * x + 200 * (z % 4) + 1, 20 * y + 200 * (z / 4)), sf::Vector2f(20 * x + 200 * (z % 4) + 1, 20 * (y + 1) + 200 * (z / 4)) };
						line[0].color = lineColor;
						line[1].color = lineColor;
						window.draw(line, 2, sf::Lines);
					}
					else if (x == width - 1)
					{
						sf::Vertex line[2]{ sf::Vector2f(200 + 200 * (z % 4), 20 * y + 200 * (z / 4)), sf::Vector2f(200 + 200 * (z % 4), 20 * (y + 1) + 200 * (z / 4)) };
						line[0].color = lineColor;
						line[1].color = lineColor;
						window.draw(line, 2, sf::Lines);
					}

					if (y == 0)
					{
						sf::Vertex line[2]{ sf::Vector2f(20 * x + 200 * (z % 4), 200 * (z / 4) + 1), sf::Vector2f(20 * (x + 1) + 200 * (z % 4), 200 * (z / 4) + 1) };
						line[0].color = lineColor;
						line[1].color = lineColor;
						window.draw(line, 2, sf::Lines);
					}
					else if (y == height - 1)
					{
						sf::Vertex line[2]{ sf::Vector2f(20 * x + 200 * (z % 4), 200 + 200 * (z / 4)), sf::Vector2f(20 * (x + 1) + 200 * (z % 4), 200 + 200 * (z / 4)) };
						line[0].color = lineColor;
						line[1].color = lineColor;
						window.draw(line, 2, sf::Lines);
					}
				}
		if (hx != -1)
		{
			sf::RectangleShape hoverBlock(sf::Vector2f(20, 20));
			hoverBlock.setFillColor(sf::Color(0x77, 0x77, 0x77, 0x77));
			for (int x = hx - 1; x <= hx + 1; x++)
			{
				if (x < 0 || x >= width) continue;
				for (int y = hy - 1; y <= hy + 1; y++)
				{
					if (y < 0 || y >= height) continue;
					for (int z = hz - 1; z <= hz + 1; z++)
					{
						if (z < 0 || z >= depth) continue;

						hoverBlock.setPosition(20 * x + 200 * (z % 4), 20 * y + 200 * (z / 4));
						window.draw(hoverBlock);
					}
				}
			}

		}
	}
};

int main()
{
	std::srand(time(0));

	constexpr int WIDTH = 800;
	constexpr int HEIGHT = 400;

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "MS");

	sf::Image numbers;
	numbers.loadFromFile("res/numbers2.png");
	sf::Texture numberSheet;
	numberSheet.loadFromImage(numbers);
	std::vector<sf::Sprite> numberVector;
	for (int j = 0; j < 2; j++)
		for (int i = 0; i < 14; i++)
		{
			numberVector.push_back(sf::Sprite(numberSheet, sf::IntRect(i * 10, j * 10, 10, 10)));
			numberVector.back().setScale(2, 2);
		}

	Board board(10, 10, 8, 30);

	Board::State gameState = Board::State::NONE;

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				if (gameState == Board::State::NONE)
				{
					board.update(e.mouseButton.x, e.mouseButton.y, e);
					gameState = board.getState();
				}
				break;
			case sf::Event::MouseMoved:
				if (gameState == Board::State::NONE)
				{
					board.update(e.mouseMove.x, e.mouseMove.y, e);
					gameState = board.getState();
				}
				break;
			}
		}

		window.clear();

		board.draw(window, numberVector);

		window.display();
	}

	return 0;
}