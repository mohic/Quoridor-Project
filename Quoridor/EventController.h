#pragma once

#include <fstream>
#include <vector>
#include <osgGA\GUIEventAdapter>
#include <osgGA\EventVisitor>
#include <osgUtil\LineSegmentIntersector>
#include "Controller.h"
#include "GameView.h"
#include "Model.h"
#include "Point.h"

/**
	Controlleur des diff�rents �v�nements clavier/souris
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class EventController : public osg::NodeCallback
{
private:
	bool mustChangePlayer;														// indique si oui ou non il faut changer de joueur apr�s l'action sp�cifique
	bool modeBarriere;															// indique si on est en mode barri�re ou normal

	void handleMouse(osg::ref_ptr<osgGA::GUIEventAdapter> ea, osg::Node *node);	// effectue des traitements pour les �v�nements souris
	void handleMouseClick(Point position);										// effectue des traitements pour les �v�nements souris lors du clique
	void handleMouseMove(Point position);										// effectue des traitements pour les �v�nements souris lors du boug�
	void handleKeyboard(int key);												// effectue des traitements pour les �v�nements clavier
	void refreshScene();														// rafra�chit le graphe de sc�ne
	void performAction(Config::Button button);									// appelle les diff�rentes m�thodes en fonctions de l'action � effectuer
	
public:
	/**
		constructeur publique
	*/
	EventController();

	/**
		surcharge l'op�rateur() comme indiqu� par OSG pour permettre d'obtenir les �v�nements
	*/
	virtual void operator()(osg::Node *node, osg::NodeVisitor *nv);
};
