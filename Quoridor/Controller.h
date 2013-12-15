#pragma once

#include <vector>
#include <osg\MatrixTransform>
#include <osg\Camera>
#include "Config.h"
#include "Model.h"
#include "Point.h"
#include "StateException.h"

/**
	Controlleur du jeu
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Controller
{
private:
	static Controller *instance;										// instance de la vue pour le pattern singleton
	Controller();														// constructeur privé
	
	osg::ref_ptr<osg::MatrixTransform> container;						// sert de container pour afficher les différents éléments mobiles
	std::vector<std::vector<osg::ref_ptr<osg::MatrixTransform>>> cases;	// emplacement des cases. La première dimension donne l'emplacement X, la deuxième Y
	std::vector<osg::ref_ptr<osg::MatrixTransform>> barrieres[2];		// MatrixTransform des différentes barrières par joueurs
	osg::ref_ptr<osg::MatrixTransform> pions[2];						// pions sur le jeu
	osg::ref_ptr<osg::MatrixTransform> virtualBarriere;					// sert à afficher une barrière servant à choisir où placer la barrière

	bool isInitialized;													// détermine si le controlleur a été initialisé ou non

	void computePions();												// calculer la position des pions
	void computeVirtualBarriere();										// calculer la position de la barrière virtuelle
	void computeBarrieres();											// calculer la position des barrières

public:
	/**
		méthode de récuperation de l'instance du controller
		@return l'instance du controller
	*/
	static Controller *getInstance();

	/**
		méthode de définition de la barrière virtuelle
		@param virtualBarriere la nouvelle barrière virtuelle à définir 
	*/
	void setVirtualBarriere(osg::ref_ptr<osg::MatrixTransform> virtualBarriere)
	{
		this->virtualBarriere = virtualBarriere;
	}

	/**
		méthode de définition des cases
		@param position la position de la case sur le plateau
		@param c la Matrix de la case
	*/
	void setCase(Point position, osg::ref_ptr<osg::MatrixTransform> c)
	{
		cases[position.getX()][position.getY()] = c;
	}

	/**
		méthode de définition des barrières
		@param position la position de la barrière dans le jeu. Le x définit à quel joueur appartient la barrière, le y la position
		@param barriere la barrière
	*/
	void setBarriere(Point position, osg::ref_ptr<osg::MatrixTransform> barriere)
	{
		if (position.getX() < 1 || position.getX() > 2)
			throw new ArgumentInvalidException("Numéro de joueur incorrect");

		barrieres[position.getX() - 1][position.getY()] = barriere;
	}

	/**
		méthode de définition des pions
		@param joueur le joueur
		@param pion le pion
	*/
	void setPion(int joueur, osg::ref_ptr<osg::MatrixTransform> pion)
	{
		if (joueur < 1 || joueur > 2)
			throw new ArgumentInvalidException("Numéro de joueur incorrect");

		pions[joueur - 1] = pion;
	}

	/**
		initialise le controlleur en ajoutant les différents éléments à la caméra et en calculant les différents positions
		@param container le container devant contenir les différents éléments mobiles
	*/
	void initialize(osg::ref_ptr<osg::MatrixTransform> container);

	/**
		afficher la barrière virtuelle
	*/
	void showVirtualBarriere();

	/**
		cacher la barrière virtuelle
	*/
	void hideVirtualBarriere();

	/**
		calculer toutes les positions
	*/
	void computeAllPositions();
};
