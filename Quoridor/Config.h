#pragma once

#include <osg\Vec3>
#include <osg\Vec4>
#include "ArgumentInvalidException.h"

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
#define ESPACEMENT				10														// espacement entre les différentes zones de jeu

// spécificités de la caméra des messages
#define CAMERA_MESSAGE_BOTTOM	-8														// coordonnée du bas du plan de délimitation du message
#define CAMERA_MESSAGE_TOP		8														// coordonnée du haut du plan de délimitation du message
//TODO: changer le 16/200 pour qu'il s'autocalcule via camera_message_bottom etc
#define MESSAGE_HEIGHT			((16.0 / 200) * (FENETRE_WIDTH - (ESPACEMENT * 2)))		// hauteur de la zone des différents messages

// spécificités de la caméra du plateau
#define CAMERA_PLATEAU_BOTTOM	CAMERA_LEFT												// coordonnée du bas du plan de délimitation du plateau
#define CAMERA_PLATEAU_TOP		CAMERA_RIGHT											// coordonnée du haut du plan de délimitation du plateau
#define PLATEAU_TAILLE			(FENETRE_HEIGHT - MESSAGE_HEIGHT - (ESPACEMENT * 3))	// taille du plateau

// spécificités de la caméra des actions
#define ACTIONS_WIDTH			(FENETRE_WIDTH - (PLATEAU_TAILLE + (ESPACEMENT * 3)))	// largeur du viewport des actions
//TODO: changer les 200 en rapport avec left-right camera
#define CAMERA_ACTIONS_BOTTOM	-(((PLATEAU_TAILLE / ACTIONS_WIDTH) * 200) / 2)			// coordonnée du bas du plan de délimitation des actions
#define CAMERA_ACTIONS_TOP		(((PLATEAU_TAILLE / ACTIONS_WIDTH) * 200) / 2)			// coordonnée du haut du plan de délimitation des action

// spécificités des textes
#define FONT_SIZE				5														// taille de la police

/**********\
*          *
* Couleurs *
*          *
\**********/

#define RED		Vec4(  1,   0,   0, 1)	// couleur rouge
#define BLUE	Vec4(  0,   0,   1, 1)	// couleur bleu
#define BROWN	Vec4(.41, .25, .17, 1)	// couleur brune
#define BEIGE	Vec4(.96, .96, .86, 1)	// couleur beige
#define BLACK	Vec4(  0,   0,   0, 1)	// couleur noire


/****************************\
*                            *
* Couleurs des objets du jeu *
*                            *
\****************************/

#define COLOR_PLATEAU			BEIGE	// couleur de plateau
#define COLOR_CASE				BROWN	// couleur des cases
#define COLOR_BARRIERE			BLACK	// couleur des barrières
#define COLOR_VIRTUAL_BARRIERE	RED		// couleur de la barrière virtuelle (servant à choisir un emplacement pour la future barrière)
#define COLOR_PION_1			RED		// couleur du pion 1
#define COLOR_PION_2			BLUE	// couleur du pion 2
#define COLOR_MESSAGE			BLACK	// couleur du message

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
