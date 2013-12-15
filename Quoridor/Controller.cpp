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

	isInitialized = true;

	// calculer les positions
	computeAllPositions();
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

		// définition de sa position
		pions[i]->setMatrix(cases[p.getX()][p.getY()]->getMatrix());
	}
}

void Controller::computeVirtualBarriere()
{
	//TODO: calculer QUE lorsque la barrière virtuelle a bougé

	Point p = Model::getInstance()->getVirtualBarriere();

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
	//TODO: calculer QUE lorsqu'une barrière a bougé
	
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

	// placement des barrières dans la zone rangement du joueur 1
	for (int i = 0; i < barriereZoneJ1; i++)
	{
		mt = barrieres[0][i];
		mt->setMatrix(Matrix::identity());
		mt->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
		mt->postMult(Matrix::translate(Vec3(px, py, 0)));

		px += Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure();
	}

	// emplacement de la première barrière dans la zone de rangement du joueur 2
	px = pxInit;

	// placement des barrières dans la zone rangement du joueur 2
	for (int i = 0; i < barriereZoneJ2; i++)
	{
		mt = barrieres[1][i];
		mt->setMatrix(Matrix::identity());
		mt->postMult(Matrix::rotate(osg::inDegrees(90.0), Z_AXIS));
		mt->postMult(Matrix::translate(Vec3(px, -py, 0)));

		px += Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure();
	}

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

void Controller::computeAllPositions()
{
	if (!isInitialized)
		throw new StateException("État incorrect: Veuillez appeler la méthode initialize une fois que tout les éléments sont setter");

	// position des pions
	computePions();

	// position de la barrière virtuelle
	computeVirtualBarriere();

	// position des barrières
	computeBarrieres();
}
