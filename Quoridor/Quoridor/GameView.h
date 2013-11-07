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
	GameView();								// constructeur priv�
	void initMessage();						// initialise le message utilisateur

	osg::ref_ptr<osgText::Text> message;	// message � afficher � l'utilisateur

public:
	/**
		m�thode de r�cup�ration de l'instance de GameView
		@return l'instance de GameView
	*/
	static GameView *getInstance();

	/**
		m�thode de construction du graphe de sc�ne
		@return le graphe de sc�ne
	*/
	osg::ref_ptr<osgViewer::Viewer> buildSceneGraph();

	/**
		m�thode de rafra�chissement du message
	*/
	void refreshMessage();
};
