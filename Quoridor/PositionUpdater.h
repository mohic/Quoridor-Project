#pragma once

#include <osg\NodeCallBack>
#include <osg\MatrixTransform>
//#include "Controller.h"
//#include "GameView.h"

/**
	Controlleur servant � redessiner la sc�ne � chaque frame
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class PositionUpdater : public osg::NodeCallback
{
public:
	/**
		surcharge l'op�rateur() comme indiqu� par OSG pour permettre d'obtenir les �v�nements
	*/
	virtual void operator()(osg::Node *node, osg::NodeVisitor *nv);
};
