#include "EventController.h"

using namespace std;
using namespace osg;
using namespace osgGA;
using namespace osgUtil;

//TODO: refresh que lorsqu'un élément à bougé (et pour refresh view, que quand la vue a changé)

EventController::EventController()
{
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
	performAction(GameView::getInstance()->testerCollisionAvecBouton(position));
}

void EventController::handleMouseMove(Point position)
{
	//TODO: à faire
	// position = 0, 0 si sur rien et si sur quelque chose, != 0, 0
	cout << "Moved: (" << position.getX() << ", " << position.getY() << ')' << endl;

	ref_ptr<Camera> camera = GameView::getInstance()->getActionsCamera();
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
	}

	// tester si le jeu est terminé
	if (Model::getInstance()->getPartieTerminee())
		return;

	// autres touches
	switch (button)
	{	
		case Config::Button::CANCEL: // annuler un coup
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
