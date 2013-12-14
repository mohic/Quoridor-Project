#include "GameView.h"

using namespace std;
using namespace osg;

GameView *GameView::instance = 0;

//TODO: bug en vue perspective sur la barri�re virtuelle qui a deux couleurs
//TODO: bug bouton direction (le bouton changer se sens "d�borde" sur les autres
//      dans la d�tection de collision)

GameView::GameView()
{
	// configuration des cam�ras
	center         = POSITION_CENTRE;
	eye            = Vec3(0, 0, 1);
	up             = Vec3(0, 1, 0);
	eyeOrtho       = Vec3(0, 0, 1);
	eyePerspective = Vec3(0, -130, 150);

	offset         = 0;
}

GameView *GameView::getInstance()
{
	if (instance == 0)
		instance = new GameView();

	return instance;
}

void GameView::initMessage()
{
	// cr�ation de la police d'�criture
	ref_ptr<osgText::Font> font = osgText::readFontFile("resources/fonts/messages.ttf");

	// initialisation du message utilisateur
	userMessage = new osgText::Text();
	userMessage->setFont(font);
	userMessage->setCharacterSize(FONT_SIZE);
	userMessage->setPosition(Vec3(-98, 1, 0));
	userMessage->setText(Model::getInstance()->getUserMessage());
	userMessage->setColor(COLOR_MESSAGE);

	// initialisation du message d'erreur
	errorMessage = new osgText::Text();
	errorMessage->setFont(font);
	errorMessage->setCharacterSize(FONT_SIZE);
	errorMessage->setPosition(Vec3(-98, -5.5, 0));
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
	viewer->getCamera()->setAllowEventFocus(false);
	viewer->getCamera()->setClearColor(COLOR_BORDER);

	// changer le titre de la fen�tre
	osgViewer::Viewer::Windows windows; // variable servant � contenir la liste des diff�rentes fen�tre cr��e par OSG
										// sous forme d'un vecteur. Avec la configuration par d�faut, OSG cr�e autant
										// de fen�tre en plein �cran que d'�cran branch� � l'ordinateur ex�cutant le
										// programme.
	viewer->getWindows(windows); // obtenir la liste des diff�rentes fen�tre d'OSG

	for (unsigned int i = 0; i < windows.size(); i++) // d�finir un titre pour chacune des fen�tres
		windows[i]->setWindowName("Quoridor - 3BIN - IPL");
}

void GameView::createAndConfigureCameraGameArea()
{
	cameraGameArea = new Camera();

	// vue parall�le (par d�faut)
	cameraGameArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraGameArea->setProjectionMatrixAsOrtho(CAMERA_LEFT, CAMERA_RIGHT, CAMERA_PLATEAU_BOTTOM, CAMERA_PLATEAU_TOP, CAMERA_NEAR, CAMERA_FAR);
	cameraGameArea->setViewport(new Viewport(ESPACEMENT, ESPACEMENT + MESSAGE_HEIGHT + ESPACEMENT, PLATEAU_TAILLE, PLATEAU_TAILLE));
	cameraGameArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraGameArea->setClearColor(BLACK);
}

void GameView::createAndConfigureCameraDisplayArea()
{
	cameraDisplayArea = new Camera();

	cameraDisplayArea->setViewMatrixAsLookAt(eye, center, up);
	cameraDisplayArea->setProjectionMatrixAsOrtho(CAMERA_LEFT, CAMERA_RIGHT, CAMERA_MESSAGE_BOTTOM, CAMERA_MESSAGE_TOP, CAMERA_NEAR, CAMERA_FAR);
	cameraDisplayArea->setViewport(new Viewport(ESPACEMENT, ESPACEMENT, FENETRE_WIDTH - ESPACEMENT - ESPACEMENT, MESSAGE_HEIGHT));
	cameraDisplayArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraDisplayArea->setClearColor(BLACK);
}

void GameView::createAndConfigureCameraActionsArea()
{
	cameraActionsArea = new Camera();

	cameraActionsArea->setViewMatrixAsLookAt(eye, center, up);
	cameraActionsArea->setProjectionMatrixAsOrtho(CAMERA_LEFT, CAMERA_RIGHT, CAMERA_ACTIONS_BOTTOM, CAMERA_ACTIONS_TOP, CAMERA_NEAR, CAMERA_FAR);
	cameraActionsArea->setViewport(new Viewport((ESPACEMENT * 2) + PLATEAU_TAILLE, (ESPACEMENT *2 ) + MESSAGE_HEIGHT, ACTIONS_WIDTH, PLATEAU_TAILLE));
	cameraActionsArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraActionsArea->setClearColor(BLACK);
}

void GameView::createClassicButton()
{
	ref_ptr<ShapeDrawable> button = new ShapeDrawable();
	button->setShape(new Box(POSITION_CENTRE, 70, 70, 1));
	button->setColor(WHITE);

	classicButton = new Geode();

	StateSet *stateset = classicButton->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	classicButton->addDrawable(button.get());
}

void GameView::drawPlate()
{
	ref_ptr<Texture2D> texture = new Texture2D();
	ref_ptr<Image> img = osgDB::readImageFile("resources/textures/plate.jpg");
	bool textureFound = img != 0;
	texture->setImage(img);

	ref_ptr<ShapeDrawable> plateau = new ShapeDrawable();
	plateau->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTaillePlateau(), Config::getInstance()->getTaillePlateau(), 5));
	if (!textureFound) plateau->setColor(COLOR_PLATEAU);

	ref_ptr<Geode> geodePlateau = new Geode();

	StateSet *stateset = geodePlateau->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);
	if (textureFound) stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	geodePlateau->addDrawable(plateau.get());
	cameraGameArea->addChild(geodePlateau.get());

	
}

void GameView::drawCases()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;

	// cr�ation de la texture d'une case
	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/case.jpg");
	texture->setImage(img);

	// cr�ation de l'apparence d'une case
	ref_ptr<ShapeDrawable> casePlateau = new ShapeDrawable();
	casePlateau->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleCase(), Config::getInstance()->getTailleCase(), 8));
	casePlateau->setColor(COLOR_CASE);
	
	// cr�ation de la g�ode avec ajout de la texture
	ref_ptr<Geode> geodeCase = new Geode();
	
	StateSet *stateset = geodeCase->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	geodeCase->addDrawable(casePlateau.get());

	// cr�ation et positionnement de toutes les cases
	ref_ptr<MatrixTransform> caseTransform;

	int pxInit =  - ((Config::getInstance()->getNbrCases() - 1) / 2 * Config::getInstance()->getTailleCase()) // retire le d�calage par rapport � chaque case
		     - ((Config::getInstance()->getNbrBarriereJoueur() - 2) / 2 * Config::getInstance()->getTailleRainure()); // retire le d�calage par rapport au rainure

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

	for (int joueur = 1; joueur <= 2; joueur++)
	{
		for (int i = 0; i < Config::getInstance()->getNbrBarriereJoueur(); i++)
		{
			ref_ptr<MatrixTransform> mt = new MatrixTransform();
			mt->addChild(geodeBarriere.get());

			Controller::getInstance()->setBarriere(Point(joueur, i), mt.get());
		}
	}
}

void GameView::drawVirtualFence()
{
	ref_ptr<ShapeDrawable> virtualFence = new ShapeDrawable();
	virtualFence->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleBarriere() + 0.1, Config::getInstance()->getTailleRainure() + 0.1, 18 + 0.1));
	virtualFence->setColor(COLOR_VIRTUAL_BARRIERE);

	geodeVirtualFence = new Geode();

	StateSet *stateset = geodeVirtualFence->getOrCreateStateSet();

	geodeVirtualFence->addDrawable(virtualFence.get());

	ref_ptr<MatrixTransform> mt = new MatrixTransform();
	mt->addChild(geodeVirtualFence.get());

	Controller::getInstance()->setVirtualBarriere(mt);
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

	ref_ptr<MatrixTransform> mt1 = new MatrixTransform();
	mt1->addChild(geodePion1.get());
	ref_ptr<MatrixTransform> mt2 = new MatrixTransform();
	mt2->addChild(geodePion2.get());

	Controller::getInstance()->setPion(1, mt1);
	Controller::getInstance()->setPion(2, mt2);
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
	ref_ptr<Texture2D> texture;
	ref_ptr<Texture2D> textureHover;
	ref_ptr<Image> img;
	StateSet *stateset;
	ref_ptr<MatrixTransform> button;
	ref_ptr<Switch> sw;

	// matrixTransform contenant les touches de directions et de changements de direction
	ref_ptr<MatrixTransform> arrows = new MatrixTransform();
	arrows->setMatrix(Matrix::identity());
	arrows->postMult(Matrix::translate(0, 270, 0));

	cameraActionsArea->addChild(arrows.get());

	// cr�ation de la texture d'une fl�che
	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/arrow.png");
	texture->setImage(img);

	textureHover = new Texture2D();
	img = osgDB::readImageFile("resources/textures/arrow_hover.png");
	textureHover->setImage(img);

	// fl�che du haut
	sw = new Switch();
	sw->setName("upArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	button->postMult(Matrix::translate(Vec3(0, 60, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	button->postMult(Matrix::translate(Vec3(0, 60, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// fl�che de gauche
	sw = new Switch();
	sw->setName("leftArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	button->postMult(Matrix::translate(Vec3(-60, 0, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	button->postMult(Matrix::translate(Vec3(-60, 0, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// fl�che du bas
	sw = new Switch();
	sw->setName("downArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	button->postMult(Matrix::translate(Vec3(0, -60, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	button->postMult(Matrix::translate(Vec3(0, -60, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// fl�che de droite
	sw = new Switch();
	sw->setName("rightArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());;
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::translate(Vec3(60, 0, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());;
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	button->postMult(Matrix::translate(Vec3(60, 0, 0)));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// dessin du bouton de changement de sens
	sw = new Switch();
	sw->setName("directionButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(0.75, 0.75, 1));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/direction_no.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(0.75, 0.75, 1));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/direction_no_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());
}

void GameView::drawGameButtons()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;
	StateSet *stateset;
	ref_ptr<MatrixTransform> button;
	ref_ptr<Switch> sw;

	// dessin du bouton de changement de mode
	sw = new Switch();
	sw->setName("modeButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, 130, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/fence.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, 130, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/fence_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton de validation de la barri�re virtuelle
	sw = new Switch();
	sw->setName("validateButton");
	
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, 130, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/validate_no.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, 130, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/validate_no_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour recommencer une partie
	sw = new Switch();
	sw->setName("restartButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -240, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/restart.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -240, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/restart_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour annuler le dernier coups
	sw = new Switch();
	sw->setName("undoButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -240, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/undo.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -240, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/undo_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());
}

void GameView::drawSaveAndLoadButtons()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;
	StateSet *stateset;
	ref_ptr<MatrixTransform> button;
	ref_ptr<Switch> sw;

	// dessin du bouton de sauvegarde d'une partie
	sw = new Switch();
	sw->setName("saveButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -325, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/save.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -325, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/save_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton de chargement d'une partie
	sw = new Switch();
	sw->setName("loadButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -325, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/load.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -325, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/load_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());
}

void GameView::drawViewButtons()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;
	StateSet *stateset;
	ref_ptr<MatrixTransform> button;
	ref_ptr<Switch> sw;

	// dessin du bouton pour changer de style de vue
	sw = new Switch();
	sw->setName("viewButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -115, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/perspective.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -115, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/perspective_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour zoomer en avant
	sw = new Switch();
	sw->setName("zoomInButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -60, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_in.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -60, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_in_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour zoomer en arri�re
	sw = new Switch();
	sw->setName("zoomOutButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -150, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_out.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -150, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_out_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour tourner vers la gauche
	sw = new Switch();
	sw->setName("turnLeftButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, 40, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/turn_left.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, 40, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/turn_left_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour tourner vers la droite
	sw = new Switch();
	sw->setName("turnRightButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, 40, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/turn_right.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, 40, 0)));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/turn_right_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());
}

string GameView::obtenirNomBoutonCollision(Point position)
{
	// REMARQUE: getChildIndex renvoie le nombre d'enfants si l'enfant pass� en param�tre
	//           n'a pas �t� trouv� donc le 0 permet de dire renvoie d'office le nombre
	//           d'enfants contenu dans la cam�ra des boutons d'actions
	for (unsigned int i = 0; i < cameraActionsArea->getChildIndex(0); i++)
	{
		ref_ptr<Node> n = cameraActionsArea->getChild(i);

		BoundingSphere bs = n->getBound(); //TODO probl�me sur les fl�ches de direction si l'on clique trop pr�t du changement de direction

		// tester si position est dans le noeud
		if (!bs.contains(Vec3(position.getX(), position.getY(), 0)))
			continue;

		// v�rifier sur quel bouton...
		if (n->getName() != "") // si simple bouton
			return n->getName();
		else { // si container
			ref_ptr<MatrixTransform> mt = dynamic_cast<MatrixTransform *>(n.get());

			for (unsigned int j = 0; j < mt->getChildIndex(0); j++)
			{
				ref_ptr<Node> n_mt = mt->getChild(j);
				BoundingSphere bs_mt = n_mt->getBound();

				Vec3 c = bs.center();

				if (!bs_mt.contains(Vec3(position.getX() - c.x(), position.getY() - c.y(), 0)))
					continue;

				return n_mt->getName();
			}
		}
	}

	return "";
}

Config::Button GameView::testerCollisionAvecBouton(Point position)
{
	// TODO: utiliser la m�thode obtenir nom bouton collision pour �viter une r�p�tition de code
	Config::Button result = Config::Button::UNKNOWN;

	// REMARQUE: getChildIndex renvoie le nombre d'enfants si l'enfant pass� en param�tre
	//           n'a pas �t� trouv� donc le 0 permet de dire renvoie d'office le nombre
	//           d'enfants contenu dans la cam�ra des boutons d'actions
	for (unsigned int i = 0; i < cameraActionsArea->getChildIndex(0); i++)
	{
		ref_ptr<Node> n = cameraActionsArea->getChild(i);
		BoundingSphere bs = n->getBound(); //TODO probl�me sur les fl�ches de direction si l'on clique trop pr�t du changement de direction

		// tester si position est dans le noeud
		if (!bs.contains(Vec3(position.getX(), position.getY(), 0)))
			continue;

		// v�rifier sur quel bouton...
		if (n->getName() != "") // si simple bouton
			result = checkButton(n->getName());
		else { // si container
			ref_ptr<MatrixTransform> mt = dynamic_cast<MatrixTransform *>(n.get());

			for (unsigned int j = 0; j < mt->getChildIndex(0); j++)
			{
				ref_ptr<Node> n_mt = mt->getChild(j);
				BoundingSphere bs_mt = n_mt->getBound();

				Vec3 c = bs.center();

				if (!bs_mt.contains(Vec3(position.getX() - c.x(), position.getY() - c.y(), 0)))
					continue;

				result = checkButton(n_mt->getName());
			}
		}

		if (result != Config::Button::UNKNOWN) // si bouton trouv�, arr�ter de chercher
			break;
	}

	return result;
}

Config::Button GameView::checkButton(std::string name)
{
	if (name == "restartButton") // bouton red�marrer
		return Config::Button::RESTART;
	else if (name == "saveButton") // bouton enregistrer une partie
		return Config::Button::SAVE;
	else if (name == "loadButton") // bouton charger une partie
		return Config::Button::LOAD;
	else if (name == "validateButton") // bouton valider le placement d'une barri�re
		return Config::Button::VALIDATE;
	else if (name == "modeButton") // bouton changer de mode
		return Config::Button::MODE;
	else if (name == "upArrowButton") // bouton fl�che vers le haut
		return Config::Button::ARROW_UP;
	else if (name == "leftArrowButton") // bouton fl�che vers la gauche
		return Config::Button::ARROW_LEFT;
	else if (name == "downArrowButton") // bouton fl�che vers le bas
		return Config::Button::ARROW_DOWN;
	else if (name == "rightArrowButton") // bouton fl�che vers la droite
		return Config::Button::ARROW_RIGHT;
	else if (name == "directionButton") // bouton changement de direction
		return Config::Button::DIRECTION;
	else if (name == "undoButton") // bouton annuler le dernier coup
		return Config::Button::UNDO;
	else if (name == "viewButton") // bouton changer de vue
		return Config::Button::VIEW;
	else if (name == "zoomInButton") // bouton zoomer en avant
		return Config::Button::ZOOM_IN;
	else if (name == "zoomOutButton") // bouton zoomer en arri�re
		return Config::Button::ZOOM_OUT;
	else if (name == "turnLeftButton") // bouton tourner vers la gauche
		return Config::Button::TURN_LEFT;
	else if (name == "turnRightButton") // bouton tourner vers la droite
		return Config::Button::TURN_RIGHT;

	return Config::Button::UNKNOWN; // bouton inconnu
}

void GameView::changeTexture(ref_ptr<Switch> sw, string textureName)
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;
	StateSet *stateset;

	for (unsigned int i = 0; i < sw->getChildIndex(0); i++) {
		if (sw->getValue(i)) { // si est un �tat normal
			texture = new Texture2D();
			img = osgDB::readImageFile("resources/textures/" + textureName +  ".png");
			texture->setImage(img);

			stateset = sw->getChild(i)->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
		} else { // si est l'�tat �clair�
			texture = new Texture2D();
			img = osgDB::readImageFile("resources/textures/" + textureName +  "_hover.png");
			texture->setImage(img);

			stateset = sw->getChild(i)->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
		}
	}
}

void GameView::refreshButtons()
{
	for (unsigned int i = 0; i < cameraActionsArea->getChildIndex(0); i++) {
		ref_ptr<Node> n = cameraActionsArea->getChild(i);

		// si container
		if (n->getName() == "") {
			ref_ptr<MatrixTransform> mt = dynamic_cast<MatrixTransform *>(n.get());

			if (mt != 0) {
				for (unsigned int j = 0; j < mt->getChildIndex(0); j++)
				{
					ref_ptr<Node> nn = mt->getChild(j);
					
					if (nn->getName() == "directionButton") {
						n = nn;
						break;
					}
				}
			}
		}

		ref_ptr<Switch> sw = n->asSwitch();

		if (sw == 0)
			continue;

		if (sw->getName() == "viewButton") {
			if (Model::getInstance()->getView() == Model::View::PARALLELE)
				changeTexture(sw, "perspective");
			else
				changeTexture(sw, "parallel");
		}

		if (Model::getInstance()->getMode() == Model::Mode::PIONS) {
			if (sw->getName() == "validateButton")
				changeTexture(sw, "validate_no");
			else if (sw->getName() == "modeButton")
				changeTexture(sw, "fence");
			else if (sw->getName() == "directionButton")
				changeTexture(sw, "direction_no");
		} else {
			if (sw->getName() == "validateButton")
				changeTexture(sw, "validate");
			else if (sw->getName() == "modeButton")
				changeTexture(sw, "pawn");
			else if (sw->getName() == "directionButton")
				changeTexture(sw, "direction");
		}
	}
}

void GameView::refreshView()
{
	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // si vue parall�le
		cameraGameArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
		cameraGameArea->setProjectionMatrixAsOrtho(CAMERA_LEFT - offset, CAMERA_RIGHT + offset, CAMERA_PLATEAU_BOTTOM - offset, CAMERA_PLATEAU_TOP + offset, CAMERA_NEAR, CAMERA_FAR);
	} else { // si vue perspective
		cameraGameArea->setViewMatrixAsLookAt(eyePerspective, center, up);
		cameraGameArea->setProjectionMatrixAsPerspective(70, 1, CAMERA_NEAR, CAMERA_FAR);
	}
}

void GameView::zoomIn()
{
	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // mode parall�le
		if (offset > -LIMIT_ORTHO)
			offset -= 5;
	} else { // mode perspective
		if (eyePerspective.y() < LIMIT_PERSPECTIVE_MIN.y() && eyePerspective.z() > LIMIT_PERSPECTIVE_MIN.z())
			eyePerspective = Vec3(eyePerspective.x(), eyePerspective.y() + 5, eyePerspective.z() - 5);
	}
}

void GameView::zoomOut()
{
	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // mode parall�le
		if (offset < LIMIT_ORTHO)
			offset += 5;
	} else { // mode perspective
		if (eyePerspective.y() > LIMIT_PERSPECTIVE_MAX.y() && eyePerspective.z() < LIMIT_PERSPECTIVE_MAX.z())
			eyePerspective = Vec3(eyePerspective.x(), eyePerspective.y() - 5, eyePerspective.z() + 5);
	}
}

void GameView::turnLeft()
{
	cout << "Turn Left" << endl;

	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // mode parall�le
		//TODO: __
	} else { // mode perspective
		//TODO: __
	}
}

void GameView::turnRight()
{
	cout << "Turn Right" << endl;

	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // mode parall�le
		//TODO: __
		//eyeOrtho = Vec3(eyeOrtho.x() + cos(PI), eyeOrtho.y() + cos(PI), eyeOrtho.z());
		//eyeOrtho = Vec3(50, 50, 0);
	} else { // mode perspective
		//TODO: __
	}
}

ref_ptr<osgViewer::Viewer> GameView::buildSceneGraph()
{
	// cr�ation et configuration du viewer
	createAndConfigureViewer();

	// cr�ation et configuration de la cam�ra de la zone de jeu
	createAndConfigureCameraGameArea();

	// cr�ation et configuration de la cam�ra d'affichage des messages
	createAndConfigureCameraDisplayArea();

	// cr�ation et configuration de la cam�ra affichant les diff�rents boutons graphiques
	createAndConfigureCameraActionsArea();

	// dessin du plateau
	drawPlate();

	// dessin des cases
	drawCases();

	// dessin des barri�res
	drawFences();

	// dessin de la barri�re virtuelle
	drawVirtualFence();

	// dessin des pions
	drawPawns();

	// configuration de l'affichage des messages
	drawMessage();

	// cr�er un bouton
	createClassicButton();

	// dessiner les boutons fl�ch�s et de changements de sens
	drawArrowAndDirectionButtons();

	// dessin des diff�rents boutons de commandes
	drawGameButtons();
	drawSaveAndLoadButtons();
	drawViewButtons();

	// initialisation du controller
	Controller::getInstance()->initialize(cameraGameArea.get());

	// cr�ation d'un noeud racine de type group contenant les cam�ras
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
