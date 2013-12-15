#pragma once
#pragma warning (disable : 4250) // supprime le warning venant de OSG avec l'include osgDB\readFile
								 // qui avertit que tel classe h�rite de l'autre classe par dominance

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
	GameView();											// constructeur priv�

	osg::ref_ptr<osgText::Text> userMessage;			// message � afficher � l'utilisateur
	osg::ref_ptr<osgText::Text> errorMessage;			// message d'erreur � afficher � l'utilisateur

	osg::Vec3 center;									// centre point� par la cam�ra
	osg::Vec3 up;										// orientation de la cam�ra
	osg::Vec3 eye;										// position de la cam�ra pour les vues non jeu
	osg::Vec3 eyeOrtho;									// position de la cam�ra en vue orthographique
	int offset;											// sert pour le zoom da la cam�ra en vue parall�le
	osg::Vec3 eyePerspective;							// position de la cam�ra en vue perspective

	osg::ref_ptr<osgViewer::Viewer> viewer;				// le viewer

	osg::ref_ptr<osg::Camera> cameraGameArea;			// cam�ra servant � afficher la zone de jeu
	osg::ref_ptr<osg::Camera> cameraDisplayArea;		// cam�ra de la zone de texte
	osg::ref_ptr<osg::Camera> cameraActionsArea;		// cam�ra de la zone des bouton d'actions

	osg::ref_ptr<osg::Geode> geodeVirtualFence;			// g�ode de la barri�re virtuelle
	osg::ref_ptr<osg::Geode> classicButton;				// bouton classique

	int currentAngle;									// retient l'angle actuel pour modifier la configuration des touches directionnelle en fonction de l'angle
	osg::ref_ptr<osg::MatrixTransform> arrows;			// ensemble des fl�ches + le bouton changement de direction
	osg::ref_ptr<osg::MatrixTransform> rotate;			// sert pour la rotation du plateau de jeu

	void initMessage();									// initialise le message utilisateur

	void createAndConfigureViewer();					// cr�e et configure le viewer
	void createAndConfigureCameraGameArea();			// cr�e et configure la cam�ra repr�sentant la zone de jeu
	void createAndConfigureCameraDisplayArea();			// cr�e et configure la cam�ra repr�sentant la zone d'affichage
	void createAndConfigureCameraActionsArea();			// cr�e et configure la cam�ra repr�sentant la zone des actions

	void createClassicButton();							// cr�e un bouton classique

	void drawPlate();									// dessine le plateau
	void drawCases();									// dessine les cases
	void drawFences();									// dessine les barri�res
	void drawVirtualFence();							// dessine la barri�re virtuelle
	void drawPawns();									// dessine les pions
	void drawMessage();									// dessine le message
	void drawArrowAndDirectionButtons();				// dessine les boutons fl�ch�s et le bouton changer de sens
	void drawGameButtons();								// dessine les diff�rents boutons servant � jouer
	void drawSaveAndLoadButtons();						// dessine les diff�rents boutons servant � l'enregistrement et au chargement du jeu
	void drawViewButtons();								// dessine les diff�rents boutons servant � la gestion de la vue du jeu

	Config::Button checkButton(std::string name);		// v�rifie quel bouton � �t� cliqu�
	void changeTexture(osg::ref_ptr<osg::Switch> sw,
						std::string textureName);		// change la texture des diff�rents �l�ments du switch

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
		m�thode de r�cup�ration de la camera
	*/
	osg::Camera *getActionsCamera()
	{
		return cameraActionsArea.get();
	}

	/**
		m�thode de r�cup�ration du nom du bouton sur lequel l'on clique
		@param position la position du clique de la souris
	*/
	std::string GameView::obtenirNomBoutonCollision(Point position);

	/**
		tester si le point entre en collision avec un bouton contenu dans le viewport de la cam�ra des boutons d'actions
		@param position le point � tester
		@return quel bouton a �t� cliqu�
	*/
	Config::Button testerCollisionAvecBouton(Point position);

	/**
		m�thode de rafraichissement des boutons
	*/
	void refreshButtons();

	/**
		m�thode de rafraichissement de la vue
	*/
	void refreshView();

	/**
		m�thode du zoom de la cam�ra
	*/
	void zoomIn();

	/**
		m�thode du d�zoom de la cam�ra
	*/
	void zoomOut();

	/**
		m�thode de la rotation vers la gauche de la cam�ra
	*/
	void turnLeft();

	/**
		m�thode de la rotation vers la droite de la cam�ra
	*/
	void turnRight();

	/**
		m�thode de r�cup�ration de l'angle de la cam�ra
		@return l'angle de la cam�ra
	*/
	int getAngle()
	{
		return currentAngle;
	}

	/**
		m�thode cr�eant une forme de pyramide centr�e sur le milieu de la base
		@param size la taille de la base
		@param height la hauteur de la pyramide
		@param color la couleur de la pyramide
		@return la pyramide cr�e
	*/
	osg::ref_ptr<osg::Geode> createPyramidShape(float size, float height, osg::Vec4 color);
};
