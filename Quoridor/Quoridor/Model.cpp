#include "Model.h"

using namespace std;

Model *Model::instance = 0;

Model::Model()
{
	// initialisation position pions
	pions[0] = Point(Config::getInstance()->getNbrCases() / 2, 0);
	pions[1] = Point(Config::getInstance()->getNbrCases() / 2, Config::getInstance()->getNbrCases() - 1);

	// initialisation des barri�res d�j� plac�es, du place prises et du sens
	barrierePlacee[0] = vector<Point>();
	barrierePlacee[1] = vector<Point>();
	sensBarrierePlacee[0] = vector<bool>();
	sensBarrierePlacee[1] = vector<bool>();
	placePrises = vector<vector<bool>>((Config::getInstance()->getNbrCases() * 2) - 1, vector<bool>((Config::getInstance()->getNbrCases() * 2) - 1));

	// initialisation de la barri�re virtuelle
	virtualBarriere = Point(Config::getInstance()->getNbrCases() / 2, Config::getInstance()->getNbrCases() / 2);
	sensVirtualBarriere = false;

	// initialisation du joueur en cours
	joueurEnCours = 1;

	// initialisation du mode
	currentMode = Mode::PIONS;

	// initialisation du message utilisateur
	message = "Bienvenue";
}

bool Model::testerPassagePion(Point position, Direction direction)
{
	switch (direction)
	{
		case Model::LEFT:
			return !placePrises[position.getX() * 2 - 1][position.getY() * 2];
		case Model::RIGHT:
			return !placePrises[position.getX() * 2 + 1][position.getY() * 2];
		case Model::UP:
			return !placePrises[position.getX() * 2][position.getY() * 2 - 1];
		case Model::DOWN:
			return !placePrises[position.getX() * 2][position.getY() * 2 + 1];
	}

	return false;
}

bool Model::deplacerPion(int joueur, Direction direction)
{
	//TODO: sauter au dessus du pion adverse si possible

	// r�cup�ration de l'emplacement du pion du joueur
	Point p = pions[joueur - 1];

	switch (direction)
	{
		case Model::UP: // si le pion doit se d�placer en haut
			// teste si le pion est tout en haut ou bloqu� par une barri�re
			if (p.getY() <= 0 || !testerPassagePion(p, Direction::UP))
				return false;

			pions[joueur - 1] = Point(p.getX(), p.getY() - 1);

			break;
		case Model::DOWN: // si le pion doit se d�placer en bas
			// teste si le pion est tout en bas ou bloqu� par une barri�re
			if (p.getY() >= Config::getInstance()->getNbrCases() - 1 || !testerPassagePion(p, Direction::DOWN))
				return false;

			pions[joueur - 1] = Point(p.getX(), p.getY() + 1);

			break;
		case Model::LEFT: // si le pion doit se d�placer � gauche
			// teste si le pion est tout � gauche ou bloqu� par une barri�re
			if (p.getX() <= 0 || !testerPassagePion(p, Direction::LEFT))
				return false;

			pions[joueur - 1] = Point(p.getX() - 1, p.getY());

			break;
		case Model::RIGHT: // si le pion doit se d�placer � droite
			// teste si le pion est tout � droite ou bloqu� par une barri�re
			if (p.getX() >= Config::getInstance()->getNbrCases() - 1 || !testerPassagePion(p, Direction::RIGHT))
				return false;

			pions[joueur - 1] = Point(p.getX() + 1, p.getY());

			break;
	}

	return true;
}

bool Model::testerPositionnementBarriere(Point position, bool vertical, bool colision)
{
	// tester si la barri�re ne d�passe pas des limites
	if (vertical &&
		(position.getX() <= 0 || position.getX() >= Config::getInstance()->getNbrCases()
			|| position.getY() < 0 || position.getY() >= Config::getInstance()->getNbrCases() - 1))
		return false;
	else if (!vertical &&
		(position.getX() < 0 || position.getX() >= Config::getInstance()->getNbrCases() - 1
			|| position.getY() <= 0 || position.getY() >= Config::getInstance()->getNbrCases()))
		return false;

	// si ne doit pas tester la colision avec d'autre barri�re (utilis� pour permettre � la barri�re virtuelle
	// de se d�placer par dessus les autres)
	if (!colision)
		return true;

	// tester si la barri�re peut �tre plac�e sur cet emplacement
	if (vertical) {
		if (placePrises[position.getX() * 2 - 1][position.getY() * 2]
			|| placePrises[position.getX() * 2 - 1][position.getY() * 2 + 1]
			|| placePrises[position.getX() * 2 - 1][position.getY() * 2 + 2])
				return false;
	} else {
		if(placePrises[position.getX() * 2][position.getY() * 2 - 1]
			|| placePrises[position.getX() * 2 + 1][position.getY() * 2 - 1]
			|| placePrises[position.getX() * 2 + 2][position.getY() * 2 - 1])
				return false;
	}

	return true;
}

bool Model::placerBarriere(int joueur, Point position, bool vertical)
{
	// si je joueur a plac� toutes ses barri�res
	if(barrierePlacee[joueur - 1].size() >= (unsigned int)Config::getInstance()->getNbrBarriereJoueur())
		return false;

	// tester si la barri�re ne sort pas des limites et n'est pas en colision avec une autre
	if (!testerPositionnementBarriere(position, vertical, true))
		return false;

	// ajouter les barri�res dans les barri�res plac�es
	barrierePlacee[joueur - 1].push_back(position);

	// enregistrer le sens des barri�res plac�es
	sensBarrierePlacee[joueur - 1].push_back(vertical);

	// enregistrer les emplacements de barri�res d�j� prit
	if (vertical) {
		placePrises[position.getX() * 2 - 1][position.getY() * 2] = true;
		placePrises[position.getX() * 2 - 1][position.getY() * 2 + 1] = true;
		placePrises[position.getX() * 2 - 1][position.getY() * 2 + 2] = true;
	} else {
		placePrises[position.getX() * 2][position.getY() * 2 - 1] = true;
		placePrises[position.getX() * 2 + 1][position.getY() * 2 - 1] = true;
		placePrises[position.getX() * 2 + 2][position.getY() * 2 - 1] = true;
	}

	return true;
}

bool Model::setVirtualBarriere(Point position)
{
	// tester si la barri�re ne d�passe pas des limites
	if (!testerPositionnementBarriere(position, sensVirtualBarriere, false))
		return false;

	virtualBarriere = position;

	return true;
}

bool Model::deplacerVirtualBarriere(Direction direction)
{
	switch (direction)
	{
		case Model::UP: // si la barri�re virtuelle doit se d�placer en haut
			return setVirtualBarriere(Point(virtualBarriere.getX(), virtualBarriere.getY() - 1));
		case Model::DOWN: // si la barri�re virtuelle doit se d�placer en bas
			return setVirtualBarriere(Point(virtualBarriere.getX(), virtualBarriere.getY() + 1));
		case Model::LEFT: // si la barri�re virtuelle doit se d�placer � gauche
			return setVirtualBarriere(Point(virtualBarriere.getX() - 1, virtualBarriere.getY()));
		case Model::RIGHT: // si la barri�re virtuelle doit se d�placer � droite
			return setVirtualBarriere(Point(virtualBarriere.getX() + 1, virtualBarriere.getY()));
	}

	return false;
}

bool Model::changerSensVirtualBarriere()
{
	// changer le sens de la barri�re virtuelle
	sensVirtualBarriere = !sensVirtualBarriere;

	bool hasSuccessed = setVirtualBarriere(virtualBarriere);

	// restaurer l'ancien �tat n'est pas possible
	if (!hasSuccessed) {
		sensVirtualBarriere = !sensVirtualBarriere;
		return false;
	}
	
	return true;
}

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

	// enregistrement des barri�res plac�es
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

	// enregistrement du sens des barri�res plac�es
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

	// enregistrement de la barri�re virtuelle
	p = model.virtualBarriere;
	stream << p.getX() << ' ' << p.getY() << ' ';

	// enregistrement du sens de la barri�re virtuelle
	stream << model.sensVirtualBarriere << ' ';

	// enregistrement du joueur en cours
	stream << model.joueurEnCours << ' ';

	// enregistrement du mode
	stream << model.currentMode << ' ';

	// enregistrement du message affich�
	stream << model.message;

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

	// chargement des barri�res plac�es
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

	// chargement du sens des barri�res plac�es
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

	// chargement de la position de la barri�re virtuelle
	stream >> x;
	stream >> y;
	model.virtualBarriere = Point(x, y);

	// chargement du sens de la barri�re virtuelle
	stream >> model.sensVirtualBarriere;

	// chargement du joueurEnCours
	stream >> model.joueurEnCours;

	// chargement du mode de jeu
	stream >> r;
	model.currentMode = (Model::Mode)r;

	// chargement du message affich�
	getline(stream, model.message);

	return stream;
}

Model *Model::getInstance()
{
	if (instance == 0)
		instance = new Model();

	return instance;
}