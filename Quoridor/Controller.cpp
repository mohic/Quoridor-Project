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
	// création du vecteur de vecteur de cases
	cases = vector<vector<ref_ptr<MatrixTransform>>>(Config::getInstance()->getNbrCases(), vector<ref_ptr<MatrixTransform>>(Config::getInstance()->getNbrCases()));
	
	// créations du vecteur des barrières
	barrieres[0] = vector<ref_ptr<MatrixTransform>>(Config::getInstance()->getNbrBarriereJoueur());
	barrieres[1] = vector<ref_ptr<MatrixTransform>>(Config::getInstance()->getNbrBarriereJoueur());

	// création de chaque MatrixTransform
	for (unsigned int i = 0; i < barrieres[0].size(); i++)
	{
		barrieres[0][i] = new MatrixTransform();
		barrieres[1][i] = new MatrixTransform();
	}

	// créations des MatrixTransform des pions
	pions[0] = new MatrixTransform();
	pions[1] = new MatrixTransform();

	// création de la barrière virtuelle
	virtualBarriere = new MatrixTransform();
}

vector<ref_ptr<MatrixTransform>> Controller::getPions()
{
	// création du vecteur de vecteur de pions
	vector<ref_ptr<MatrixTransform>> result = vector<ref_ptr<MatrixTransform>>(2);

	for (unsigned int i = 0; i < result.size(); i++)
	{
		//récupération de la coordonnée d'un pion
		Point p = Model::getInstance()->getPion(i + 1);

		// création de la Matrix grâce au coordonnées
		pions[i]->setMatrix(cases[p.getX()][p.getY()]->getMatrix());
		result[i] = pions[i];
	}
	
	return result;
}

void Controller::setVirtualBarriere(ref_ptr<Node> node)
{
	// ajout du node dans les barrières virtuelles
	virtualBarriere->addChild(node);
}

void Controller::setCamera(ref_ptr<Camera> camera)
{
	this->camera = camera;
	
	// ajout des barrières dans la caméra pour l'affichage
	for (unsigned int i = 0; i < barrieres[0].size(); i++)
	{
		camera->addChild(barrieres[0][i].get());
		camera->addChild(barrieres[1][i].get());
	}

	// ajout des pions dans la caméra pour l'affichage
	camera->addChild(pions[0].get());
	camera->addChild(pions[1].get());
}

vector<ref_ptr<MatrixTransform>> Controller::getBarrieres()
{
	// création du vecteur de vecteur des barrières
	vector<ref_ptr<MatrixTransform>> result = vector<ref_ptr<MatrixTransform>>();

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

		result.push_back(mt.get());
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

		result.push_back(mt.get());
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

		result.push_back(mt.get());
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

		result.push_back(mt.get());
	}

	return result;
}

ref_ptr<MatrixTransform> Controller::getVirtualBarriere()
{
	// calculer les barrières virtuelles
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

	// placement de la barrières uniquement si la caméra ne la contient pas déjà
	if (!camera->containsNode(virtualBarriere))
		camera->addChild(virtualBarriere);

	return virtualBarriere;
}

void Controller::removeVirtualBarriere()
{
	// supression de la caméra de la barrière virtuelle
	camera->removeChild(virtualBarriere);
}
