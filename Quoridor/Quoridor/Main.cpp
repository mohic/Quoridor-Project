#include <osgViewer\Viewer>
#include "GameView.h"

using namespace osg;

int main(void)
{
	ref_ptr<osgViewer::Viewer> viewer = GameView::getInstance()->buildSceneGraph();

	// lancement de la boucle de rendu
	return viewer->run();
}
