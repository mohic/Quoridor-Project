#include "Model.h"

using namespace std;

Model *Model::instance = 0;

Model::Model()
{
	// initialisation position pions
	pions[0] = Point(Config::getInstance()->getNbrCases() / 2, 0);
	pions[1] = Point(Config::getInstance()->getNbrCases() / 2, Config::getInstance()->getNbrCases() - 1);

	// initialisation des barrières déjà placées, du place prises et du sens
	barrierePlacee[0] = vector<Point>();
	barrierePlacee[1] = vector<Point>();
	sensBarrierePlacee[0] = vector<bool>();
	sensBarrierePlacee[1] = vector<bool>();
	placePrises = vector<vector<bool>>((Config::getInstance()->getNbrCases() * 2) - 1, vector<bool>((Config::getInstance()->getNbrCases() * 2) - 1));

	// initialisation de la barrière virtuelle
	virtualBarriere = Point(Config::getInstance()->getNbrCases() / 2, Config::getInstance()->getNbrCases() / 2);
	sensVirtualBarriere = false;

	// initialisation du joueur en cours
	joueurEnCours = 1;

	// initialisation de partie terminée
	partieTerminee = false;

	// initialisation du mode et de la vue
	currentMode = Mode::PIONS;
	currentView = View::PARALLELE;

	// initialisation du message utilisateur
	userMessage = "Au tour du joueur 1";
	errorMessage = "Bienvenue";

	// initialisation du vecteur contenant les différents coups joués
	coups = vector<string>();
}

bool Model::testerPassagePion(Point position, Direction direction)
{
	switch (direction)
	{
		case Model::LEFT:
			return position.getX() > 0 && !placePrises[position.getX() * 2 - 1][position.getY() * 2];
		case Model::RIGHT:
			return position.getX() < Config::getInstance()->getNbrCases() - 1 && !placePrises[position.getX() * 2 + 1][position.getY() * 2];
		case Model::UP:
			return position.getY() > 0 && !placePrises[position.getX() * 2][position.getY() * 2 - 1];
		case Model::DOWN:
			return position.getY() < Config::getInstance()->getNbrCases() - 1 && !placePrises[position.getX() * 2][position.getY() * 2 + 1];
	}

	return false;
}

bool Model::deplacerPion(int joueur, Direction direction)
{
	coupsJoue(true);

	// récupération de l'emplacement initial du pion du joueur
	Point p = pions[joueur - 1];
	bool succeed = true;

	switch (direction)
	{
		case Model::UP: // si le pion doit se déplacer en haut
			// teste si le pion est tout en haut ou bloqué par une barrière
			if (!testerPassagePion(p, Direction::UP))
				succeed = false;
			else
				pions[joueur - 1] = Point(p.getX(), p.getY() - 1);

			break;
		case Model::DOWN: // si le pion doit se déplacer en bas
			// teste si le pion est tout en bas ou bloqué par une barrière
			if (!testerPassagePion(p, Direction::DOWN))
				succeed = false;
			else
				pions[joueur - 1] = Point(p.getX(), p.getY() + 1);

			break;
		case Model::LEFT: // si le pion doit se déplacer à gauche
			// teste si le pion est tout à gauche ou bloqué par une barrière
			if (!testerPassagePion(p, Direction::LEFT))
				succeed = false;
			else
				pions[joueur - 1] = Point(p.getX() - 1, p.getY());

			break;
		case Model::RIGHT: // si le pion doit se déplacer à droite
			// teste si le pion est tout à droite ou bloqué par une barrière
			if (!testerPassagePion(p, Direction::RIGHT))
				succeed = false;
			else
				pions[joueur - 1] = Point(p.getX() + 1, p.getY());

			break;
	}

	// si échec du déplacement
	if (!succeed) {
		errorMessage = "Mouvement impossible";
		coupsJoue(false);

		return false;
	}

	// condition pour sauter au dessus de l'autre joueur
	int autreJoueur = (joueur == 1 ? 2 : 1);

	if (pions[joueur - 1].getX() == pions[autreJoueur - 1].getX() && pions[joueur - 1].getY() == pions[autreJoueur - 1].getY()) {
		if (!deplacerPion(joueur, direction)) {
			pions[joueur - 1] = p; // remettre le pion à sa position initiale
			coupsJoue(false);

			return false;
		}

		coupsJoue(false); // éviter d'enregistrer l'état intermédiaire du saut
	}

	// vérifier victoire
	if ((joueur == 1 && pions[joueur - 1].getY() == Config::getInstance()->getNbrCases() - 1) || (joueur == 2 && pions[joueur - 1].getY() == 0)) {
		userMessage = "Le joueur " + to_string(joueur) + " gagne la partie";
		partieTerminee = true;
	}

	return true;
}

bool Model::testerPositionnementBarriere(Point position, bool vertical, bool colision)
{
	// tester si la barrière ne dépasse pas des limites
	if (vertical &&
		(position.getX() <= 0 || position.getX() >= Config::getInstance()->getNbrCases()
			|| position.getY() < 0 || position.getY() >= Config::getInstance()->getNbrCases() - 1))
		return false;
	else if (!vertical &&
		(position.getX() < 0 || position.getX() >= Config::getInstance()->getNbrCases() - 1
			|| position.getY() <= 0 || position.getY() >= Config::getInstance()->getNbrCases()))
		return false;

	// si ne doit pas tester la colision avec d'autre barrière (utilisé pour permettre à la barrière virtuelle
	// de se déplacer par dessus les autres)
	if (!colision)
		return true;

	// tester si la barrière peut être placée sur cet emplacement
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
	coupsJoue(true);

	// si je joueur a placé toutes ses barrières
	if(barrierePlacee[joueur - 1].size() >= (unsigned int)Config::getInstance()->getNbrBarriereJoueur()) {
		errorMessage = "Toutes les barrières sont placées";
		coupsJoue(false);

		return false;
	}

	// tester si la barrière ne sort pas des limites et n'est pas en colision avec une autre
	if (!testerPositionnementBarriere(position, vertical, true)) {
		errorMessage = "Impossible de placer une barrière à cet endroit";
		coupsJoue(false);

		return false;
	}

	// ajouter les barrières dans les barrières placées
	barrierePlacee[joueur - 1].push_back(position);

	// enregistrer le sens des barrières placées
	sensBarrierePlacee[joueur - 1].push_back(vertical);

	// enregistrer les emplacements de barrières déjà prit
	if (vertical) {
		placePrises[position.getX() * 2 - 1][position.getY() * 2] = true;
		placePrises[position.getX() * 2 - 1][position.getY() * 2 + 1] = true;
		placePrises[position.getX() * 2 - 1][position.getY() * 2 + 2] = true;
	} else {
		placePrises[position.getX() * 2][position.getY() * 2 - 1] = true;
		placePrises[position.getX() * 2 + 1][position.getY() * 2 - 1] = true;
		placePrises[position.getX() * 2 + 2][position.getY() * 2 - 1] = true;
	}

	// tester si le placement ne bloque pas un des joueurs, si c'est le cas, retour en arrière
	if (!testerBlocagePions()) {
		// retirer les barrières dans les barrières placées
		barrierePlacee[joueur - 1].pop_back();

		// supprimer l'enregistrement du sens des barrières placées
		sensBarrierePlacee[joueur - 1].pop_back();

		// supprimer l'enregistrement des emplacements de barrières déjà prit
		if (vertical) {
			placePrises[position.getX() * 2 - 1][position.getY() * 2] = false;
			placePrises[position.getX() * 2 - 1][position.getY() * 2 + 1] = false;
			placePrises[position.getX() * 2 - 1][position.getY() * 2 + 2] = false;
		} else {
			placePrises[position.getX() * 2][position.getY() * 2 - 1] = false;
			placePrises[position.getX() * 2 + 1][position.getY() * 2 - 1] = false;
			placePrises[position.getX() * 2 + 2][position.getY() * 2 - 1] = false;
		}

		errorMessage = "Placer une barrière ici bloque un des deux joueurs";
		coupsJoue(false);

		return false;
	}

	return true;
}

bool Model::setVirtualBarriere(Point position)
{
	// tester si la barrière ne dépasse pas des limites
	if (!testerPositionnementBarriere(position, sensVirtualBarriere, false))
		return false;

	virtualBarriere = position;

	return true;
}

bool Model::deplacerVirtualBarriere(Direction direction)
{
	switch (direction)
	{
		case Model::UP: // si la barrière virtuelle doit se déplacer en haut
			return setVirtualBarriere(Point(virtualBarriere.getX(), virtualBarriere.getY() - 1));
		case Model::DOWN: // si la barrière virtuelle doit se déplacer en bas
			return setVirtualBarriere(Point(virtualBarriere.getX(), virtualBarriere.getY() + 1));
		case Model::LEFT: // si la barrière virtuelle doit se déplacer à gauche
			return setVirtualBarriere(Point(virtualBarriere.getX() - 1, virtualBarriere.getY()));
		case Model::RIGHT: // si la barrière virtuelle doit se déplacer à droite
			return setVirtualBarriere(Point(virtualBarriere.getX() + 1, virtualBarriere.getY()));
	}

	return false;
}

bool Model::changerSensVirtualBarriere()
{
	// changer le sens de la barrière virtuelle
	sensVirtualBarriere = !sensVirtualBarriere;

	bool hasSuccessed = setVirtualBarriere(virtualBarriere);

	// restaurer l'ancien état n'est pas possible
	if (!hasSuccessed) {
		sensVirtualBarriere = !sensVirtualBarriere;
		return false;
	}
	
	return true;
}

bool Model::testerBlocagePions()
{
	caseVerifiee = vector<Point>();

	if(testerBlocagePion(1, pions[0])) {
		caseVerifiee = vector<Point>();

		return testerBlocagePion(2, pions[1]);
	}

	return false;
}

bool Model::testerBlocagePion(int joueur, Point caseATester)
{
	// condition de sortie si la case est en dehors du tableau
	if (caseATester.getX() < 0
			|| caseATester.getX() > Config::getInstance()->getNbrCases()
			|| caseATester.getY() > Config::getInstance()->getNbrCases()
			|| caseATester.getY() < 0)
		return false;

	// condition de sortie si la case a déjà été vérifiée
	vector<Point>::iterator it = find(caseVerifiee.begin(), caseVerifiee.end(), caseATester);

	if (it != caseVerifiee.end())
		return false;

	// ajouter la case courante dans les case déjà vérifiée
	caseVerifiee.push_back(caseATester);

	// condition de sortie si la ligne d'arrivée est atteinte
	if ((joueur == 1 && caseATester.getY() == Config::getInstance()->getNbrCases() - 1)
			|| (joueur == 2 && caseATester.getY() == 0))
		return true;

	// condition de sortie si plus aucune possibilité
	bool left  = false;
	bool right = false;
	bool down  = false;
	bool up    = false;

	if (testerPassagePion(caseATester, Direction::LEFT))
		left = testerBlocagePion(joueur, Point(caseATester.getX() - 1, caseATester.getY()));

	if (testerPassagePion(caseATester, Direction::RIGHT))
		right = testerBlocagePion(joueur, Point(caseATester.getX() + 1, caseATester.getY()));

	if (testerPassagePion(caseATester, Direction::DOWN))
		down = testerBlocagePion(joueur, Point(caseATester.getX(), caseATester.getY() + 1));

	if (testerPassagePion(caseATester, Direction::UP))
		up = testerBlocagePion(joueur, Point(caseATester.getX(), caseATester.getY() - 1));

	return left || right || down || up;
}

void Model::coupsJoue(bool action)
{
	if (action) {
		stringstream ss;
	
		ss << *this;

		coups.push_back(ss.str());
	} else {
		if (!coups.empty())
			coups.pop_back();
	}
}

bool Model::annulerDernierCoup()
{
	// si plus de coups
	if (coups.empty())
		return false;

	// si un coup peut être annulé
	stringstream ss;

	ss << coups.back();
	coups.pop_back(); // supprimer l'élément du tableau

	ss >> *this;

	return true;
}

Model *Model::getInstance()
{
	if (instance == 0)
		instance = new Model();

	return instance;
}
