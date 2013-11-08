#include "Config.h"

using namespace std;

Config *Config::instance = 0;

Config *Config::getInstance()
{
	if (instance == 0)
		instance = new Config();

	return instance;
}

Config::Config()
{
	taillePlateau = 200 - 5;

	setSaveFileName("save.dat");
	setNbrCases(9); // TODO: r�gler bug barri�re
}

void Config::setNbrCases(int nbr)
{
	if ((nbr % 2) == 0)
		throw ArgumentInvalidException("Le nombre de cases doit �tre un nombre impair.");

	// enregistrement des diff�rentes tailles
	nbrCases = nbr;
	nbrBarriereJoueur = nbrCases + 1;
	tailleCase = taillePlateau / (nbrCases + 4 + (nbrCases / 4.0));
	tailleZoneRangement = tailleCase * 2;
	tailleRainure = tailleCase / 4;
	taillePion = tailleCase / 2;
	tailleBarriere = (tailleCase * 2) + tailleRainure;
}

void Config::setSaveFileName(string name)
{
	saveFileName = name;
}
