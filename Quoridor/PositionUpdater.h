#pragma once

#include <osg\NodeCallBack>
#include <osg\MatrixTransform>
//#include "Controller.h"
//#include "GameView.h"

/**
	Controlleur servant à redessiner la scène à chaque frame
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class PositionUpdater : public osg::NodeCallback
{
public:
	/**
		surcharge l'opérateur() comme indiqué par OSG pour permettre d'obtenir les événements
	*/
	virtual void operator()(osg::Node *node, osg::NodeVisitor *nv);
};
