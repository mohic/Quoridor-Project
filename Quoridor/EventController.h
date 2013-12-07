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
	Controlleur des diff�rents �v�nements clavier/souris
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class EventController : public osg::NodeCallback
{
private:
	enum Button { ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, MODE, SENS, VALIDATE, LOAD, SAVE, RESTART };	// type de bouton

	bool mustChangePlayer;																						// indique si oui ou non il faut changer de joueur apr�s l'action sp�cifique
	bool modeBarriere;																							// indique si on est en mode barri�re ou normal

	bool testButtonColision(Button button, Point position);														// test si la position est contenue dans la zone du bouton. true si dans la zone du bouton, sinon false
	void handleMouse(osg::ref_ptr<osgGA::GUIEventAdapter> ea, osg::Node *node);									// effectue des traitements pour les �v�nements souris
	void handleKeyboard(int key);																				// effectue des traitements pour les �v�nements clavier
	void refreshScene();																						// rafra�chit le graphe de sc�ne
	
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
