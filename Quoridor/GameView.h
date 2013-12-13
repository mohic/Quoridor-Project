#pragma once
#pragma warning (disable : 4250) // supprime le warning venant de OSG avec l'include osgDB\readFile
								 // qui avertit que tel classe h�rite de l'autre classe par dominance

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
	GameView();										// constructeur priv�

	osg::ref_ptr<osgText::Text> userMessage;		// message � afficher � l'utilisateur
	osg::ref_ptr<osgText::Text> errorMessage;		// message d'erreur � afficher � l'utilisateur

	osg::Vec3 center;								// centre point� par la cam�ra
	osg::Vec3 up;									// orientation de la cam�ra
	osg::Vec3 eyeOrtho;								// position de la cam�ra en vue orthographique
	osg::Vec3 eyePrimitive;							// position de la cam�ra en vue primitive

	osg::ref_ptr<osgViewer::Viewer> viewer;			// le viewer
	osg::ref_ptr<osg::Camera> cameraGameArea;		// cam�ra servant � afficher la zone de jeu
	osg::ref_ptr<osg::Camera> cameraDisplayArea;	// cam�ra de la zone de texte
	osg::ref_ptr<osg::Camera> cameraActionsArea;	// cam�ra de la zone des bouton d'actions
	osg::ref_ptr<osg::Geode> geodeVirtualFence;		// g�ode de la barri�re virtuelle

	osg::ref_ptr<osg::Geode> classicButton;			// bouton classique

	void initMessage();								// initialise le message utilisateur

	void createAndConfigureViewer();				// cr�e et configure le viewer
	void createAndConfigureCameraGameArea();		// cr�e et configure la cam�ra repr�sentant la zone de jeu
	void createAndConfigureCameraDisplayArea();		// cr�e et configure la cam�ra repr�sentant la zone d'affichage
	void createAndConfigureCameraActionsArea();		// cr�e et configure la cam�ra repr�sentant la zone des actions

	void createClassicButton();						// cr�e un bouton classique

	void drawPlate();								// dessine le plateau
	void drawCases();								// dessine les cases
	void drawFences();								// dessine les barri�res
	void drawVirtualFence();						// dessine la barri�re virtuelle
	void drawPawns();								// dessine les pions
	void drawMessage();								// dessine le message
	void drawArrowAndDirectionButtons();			// dessine les boutons fl�ch�s et le bouton changer de sens
	void drawCommandsButtons();						// dessine les diff�rents boutons de commandes

	Config::Button checkButton(std::string name);	// v�rifie quel bouton � �t� cliqu�

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

	/**
		obtenir le viewport de la cam�ra des boutons d'actions
		@return le viewport de la cam�ra des boutons d'actions
	*/
	const osg::Viewport *getActionViewport()
	{
		return cameraActionsArea->getViewport();
	}

	/**
		tester si le point entre en collision avec un bouton contenu dans le viewport de la cam�ra des boutons d'actions
		@param position le point � tester
		@return quel bouton a �t� cliqu�
	*/
	Config::Button testerCollisionAvecBouton(Point position);
};
