#include "GameView.h"

using namespace std;
using namespace osg;

GameView *GameView::instance = 0;

//TODO: bug en vue perspective sur la barrière virtuelle qui a deux couleurs ? (possible que ca soit une ombre)
//TODO: bug bouton direction (le bouton changer se sens "déborde" sur les autres
//      dans la détection de collision)
//TODO: bug détection collision lors de la rotation de la matrice contenant les flèches

GameView::GameView()
{
	// configuration des caméras
	center         = POSITION_CENTRE;
	eye            = Vec3(0, 0, 1);
	up             = Vec3(0, 1, 0);
	eyeOrtho       = EYE_ORTHO;
	eyePerspective = EYE_PERSPECTIVE;

	offset         = 0;

	// initialisation de la rotation
	rotate = new MatrixTransform();
	currentAngle = 0;
}

GameView *GameView::getInstance()
{
	if (instance == 0)
		instance = new GameView();

	return instance;
}

void GameView::initMessage()
{
	// création de la police d'écriture
	ref_ptr<osgText::Font> font = osgText::readFontFile("resources/fonts/messages.ttf");

	// initialisation du message utilisateur
	userMessage = new osgText::Text();
	userMessage->setFont(font);
	userMessage->setCharacterSize(FONT_SIZE);
	userMessage->setPosition(POS_MESSAGE_USER);
	userMessage->setText(Model::getInstance()->getUserMessage());
	userMessage->setColor(COLOR_MESSAGE);

	// initialisation du message d'erreur
	errorMessage = new osgText::Text();
	errorMessage->setFont(font);
	errorMessage->setCharacterSize(FONT_SIZE);
	errorMessage->setPosition(POS_MESSAGE_ERROR);
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

	// changer le titre de la fenêtre
	osgViewer::Viewer::Windows windows; // variable servant à contenir la liste des différentes fenêtre créée par OSG
										// sous forme d'un vecteur. Avec la configuration par défaut, OSG crée autant
										// de fenêtre en plein écran que d'écran branché à l'ordinateur exécutant le
										// programme.
	viewer->getWindows(windows); // obtenir la liste des différentes fenêtre d'OSG

	for (unsigned int i = 0; i < windows.size(); i++) // définir un titre pour chacune des fenêtres
		windows[i]->setWindowName("Quoridor - 3BIN - IPL");
}

void GameView::createAndConfigureCameraGameArea()
{
	ref_ptr<Light> light;
	ref_ptr<LightSource> source;
	StateSet *stateset;

	cameraGameArea = new Camera();

	// configuration générale
	cameraGameArea->setViewport(new Viewport(ESPACEMENT, ESPACEMENT + MESSAGE_HEIGHT + ESPACEMENT, PLATEAU_TAILLE, PLATEAU_TAILLE));
	cameraGameArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraGameArea->setClearColor(BLACK);

	// vue parallèle (par défaut)
	cameraGameArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraGameArea->setProjectionMatrixAsOrtho(CAMERA_LEFT, CAMERA_RIGHT, CAMERA_PLATEAU_BOTTOM, CAMERA_PLATEAU_TOP, CAMERA_NEAR, CAMERA_FAR);

	// création de la lumière positionnelle à lumière diffuse
	light = new Light;
	light->setLightNum(0);
	light->setDiffuse(Vec4(1, 1, 1, 0));
	light->setPosition(POS_ECLAIRAGE);

	light->setConstantAttenuation(0.2);
	light->setLinearAttenuation(0.01);

	source = new LightSource();
	source->setLight(light.get());
	cameraGameArea->addChild(source.get());

	stateset = cameraGameArea->getOrCreateStateSet();

	stateset->setMode(GL_LIGHT0, StateAttribute::Values::ON);

	// création de la lumière directionnelle à lumière ambiante
	light = new Light;
	light->setLightNum(1);
	light->setAmbient(Vec4(1, 1, 1, 0));
	light->setPosition(Vec4(0, 0, 0, 0));

	source = new LightSource();
	source->setLight(light.get());
	cameraGameArea->addChild(source.get());

	stateset = cameraGameArea->getOrCreateStateSet();

	stateset->setMode(GL_LIGHT1, StateAttribute::Values::ON);
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
	button->setShape(new Box(POSITION_CENTRE, TAILLE_BOUTON, TAILLE_BOUTON, 1));
	button->setColor(WHITE);

	classicButton = new Geode();

	StateSet *stateset = classicButton->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::Values::OFF);

	classicButton->addDrawable(button.get());
}

void GameView::drawPlate()
{
	// texture
	ref_ptr<Texture2D> texture = new Texture2D();
	ref_ptr<Image> img = osgDB::readImageFile("resources/textures/plate.jpg");
	bool textureFound = img != 0;
	texture->setImage(img);

	// forme
	ref_ptr<ShapeDrawable> plateau = new ShapeDrawable();
	plateau->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTaillePlateau(), Config::getInstance()->getTaillePlateau(), HAUTEUR_PLATEAU));
	if (!textureFound) plateau->setColor(COLOR_PLATEAU);

	ref_ptr<Geode> geodePlateau = new Geode();

	StateSet *stateset = geodePlateau->getOrCreateStateSet();
	if (textureFound) stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	geodePlateau->addDrawable(plateau.get());
	rotate->addChild(geodePlateau.get());

	
}

void GameView::drawCases()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;

	// création de la texture d'une case
	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/case.jpg");
	texture->setImage(img);

	// création de l'apparence d'une case
	ref_ptr<ShapeDrawable> casePlateau = new ShapeDrawable();
	casePlateau->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleCase(), Config::getInstance()->getTailleCase(), HAUTEUR_CASE));
	casePlateau->setColor(COLOR_CASE);
	
	// création de la géode avec ajout de la texture
	ref_ptr<Geode> geodeCase = new Geode();
	
	StateSet *stateset = geodeCase->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	geodeCase->addDrawable(casePlateau.get());

	// création et positionnement de toutes les cases
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
			rotate->addChild(caseTransform.get());
		}

		py -= (Config::getInstance()->getTailleCase() + Config::getInstance()->getTailleRainure());
		px = pxInit;
	}
}

void GameView::drawFences()
{
	// barrière avec l'apparence de la zone de stockage
	ref_ptr<ShapeDrawable> barriereDock = new ShapeDrawable();
	barriereDock->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleBarriere(), Config::getInstance()->getTailleRainure(), HAUTEUR_BARRIERE));
	barriereDock->setColor(COLOR_BARRIERE);

	ref_ptr<Geode> geodeBarriereDock = new Geode();
	geodeBarriereDock->setName("dock");
	geodeBarriereDock->addDrawable(barriereDock.get());

	// barrière avec l'apparence dans le jeu
	ref_ptr<ShapeDrawable> barriereGame = new ShapeDrawable();
	barriereGame->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleBarriere(), Config::getInstance()->getTailleRainure(), HAUTEUR_BARRIERE));
	barriereGame->setColor(BLUE);

	ref_ptr<Geode> geodeBarriereGame = new Geode();
	geodeBarriereGame->setName("game");
	geodeBarriereGame->addDrawable(barriereGame.get());

	// ajouter les barrières au controller
	for (int joueur = 1; joueur <= 2; joueur++)
	{
		for (int i = 0; i < Config::getInstance()->getNbrBarriereJoueur(); i++)
		{
			// création du switch
			ref_ptr<Switch> sw = new Switch();

			sw->addChild(geodeBarriereDock.get(), true);
			sw->addChild(geodeBarriereGame.get(), false);

			ref_ptr<MatrixTransform> mt = new MatrixTransform();
			mt->addChild(sw.get());

			Controller::getInstance()->setBarriere(Point(joueur, i), mt.get());
		}
	}
}

void GameView::drawVirtualFence()
{
	ref_ptr<ShapeDrawable> virtualFence = new ShapeDrawable();
	virtualFence->setShape(new Box(POSITION_CENTRE, Config::getInstance()->getTailleBarriere() + 0.1, Config::getInstance()->getTailleRainure() + 0.1, HAUTEUR_BARRIERE + 0.1));
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
	ref_ptr<MatrixTransform> mt1 = new MatrixTransform();
	mt1->addChild(createPyramidShape(Config::getInstance()->getTaillePion(), 20, RED));

	ref_ptr<MatrixTransform> mt2 = new MatrixTransform();
	mt2->addChild(createPyramidShape(Config::getInstance()->getTaillePion(), 20, BLUE));

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
	arrows = new MatrixTransform();
	arrows->setMatrix(Matrix::identity());
	arrows->postMult(Matrix::translate(POS_CONT_FLECHES));

	cameraActionsArea->addChild(arrows.get());

	// création de la texture d'une flèche
	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/arrow.png");
	texture->setImage(img);

	textureHover = new Texture2D();
	img = osgDB::readImageFile("resources/textures/arrow_hover.png");
	textureHover->setImage(img);

	// flèche du haut
	sw = new Switch();
	sw->setName("upArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	button->postMult(Matrix::translate(POS_FLECHE_HAUT));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	button->postMult(Matrix::translate(POS_FLECHE_HAUT));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// flèche de gauche
	sw = new Switch();
	sw->setName("leftArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	button->postMult(Matrix::translate(POS_FLECHE_GAUCHE));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	button->postMult(Matrix::translate(POS_FLECHE_GAUCHE));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// flèche du bas
	sw = new Switch();
	sw->setName("downArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	button->postMult(Matrix::translate(POS_FLECHE_BAS));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	button->postMult(Matrix::translate(POS_FLECHE_BAS));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, textureHover, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	arrows->addChild(sw.get());

	// flèche de droite
	sw = new Switch();
	sw->setName("rightArrowButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());;
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::translate(POS_FLECHE_DROITE));

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());;
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));
	button->postMult(Matrix::translate(POS_FLECHE_DROITE));

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
	button->postMult(Matrix::scale(TAILLE_FLECHES));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/direction_no.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::scale(TAILLE_FLECHES));

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
	button->postMult(Matrix::translate(POS_CHANGE_MODE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/fence.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_CHANGE_MODE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/fence_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton de validation de la barrière virtuelle
	sw = new Switch();
	sw->setName("validateButton");
	
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_VALIDE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/validate_no.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_VALIDE));

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
	button->postMult(Matrix::translate(POS_RECOMMENCER));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/restart.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_RECOMMENCER));

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
	button->postMult(Matrix::translate(POS_ANNULATION));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/undo.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_ANNULATION));

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
	button->postMult(Matrix::translate(POS_SAUVEGARDE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/save.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_SAUVEGARDE));

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
	button->postMult(Matrix::translate(POS_CHARGEMENT));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/load.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_CHARGEMENT));

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
	button->postMult(Matrix::translate(POS_CHANGE_VUE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/perspective.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_CHANGE_VUE));

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
	button->postMult(Matrix::translate(POS_ZOOM));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_in.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_ZOOM));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_in_hover.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), false);

	cameraActionsArea->addChild(sw.get());

	// dessin du bouton pour zoomer en arrière
	sw = new Switch();
	sw->setName("zoomOutButton");

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_DEZOOM));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/zoom_out.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_DEZOOM));

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
	button->postMult(Matrix::translate(POS_ROTATION_GAUCHE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/turn_left.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_ROTATION_GAUCHE));

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
	button->postMult(Matrix::translate(POS_ROTATION_DROITE));

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/turn_right.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	sw->addChild(button.get(), true);

	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(POS_ROTATION_DROITE));

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
	// REMARQUE: getChildIndex renvoie le nombre d'enfants si l'enfant passé en paramètre
	//           n'a pas été trouvé donc le 0 permet de dire renvoie d'office le nombre
	//           d'enfants contenu dans la caméra des boutons d'actions
	for (unsigned int i = 0; i < cameraActionsArea->getChildIndex(0); i++)
	{
		ref_ptr<Node> n = cameraActionsArea->getChild(i);

		BoundingSphere bs = n->getBound(); //TODO problème sur les flèches de direction si l'on clique trop prêt du changement de direction

		// tester si position est dans le noeud
		if (!bs.contains(Vec3(position.getX(), position.getY(), 0)))
			continue;

		// vérifier sur quel bouton...
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
	return checkButton(obtenirNomBoutonCollision(position));
}

Config::Button GameView::checkButton(std::string name)
{
	if (name == "restartButton") // bouton redémarrer
		return Config::Button::RESTART;
	else if (name == "saveButton") // bouton enregistrer une partie
		return Config::Button::SAVE;
	else if (name == "loadButton") // bouton charger une partie
		return Config::Button::LOAD;
	else if (name == "validateButton") // bouton valider le placement d'une barrière
		return Config::Button::VALIDATE;
	else if (name == "modeButton") // bouton changer de mode
		return Config::Button::MODE;
	else if (name == "upArrowButton") // bouton flèche vers le haut
		return Config::Button::ARROW_UP;
	else if (name == "leftArrowButton") // bouton flèche vers la gauche
		return Config::Button::ARROW_LEFT;
	else if (name == "downArrowButton") // bouton flèche vers le bas
		return Config::Button::ARROW_DOWN;
	else if (name == "rightArrowButton") // bouton flèche vers la droite
		return Config::Button::ARROW_RIGHT;
	else if (name == "directionButton") // bouton changement de direction
		return Config::Button::DIRECTION;
	else if (name == "undoButton") // bouton annuler le dernier coup
		return Config::Button::UNDO;
	else if (name == "viewButton") // bouton changer de vue
		return Config::Button::VIEW;
	else if (name == "zoomInButton") // bouton zoomer en avant
		return Config::Button::ZOOM_IN;
	else if (name == "zoomOutButton") // bouton zoomer en arrière
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
		if (sw->getValue(i)) { // si est un état normal
			texture = new Texture2D();
			img = osgDB::readImageFile("resources/textures/" + textureName +  ".png");
			texture->setImage(img);

			stateset = sw->getChild(i)->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
		} else { // si est l'état éclairé
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
	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // si vue parallèle
		cameraGameArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
		cameraGameArea->setProjectionMatrixAsOrtho(CAMERA_LEFT - offset, CAMERA_RIGHT + offset, CAMERA_PLATEAU_BOTTOM - offset, CAMERA_PLATEAU_TOP + offset, CAMERA_NEAR, CAMERA_FAR);
	} else { // si vue perspective
		cameraGameArea->setViewMatrixAsLookAt(eyePerspective, center, up);
		cameraGameArea->setProjectionMatrixAsPerspective(70, 1, CAMERA_NEAR, CAMERA_FAR);
	}
}

void GameView::zoomIn()
{
	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // mode parallèle
		if (offset > -LIMIT_ORTHO)
			offset -= 5;
	} else { // mode perspective
		if (eyePerspective.y() < LIMIT_PERSPECTIVE_MIN.y() && eyePerspective.z() > LIMIT_PERSPECTIVE_MIN.z())
			eyePerspective = Vec3(eyePerspective.x(), eyePerspective.y() + 5, eyePerspective.z() - 5);
	}
}

void GameView::zoomOut()
{
	if (Model::getInstance()->getView() == Model::View::PARALLELE) { // mode parallèle
		if (offset < LIMIT_ORTHO)
			offset += 5;
	} else { // mode perspective
		if (eyePerspective.y() > LIMIT_PERSPECTIVE_MAX.y() && eyePerspective.z() < LIMIT_PERSPECTIVE_MAX.z())
			eyePerspective = Vec3(eyePerspective.x(), eyePerspective.y() - 5, eyePerspective.z() + 5);
	}
}

void GameView::turnLeft()
{
	currentAngle = (currentAngle + 5) % 360;

	rotate->setMatrix(Matrix::identity());
	rotate->postMult(Matrix::rotate(inDegrees((double)currentAngle), Z_AXIS));

	arrows->setMatrix(Matrix::identity());
	arrows->postMult(Matrix::rotate(inDegrees((double)currentAngle), Z_AXIS));
	arrows->postMult(Matrix::translate(POS_CONT_FLECHES));
}

void GameView::turnRight()
{
	currentAngle -= 5;

	if (currentAngle < 0)
		currentAngle += 360;

	rotate->setMatrix(Matrix::identity());
	rotate->postMult(Matrix::rotate(inDegrees((double)currentAngle), Z_AXIS));

	arrows->setMatrix(Matrix::identity());
	arrows->postMult(Matrix::rotate(inDegrees((double)currentAngle), Z_AXIS));
	arrows->postMult(Matrix::translate(POS_CONT_FLECHES));
}

ref_ptr<Geode> GameView::createPyramidShape(float size, float height, Vec4 color)
{
	ref_ptr<Geometry> pyramideGeometry = new Geometry();
	
	// création des vertices
	ref_ptr<Vec3Array> vertices = new Vec3Array();
	vertices->push_back(Vec3(-size, -size,      0));
	vertices->push_back(Vec3(-size,  size,      0));
	vertices->push_back(Vec3( size,  size,      0));
	vertices->push_back(Vec3( size, -size,      0));
	vertices->push_back(Vec3(   0 ,     0, height));

	pyramideGeometry->setVertexArray(vertices.get());

	// définition des différentes faces
	ref_ptr<DrawElementsUInt> base = new DrawElementsUInt(PrimitiveSet::Mode::QUADS, 0);
	base->push_back(0);
	base->push_back(1);
	base->push_back(2);
	base->push_back(3);
	pyramideGeometry->addPrimitiveSet(base.get());

	ref_ptr<DrawElementsUInt> f1 = new DrawElementsUInt(PrimitiveSet::Mode::TRIANGLES, 0);
	f1->push_back(4);
	f1->push_back(2);
	f1->push_back(1);
	pyramideGeometry->addPrimitiveSet(f1.get());

	ref_ptr<DrawElementsUInt> f2 = new DrawElementsUInt(PrimitiveSet::Mode::TRIANGLES, 0);
	f2->push_back(4);
	f2->push_back(3);
	f2->push_back(2);
	pyramideGeometry->addPrimitiveSet(f2.get());

	ref_ptr<DrawElementsUInt> f3 = new DrawElementsUInt(PrimitiveSet::Mode::TRIANGLES, 0);
	f3->push_back(4);
	f3->push_back(0);
	f3->push_back(3);
	pyramideGeometry->addPrimitiveSet(f3.get());

	ref_ptr<DrawElementsUInt> f4 = new DrawElementsUInt(PrimitiveSet::Mode::TRIANGLES, 0);
	f4->push_back(4);
	f4->push_back(1);
	f4->push_back(0);
	pyramideGeometry->addPrimitiveSet(f4.get());

	// création des couleurs
	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(color);
	colors->push_back(color);
	colors->push_back(color);
	colors->push_back(color);
	colors->push_back(color);

	pyramideGeometry->setColorArray(colors.get());
	pyramideGeometry->setColorBinding(Geometry::AttributeBinding::BIND_PER_VERTEX);

	// création du vecteur de normale
	ref_ptr<Vec3Array> normalArray = new Vec3Array;
	normalArray->push_back(Vec3(0,0,1));

	pyramideGeometry->setNormalArray(normalArray.get());
	pyramideGeometry->setNormalBinding(Geometry::AttributeBinding::BIND_PER_VERTEX);

	// création de la geode
	ref_ptr<Geode> pyramideGeode = new Geode();

	pyramideGeode->addDrawable(pyramideGeometry.get());

	return pyramideGeode.get();
}

ref_ptr<osgViewer::Viewer> GameView::buildSceneGraph()
{
	// création et configuration du viewer
	createAndConfigureViewer();

	// création et configuration de la caméra de la zone de jeu
	createAndConfigureCameraGameArea();

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

	// dessin des pions
	drawPawns();

	// configuration de l'affichage des messages
	drawMessage();

	// créer un bouton
	createClassicButton();

	// dessiner les boutons flèchés et de changements de sens
	drawArrowAndDirectionButtons();

	// dessin des différents boutons de commandes
	drawGameButtons();
	drawSaveAndLoadButtons();
	drawViewButtons();

	// initialisation du controller
	Controller::getInstance()->initialize(rotate.get());

	// ajout de la matrice de transformation (servant à la rotation) à la caméra du jeu
	cameraGameArea->addChild(rotate.get());

	// création d'un noeud racine de type group contenant les caméras
	ref_ptr<Group> root = new Group();

	root->addChild(cameraGameArea.get());
	root->addChild(cameraDisplayArea.get());
	root->addChild(cameraActionsArea.get());

	// ajouter les callbacks
	root->setEventCallback(new EventController());

	// ajouter le noeud racine au viewer
	viewer->setSceneData(root.get());

	return viewer.get();
}
