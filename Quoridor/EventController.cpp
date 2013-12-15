#include "EventController.h"

using namespace std;
using namespace osg;
using namespace osgGA;
using namespace osgUtil;

//TODO: refresh que lorsqu'un élément à bougé (et pour refresh view, que quand la vue a changé)

EventController::EventController()
{
	// initialisation des variables
	hoverButton = 0;
}

void EventController::operator()(Node *node, NodeVisitor *nv)
{
	// création de la liste des différents events possibles
	list<ref_ptr<GUIEventAdapter>> events;

	EventVisitor *ev = dynamic_cast<EventVisitor *>(nv);

	if (ev)
		events = ev->getEvents();

	ref_ptr<GUIEventAdapter> ea = events.back();

	events.pop_back();

	mustChangePlayer = false;
	
	switch (ea->getEventType())
	{
		case GUIEventAdapter::EventType::PUSH: // clique de la souris
		case GUIEventAdapter::EventType::MOVE: // bouger de la souris
			handleMouse(ea, node);
			refreshScene();
			break;
		case GUIEventAdapter::EventType::KEYDOWN: // touche pressée du clavier
			handleKeyboard(ea->getKey());
			refreshScene();
			break;
	}

	// changer de joueur si nécessaire
	if (mustChangePlayer) {
		Model::getInstance()->setJoueurEnCours(Model::getInstance()->getJoueurEnCours() == 1 ? 2 : 1);
		refreshScene();
	}
}

void EventController::handleMouse(ref_ptr<GUIEventAdapter>ea, Node *node)
{
	ref_ptr<LineSegmentIntersector> intersector = new LineSegmentIntersector(Intersector::CoordinateFrame::WINDOW, ea->getX(), ea->getY());

	IntersectionVisitor iv(intersector.get());

	// vérification que l'action se passe dans la zone du viewport des boutons d'actions
	ref_ptr<Viewport> vp = GameView::getInstance()->getActionsCamera()->getViewport();

	if (ea->getX() >= vp->x() && ea->getX() <= (vp->x() + vp->width())
		&& ea->getY() >= vp->y() && ea->getY() <= (vp->y() + vp->height()))
			node->accept(iv);
	else
		return;

	LineSegmentIntersector::Intersection result = intersector->getFirstIntersection();

	Point position = Point(result.getWorldIntersectPoint().x(), result.getWorldIntersectPoint().y());

	if (ea->getEventType() == GUIEventAdapter::EventType::MOVE) { // si l'action = la souris qui bouge
		handleMouseMove(position);
	} else if(ea->getEventType() == GUIEventAdapter::EventType::PUSH && ea->getButton() == GUIEventAdapter::MouseButtonMask::LEFT_MOUSE_BUTTON) { // si l'action = clique gauche
		// si l'action a lieu sur un objet contenu dans le viewport des boutons d'actions
		if (intersector->containsIntersections()) {
			handleMouseClick(position);
		}
	}
}

void EventController::handleMouseClick(Point position)
{
	handleMouseMove(Point(0, 0)); // permet d'éviter une inversion lors du changement de texture sur des boutons tel que le changement de mode
	performAction(GameView::getInstance()->testerCollisionAvecBouton(position));
	handleMouseMove(position);
}

bool EventController::highlightButton(osg::ref_ptr<osg::Switch> sw, std::string name)
{
	if (sw == 0) // si pas un switch
		return false;

	if (sw->getName() != name) // si pas le bouton sur lequel on est
		return false;

	for (unsigned int j = 0; j < sw->getChildIndex(0); j++)
	{
		hoverButton = sw;
		sw->setValue(j, !sw->getValue(j));
	}

	return true;
}

void EventController::handleMouseMove(Point position)
{
	if (position.getX() == 0 && position.getY() == 0) { // si hors d'un bouton
		if (hoverButton == 0) // si aucun bouton n'est affiché en surbrillance
			return;
		else { // si au moins un bouton est affiché en surbrillance, remise en mode normal
			for (unsigned int j = 0; j < hoverButton->getChildIndex(0); j++)
				hoverButton->setValue(j, !hoverButton->getValue(j));

			hoverButton = 0;

			return;
		}
	}

	// obtenir le nom du bouton sur lequel on se trouve
	string name = GameView::getInstance()->obtenirNomBoutonCollision(position);

	// cette condition permet d'éviter un problème lorsque l'on passe très vite avec la souris
	// entre plusieurs boutons

	if (hoverButton != 0) { // si déjà un bouton en surbrillance...
		if (name != hoverButton->getName()) { // ... et différent de l'actuel, nettoyer
			for (unsigned int j = 0; j < hoverButton->getChildIndex(0); j++)
				hoverButton->setValue(j, !hoverButton->getValue(j));

			hoverButton = 0;
		} else // sinon ne rien faire
			return;
	}

	// chercher après le bouton où l'on se trouve et le mettre en surbrillance
	ref_ptr<Camera> camera = GameView::getInstance()->getActionsCamera();

	for (unsigned int i = 0; i < camera->getChildIndex(0); i++) // parcours des enfants
	{
		ref_ptr<Switch> sw = camera->getChild(i)->asSwitch();

		if (sw != 0) { // si est un switch
			if (highlightButton(sw, name))
				return;
		} else { // si container
			ref_ptr<MatrixTransform> mt = dynamic_cast<MatrixTransform *>(camera->getChild(i));

			for (unsigned int j = 0; j < mt->getChildIndex(0); j++) {
				ref_ptr<Switch> sw2 = mt->getChild(j)->asSwitch();

				if (highlightButton(sw2, name))
					return;
			}
		}
	}
}

void EventController::performAction(Config::Button button)
{
	if (button == Config::Button::UNKNOWN) // si aucun bouton, ne rien faire
		return;

	// boutons pouvant agir même quand le jeu est terminé
	switch (button)
	{
		case Config::RESTART: // recommencer une partie
			Model::getInstance()->recommencerPartie();
			return;
		case Config::VIEW:
			if (Model::getInstance()->getView() == Model::View::PARALLELE)
				Model::getInstance()->setView(Model::View::PERSPECTIVE);
			else
				Model::getInstance()->setView(Model::View::PARALLELE);

			GameView::getInstance()->refreshButtons(); // met à jour la textures des boutons

			return;
		case Config::ZOOM_IN:
			GameView::getInstance()->zoomIn();
			return;
		case Config::ZOOM_OUT:
			GameView::getInstance()->zoomOut();
			return;
		case Config::TURN_LEFT:
			GameView::getInstance()->turnLeft();
			return;
		case Config::TURN_RIGHT:
			GameView::getInstance()->turnRight();
			return;
	}

	// tester si le jeu est terminé
	if (Model::getInstance()->getPartieTerminee())
		return;

	// autres touches
	switch (button)
	{	
		case Config::Button::UNDO: // annuler un coup
			Model::getInstance()->annulerDernierCoup();
			GameView::getInstance()->refreshButtons(); // met à jour la textures des boutons

			return;
		case Config::Button::MODE: // changer de mode
			// tester si le joueur a placé toutes ses barrières
			if (Model::getInstance()->getBarrierePlacee(Model::getInstance()->getJoueurEnCours()).size() >= (unsigned int)Config::getInstance()->getNbrBarriereJoueur()) {
				Model::getInstance()->setErrorMessage("Vous n'avez plus de barrière disponible");
				return;
			} else
				Model::getInstance()->setErrorMessage("");

			if (Model::getInstance()->getMode() == Model::Mode::PIONS)
				Model::getInstance()->setMode(Model::Mode::BARRIERE);
			else {
				Model::getInstance()->setMode(Model::Mode::PIONS);
				Controller::getInstance()->hideVirtualBarriere();
			}

			GameView::getInstance()->refreshButtons(); // met à jour la textures des boutons

			return;
		case Config::Button::SAVE: // enregistrer une partie
			{
				ofstream ofs = ofstream(Config::getInstance()->getSaveFileName());

				if (ofs) {
					ofs << *Model::getInstance();
					ofs.close();
				}

				return;
			}
		case Config::Button::LOAD: // charger une partie
			{
				ifstream ifs = ifstream(Config::getInstance()->getSaveFileName());

				if (ifs) {
					ifs >> *Model::getInstance();
					ifs.close();
				}

				return;
			}
	}

	if (Model::getInstance()->getMode() == Model::Mode::PIONS) { // mode pions
		switch(button)
		{
			case Config::Button::ARROW_UP: // déplacer le pion vers le haut
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::UP);
				break;
			case Config::Button::ARROW_DOWN: // déplacer le pion vers le bas
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::DOWN);
				break;	
			case Config::Button::ARROW_LEFT: // déplacer le pion vers la gauche
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::LEFT);
				break;
			case Config::Button::ARROW_RIGHT: // déplacer le pion vers la droite
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::RIGHT);
				break;
		}
	} else { // mode barrière
		switch (button)
		{
			case Config::Button::ARROW_UP: // déplacer la barrière vers le haut
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::UP);
				break;
			case Config::Button::ARROW_DOWN: // déplacer la barrière vers le bas
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::DOWN);
				break;
			case Config::Button::ARROW_LEFT: // déplacer la barrière vers la gauche
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::LEFT);
				break;
			case Config::Button::ARROW_RIGHT: // déplacer la barrière vers la droite
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::RIGHT);
				break;
			case Config::Button::DIRECTION: // changer de direction
				Model::getInstance()->changerSensVirtualBarriere();
				break;
			case Config::Button::VALIDATE: // valider le placement
				mustChangePlayer = Model::getInstance()->placerBarriere(Model::getInstance()->getJoueurEnCours(), Model::getInstance()->getVirtualBarriere(), Model::getInstance()->getSensVirtualBarriere());

				if (mustChangePlayer) {
					Model::getInstance()->setMode(Model::Mode::PIONS);
					GameView::getInstance()->refreshButtons(); // met à jour la textures des boutons
					Controller::getInstance()->hideVirtualBarriere();
				}

				break;
		}
	}
}

void EventController::handleKeyboard(int key)
{
	switch (key)
	{
		case 'R':
		case GUIEventAdapter::KeySymbol::KEY_R:
			performAction(Config::Button::RESTART);
			break;
		case 'Z':
		case GUIEventAdapter::KeySymbol::KEY_Z:
			performAction(Config::Button::UNDO);
			break;
		case 'M':
		case GUIEventAdapter::KeySymbol::KEY_M:
			performAction(Config::Button::MODE);
			break;
		case GUIEventAdapter::KeySymbol::KEY_F6:
			performAction(Config::Button::SAVE);
			break;
		case GUIEventAdapter::KeySymbol::KEY_F7:
			performAction(Config::Button::LOAD);
			break;
		case 'S':
		case GUIEventAdapter::KeySymbol::KEY_S:
			performAction(Config::Button::DIRECTION);
			break;
		case GUIEventAdapter::KeySymbol::KEY_KP_Enter:
		case GUIEventAdapter::KeySymbol::KEY_Return:
			performAction(Config::Button::VALIDATE);
			break;
		case 'V':
		case GUIEventAdapter::KeySymbol::KEY_V:
			performAction(Config::Button::VIEW);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Plus:
		case GUIEventAdapter::KeySymbol::KEY_KP_Add:
			performAction(Config::Button::ZOOM_IN);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Minus:
		case GUIEventAdapter::KeySymbol::KEY_KP_Subtract:
			performAction(Config::Button::ZOOM_OUT);
			break;
		case 'O':
		case GUIEventAdapter::KeySymbol::KEY_O:
			performAction(Config::Button::TURN_LEFT);
			break;
		case 'P':
		case GUIEventAdapter::KeySymbol::KEY_P:
			performAction(Config::Button::TURN_RIGHT);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Up:
		case GUIEventAdapter::KeySymbol::KEY_Down:
		case GUIEventAdapter::KeySymbol::KEY_Left:
		case GUIEventAdapter::KeySymbol::KEY_Right:
			manageArrow(key);
			break;
	}
}

void EventController::manageArrow(int key)
{
	int angle = GameView::getInstance()->getAngle();

	Config::Button upArrow;
	Config::Button downArrow;
	Config::Button leftArrow;
	Config::Button rightArrow;

	if (angle >= 315 || angle < 45) { // si vers le bas
		upArrow = Config::Button::ARROW_UP;
		downArrow = Config::Button::ARROW_DOWN;
		leftArrow = Config::Button::ARROW_LEFT;
		rightArrow = Config::Button::ARROW_RIGHT;
	} else if (angle >= 45 && angle < 135) { // si vers la droite
		upArrow = Config::Button::ARROW_RIGHT;
		downArrow = Config::Button::ARROW_LEFT;
		leftArrow = Config::Button::ARROW_UP;
		rightArrow = Config::Button::ARROW_DOWN;
	} else if (angle >= 135 && angle < 225) { // si à l'envers
		upArrow = Config::Button::ARROW_DOWN;
		downArrow = Config::Button::ARROW_UP;
		leftArrow = Config::Button::ARROW_RIGHT;
		rightArrow = Config::Button::ARROW_LEFT;
	} else { // si vers la gacuhe
		upArrow = Config::Button::ARROW_LEFT;
		downArrow = Config::Button::ARROW_RIGHT;
		leftArrow = Config::Button::ARROW_DOWN;
		rightArrow = Config::Button::ARROW_UP;
	}

	switch (key)
	{
		case GUIEventAdapter::KeySymbol::KEY_Up:
			performAction(upArrow);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Down:
			performAction(downArrow);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Left:
			performAction(leftArrow);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Right:
			performAction(rightArrow);
			break;
	}
}

void EventController::refreshScene()
{
	// rafraîchit le message
	GameView::getInstance()->refreshMessage();

	// si mode barrière, ajouter la barrière virtuelle au graphe de scène
	if (Model::getInstance()->getMode() == Model::Mode::BARRIERE)
		Controller::getInstance()->showVirtualBarriere();
	else // sinon ne pas la dessiner (la retirer si déjà dessiner)
		Controller::getInstance()->hideVirtualBarriere();

	// rafraîchit la vue
	Controller::getInstance()->computeAllPositions();
	GameView::getInstance()->refreshView();
}
