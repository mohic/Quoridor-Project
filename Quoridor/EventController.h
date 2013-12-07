#pragma once

#include <fstream>
#include <vector>
#include <osgGA\GUIEventAdapter>
#include <osgGA\EventVisitor>
#include <osgUtil\LineSegmentIntersector>
#include "Controller.h"
#include "GameView.h"
#include "Model.h"

/**
	Controlleur des différents événements clavier/souris
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class EventController : public osg::NodeCallback
{
private:
	bool mustChangePlayer;																						// indique si oui ou non il faut changer de joueur après l'action spécifique
	bool modeBarriere;																							// indique si on est en mode barrière ou normal

	void handleMouse(osg::ref_ptr<osgGA::GUIEventAdapter> ea, osg::Node *node);									// effectue des traitements pour les événements souris
	void handleKeyboard(int key);																				// effectue des traitements pour les événements clavier
	void refreshScene();																						// rafraîchit le graphe de scène
	void buttonClicked(Config::Button button);																	// appelle les différentes méthodes en fonctions du bouton cliqué
	
public:
	/**
		constructeur publique
	*/
	EventController();

	/**
		surcharge l'opérateur() comme indiqué par OSG pour permettre d'obtenir les événements
	*/
	virtual void operator()(osg::Node *node, osg::NodeVisitor *nv);
};
