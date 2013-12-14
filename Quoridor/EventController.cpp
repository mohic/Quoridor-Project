#include "EventController.h"

using namespace std;
using namespace osg;
using namespace osgGA;
using namespace osgUtil;

//TODO: refresh que lorsqu'un �l�ment � boug� (et pour refresh view, que quand la vue a chang�)

EventController::EventController()
{
}

void EventController::operator()(Node *node, NodeVisitor *nv)
{
	// cr�ation de la liste des diff�rents events possibles
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
		case GUIEventAdapter::EventType::KEYDOWN: // touche press�e du clavier
			handleKeyboard(ea->getKey());
			refreshScene();
			break;
	}

	// changer de joueur si n�cessaire
	if (mustChangePlayer) {
		Model::getInstance()->setJoueurEnCours(Model::getInstance()->getJoueurEnCours() == 1 ? 2 : 1);
		refreshScene();
	}
}

void EventController::handleMouse(ref_ptr<GUIEventAdapter>ea, Node *node)
{
	ref_ptr<LineSegmentIntersector> intersector = new LineSegmentIntersector(Intersector::CoordinateFrame::WINDOW, ea->getX(), ea->getY());

	IntersectionVisitor iv(intersector.get());

	// v�rification que l'action se passe dans la zone du viewport des boutons d'actions
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
	performAction(GameView::getInstance()->testerCollisionAvecBouton(position));
}

void EventController::handleMouseMove(Point position)
{
	//TODO: � faire
	// position = 0, 0 si sur rien et si sur quelque chose, != 0, 0
	cout << "Moved: (" << position.getX() << ", " << position.getY() << ')' << endl;

	ref_ptr<Camera> camera = GameView::getInstance()->getActionsCamera();
}

void EventController::performAction(Config::Button button)
{
	if (button == Config::Button::UNKNOWN) // si aucun bouton, ne rien faire
		return;

	// boutons pouvant agir m�me quand le jeu est termin�
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

			GameView::getInstance()->refreshButtons(); // met � jour la textures des boutons

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

	// tester si le jeu est termin�
	if (Model::getInstance()->getPartieTerminee())
		return;

	// autres touches
	switch (button)
	{	
		case Config::Button::CANCEL: // annuler un coup
			Model::getInstance()->annulerDernierCoup();
			GameView::getInstance()->refreshButtons(); // met � jour la textures des boutons

			return;
		case Config::Button::MODE: // changer de mode
			// tester si le joueur a plac� toutes ses barri�res
			if (Model::getInstance()->getBarrierePlacee(Model::getInstance()->getJoueurEnCours()).size() >= (unsigned int)Config::getInstance()->getNbrBarriereJoueur()) {
				Model::getInstance()->setErrorMessage("Vous n'avez plus de barri�re disponible");
				return;
			} else
				Model::getInstance()->setErrorMessage("");

			if (Model::getInstance()->getMode() == Model::Mode::PIONS)
				Model::getInstance()->setMode(Model::Mode::BARRIERE);
			else {
				Model::getInstance()->setMode(Model::Mode::PIONS);
				Controller::getInstance()->hideVirtualBarriere();
			}

			GameView::getInstance()->refreshButtons(); // met � jour la textures des boutons

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
			case Config::Button::ARROW_UP: // d�placer le pion vers le haut
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::UP);
				break;
			case Config::Button::ARROW_DOWN: // d�placer le pion vers le bas
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::DOWN);
				break;	
			case Config::Button::ARROW_LEFT: // d�placer le pion vers la gauche
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::LEFT);
				break;
			case Config::Button::ARROW_RIGHT: // d�placer le pion vers la droite
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::RIGHT);
				break;
		}
	} else { // mode barri�re
		switch (button)
		{
			case Config::Button::ARROW_UP: // d�placer la barri�re vers le haut
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::UP);
				break;
			case Config::Button::ARROW_DOWN: // d�placer la barri�re vers le bas
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::DOWN);
				break;
			case Config::Button::ARROW_LEFT: // d�placer la barri�re vers la gauche
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::LEFT);
				break;
			case Config::Button::ARROW_RIGHT: // d�placer la barri�re vers la droite
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::RIGHT);
				break;
			case Config::Button::DIRECTION: // changer de direction
				Model::getInstance()->changerSensVirtualBarriere();
				break;
			case Config::Button::VALIDATE: // valider le placement
				mustChangePlayer = Model::getInstance()->placerBarriere(Model::getInstance()->getJoueurEnCours(), Model::getInstance()->getVirtualBarriere(), Model::getInstance()->getSensVirtualBarriere());

				if (mustChangePlayer) {
					Model::getInstance()->setMode(Model::Mode::PIONS);
					GameView::getInstance()->refreshButtons(); // met � jour la textures des boutons
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
			performAction(Config::Button::CANCEL);
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
		case GUIEventAdapter::KeySymbol::KEY_Up:
			performAction(Config::Button::ARROW_UP);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Down:
			performAction(Config::Button::ARROW_DOWN);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Left:
			performAction(Config::Button::ARROW_LEFT);
			break;
		case GUIEventAdapter::KeySymbol::KEY_Right:
			performAction(Config::Button::ARROW_RIGHT);
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
	}
}

void EventController::refreshScene()
{
	// rafra�chit le message
	GameView::getInstance()->refreshMessage();

	// si mode barri�re, ajouter la barri�re virtuelle au graphe de sc�ne
	if (Model::getInstance()->getMode() == Model::Mode::BARRIERE)
		Controller::getInstance()->showVirtualBarriere();
	else // sinon ne pas la dessiner (la retirer si d�j� dessiner)
		Controller::getInstance()->hideVirtualBarriere();

	// rafra�chit la vue
	Controller::getInstance()->computeAllPositions();
	GameView::getInstance()->refreshView();
}
