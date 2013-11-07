#include "EventController.h"

using namespace std;
using namespace osg;
using namespace osgGA;
using namespace osgUtil;

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
		case GUIEventAdapter::PUSH:
			handleMouse(ea, node);
			break;
		case GUIEventAdapter::KEYDOWN:
			handleKeyboard(ea->getKey());
			break;
	}

	// changer de joueur si n�cessaire
	if (mustChangePlayer) {
		Model::getInstance()->setJoueurEnCours(Model::getInstance()->getJoueurEnCours() == 1 ? 2 : 1);
		mustChangePlayer = true;
	}
}

void EventController::handleMouse(ref_ptr<GUIEventAdapter>ea, Node *node)
{
	if (ea->getButton() == GUIEventAdapter::MouseButtonMask::LEFT_MOUSE_BUTTON)
	{
		ref_ptr<LineSegmentIntersector> intersector = new LineSegmentIntersector(Intersector::CoordinateFrame::WINDOW, ea->getX(), ea->getY());

		IntersectionVisitor iv(intersector.get());

		// v�rification que le clic se passe dans la zone du viewport des boutons d'actions
		if (ea->getX() >= 10 + FENETRE_WIDTH - (FENETRE_WIDTH / 3) - 10
			&& ea->getX() <= 10 + FENETRE_WIDTH - 10
			&& ea->getY() >= 10
			&& ea->getY() <= 90 + 10)
				node->accept(iv);

		if (intersector->containsIntersections()) {
			LineSegmentIntersector::Intersection result = intersector->getFirstIntersection();

			Point position = Point(result.getWorldIntersectPoint().x(), result.getWorldIntersectPoint().y());

			if (testButtonColision(Button::LOAD, position)) { // charger une partie
				ifstream ifs = ifstream(Config::getInstance()->getSaveFileName());

				if (ifs) {
					ifs >> *Model::getInstance();
					ifs.close();
				}
			} else if (testButtonColision(Button::SAVE, position)) { // enregistrer une partie
				ofstream ofs = ofstream(Config::getInstance()->getSaveFileName());

				if (ofs) {
					ofs << *Model::getInstance();
					ofs.close();
				}
			} else if (testButtonColision(Button::MODE, position)) { // changer de mode
				if (Model::getInstance()->getMode() == Model::Mode::PIONS)
					Model::getInstance()->setMode(Model::Mode::BARRIERE);
				else {
					Model::getInstance()->setMode(Model::Mode::PIONS);
					Controller::getInstance()->removeVirtualBarriere();
				}
			} else if (Model::getInstance()->getMode() == Model::Mode::PIONS) { // mode pions
				if (testButtonColision(Button::ARROW_UP, position)) // zone fl�che haut
					mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::UP);
				else if (testButtonColision(Button::ARROW_DOWN, position)) // zone fl�che bas
					mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::DOWN);
				else if(testButtonColision(Button::ARROW_LEFT, position)) // zone fl�che gauche
					mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::LEFT);
				else if (testButtonColision(Button::ARROW_RIGHT, position)) // zone fl�che droite
					mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::RIGHT);
			} else { // mode barri�re
				if (testButtonColision(Button::ARROW_UP, position)) // zone fl�che haut
					Model::getInstance()->deplacerVirtualBarriere(Model::Direction::UP);
				else if (testButtonColision(Button::ARROW_DOWN, position)) // zone fl�che bas
					Model::getInstance()->deplacerVirtualBarriere(Model::Direction::DOWN);
				else if(testButtonColision(Button::ARROW_LEFT, position)) // zone fl�che gauche
					Model::getInstance()->deplacerVirtualBarriere(Model::Direction::LEFT);
				else if (testButtonColision(Button::ARROW_RIGHT, position)) // zone fl�che droite
					Model::getInstance()->deplacerVirtualBarriere(Model::Direction::RIGHT);
				else if (testButtonColision(Button::SENS, position)) // changer le sens de la barri�re virtuelle
					Model::getInstance()->changerSensVirtualBarriere();
				else if (testButtonColision(Button::VALIDATE, position)) // valider la barri�re virtuelle
				{
					mustChangePlayer = Model::getInstance()->placerBarriere(Model::getInstance()->getJoueurEnCours(), Model::getInstance()->getVirtualBarriere(), Model::getInstance()->getSensVirtualBarriere());

					if (mustChangePlayer) {
						Model::getInstance()->setMode(Model::Mode::PIONS);
						Controller::getInstance()->removeVirtualBarriere();
					}
				}
			}
		}
	}
}

bool EventController::testButtonColision(Button button, Point position)
{
	// TODO: am�liorer les if si dessous pour obtenir les coordonn�es directement depuis l'objet
	switch (button)
	{
	case EventController::ARROW_UP:
		return position.getX() > -70 && position.getX() < -50 && position.getY() < 35 && position.getY() > 9;
	case EventController::ARROW_DOWN:
		return position.getX() > -70 && position.getX() < -50 && position.getY() < -8 && position.getY() > -33;
	case EventController::ARROW_LEFT:
		return position.getX() > -95 && position.getX() < -68 && position.getY() < 10 && position.getY() > -10;
	case EventController::ARROW_RIGHT:
		return position.getX() > -52 && position.getX() < -25 && position.getY() < 10 && position.getY() > -10;
	case EventController::SENS:
		return position.getX() > -65 && position.getX() < -54 && position.getY() < 4 && position.getY() > -3;
	case EventController::MODE:
		return position.getX() > -15 && position.getX() < 14 && position.getY() < 30 && position.getY() > 12;
	case EventController::VALIDATE:
		return position.getX() > -15 && position.getX() < 14 && position.getY() < -10 && position.getY() > -27;
	case EventController::LOAD:
		return position.getX() > 45 && position.getX() < 75 && position.getY() < 30 && position.getY() > 12;
	case EventController::SAVE:
		return position.getX() > 45 && position.getX() < 75 && position.getY() < -10 && position.getY() > -27;
	}

	return false;
}

void EventController::handleKeyboard(int key)
{
	if (key == GUIEventAdapter::KEY_M) { // changer de mode
		if (Model::getInstance()->getMode() == Model::Mode::PIONS)
			Model::getInstance()->setMode(Model::Mode::BARRIERE);
		else {
			Model::getInstance()->setMode(Model::Mode::PIONS);
			Controller::getInstance()->removeVirtualBarriere();
		}

		return;
	}

	if (key == GUIEventAdapter::KEY_F6) { // enregistrer une partie
		ofstream ofs = ofstream(Config::getInstance()->getSaveFileName());

		if (ofs) {
			ofs << *Model::getInstance();
			ofs.close();
		}

		return;
	} else if (key == GUIEventAdapter::KEY_F7) { // charger une partie
		ifstream ifs = ifstream(Config::getInstance()->getSaveFileName());

		if (ifs) {
			ifs >> *Model::getInstance();
			ifs.close();
		}

		return;
	}

	if (Model::getInstance()->getMode() == Model::Mode::PIONS) { // mode pions
		switch(key)
		{
			case GUIEventAdapter::KeySymbol::KEY_Up: // si appuy� sur la touche "fl�che du haut"
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::UP);
				break;
			case GUIEventAdapter::KeySymbol::KEY_Down: // si appuy� sur la touche "fl�che du bas"
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::DOWN);
				break;	
			case GUIEventAdapter::KeySymbol::KEY_Left: // si appuy� sur la touche "fl�che de gauche"
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::LEFT);
				break;
			case GUIEventAdapter::KeySymbol::KEY_Right: // si appuy� sur la touche "fl�che de droite"
				mustChangePlayer = Model::getInstance()->deplacerPion(Model::getInstance()->getJoueurEnCours(), Model::Direction::RIGHT);
				break;
		}
	} else { // mode barri�re
		switch (key)
		{
			case GUIEventAdapter::KeySymbol::KEY_Up: // si appuy� sur la touche "fl�che du haut"
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::UP);
				break;
			case GUIEventAdapter::KeySymbol::KEY_Down: // si appuy� sur la touche "fl�che du base"
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::DOWN);
				break;
			case GUIEventAdapter::KeySymbol::KEY_Left: // si appuy� sur la touche "fl�che de gauche"
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::LEFT);
				break;
			case GUIEventAdapter::KeySymbol::KEY_Right: // si appuy� sur la touche "fl�che de droite"
				Model::getInstance()->deplacerVirtualBarriere(Model::Direction::RIGHT);
				break;
			case GUIEventAdapter::KeySymbol::KEY_S: // si appuy� sur la touche "S"
				Model::getInstance()->changerSensVirtualBarriere();
				break;
			case GUIEventAdapter::KeySymbol::KEY_Return: // si appuy� sur la touche "Entr�e"
				mustChangePlayer = Model::getInstance()->placerBarriere(Model::getInstance()->getJoueurEnCours(), Model::getInstance()->getVirtualBarriere(), Model::getInstance()->getSensVirtualBarriere());

				if (mustChangePlayer) {
					Model::getInstance()->setMode(Model::Mode::PIONS);
					Controller::getInstance()->removeVirtualBarriere();
				}

				break;
		}
	}
}