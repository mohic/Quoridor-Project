#pragma once

/**
	classe servant à définir un point de coordonnées x et y
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Point
{
private:
	int x;	// coordonnée x du point
	int y;	// coordonnée y du point

public:
	Point(); // constructeur par défaut d'un point

	/**
		méthode de construction d'un point avec les coordonnées x et y données
		@param x la coordonnée x
		@param y la coordonnée y
	*/
	Point(int x, int y);

	/**
		méthode de récupération de la coordonnée x du point
		@return la coordonnée x du point
	*/
	int getX()
	{
		return x;
	}

	/**
		méthode de récupération de la coordonnée y du point
		@return la coordonnée y du point
	*/
	int getY()
	{
		return y;
	}

	friend bool operator==(Point const &p1, Point const &p2);
};
