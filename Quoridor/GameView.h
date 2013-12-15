#pragma once
#pragma warning (disable : 4250) // supprime le warning venant de OSG avec l'include osgDB\readFile
								 // qui avertit que tel classe hérite de l'autre classe par dominance

#include <string>
#include <vector>
#include <osg\BlendFunc>
#include <osg\Geode>
#include <osg\Material>
#include <osg\MatrixTransform>
#include <osg\ShapeDrawable>
#include <osg\Texture2D>
#include <osgDB\ReadFile>
#include <osgText\Text>
#include <osgViewer\Viewer>
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
	static GameView *instance;							// instance de la vue pour le pattern singleton
	GameView();											// constructeur privé

	osg::ref_ptr<osgText::Text> userMessage;			// message à afficher à l'utilisateur
	osg::ref_ptr<osgText::Text> errorMessage;			// message d'erreur à afficher à l'utilisateur

	osg::Vec3 center;									// centre pointé par la caméra
	osg::Vec3 up;										// orientation de la caméra
	osg::Vec3 eye;										// position de la caméra pour les vues non jeu
	osg::Vec3 eyeOrtho;									// position de la caméra en vue orthographique
	int offset;											// sert pour le zoom da la caméra en vue parallèle
	osg::Vec3 eyePerspective;							// position de la caméra en vue perspective

	osg::ref_ptr<osgViewer::Viewer> viewer;				// le viewer

	osg::ref_ptr<osg::Camera> cameraGameArea;			// caméra servant à afficher la zone de jeu
	osg::ref_ptr<osg::Camera> cameraDisplayArea;		// caméra de la zone de texte
	osg::ref_ptr<osg::Camera> cameraActionsArea;		// caméra de la zone des bouton d'actions

	osg::ref_ptr<osg::Geode> geodeVirtualFence;			// géode de la barrière virtuelle
	osg::ref_ptr<osg::Geode> classicButton;				// bouton classique

	int currentAngle;									// retient l'angle actuel pour modifier la configuration des touches directionnelle en fonction de l'angle
	osg::ref_ptr<osg::MatrixTransform> arrows;			// ensemble des flèches + le bouton changement de direction
	osg::ref_ptr<osg::MatrixTransform> rotate;			// sert pour la rotation du plateau de jeu

	void initMessage();									// initialise le message utilisateur

	void createAndConfigureViewer();					// crée et configure le viewer
	void createAndConfigureCameraGameArea();			// crée et configure la caméra représentant la zone de jeu
	void createAndConfigureCameraDisplayArea();			// crée et configure la caméra représentant la zone d'affichage
	void createAndConfigureCameraActionsArea();			// crée et configure la caméra représentant la zone des actions

	void createClassicButton();							// crée un bouton classique

	void drawPlate();									// dessine le plateau
	void drawCases();									// dessine les cases
	void drawFences();									// dessine les barrières
	void drawVirtualFence();							// dessine la barrière virtuelle
	void drawPawns();									// dessine les pions
	void drawMessage();									// dessine le message
	void drawArrowAndDirectionButtons();				// dessine les boutons flèchés et le bouton changer de sens
	void drawGameButtons();								// dessine les différents boutons servant à jouer
	void drawSaveAndLoadButtons();						// dessine les différents boutons servant à l'enregistrement et au chargement du jeu
	void drawViewButtons();								// dessine les différents boutons servant à la gestion de la vue du jeu

	Config::Button checkButton(std::string name);		// vérifie quel bouton à été cliqué
	void changeTexture(osg::ref_ptr<osg::Switch> sw,
						std::string textureName);		// change la texture des différents éléments du switch

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
		méthode de récupération de la camera
	*/
	osg::Camera *getActionsCamera()
	{
		return cameraActionsArea.get();
	}

	/**
		méthode de récupération du nom du bouton sur lequel l'on clique
		@param position la position du clique de la souris
	*/
	std::string GameView::obtenirNomBoutonCollision(Point position);

	/**
		tester si le point entre en collision avec un bouton contenu dans le viewport de la caméra des boutons d'actions
		@param position le point à tester
		@return quel bouton a été cliqué
	*/
	Config::Button testerCollisionAvecBouton(Point position);

	/**
		méthode de rafraichissement des boutons
	*/
	void refreshButtons();

	/**
		méthode de rafraichissement de la vue
	*/
	void refreshView();

	/**
		méthode du zoom de la caméra
	*/
	void zoomIn();

	/**
		méthode du dézoom de la caméra
	*/
	void zoomOut();

	/**
		méthode de la rotation vers la gauche de la caméra
	*/
	void turnLeft();

	/**
		méthode de la rotation vers la droite de la caméra
	*/
	void turnRight();

	/**
		méthode de récupération de l'angle de la caméra
		@return l'angle de la caméra
	*/
	int getAngle()
	{
		return currentAngle;
	}

	/**
		méthode créeant une forme de pyramide centrée sur le milieu de la base
		@param size la taille de la base
		@param height la hauteur de la pyramide
		@param color la couleur de la pyramide
		@return la pyramide crée
	*/
	osg::ref_ptr<osg::Geode> createPyramidShape(float size, float height, osg::Vec4 color);
};
