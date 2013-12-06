#include "Point.h"

Point::Point()
{
}

Point::Point(int x, int y)
{
	// position en x du point
	this->x = x;

	// position en y du point
	this->y = y;
}

bool operator==(Point const &p1, Point const &p2)
{
	return p1.x == p2.x && p1.y == p2.y;
}
