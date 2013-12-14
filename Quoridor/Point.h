#pragma once

/**
	classe servant � d�finir un point de coordonn�es x et y
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Point
{
private:
	int x;	// coordonn�e x du point
	int y;	// coordonn�e y du point

public:
	Point(); // constructeur par d�faut d'un point

	/**
		m�thode de construction d'un point avec les coordonn�es x et y donn�es
		@param x la coordonn�e x
		@param y la coordonn�e y
	*/
	Point(int x, int y);

	/**
		m�thode de r�cup�ration de la coordonn�e x du point
		@return la coordonn�e x du point
	*/
	int getX()
	{
		return x;
	}

	/**
		m�thode de r�cup�ration de la coordonn�e y du point
		@return la coordonn�e y du point
	*/
	int getY()
	{
		return y;
	}

	friend bool operator==(Point const &p1, Point const &p2);
};
