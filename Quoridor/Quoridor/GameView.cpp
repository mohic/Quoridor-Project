#include "GameView.h"

using namespace std;
using namespace osg;

GameView *GameView::instance = 0;

GameView::GameView()
{
}


GameView *GameView::getInstance(void)
{
	if (instance == 0)
		instance = new GameView();

	return instance;
}


ref_ptr<osgViewer::Viewer> GameView::buildSceneGraph()
{
	StateSet *stateset;

	ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	
	// configuration du viewer
	viewer->setUpViewInWindow(FENETRE_X, FENETRE_Y, FENETRE_WIDTH, FENETRE_HEIGHT);
	viewer->getCamera()->setClearColor(BLACK);
	viewer->getCamera()->setAllowEventFocus(false);

	// configuration des caméras
	Vec3 eye    = Vec3(0, 0, 1);
	Vec3 center = Vec3(0, 0, 0);
	Vec3 up     = Vec3(0, 1, 0);

	ref_ptr<Camera> cameraPlateau = new Camera();

	cameraPlateau->setViewMatrixAsLookAt(eye, center, up);
	cameraPlateau->setProjectionMatrixAsOrtho(-100, 100, -100, 100, 0.5, 5);
	cameraPlateau->setViewport(new Viewport(10, FENETRE_HEIGHT - (FENETRE_WIDTH - 10), FENETRE_WIDTH - 20, FENETRE_WIDTH - 20));
	cameraPlateau->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraPlateau->setClearColor(COLOR_PLATEAU);

	Controller::getInstance()->setCamera(cameraPlateau.get());

	ref_ptr<Camera> cameraTexte = new Camera;

	cameraTexte->setViewMatrixAsLookAt(eye, center, up);
	cameraTexte->setProjectionMatrixAsOrtho(-100, 100, -8, 8, 0.5, 5);
	cameraTexte->setViewport(new Viewport(10, 10, FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 20, 90));
	cameraTexte->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraTexte->setClearColor(COLOR_PLATEAU);

	ref_ptr<Camera> cameraAction = new Camera();

	cameraAction->setViewMatrixAsLookAt(eye, center, up);
	cameraAction->setProjectionMatrixAsOrtho(-100, 100, -40, 40, 0.5, 5);
	cameraAction->setViewport(new Viewport(10 + FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 10, 10, FENETRE_WIDTH - (10 + FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 10) - 10, 90));
	cameraAction->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraAction->setClearColor(COLOR_PLATEAU);

	// dessin du plateau
	ref_ptr<ShapeDrawable> plateau = new ShapeDrawable();
	plateau->setShape(new Box(Vec3(0, 0, 0), Config::getInstance()->getTaillePlateau(), Config::getInstance()->getTaillePlateau(), 0));
	plateau->setColor(COLOR_PLATEAU);

	ref_ptr<Geode> geodePlateau = new Geode();

	stateset = geodePlateau->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodePlateau->addDrawable(plateau.get());
	cameraPlateau->addChild(geodePlateau.get());

	// dessin des cases
	ref_ptr<ShapeDrawable> casePlateau = new ShapeDrawable();
	casePlateau->setShape(new Box(Vec3(0, 0, 0), Config::getInstance()->getTailleCase(), Config::getInstance()->getTailleCase(), 1));
	casePlateau->setColor(COLOR_CASE);
	
	ref_ptr<Geode> geodeCase = new Geode();
	
	stateset = geodeCase->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeCase->addDrawable(casePlateau.get());

	ref_ptr<MatrixTransform> caseTransform;
	int px = -90 + (Config::getInstance()->getTailleCase() * 2) + Config::getInstance()->getTailleRainure();
	int py = 90 - (Config::getInstance()->getTailleCase() * 2) - Config::getInstance()->getTailleRainure();

	for (int y = 0; y < Config::getInstance()->getNbrCases(); y++)
	{
		for (int x = 0; x < Config::getInstance()->getNbrCases(); x++)
		{
			caseTransform = new MatrixTransform();
			caseTransform->addChild(geodeCase.get());
			caseTransform->setMatrix(Matrix::identity());
			caseTransform->postMult(Matrix::translate(Vec3(px, py, 0)));

			px += Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure();

			Controller::getInstance()->setCase(Point(x, y), caseTransform.get());
			cameraPlateau->addChild(caseTransform.get());
		}

		py -= (Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure());
		px = -90 + (Config::getInstance()->getTailleCase() * 2) + Config::getInstance()->getTailleRainure();
	}

	// dessin des barrières
	ref_ptr<ShapeDrawable> barriere = new ShapeDrawable();
	barriere->setShape(new Box(Vec3(0, 0, 0), Config::getInstance()->getTailleBarriere(), Config::getInstance()->getTailleRainure(), 2));
	barriere->setColor(COLOR_BARRIERE);

	ref_ptr<Geode> geodeBarriere = new Geode();

	stateset = geodeBarriere->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeBarriere->addDrawable(barriere.get());

	vector<ref_ptr<MatrixTransform>> barrieres = Controller::getInstance()->getBarrieres();

	for (unsigned int i = 0; i < barrieres.size(); i++)
		barrieres[i]->addChild(geodeBarriere.get());

	// dessin de la barrière virtuelle
	ref_ptr<ShapeDrawable> virtualBarriere = new ShapeDrawable();
	virtualBarriere->setShape(new Box(Vec3(0, 0, 0), Config::getInstance()->getTailleBarriere(), Config::getInstance()->getTailleRainure(), 3));
	virtualBarriere->setColor(COLOR_VIRTUAL_BARRIERE);

	ref_ptr<Geode> geodeVirtualBarriere = new Geode();

	stateset = geodeVirtualBarriere->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeVirtualBarriere->addDrawable(virtualBarriere.get());

	Controller::getInstance()->setVirtualBarriere(geodeVirtualBarriere.get());

	// dessin des pions
	ref_ptr<ShapeDrawable> pion1 = new ShapeDrawable();
	ref_ptr<ShapeDrawable> pion2 = new ShapeDrawable();
	ref_ptr<Geode> geodePion1 = new Geode();
	ref_ptr<Geode> geodePion2 = new Geode();

	pion1->setShape(new Cylinder(Vec3(0, 0, 0), Config::getInstance()->getTaillePion(), 2));
	pion2->setShape(new Cylinder(Vec3(0, 0, 0), Config::getInstance()->getTaillePion(), 2));
	pion1->setColor(COLOR_PION_1);
	pion2->setColor(COLOR_PION_2);

	stateset = geodePion1->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	stateset = geodePion2->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodePion1->addDrawable(pion1.get());
	geodePion2->addDrawable(pion2.get());

	vector<ref_ptr<MatrixTransform>> pions = Controller::getInstance()->getPions();

	pions[0]->addChild(geodePion1.get());
	pions[1]->addChild(geodePion2.get());
	
	// configuration de l'affichage des messages
	ref_ptr<Geode> geodeMessage = new Geode();
	
	stateset = geodeMessage->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	initMessage();

	geodeMessage->addDrawable(message);
	cameraTexte->addChild(geodeMessage.get());

	// créer une flèche
	ref_ptr<ShapeDrawable> topArrow = new ShapeDrawable();
	topArrow->setShape(new Cone(Vec3(0, 0, .3), .8, 1));
	topArrow->setColor(BLACK);

	ref_ptr<Geode> geodeTopArrow = new Geode();
	stateset = geodeTopArrow->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);
	geodeTopArrow->addDrawable(topArrow.get());

	ref_ptr<MatrixTransform> topArrowTransform = new MatrixTransform();
	topArrowTransform->addChild(geodeTopArrow.get());
	topArrowTransform->setMatrix(Matrix::identity());
	topArrowTransform->setMatrix(Matrix::rotate(inDegrees(-90.0), X_AXIS));

	ref_ptr<ShapeDrawable> baseArrow = new ShapeDrawable();
	baseArrow->setShape(new Box(Vec3(0, -.3, 0), .8, 1, 1));
	baseArrow->setColor(BLACK);

	ref_ptr<Geode> geodeBaseArrow = new Geode();
	
	stateset = geodeBaseArrow->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeBaseArrow->addDrawable(baseArrow.get());

	// dessiner les boutons flèchés
	ref_ptr<MatrixTransform> arrowUp = new MatrixTransform();
	arrowUp->addChild(topArrowTransform.get());
	arrowUp->addChild(geodeBaseArrow.get());
	arrowUp->setMatrix(Matrix::identity());
	arrowUp->postMult(Matrix::scale(15, 15, 2));
	arrowUp->postMult(Matrix::translate(Vec3(-60, 20, 0)));

	ref_ptr<MatrixTransform> arrowLeft = new MatrixTransform();
	arrowLeft->addChild(topArrowTransform.get());
	arrowLeft->addChild(geodeBaseArrow.get());
	arrowLeft->setMatrix(Matrix::identity());
	arrowLeft->postMult(Matrix::scale(15, 15, 2));
	arrowLeft->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	arrowLeft->postMult(Matrix::translate(Vec3(-80, 0, 0)));

	ref_ptr<MatrixTransform> arrowDown = new MatrixTransform();
	arrowDown->addChild(topArrowTransform.get());
	arrowDown->addChild(geodeBaseArrow.get());
	arrowDown->setMatrix(Matrix::identity());
	arrowDown->postMult(Matrix::scale(15, 15, 2));
	arrowDown->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	arrowDown->postMult(Matrix::translate(Vec3(-60, -20, 0)));

	ref_ptr<MatrixTransform> arrowRight = new MatrixTransform();
	arrowRight->addChild(topArrowTransform.get());
	arrowRight->addChild(geodeBaseArrow.get());
	arrowRight->setMatrix(Matrix::identity());
	arrowRight->postMult(Matrix::scale(15, 15, 2));
	arrowRight->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	arrowRight->postMult(Matrix::translate(Vec3(-40, 0, 0)));

	cameraAction->addChild(arrowUp.get());
	cameraAction->addChild(arrowLeft.get());
	cameraAction->addChild(arrowDown.get());
	cameraAction->addChild(arrowRight.get());

	// dessin du bouton de changement de sens
	ref_ptr<ShapeDrawable> buttonSens = new ShapeDrawable();
	buttonSens->setShape(new Box(Vec3(0, 0, 0), 1));
	buttonSens->setColor(BLACK);

	ref_ptr<Geode> geodeButtonSens = new Geode();

	stateset = geodeButtonSens->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeButtonSens->addDrawable(buttonSens.get());

	ref_ptr<MatrixTransform> transformButtonSens = new MatrixTransform();
	transformButtonSens->addChild(geodeButtonSens.get());
	transformButtonSens->setMatrix(Matrix::identity());
	transformButtonSens->postMult(Matrix::scale(11, 10, 1));
	transformButtonSens->postMult(Matrix::translate(Vec3(-60, 0, 0)));

	cameraAction->addChild(transformButtonSens.get());

	// créer un bouton
	ref_ptr<ShapeDrawable> button = new ShapeDrawable();
	button->setShape(new Box(Vec3(0, 0, 0), 1.5, 1, 1));
	button->setColor(BLACK);

	ref_ptr<Geode> geodeButton = new Geode();

	stateset = geodeButton->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeButton->addDrawable(button.get());

	// dessin du bouton de changement de mode
	ref_ptr<MatrixTransform> transformButtonMode = new MatrixTransform();
	transformButtonMode->addChild(geodeButton.get());
	transformButtonMode->setMatrix(Matrix::identity());
	transformButtonMode->postMult(Matrix::scale(20, 20, 1));
	transformButtonMode->postMult(Matrix::translate(Vec3(0, 20, 0)));

	cameraAction->addChild(transformButtonMode.get());

	// dessin du bouton de validation de la barrière virtuelle
	ref_ptr<MatrixTransform> transformButtonValidate = new MatrixTransform();
	transformButtonValidate->addChild(geodeButton.get());
	transformButtonValidate->setMatrix(Matrix::identity());
	transformButtonValidate->postMult(Matrix::scale(20, 20, 1));
	transformButtonValidate->postMult(Matrix::translate(Vec3(0, -20, 0)));

	cameraAction->addChild(transformButtonValidate.get());

	// dessin du bouton de chargement d'une partie
	ref_ptr<MatrixTransform> transformButtonLoad = new MatrixTransform();
	transformButtonLoad->addChild(geodeButton.get());
	transformButtonLoad->setMatrix(Matrix::identity());
	transformButtonLoad->postMult(Matrix::scale(20, 20, 1));
	transformButtonLoad->postMult(Matrix::translate(Vec3(60, 20, 0)));

	cameraAction->addChild(transformButtonLoad.get());

	// dessin du bouton de sauvegarde d'une partie
	ref_ptr<MatrixTransform> transformButtonSave = new MatrixTransform();
	transformButtonSave->addChild(geodeButton.get());
	transformButtonSave->setMatrix(Matrix::identity());
	transformButtonSave->postMult(Matrix::scale(20, 20, 1));
	transformButtonSave->postMult(Matrix::translate(Vec3(60, -20, 0)));

	cameraAction->addChild(transformButtonSave.get());

	// création d'un noeud racine de type group contenant les caméras
	ref_ptr<Group> root = new Group();

	root->addChild(cameraPlateau.get());
	root->addChild(cameraTexte.get());
	root->addChild(cameraAction.get());

	// ajouter les callbacks
	root->setEventCallback(new EventController());
	root->setUpdateCallback(new PositionUpdater());

	// ajouter le noeud racine au viewer
	viewer->setSceneData(root.get());

	return viewer.get();
}

void GameView::initMessage()
{
	message = new osgText::Text();
	message->setCharacterSize(3);
	message->setPosition(Vec3(-98, -1.5, 0));
	message->setText(Model::getInstance()->getMessage());
	message->setColor(COLOR_MESSAGE);
}

void GameView::refreshMessage()
{
	message->setText(Model::getInstance()->getMessage());
}