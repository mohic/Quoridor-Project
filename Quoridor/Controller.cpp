#include "Controller.h"

using namespace std;
using namespace osg;

Controller *Controller::instance = 0;

Controller *Controller::getInstance()
{
	if (instance == 0)
		instance = new Controller();

	return instance;
}

Controller::Controller()
{
	// initialise la caméra
	container = 0;

	// initialise isInitialized
	isInitialized = false;

	// création du vecteur de vecteur de cases
	cases = vector<vector<ref_ptr<MatrixTransform>>>(Config::getInstance()->getNbrCases(), vector<ref_ptr<MatrixTransform>>(Config::getInstance()->getNbrCases()));
	
	// création du vecteur des barrières
	barrieres[0] = vector<ref_ptr<MatrixTransform>>(Config::getInstance()->getNbrBarriereJoueur());
	barrieres[1] = vector<ref_ptr<MatrixTransform>>(Config::getInstance()->getNbrBarriereJoueur());
}

void Controller::showVirtualBarriere()
{
	if (!isInitialized)
		throw new StateException("État incorrect: Veuillez appeler la méthode initialize une fois que tout les éléments sont setter");

	if (!container->containsNode(virtualBarriere))
		container->addChild(virtualBarriere);
}

void Controller::initialize(osg::ref_ptr<osg::MatrixTransform> container)
{
	// éviter d'initialiser 2x
	if (isInitialized)
		return;

	this->container = container;

	// ajout des cases
	for (unsigned int i = 0; i < cases.size(); i++)
		for (unsigned int j = 0; j < cases[i].size(); j++)
			container->addChild(cases[i][j].get());

	// ajout des barrières
	for (unsigned int i = 0; i < barrieres[0].size(); i++)
	{
		container->addChild(barrieres[0][i].get());
		container->addChild(barrieres[1][i].get());
	}

	// ajout des pions
	container->addChild(pions[0]);
	container->addChild(pions[1]);

	// calculer les positions
	computeAllPositions();

	// mettre la classe en état initialisé
	isInitialized = true;
}

void Controller::hideVirtualBarriere()
{
	if (!isInitialized)
		throw new StateException("État incorrect: Veuillez appeler la méthode initialize une fois que tout les éléments sont setter");

	if (container->containsNode(virtualBarriere))
		container->removeChild(virtualBarriere);
}

void Controller::computePions()
{
	for (int i = 0; i < 2; i++)
	{
		// récupération de la position du pion
		Point p = Model::getInstance()->getPion(i + 1);

		if (isInitialized && p == cache_pions[i]) // si le pion n'a pas bougé et que la classe est initialisée, l'ignorer
			continue;

		cache_pions[i] = p;

		Vec3 pos0 = cases[cache_pions[i].getX()][cache_pions[i].getY()]->getMatrix().getTrans();
		Vec3 pos1 = cases[p.getX()][p.getY()]->getMatrix().getTrans();

		// définition de sa position finale
		pions[i]->setMatrix(cases[p.getX()][p.getY()]->getMatrix());

		// animation du pion
		ref_ptr<AnimationPath> animPath = new AnimationPath();
		animPath->setLoopMode(AnimationPath::LoopMode::NO_LOOPING);

		AnimationPath::ControlPoint key0 = AnimationPath::ControlPoint(pos0);
		AnimationPath::ControlPoint key1 = AnimationPath::ControlPoint(pos1);

		animPath->insert(0, key0);
		animPath->insert(.2, key1);

		ref_ptr<AnimationPathCallback> animCallback = new AnimationPathCallback(animPath.get());
		pions[i]->setUpdateCallback(animCallback.get());
	}
}

void Controller::computeVirtualBarriere()
{
	Point p = Model::getInstance()->getVirtualBarriere();
	bool sens = Model::getInstance()->getSensVirtualBarriere();

	if (isInitialized && p == cache_virtualFence && sens == cache_sens) // si la barrière virtuelle n'a pas bougé et que la classe est initialisée, l'ignorer
		return;

	cache_virtualFence = p;
	cache_sens = sens;

	virtualBarriere->setMatrix(Matrix::identity());

	if (Model::getInstance()->getSensVirtualBarriere()) { // si vertical
		virtualBarriere->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
		virtualBarriere->postMult(cases[p.getX() - 1][p.getY()]->getMatrix());
		virtualBarriere->postMult(Matrix::translate(Vec3((Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure()) / 2.0, (-Config::getInstance()->getTailleBarriere() / 2.0) + (Config::getInstance()->getTailleCase() / 2.0), 0)));
	} else { // si horizontale
		virtualBarriere->setMatrix(cases[p.getX()][p.getY() - 1]->getMatrix());
		virtualBarriere->postMult(Matrix::translate(Vec3((Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure()) / 2.0, (-Config::getInstance()->getTailleBarriere() / 2.0) + (Config::getInstance()->getTailleCase() / 2.0), 0)));
	}
}

void Controller::computeBarrieres()
{
	bool mustCalculate1 = false;
	bool mustCalculate2 = false;
	
	ref_ptr<MatrixTransform> mt;

	// calculer les barrières dans les zones de rangements
	int barriereZoneJ1 = Model::getInstance()->getNbrBarriereAPlacer(1);
	int barriereZoneJ2 = Model::getInstance()->getNbrBarriereAPlacer(2);

	// emplacement de la première barrière dans la zone de rangement du joueur 1
	int pxInit = - ((Config::getInstance()->getNbrCases() - 1) / 2 * Config::getInstance()->getTailleCase()) // retire le décalage par rapport à chaque case
		- ((Config::getInstance()->getNbrBarriereJoueur() - 2) / 2 * Config::getInstance()->getTailleRainure()) // retire le décalage par rapport au rainure
		- (Config::getInstance()->getTailleCase() / 2 + Config::getInstance()->getTailleRainure() / 2); // retire une demi case et une demi rainure
	
	int px = pxInit;

	int py = -pxInit + Config::getInstance()->getTailleRainure() * 2 + Config::getInstance()->getTailleCase();

	if (!isInitialized || barriereZoneJ1 != cache_barriereRestante[0]) { // si le nombre de barrière a bougé ou que la classe n'est pas initialisée, calculer
		cache_barriereRestante[0] = barriereZoneJ1;
		mustCalculate1 = true; // indiquer qu'il faut calculer la position des barrières de J1

		// placement des barrières dans la zone rangement du joueur 1
		for (int i = 0; i < barriereZoneJ1; i++)
		{
			mt = barrieres[0][i];
			mt->setMatrix(Matrix::identity());
			mt->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
			mt->postMult(Matrix::translate(Vec3(px, py, 0)));

			px += Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure();
		}
	}

	// emplacement de la première barrière dans la zone de rangement du joueur 2
	px = pxInit;

	if (!isInitialized || barriereZoneJ2 != cache_barriereRestante[1]) { // si le nombre de barrière a bougé ou que la classe n'est pas initialisée, calculer
		cache_barriereRestante[1] = barriereZoneJ2;
		mustCalculate2 = true; // indiquer qu'il faut calculer la position des barrières de J2

		// placement des barrières dans la zone rangement du joueur 2
		for (int i = 0; i < barriereZoneJ2; i++)
		{
			mt = barrieres[1][i];
			mt->setMatrix(Matrix::identity());
			mt->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
			mt->postMult(Matrix::translate(Vec3(px, -py, 0)));

			px += Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure();
		}
	}

	if (mustCalculate1) { // si doit être calculé
		// calculer les barrières déjà placées du joueur 1
		vector<Point> bp1 = Model::getInstance()->getBarrierePlacee(1);
		vector<bool> sensBp1 = Model::getInstance()->getSensBarrierePlacee(1);

		// placement des barrières placées par je joueur 1
		for (unsigned int i = 0; i < bp1.size(); i++)
		{
			mt = barrieres[0][barriereZoneJ1 + i];
			mt->setMatrix(Matrix::identity());

			if (sensBp1[i]) { // si vertical
				mt->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
				mt->postMult(cases[bp1[i].getX() - 1][bp1[i].getY()]->getMatrix());
				mt->postMult(Matrix::translate(Vec3((Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure()) / 2.0, (-Config::getInstance()->getTailleBarriere() / 2.0) + (Config::getInstance()->getTailleCase() / 2.0), 0)));
			} else { // si horizontale
				mt->setMatrix(cases[bp1[i].getX()][bp1[i].getY() - 1]->getMatrix());
				mt->postMult(Matrix::translate(Vec3((Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure()) / 2.0, (-Config::getInstance()->getTailleBarriere() / 2.0) + (Config::getInstance()->getTailleCase() / 2.0), 0)));
			}
		}
	}

	if (mustCalculate2) { // si doit être calculé
		// calculer les barrières déjà placées du joueur 2
		vector<Point> bp2 = Model::getInstance()->getBarrierePlacee(2);
		vector<bool> sensBp2 = Model::getInstance()->getSensBarrierePlacee(2);

		// placement des barrières placées par je joueur 2
		for (unsigned int i = 0; i < bp2.size(); i++)
		{
			mt = barrieres[1][barriereZoneJ2 + i];
			mt->setMatrix(Matrix::identity());

			if (sensBp2[i]) { // si vertical
				mt->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
				mt->postMult(cases[bp2[i].getX() - 1][bp2[i].getY()]->getMatrix());
				mt->postMult(Matrix::translate(Vec3((Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure()) / 2.0, (-Config::getInstance()->getTailleBarriere() / 2.0) + (Config::getInstance()->getTailleCase() / 2.0), 0)));
			} else { // si horizontale
				mt->setMatrix(cases[bp2[i].getX()][bp2[i].getY() - 1]->getMatrix());
				mt->postMult(Matrix::translate(Vec3((Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure()) / 2.0, (-Config::getInstance()->getTailleBarriere() / 2.0) + (Config::getInstance()->getTailleCase() / 2.0), 0)));
			}
		}
	}
}

void Controller::computeAllPositions()
{
	// position des pions
	computePions();

	// position de la barrière virtuelle
	computeVirtualBarriere();

	// position des barrières
	computeBarrieres();
}
