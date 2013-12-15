#pragma once

#include <osg\Vec3>
#include <osg\Vec4>
#include "ArgumentInvalidException.h"

#define ABS(value)  ( (value) >=0 ? (value) : -(value) ) // fonction pour obtenir la valeur absolue d'un nombre donné

/************************\
*                        *
* Configuration générale *
*                        *
\************************/

// fenêtre générale
#define FENETRE_X				50														// position x de la fenêtre à l'écran
#define FENETRE_Y				50														// position y de la fenêtre à l'écran
#define FENETRE_HEIGHT			700														// hauteur de la fenêtre
#define FENETRE_WIDTH			800														// largeur de la fenêtre

// centre de l'univers
#define POSITION_CENTRE			Vec3(0, 0, 0)											// position du centre

// caméra générale + espacement des différentes zones
#define CAMERA_NEAR				0.5														// distance d'affichage minimal de la caméra
#define CAMERA_FAR				5														// distance d'affichage maximal de la caméra
#define CAMERA_LEFT				-100													// coordonnée de gauche du plan de délimitation
#define CAMERA_RIGHT			100														// coordonnée de droite du plan de délimitation
#define WIDTH_CAMERA			(ABS(CAMERA_LEFT) + ABS(CAMERA_RIGHT))					// largeur du plan de délimitation
#define ESPACEMENT				10														// espacement entre les différentes zones de jeu
#define LIMIT_ORTHO				25														// limite du zoom en avant et en arrière de la caméra en vue parallèle
#define LIMIT_PERSPECTIVE_MIN	Vec3(0, -85, 105)										// limite du zoom en avant de la caméra en vue perspective (le x n'est pas prit en compte)
#define LIMIT_PERSPECTIVE_MAX	Vec3(0, -165, 185)										// limite du zoom en arrière de la caméra en vue perspective (le x n'est pas prit en compte)


// différentes positions d'éléments
#define EYE_PERSPECTIVE			Vec3(0, -130, 150)										//emplacement de l'oeil en perspective
#define EYE_ORTHO				Vec3(0, 0, 1)											//emplacement de l'oeil en orthogonale
#define POS_MESSAGE_USER	Vec3(-98, 1, 0)												//emplacement du message de joueur qui joue
#define POS_MESSAGE_ERROR	Vec3(-98, -5.5, 0)											//emplacement du message d'avertissement d'erreur
#define POS_CONT_FLECHES	Vec3(0, 270, 0)												//position du conteneur des fleches
#define POS_FLECHE_GAUCHE	Vec3(0, 60, 0)												//position de la flèche gauche


// spécificités de la caméra des messages
#define CAMERA_MESSAGE_BOTTOM	-8														// coordonnée du bas du plan de délimitation des messages
#define CAMERA_MESSAGE_TOP		8														// coordonnée du haut du plan de délimitation des messages
#define HEIGHT_CAMERA_MESSAGE	(ABS(CAMERA_MESSAGE_BOTTOM) + ABS(CAMERA_MESSAGE_TOP))	// hauteur du plan de délimitation des messages
#define MESSAGE_HEIGHT			(((HEIGHT_CAMERA_MESSAGE + 0.0) / WIDTH_CAMERA) \
									* (FENETRE_WIDTH - (ESPACEMENT * 2)))				// hauteur de la zone des différents messages

// spécificités de la caméra du plateau
#define CAMERA_PLATEAU_BOTTOM	CAMERA_LEFT												// coordonnée du bas du plan de délimitation du plateau
#define CAMERA_PLATEAU_TOP		CAMERA_RIGHT											// coordonnée du haut du plan de délimitation du plateau
#define PLATEAU_TAILLE			(FENETRE_HEIGHT - MESSAGE_HEIGHT - (ESPACEMENT * 3))	// taille du plateau

// spécificités de la caméra des actions
#define ACTIONS_WIDTH			(FENETRE_WIDTH - (PLATEAU_TAILLE + (ESPACEMENT * 3)))	// largeur du viewport des actions
#define CAMERA_ACTIONS_BOTTOM	-(((PLATEAU_TAILLE / ACTIONS_WIDTH) * WIDTH_CAMERA) \
									/ 2.0)												// coordonnée du bas du plan de délimitation des actions
#define CAMERA_ACTIONS_TOP		(((PLATEAU_TAILLE / ACTIONS_WIDTH) * WIDTH_CAMERA) \
									/ 2.0)												// coordonnée du haut du plan de délimitation des action


//spécificités des différents éléments
#define TAILLE_BOUTON			70														//taille des boutons
#define HAUTEUR_PLATEAU			5														//hauteur du plateau
#define HAUTEUR_CASE			8														//hauteur des cases
#define HAUTEUR_BARRIERE		18														//hauteur des barrieres
#define HAUTEUR_PION			20														//hauteur des pions

//angle
#define MAX_ANGLE_HAUT			225														//maximum de l'angle pour regarder vers le haut
#define MAX_ANGLE_BAS			45														//maximum de l'angle pour regarder vers le bas
#define MAX_ANGLE_DROITE		135														//maximum de l'angle pour regarder vers la droite
#define MAX_ANGLE_GAUCHE		315														//maximum de l'angle pour regarder vers la gauche

// spécificités des textes
#define FONT_SIZE				5														// taille de la police

/**********\
*          *
* Couleurs *
*          *
\**********/

#define TRANSPARENT	Vec4(  0,   0,   0, 0)	// couleur transparente
#define RED			Vec4(  1,   0,   0, 1)	// couleur rouge
#define BLUE		Vec4(  0,   0,   1, 1)	// couleur bleu
#define BROWN		Vec4(.41, .25, .17, 1)	// couleur brune
#define GREY		Vec4(.13, .13, .13, 1)	// couleur grise
#define WHITE		Vec4(  1,   1,   1, 1)	// couleur blanche
#define BLACK		Vec4(  0,   0,   0, 1)	// couleur noire


/****************************\
*                            *
* Couleurs des objets du jeu *
*                            *
\****************************/

#define COLOR_PLATEAU			WHITE	// couleur de plateau
#define COLOR_CASE				BROWN	// couleur des cases
#define COLOR_BARRIERE			BLACK	// couleur des barrières
#define COLOR_VIRTUAL_BARRIERE	RED		// couleur de la barrière virtuelle (servant à choisir un emplacement pour la future barrière)
#define COLOR_PION_1			RED		// couleur du pion 1
#define COLOR_PION_2			BLUE	// couleur du pion 2
#define COLOR_MESSAGE			WHITE	// couleur du message
#define COLOR_BORDER			GREY	// couleur des bordures

/**********************\
*                      *
* Configuration du jeu *
*                      *
\**********************/

/**
	classe servant à obtenir toute la configuration initiale du jeu
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Config
{
private:
	static Config *instance;	// instance de la config pour le pattern singleton
	Config();					// constructeur privé

	int nbrCases;				// nombre de case par ligne / colonne sur le plateau de jeu. Doit être un nombre impair
	int nbrBarriereJoueur;		// nombre de barrière par joueur
	int taillePlateau;			// taille du plateau (maximum 200)
	int tailleCase;				// taille d'une case
	int tailleZoneRangement;	// taille de la zone de rangement
	int tailleRainure;			// taille d'une rainure
	int taillePion;				// taille d'un pion
	int tailleBarriere;			// taille d'une barrière

	std::string saveFileName;	// nom du fichier de sauvegarde

public:
	enum Button { UNKNOWN, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, MODE,
				  DIRECTION, VALIDATE, LOAD, SAVE, RESTART, UNDO, VIEW,
				  ZOOM_IN, ZOOM_OUT, TURN_LEFT, TURN_RIGHT }; // types de bouton

	/**
		méthode de récupération de l'instance de la config
		@return l'instance de la config
	*/
	static Config *getInstance();

	// getters ********************************************

	/**
		méthode de récupération du nombre de cases
		@return le nombre de cases d'une ligne de plateau
	*/
	int getNbrCases()
	{
		return nbrCases;
	}

	/**
		méthode de récupération du nombre de barrière par joueur
		@return le nombre de barrière par joueur
	*/
	int getNbrBarriereJoueur()
	{
		return nbrBarriereJoueur;
	}

	/**
		méthode de récupération de la taille du plateau
		@return la taille du plateau
	*/
	int getTaillePlateau()
	{
		return taillePlateau;
	}

	/**
		méthode de récupération de la taille des cases
		@return la taille des cases
	*/
	int getTailleCase()
	{
		return tailleCase;
	}

	/**
		méthode de récupération de la taille des zone de rangement des barrières
		@return la taille des zone de rangement des barrières
	*/
	int getTailleZoneRangement()
	{
		return tailleZoneRangement;
	}

	/**
		méthode de récupération de la taille des rainure entre les cases
		@return la taille des rainure entre les cases
	*/
	int getTailleRainure()
	{
		return tailleRainure;
	}

	/**
		méthode de récupération de la taille des pions
		@return la taille des pions
	*/
	int getTaillePion()
	{
		return taillePion;
	}

	/**
		méthode de récupération de la taille des barrières
		@return la taille des barrières
	*/
	int getTailleBarriere()
	{
		return tailleBarriere;
	}

	/**
		méthode de récupération du nom de fichier de sauvegarde
		@return le nom du fichier de sauvegarde
	*/
	std::string getSaveFileName()
	{
		return saveFileName;
	}

	// setters ********************************************

	/**
		méthode de définition du nombre de cases d'une ligne du plateau
		@param nbr le nombre de cases d'une ligne du plateau
	*/
	void setNbrCases(int nbr);

	/**
		méthode de définition du nom de fichier de sauvegarde
		@param nbr le nom du fichier de sauvegarde
	*/
	void setSaveFileName(std::string name);
};
