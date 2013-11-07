#include "PositionUpdater.h"

using namespace osg;

void PositionUpdater::operator()(Node *node, NodeVisitor *nv)
{
	// rafraîchit le message
	GameView::getInstance()->refreshMessage();

	// rafraîchit la vue
	Controller::getInstance()->getBarrieres();
	Controller::getInstance()->getPions();

	// si mode barrière, dessiner aussi la barrière virtuelle
	if (Model::getInstance()->getMode() == Model::Mode::BARRIERE)
		Controller::getInstance()->getVirtualBarriere();
	else // sinon ne pas la dessiner (la retirer si déjà dessiner)
		Controller::getInstance()->removeVirtualBarriere();
}
