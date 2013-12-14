#include "GameView.h"

using namespace std;
using namespace osg;

GameView *GameView::instance = 0;

//TODO: bug annuler juste après un saut
//TODO: bug en vue perspective sur la barrière virtuelle qui a deux couleurs

GameView::GameView()
{
	// configuration des caméras
	center         = POSITION_CENTRE;
	up             = Vec3(0, 1, 0);
	eyeOrtho       = Vec3(0, 0, 1);
	offset         = 0;
	eyePerspective = Vec3(0, -130, 150);
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
	//ref_ptr<Texture2D> texture = new Texture2D();
	//ref_ptr<Image> img = osgDB::readImageFile("resources/textures/background.jpg");
	//bool textureFound = img != 0;
	//texture->setImage(img);

	viewer = new osgViewer::Viewer();

	viewer->setUpViewInWindow(FENETRE_X, FENETRE_Y, FENETRE_WIDTH, FENETRE_HEIGHT);
	viewer->getCamera()->setAllowEventFocus(false);
	viewer->getCamera()->setClearColor(COLOR_BORDER);

	//StateSet *stateset = viewer->getCamera()->getOrCreateStateSet();
	//stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

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
	cameraGameArea = new Camera();

	// vue parallèle (par défaut)
	cameraGameArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraGameArea->setProjectionMatrixAsOrtho(CAMERA_LEFT, CAMERA_RIGHT, CAMERA_PLATEAU_BOTTOM, CAMERA_PLATEAU_TOP, CAMERA_NEAR, CAMERA_FAR);
	cameraGameArea->setViewport(new Viewport(ESPACEMENT, ESPACEMENT + MESSAGE_HEIGHT + ESPACEMENT, PLATEAU_TAILLE, PLATEAU_TAILLE));
	cameraGameArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraGameArea->setClearColor(BLACK);
}

void GameView::createAndConfigureCameraDisplayArea()
{
	cameraDisplayArea = new Camera();

	cameraDisplayArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
	cameraDisplayArea->setProjectionMatrixAsOrtho(CAMERA_LEFT, CAMERA_RIGHT, CAMERA_MESSAGE_BOTTOM, CAMERA_MESSAGE_TOP, CAMERA_NEAR, CAMERA_FAR);
	cameraDisplayArea->setViewport(new Viewport(ESPACEMENT, ESPACEMENT, FENETRE_WIDTH - ESPACEMENT - ESPACEMENT, MESSAGE_HEIGHT));
	cameraDisplayArea->setReferenceFrame(Camera::ABSOLUTE_RF);
	cameraDisplayArea->setClearColor(BLACK);
}

void GameView::createAndConfigureCameraActionsArea()
{
	cameraActionsArea = new Camera();

	cameraActionsArea->setViewMatrixAsLookAt(eyeOrtho, center, up);
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
	ref_ptr<Image> img;
	StateSet *stateset;

	// création de la texture d'une flèche
	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/arrow.png");
	texture->setImage(img);

	// flèche du haut
	ref_ptr<MatrixTransform> arrowUp = new MatrixTransform();
	arrowUp->addChild(classicButton.get());
	arrowUp->setMatrix(Matrix::identity());
	arrowUp->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	arrowUp->postMult(Matrix::rotate(inDegrees(90.0), Z_AXIS));
	arrowUp->postMult(Matrix::translate(Vec3(0, 60, 0)));
	arrowUp->setName("upArrowButton");

	stateset = arrowUp->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	// flèche de gauche
	ref_ptr<MatrixTransform> arrowLeft = new MatrixTransform();
	arrowLeft->addChild(classicButton.get());
	arrowLeft->setMatrix(Matrix::identity());
	arrowLeft->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	arrowLeft->postMult(Matrix::rotate(inDegrees(180.0), Z_AXIS));
	arrowLeft->postMult(Matrix::translate(Vec3(-60, 0, 0)));
	arrowLeft->setName("leftArrowButton");

	stateset = arrowLeft->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	// flèche du bas
	ref_ptr<MatrixTransform> arrowDown = new MatrixTransform();
	arrowDown->addChild(classicButton.get());
	arrowDown->setMatrix(Matrix::identity());
	arrowDown->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	arrowDown->postMult(Matrix::rotate(inDegrees(-90.0), Z_AXIS));
	arrowDown->postMult(Matrix::translate(Vec3(0, -60, 0)));
	arrowDown->setName("downArrowButton");

	stateset = arrowDown->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	// flèche de droite
	ref_ptr<MatrixTransform> arrowRight = new MatrixTransform();
	arrowRight->addChild(classicButton.get());;
	arrowRight->setMatrix(Matrix::identity());
	arrowRight->postMult(Matrix::scale(Vec3(0.75, 0.75, 1)));
	arrowRight->postMult(Matrix::translate(Vec3(60, 0, 0)));
	arrowRight->setName("rightArrowButton");

	stateset = arrowRight->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	// dessin du bouton de changement de sens
	ref_ptr<MatrixTransform> direction = new MatrixTransform();
	direction->addChild(classicButton.get());
	direction->setMatrix(Matrix::identity());
	direction->postMult(Matrix::scale(0.75, 0.75, 1));
	direction->setName("directionButton");

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/direction_no.png");
	texture->setImage(img);

	stateset = direction->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	// matrixTransform contenant les touches de directions et de changements de direction
	ref_ptr<MatrixTransform> arrows = new MatrixTransform();

	arrows->addChild(arrowUp.get());
	arrows->addChild(arrowLeft.get());
	arrows->addChild(arrowDown.get());
	arrows->addChild(arrowRight.get());
	arrows->addChild(direction.get());

	arrows->setMatrix(Matrix::identity());
	arrows->postMult(Matrix::translate(0, 270, 0));

	cameraActionsArea->addChild(arrows.get());
}

void GameView::drawCommandsButtons()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;
	StateSet *stateset;
	ref_ptr<MatrixTransform> button;

	// dessin du bouton de changement de mode
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, 130, 0)));
	button->setName("modeButton");

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/fence.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton de validation de la barrière virtuelle
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, 130, 0)));
	button->setName("validateButton");

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/validate_no.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton de sauvegarde d'une partie
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -325, 0)));
	button->setName("saveButton");

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/save.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton de chargement d'une partie
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -325, 0)));
	button->setName("loadButton");

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/load.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton pour recommencer une partie
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(0, -240, 0)));
	button->setName("restartButton");

	texture = new Texture2D();
	img = osgDB::readImageFile("resources/textures/restart.png");
	texture->setImage(img);

	stateset = button->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton pour changer de style de vue
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(-50, -115, 0)));
	button->setName("viewButton");

	//texture = new Texture2D();
	//img = osgDB::readImageFile("resources/textures/restart.png");
	//texture->setImage(img);

	//stateset = button->getOrCreateStateSet();
	//stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton pour zoomer en avant
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -60, 0)));
	button->setName("zoomInButton");

	//texture = new Texture2D();
	//img = osgDB::readImageFile("resources/textures/restart.png");
	//texture->setImage(img);

	//stateset = button->getOrCreateStateSet();
	//stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());

	// dessin du bouton pour zoomer en arrière
	button = new MatrixTransform();
	button->addChild(classicButton.get());
	button->setMatrix(Matrix::identity());
	button->postMult(Matrix::translate(Vec3(50, -150, 0)));
	button->setName("zoomOutButton");

	//texture = new Texture2D();
	//img = osgDB::readImageFile("resources/textures/restart.png");
	//texture->setImage(img);

	//stateset = button->getOrCreateStateSet();
	//stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);

	cameraActionsArea->addChild(button.get());
}

Config::Button GameView::testerCollisionAvecBouton(Point position)
{
	Config::Button result = Config::Button::UNKNOWN;

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

		if (result != Config::Button::UNKNOWN) // si bouton trouvé, arrêter de chercher
			break;
	}

	return result;
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
	else if (name == "cancelButton") // bouton annuler le dernier coup
		return Config::Button::CANCEL;
	else if (name == "viewButton") // bouton changer de vue
		return Config::Button::VIEW;
	else if (name == "zoomInButton") // bouton zoomer en avant
		return Config::Button::ZOOM_IN;
	else if (name == "zoomOutButton") // bouton zoomer en arrière
		return Config::Button::ZOOM_OUT;

	return Config::Button::UNKNOWN; // bouton inconnu
}

void GameView::refreshButtons()
{
	ref_ptr<Texture2D> texture;
	ref_ptr<Image> img;
	StateSet *stateset;

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

		// si pas un bouton qui change de texture -> l'ignorer
		if (n->getName() != "directionButton" && n->getName() != "modeButton" && n->getName() != "validateButton")
			continue;

		if (Model::getInstance()->getMode() == Model::Mode::PIONS) {
			if (n->getName() == "validateButton") {
				texture = new Texture2D();
				img = osgDB::readImageFile("resources/textures/validate_no.png");
				texture->setImage(img);

				stateset = n->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
			} else if (n->getName() == "modeButton") {
				texture = new Texture2D();
				img = osgDB::readImageFile("resources/textures/fence.png");
				texture->setImage(img);

				stateset = n->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
			} else if (n->getName() == "directionButton") {
				texture = new Texture2D();
				img = osgDB::readImageFile("resources/textures/direction_no.png");
				texture->setImage(img);

				stateset = n->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
			}
		} else {
			if (n->getName() == "validateButton") {
				texture = new Texture2D();
				img = osgDB::readImageFile("resources/textures/validate.png");
				texture->setImage(img);

				stateset = n->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
			} else if (n->getName() == "modeButton") {
				texture = new Texture2D();
				img = osgDB::readImageFile("resources/textures/pawn.png");
				texture->setImage(img);

				stateset = n->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
			} else if (n->getName() == "directionButton") {
				texture = new Texture2D();
				img = osgDB::readImageFile("resources/textures/direction.png");
				texture->setImage(img);

				stateset = n->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture, StateAttribute::Values::ON);
			}
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
	drawCommandsButtons();

	// initialisation du controller
	Controller::getInstance()->initialize(cameraGameArea.get());

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
