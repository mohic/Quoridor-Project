#pragma once

#include <osg/Vec4>
#include "ArgumentInvalidException.h"

/************************\
*                        *
* Configuration g�n�rale *
*                        *
\************************/
#define FENETRE_X		50	// position x de la fen�tre � l'�cran
#define FENETRE_Y		50	// position y de la fen�tre � l'�cran
#define FENETRE_HEIGHT	900	// hauteur de la fen�tre
#define FENETRE_WIDTH	800	// largeur de la fen�tre

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
#define COLOR_BARRIERE			BLACK	// couleur des barri�res
#define COLOR_VIRTUAL_BARRIERE	RED		// couleur de la barri�re virtuelle (servant � choisir un emplacement pour la future barri�re)
#define COLOR_PION_1			RED		// couleur du pion 1
#define COLOR_PION_2			BLUE	// couleur du pion 2
#define COLOR_MESSAGE			BLACK	// couleur du message

/**********************\
*                      *
* Configuration du jeu *
*                      *
\**********************/

/**
	classe servant � obtenir toute la configuration initiale du jeu
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Config
{
private:
	static Config *instance;	// instance de la config pour le pattern singleton
	Config();					// constructeur priv�

	int nbrCases;				// nombre de case par ligne / colonne sur le plateau de jeu. Doit �tre un nombre impair
	int nbrBarriereJoueur;		// nombre de barri�re par joueur
	int taillePlateau;			// taille du plateau (maximum 200)
	int tailleCase;				// taille d'une case
	int tailleZoneRangement;	// taille de la zone de rangement
	int tailleRainure;			// taille d'une rainure
	int taillePion;				// taille d'un pion
	int tailleBarriere;			// taille d'une barri�re

	std::string saveFileName;	// nom du fichier de sauvegarde

public:
	/**
		m�thode de r�cup�ration de l'instance de la config
		@return l'instance de la config
	*/
	static Config *getInstance();

	// getters ********************************************

	/**
		m�thode de r�cup�ration du nombre de cases
		@return le nombre de cases d'une ligne de plateau
	*/
	int getNbrCases()
	{
		return nbrCases;
	}

	/**
		m�thode de r�cup�ration du nombre de barri�re par joueur
		@return le nombre de barri�re par joueur
	*/
	int getNbrBarriereJoueur()
	{
		return nbrBarriereJoueur;
	}

	/**
		m�thode de r�cup�ration de la taille du plateau
		@return la taille du plateau
	*/
	int getTaillePlateau()
	{
		return taillePlateau;
	}

	/**
		m�thode de r�cup�ration de la taille des cases
		@return la taille des cases
	*/
	int getTailleCase()
	{
		return tailleCase;
	}

	/**
		m�thode de r�cup�ration de la taille des zone de rangement des barri�res
		@return la taille des zone de rangement des barri�res
	*/
	int getTailleZoneRangement()
	{
		return tailleZoneRangement;
	}

	/**
		m�thode de r�cup�ration de la taille des rainure entre les cases
		@return la taille des rainure entre les cases
	*/
	int getTailleRainure()
	{
		return tailleRainure;
	}

	/**
		m�thode de r�cup�ration de la taille des pions
		@return la taille des pions
	*/
	int getTaillePion()
	{
		return taillePion;
	}

	/**
		m�thode de r�cup�ration de la taille des barri�res
		@return la taille des barri�res
	*/
	int getTailleBarriere()
	{
		return tailleBarriere;
	}

	/**
		m�thode de r�cup�ration du nom de fichier de sauvegarde
		@return le nom du fichier de sauvegarde
	*/
	std::string getSaveFileName()
	{
		return saveFileName;
	}

	// setters ********************************************

	/**
		m�thode de d�finition du nombre de cases d'une ligne du plateau
		@param nbr le nombre de cases d'une ligne du plateau
	*/
	void setNbrCases(int nbr);

	/**
		m�thode de d�finition du nom de fichier de sauvegarde
		@param nbr le nom du fichier de sauvegarde
	*/
	void setSaveFileName(std::string name);
};