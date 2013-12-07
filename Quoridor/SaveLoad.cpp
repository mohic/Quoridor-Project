#include "SaveLoad.h"

using namespace std;

ostream &operator<<(ostream &stream, Model const &model)
{
	Point p;
	vector<Point> vp;
	vector<bool> vb;
	vector<vector<bool>> vvb;

	// enregistrement du nombre de cases
	stream << Config::getInstance()->getNbrCases() << ' ';

	// enregistrement des pions
	p = model.pions[0];
	stream << p.getX() << ' ' << p.getY() << ' ';

	p = model.pions[1];
	stream << p.getX() << ' ' << p.getY() << ' ';

	// enregistrement des barrières placées
	vp = model.barrierePlacee[0];

	stream << vp.size() << ' ';

	for (unsigned int i = 0; i < vp.size(); i++)
	{
		p = vp[i];
		stream << p.getX() << ' ' << p.getY() << ' ';
	}

	vp = model.barrierePlacee[1];

	stream << vp.size() << ' ';

	for (unsigned int i = 0; i < vp.size(); i++)
	{
		p = vp[i];
		stream << p.getX() << ' ' << p.getY() << ' ';
	}

	// enregistrement du sens des barrières placées
	vb = model.sensBarrierePlacee[0];

	stream << vb.size() << ' ';

	for (unsigned int i = 0; i < vb.size(); i++)
		stream << vb[i] << ' ';

	vb = model.sensBarrierePlacee[1];

	stream << vb.size() << ' ';

	for (unsigned int i = 0; i < vb.size(); i++)
		stream << vb[i] << ' ';

	// enregistrement des places prises
	vvb = model.placePrises;

	stream << vvb.size() << ' ';

	for (unsigned int i = 0; i < vvb.size(); i++)
	{
		vb = vvb[i];

		stream << vb.size() << ' ';

		for (unsigned int j = 0; j < vb.size(); j++)
		{
			stream << vb[j] << ' ';
		}
	}

	// enregistrement de la barrière virtuelle
	p = model.virtualBarriere;
	stream << p.getX() << ' ' << p.getY() << ' ';

	// enregistrement du sens de la barrière virtuelle
	stream << model.sensVirtualBarriere << ' ';

	// enregistrement du joueur en cours
	stream << model.joueurEnCours << ' ';

	// enregistrement du mode
	stream << model.currentMode << ' ';

	// enregistrement des messages affichés
	stream << model.userMessage << endl;
	stream << model.errorMessage << endl;

	return stream;
}

istream &operator>>(istream &stream, Model &model)
{
	int r, l, x, y;
	bool b;

	// chargement du nombre de cases
	stream >> r;
	Config::getInstance()->setNbrCases(r);

	// chargement de la position des pions
	stream >> x;
	stream >> y;
	model.pions[0] = Point(x, y);

	stream >> x;
	stream >> y;
	model.pions[1] = Point(x, y);

	// chargement des barrières placées
	stream >> r;
	model.barrierePlacee[0] = vector<Point>(r);

	for (unsigned int i = 0; i < model.barrierePlacee[0].size(); i++)
	{
		stream >> x;
		stream >> y;

		model.barrierePlacee[0][i] = Point(x, y);
	}

	stream >> r;
	model.barrierePlacee[1] = vector<Point>(r);

	for (unsigned int i = 0; i < model.barrierePlacee[1].size(); i++)
	{
		stream >> x;
		stream >> y;

		model.barrierePlacee[1][i] = Point(x, y);
	}

	// chargement du sens des barrières placées
	stream >> r;
	model.sensBarrierePlacee[0] = vector<bool>(r);

	for (unsigned int i = 0; i < model.sensBarrierePlacee[0].size(); i++)
	{
		stream >> b;
		model.sensBarrierePlacee[0][i] = b;
	}

	stream >> r;
	model.sensBarrierePlacee[1] = vector<bool>(r);

	for (unsigned int i = 0; i < model.sensBarrierePlacee[1].size(); i++)
	{
		stream >> b;
		model.sensBarrierePlacee[1][i] = b;
	}

	// chargement des places prises
	stream >> r;

	if (r != (Config::getInstance()->getNbrCases() * 2 - 1))
		throw LoadException("Fichier de sauvegarde corrompu");

	model.placePrises = vector<vector<bool>>(r);

	for (unsigned int i = 0; i < model.placePrises.size(); i++)
	{
		stream >> l;

		if (r != (Config::getInstance()->getNbrCases() * 2 - 1))
			throw LoadException("Fichier de sauvegarde corrompu");

		model.placePrises[i] = vector<bool>(l);

		for (unsigned int j = 0; j < model.placePrises[i].size(); j++)
		{
			stream >> b;
			model.placePrises[i][j] = b;
		}
	}

	// chargement de la position de la barrière virtuelle
	stream >> x;
	stream >> y;
	model.virtualBarriere = Point(x, y);

	// chargement du sens de la barrière virtuelle
	stream >> model.sensVirtualBarriere;

	// chargement du joueurEnCours
	stream >> model.joueurEnCours;

	// chargement du mode de jeu
	stream >> r;
	model.currentMode = (Model::Mode)r;

	// chargement des messages affichés
	getline(stream, model.userMessage);
	model.userMessage.erase(0, 1); // supprimer le 1er caractère
	getline(stream, model.errorMessage);

	return stream;
}
