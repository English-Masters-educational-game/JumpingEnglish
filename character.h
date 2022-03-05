#pragma once

class Character
{
public:
	int x;
	int y;

public:
	Character()
	{
		x = 0;
		y = 0;
	}

	void jump()
	{
		if (this->y < 800)
		{
			y += 20;
		}
	}

	void down()
	{
		if (this->y > 0)
		{
			y -= 20;
		}
	}

	void right()
	{
		if (this->x < 400)
		{
			x += 20;
		}
	}

	void left()
	{
		if (this->x > 0)
		{
			x -= 20;
		}
	}
};
