#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <osgGA\GUIEventAdapter>
#include <osgGA\EventVisitor>
#include <osgUtil\LineSegmentIntersector>
#include "Controller.h"
#include "GameView.h"
#include "Model.h"
#include "Point.h"

/**
	Controlleur des différents événements clavier/souris
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class EventController : public osg::NodeCallback
{
private:
	bool mustChangePlayer;														// indique si oui ou non il faut changer de joueur après l'action spécifique
	osg::ref_ptr<osg::Switch> hoverButton;										// contient le bouton actuellement en surbrillance, 0 si aucun

	void handleMouse(osg::ref_ptr<osgGA::GUIEventAdapter> ea, osg::Node *node);	// effectue des traitements pour les événements souris
	void handleMouseClick(Point position);										// effectue des traitements pour les événements souris lors du clique
	void handleMouseMove(Point position);										// effectue des traitements pour les événements souris lors du bougé
	void handleKeyboard(int key);												// effectue des traitements pour les événements clavier
	void refreshScene();														// rafraîchit le graphe de scène
	void performAction(Config::Button button);									// appelle les différentes méthodes en fonctions de l'action à effectuer
	bool highlightButton(osg::ref_ptr<osg::Switch> sw, std::string name);		// met en surbrillance le bouton si nécessaire
	
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
