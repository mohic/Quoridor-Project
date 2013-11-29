#include "GameView.h"

using namespace std;
using namespace osg;

GameView *GameView::instance = 0;

GameView::GameView()
{
	// configuration des caméras
	center       = POSITION_CENTRE;
	up           = Vec3(0, 1, 0);
	eyeOrtho     = Vec3(0, 0, 1);
	eyePrimitive = Vec3(0, -50, 100);
}

GameView *GameView::getInstance()
{
	if (instance == 0)
		instance = new GameView();

	return instance;
}

void GameView::initMessage()
{
	// initialisation du message utilisateur
	userMessage = new osgText::Text();
	userMessage->setCharacterSize(3);
	userMessage->setPosition(Vec3(-98, -3, 0));
	userMessage->setText(Model::getInstance()->getUserMessage());
	userMessage->setColor(COLOR_MESSAGE);

	// initialisation du message d'erreur
	errorMessage = new osgText::Text();
	errorMessage->setCharacterSize(3);
	errorMessage->setPosition(Vec3(-98, 0, 0));
	errorMessage->setText(Model::getInstance()->getErrorMessage());
	errorMessage->setColor(COLOR_MESSAGE);
}

void GameView::refreshMessage()
{
	userMessage->setText(Model::getInstance()->getUserMessage());
	errorMessage->setText(Model::getInstance()->getErrorMessage());
}

void GameView::createAndConfigureViewer()
{
	viewer = new osgViewer::Viewer();

	viewer->setUpViewInWindow(FENETRE_X, FENETRE_Y, FENETRE_WIDTH, FENETRE_HEIGHT);
	viewer->getCamera()->setClearColor(BLACK);
	viewer->getCamera()->setAllowEventFocus(false);
}

void GameView::createAndConfigureCameraGameArea()
{
	cameraGameArea = new Camera();

	// vue orthographique
	cameraGameArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraGameArea->setProjectionMatrixAsOrtho(-100, 100, -100, 100, 0.5, 5);
	cameraGameArea->setViewport(new Viewport(10, FENETRE_HEIGHT - (FENETRE_WIDTH - 10), FENETRE_WIDTH - 20, FENETRE_WIDTH - 20));
	cameraGameArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraGameArea->setClearColor(BLACK);

	// TODO: vue perspective
	//cameraPlateau->setViewMatrixAsLookAt(eyePlateau, center, up);
	//cameraPlateau->setProjectionMatrixAsPerspective(70, 1, 0.1, 200);
}

void GameView::createAndConfigureCameraDisplayArea()
{
	cameraDisplayArea = new Camera();

	cameraDisplayArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraDisplayArea->setProjectionMatrixAsOrtho(-100, 100, -8, 8, 0.5, 5); // TODO: rapport largeur/hauteur == 200/16
	cameraDisplayArea->setViewport(new Viewport(10, 10, FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 20, 90)); // TODO: (FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 20)/90
	cameraDisplayArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraDisplayArea->setClearColor(COLOR_PLATEAU);
}

void GameView::createAndConfigureCameraActionsArea()
{
	cameraActionsArea = new Camera();

	cameraActionsArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraActionsArea->setProjectionMatrixAsOrtho(-100, 100, -40, 40, 0.5, 5);
	cameraActionsArea->setViewport(new Viewport(10 + FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 10, 10, FENETRE_WIDTH - (10 + FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 10) - 10, 90));
	cameraActionsArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraActionsArea->setClearColor(COLOR_PLATEAU);
}

void GameView::createArrowButton()
{
	ref_ptr<ShapeDrawable> topArrow = new ShapeDrawable();
	topArrow->setShape(new Cone(Vec3(0, 0, .3), .8, 1));
	topArrow->setColor(BLACK);

	ref_ptr<Geode> geodeTopArrow = new Geode();
	StateSet *stateset = geodeTopArrow->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);
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
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	geodeBaseArrow->addDrawable(baseArrow.get());

	arrowButton = new MatrixTransform();
	arrowButton->addChild(topArrowTransform.get());
	arrowButton->addChild(geodeBaseArrow.get());
	arrowButton->setMatrix(Matrix::identity());
	arrowButton->postMult(Matrix::scale(15, 15, 2));
}

void GameView::createClassicButton()
{
	ref_ptr<ShapeDrawable> button = new ShapeDrawable();
	button->setShape(new Box(POSITION_CENTRE, 1.5, 1, 1));
	button->setColor(BLACK);

	classicButton = new Geode();

	StateSet *stateset = classicButton->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	classicButton->addDrawable(button.get());
}

void GameView::drawPlate()
{
	ref_ptr<Texture2D> textPlateau = new Texture2D();

	ref_ptr<Image> imgPlateau = osgDB::readImageFile("resources/plateau.jpg");
	textPlateau->setImage(imgPlateau);

	ref_ptr<ShapeDrawable> plateau = new ShapeDrawable();
	plateau->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTaillePlateau(), Config::getInstance()->getTaillePlateau(), 5));
	plateau->setColor(COLOR_PLATEAU);

	ref_ptr<Geode> geodePlateau = new Geode();

	StateSet *stateset = geodePlateau->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);
	stateset->setTextureAttributeAndModes(0, textPlateau, StateAttribute::Values::ON);

	geodePlateau->addDrawable(plateau.get());
	cameraGameArea->addChild(geodePlateau.get());
}

void GameView::drawCases()
{
	ref_ptr<ShapeDrawable> casePlateau = new ShapeDrawable();
	casePlateau->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleCase(), Config::getInstance()->getTailleCase(), 8));
	casePlateau->setColor(COLOR_CASE);
	
	ref_ptr<Geode> geodeCase = new Geode();
	
	StateSet *stateset = geodeCase->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	geodeCase->addDrawable(casePlateau.get());

	ref_ptr<MatrixTransform> caseTransform;

	int pxInit =  - ((Config::getInstance()->getNbrCases() - 1) / 2 * Config::getInstance()->getTailleCase()) // retire le décalage par rapport à chaque case
		     - ((Config::getInstance()->getNbrBarriereJoueur() - 2) / 2 * Config::getInstance()->getTailleRainure()); // retire le décalage par rapport au rainure

	int px = pxInit;
	int py = -pxInit;

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
			cameraGameArea->addChild(caseTransform.get());
		}

		py -= (Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure());
		px = pxInit;
	}
}

void GameView::drawFences()
{
	ref_ptr<ShapeDrawable> barriere = new ShapeDrawable();
	barriere->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleBarriere(), Config::getInstance()->getTailleRainure(), 18));
	barriere->setColor(COLOR_BARRIERE);

	ref_ptr<Geode> geodeBarriere = new Geode();

	StateSet *stateset = geodeBarriere->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	geodeBarriere->addDrawable(barriere.get());

	vector<ref_ptr<MatrixTransform>> barrieres = Controller::getInstance()->getBarrieres();

	for (unsigned int i = 0; i < barrieres.size(); i++)
		barrieres[i]->addChild(geodeBarriere.get());
}

void GameView::drawVirtualFence()
{
	ref_ptr<ShapeDrawable> virtualFence = new ShapeDrawable();
	virtualFence->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleBarriere() + 0.1, Config::getInstance()->getTailleRainure() + 0.1, 18 + 0.1));
	virtualFence->setColor(COLOR_VIRTUAL_BARRIERE);

	geodeVirtualFence = new Geode();

	StateSet *stateset = geodeVirtualFence->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	geodeVirtualFence->addDrawable(virtualFence.get());
}

void GameView::drawPawns()
{
	ref_ptr<ShapeDrawable> pion1 = new ShapeDrawable();
	ref_ptr<ShapeDrawable> pion2 = new ShapeDrawable();
	ref_ptr<Geode> geodePion1 = new Geode();
	ref_ptr<Geode> geodePion2 = new Geode();

	pion1->setShape(new Cylinder(POSITION_CENTRE, Config::getInstance()->getTaillePion(), 20));
	pion2->setShape(new Cylinder(POSITION_CENTRE, Config::getInstance()->getTaillePion(), 20));
	pion1->setColor(COLOR_PION_1);
	pion2->setColor(COLOR_PION_2);

	StateSet *stateset = geodePion1->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	stateset = geodePion2->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	geodePion1->addDrawable(pion1.get());
	geodePion2->addDrawable(pion2.get());

	vector<ref_ptr<MatrixTransform>> pions = Controller::getInstance()->getPions();

	pions[0]->addChild(geodePion1.get());
	pions[1]->addChild(geodePion2.get());
}

void GameView::drawMessage()
{
	ref_ptr<Geode> geodeMessage = new Geode();
	
	StateSet *stateset = geodeMessage->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	initMessage();

	// message utilisateur
	geodeMessage->addDrawable(userMessage);
	cameraDisplayArea->addChild(geodeMessage.get());

	// message d'erreur
	geodeMessage->addDrawable(errorMessage);
	cameraDisplayArea->addChild(geodeMessage.get());
}

void GameView::drawArrowAndDirectionButtons()
{
	ref_ptr<MatrixTransform> arrowUp = new MatrixTransform();
	arrowUp->addChild(arrowButton.get());
	arrowUp->setMatrix(Matrix::identity());
	arrowUp->postMult(Matrix::translate(Vec3(-60, 20, 0)));

	ref_ptr<MatrixTransform> arrowLeft = new MatrixTransform();
	arrowLeft->addChild(arrowButton.get());
	arrowLeft->setMatrix(Matrix::identity());
	arrowLeft->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	arrowLeft->postMult(Matrix::translate(Vec3(-80, 0, 0)));

	ref_ptr<MatrixTransform> arrowDown = new MatrixTransform();
	arrowDown->addChild(arrowButton.get());
	arrowDown->setMatrix(Matrix::identity());
	arrowDown->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	arrowDown->postMult(Matrix::translate(Vec3(-60, -20, 0)));

	ref_ptr<MatrixTransform> arrowRight = new MatrixTransform();
	arrowRight->addChild(arrowButton.get());;
	arrowRight->setMatrix(Matrix::identity());
	arrowRight->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	arrowRight->postMult(Matrix::translate(Vec3(-40, 0, 0)));

	cameraActionsArea->addChild(arrowUp.get());
	cameraActionsArea->addChild(arrowLeft.get());
	cameraActionsArea->addChild(arrowDown.get());
	cameraActionsArea->addChild(arrowRight.get());

	// dessin du bouton de changement de sens
	ref_ptr<ShapeDrawable> buttonSens = new ShapeDrawable();
	buttonSens->setShape(new Box(POSITION_CENTRE, 1));
	buttonSens->setColor(BLACK);

	ref_ptr<Geode> geodeButtonSens = new Geode();

	StateSet *stateset = geodeButtonSens->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	geodeButtonSens->addDrawable(buttonSens.get());

	ref_ptr<MatrixTransform> transformButtonSens = new MatrixTransform();
	transformButtonSens->addChild(geodeButtonSens.get());
	transformButtonSens->setMatrix(Matrix::identity());
	transformButtonSens->postMult(Matrix::scale(11, 10, 1));
	transformButtonSens->postMult(Matrix::translate(Vec3(-60, 0, 0)));

	cameraActionsArea->addChild(transformButtonSens.get());
}

void GameView::drawCommandsButtons()
{
	// dessin du bouton de changement de mode
	ref_ptr<MatrixTransform> transformButtonMode = new MatrixTransform();
	transformButtonMode->addChild(classicButton.get());
	transformButtonMode->setMatrix(Matrix::identity());
	transformButtonMode->postMult(Matrix::scale(20, 20, 1));
	transformButtonMode->postMult(Matrix::translate(Vec3(0, 20, 0)));

	cameraActionsArea->addChild(transformButtonMode.get());

	// dessin du bouton de validation de la barrière virtuelle
	ref_ptr<MatrixTransform> transformButtonValidate = new MatrixTransform();
	transformButtonValidate->addChild(classicButton.get());
	transformButtonValidate->setMatrix(Matrix::identity());
	transformButtonValidate->postMult(Matrix::scale(20, 20, 1));
	transformButtonValidate->postMult(Matrix::translate(Vec3(0, -20, 0)));

	cameraActionsArea->addChild(transformButtonValidate.get());

	// dessin du bouton de chargement d'une partie
	ref_ptr<MatrixTransform> transformButtonLoad = new MatrixTransform();
	transformButtonLoad->addChild(classicButton.get());
	transformButtonLoad->setMatrix(Matrix::identity());
	transformButtonLoad->postMult(Matrix::scale(20, 20, 1));
	transformButtonLoad->postMult(Matrix::translate(Vec3(60, 20, 0)));

	cameraActionsArea->addChild(transformButtonLoad.get());

	// dessin du bouton de sauvegarde d'une partie
	ref_ptr<MatrixTransform> transformButtonSave = new MatrixTransform();
	transformButtonSave->addChild(classicButton.get());
	transformButtonSave->setMatrix(Matrix::identity());
	transformButtonSave->postMult(Matrix::scale(20, 20, 1));
	transformButtonSave->postMult(Matrix::translate(Vec3(60, -20, 0)));

	cameraActionsArea->addChild(transformButtonSave.get());

	// dessin du bouton pour recommencer une partie
	ref_ptr<MatrixTransform> restartButtonSave = new MatrixTransform();
	restartButtonSave->addChild(classicButton.get());
	restartButtonSave->setMatrix(Matrix::identity());
	restartButtonSave->postMult(Matrix::scale(20, 20, 1));
	restartButtonSave->postMult(Matrix::translate(Vec3(30, 0, 0)));

	cameraActionsArea->addChild(restartButtonSave.get());
}

ref_ptr<osgViewer::Viewer> GameView::buildSceneGraph()
{
	// création et configuration du viewer
	createAndConfigureViewer();

	// création et configuration de la caméra de la zone de jeu
	createAndConfigureCameraGameArea();
	Controller::getInstance()->setCamera(cameraGameArea.get());

	// création et configuration de la caméra d'affichage des messages
	createAndConfigureCameraDisplayArea();

	// création et configuration de la caméra affichant les différents boutons graphiques
	createAndConfigureCameraActionsArea();

	// dessin du plateau
	drawPlate();

	// dessin des cases
	drawCases();

	// dessin des barrières
	drawFences();

	// dessin de la barrière virtuelle
	drawVirtualFence();
	Controller::getInstance()->setVirtualBarriere(geodeVirtualFence.get());

	// dessin des pions
	drawPawns();

	// configuration de l'affichage des messages
	drawMessage();

	// créer une flèche
	createArrowButton();

	// dessiner les boutons flèchés et de changements de sens
	drawArrowAndDirectionButtons();

	// créer un bouton
	createClassicButton();

	// dessin des différents boutons de commandes
	drawCommandsButtons();

	// création d'un noeud racine de type group contenant les caméras
	ref_ptr<Group> root = new Group();

	root->addChild(cameraGameArea.get());
	root->addChild(cameraDisplayArea.get());
	root->addChild(cameraActionsArea.get());

	// ajouter les callbacks
	root->setEventCallback(new EventController());
	root->setUpdateCallback(new PositionUpdater());

	// ajouter le noeud racine au viewer
	viewer->setSceneData(root.get());

	return viewer.get();
}
