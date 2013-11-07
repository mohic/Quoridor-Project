#pragma once

#include <osg\Geode>
#include <osg\MatrixTransform>
#include <osg\ShapeDrawable>
#include <osgText\Text>
#include <osgViewer\Viewer>
#include <vector>
#include "Config.h"
#include "Controller.h"
#include "EventController.h"
#include "Model.h"
#include "PositionUpdater.h"

/**
	classe de la View du jeu
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class GameView
{
private:
	static GameView *instance;				// instance de la vue pour le pattern singleton
	GameView();								// constructeur privé
	void initMessage();						// initialise le message utilisateur

	osg::ref_ptr<osgText::Text> message;	// message à afficher à l'utilisateur

public:
	/**
		méthode de récupération de l'instance de GameView
		@return l'instance de GameView
	*/
	static GameView *getInstance();

	/**
		méthode de construction du graphe de scène
		@return le graphe de scène
	*/
	osg::ref_ptr<osgViewer::Viewer> buildSceneGraph();

	/**
		méthode de rafraîchissement du message
	*/
	void refreshMessage();
};
