#pragma once
#pragma warning (disable : 4250) // supprime le warning venant de OSG avec l'include osgDB\readFile
								 // qui avertit que tel classe hérite de l'autre classe par dominance

#include <osg\Geode>
#include <osg\MatrixTransform>
#include <osg\ShapeDrawable>
#include <osg\Texture2D>
#include <osgDB\ReadFile>
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
	static GameView *instance;						// instance de la vue pour le pattern singleton
	GameView();										// constructeur privé

	osg::ref_ptr<osgText::Text> userMessage;		// message à afficher à l'utilisateur
	osg::ref_ptr<osgText::Text> errorMessage;		// message d'erreur à afficher à l'utilisateur

	osg::Vec3 center;								// centre pointé par la caméra
	osg::Vec3 up;									// orientation de la caméra
	osg::Vec3 eyeOrtho;								// position de la caméra en vue orthographique
	osg::Vec3 eyePrimitive;							// position de la caméra en vue primitive

	osg::ref_ptr<osgViewer::Viewer> viewer;			// le viewer
	osg::ref_ptr<osg::Camera> cameraGameArea;		// caméra servant à afficher la zone de jeu
	osg::ref_ptr<osg::Camera> cameraDisplayArea;	// caméra de la zone de texte
	osg::ref_ptr<osg::Camera> cameraActionsArea;	// caméra de la zone des bouton d'actions
	osg::ref_ptr<osg::Geode> geodeVirtualFence;		// géode de la barrière virtuelle

	osg::ref_ptr<osg::Geode> classicButton;			// bouton classique

	void initMessage();								// initialise le message utilisateur

	void createAndConfigureViewer();				// crée et configure le viewer
	void createAndConfigureCameraGameArea();		// crée et configure la caméra représentant la zone de jeu
	void createAndConfigureCameraDisplayArea();		// crée et configure la caméra représentant la zone d'affichage
	void createAndConfigureCameraActionsArea();		// crée et configure la caméra représentant la zone des actions

	void createClassicButton();						// crée un bouton classique

	void drawPlate();								// dessine le plateau
	void drawCases();								// dessine les cases
	void drawFences();								// dessine les barrières
	void drawVirtualFence();						// dessine la barrière virtuelle
	void drawPawns();								// dessine les pions
	void drawMessage();								// dessine le message
	void drawArrowAndDirectionButtons();			// dessine les boutons flèchés et le bouton changer de sens
	void drawCommandsButtons();						// dessine les différents boutons de commandes

	Config::Button checkButton(std::string name);	// vérifie quel bouton à été cliqué

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

	/**
		obtenir le viewport de la caméra des boutons d'actions
		@return le viewport de la caméra des boutons d'actions
	*/
	const osg::Viewport *getActionViewport()
	{
		return cameraActionsArea->getViewport();
	}

	/**
		tester si le point entre en collision avec un bouton contenu dans le viewport de la caméra des boutons d'actions
		@param position le point à tester
		@return quel bouton a été cliqué
	*/
	Config::Button testerCollisionAvecBouton(Point position);
};
