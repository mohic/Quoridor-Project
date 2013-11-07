#include "PositionUpdater.h"

using namespace osg;

void PositionUpdater::operator()(Node *node, NodeVisitor *nv)
{
	// rafra�chit le message
	GameView::getInstance()->refreshMessage();

	// rafra�chit la vue
	Controller::getInstance()->getBarrieres();
	Controller::getInstance()->getPions();

	// si mode barri�re, dessiner aussi la barri�re virtuelle
	if (Model::getInstance()->getMode() == Model::Mode::BARRIERE)
		Controller::getInstance()->getVirtualBarriere();
	else // sinon ne pas la dessiner (la retirer si d�j� dessiner)
		Controller::getInstance()->removeVirtualBarriere();
}
