#pragma once

#include <vector>
#include <osg\AnimationPath>
#include <osg\Camera>
#include <osg\MatrixTransform>
#include <osg\Switch>
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
	Controller();														// constructeur priv�
	
	osg::ref_ptr<osg::MatrixTransform> container;						// sert de container pour afficher les diff�rents �l�ments mobiles
	std::vector<std::vector<osg::ref_ptr<osg::MatrixTransform>>> cases;	// emplacement des cases. La premi�re dimension donne l'emplacement X, la deuxi�me Y
	std::vector<osg::ref_ptr<osg::MatrixTransform>> barrieres[2];		// MatrixTransform des diff�rentes barri�res par joueurs
	osg::ref_ptr<osg::MatrixTransform> pions[2];						// pions sur le jeu
	osg::ref_ptr<osg::MatrixTransform> virtualBarriere;					// sert � afficher une barri�re servant � choisir o� placer la barri�re

	bool isInitialized;													// d�termine si le controlleur a �t� initialis� ou non

	// variables servant � retenir un �tat donn� pour �viter de recalculer les positions des �l�ments qui n'ont pas boug�s
	Point cache_pions[2];												// position actuelle des pions
	Point cache_virtualFence;											// position actuelle de la barri�re virtuelle
	bool  cache_sens;													// sens actuel de la barri�re virtuelle
	int cache_barriereRestante[2];										// nombre actuel de barri�res restante par joueurs

	void computePions();												// calculer la position des pions
	void computeVirtualBarriere();										// calculer la position de la barri�re virtuelle
	void computeBarrieres();											// calculer la position des barri�res

public:
	/**
		m�thode de r�cuperation de l'instance du controller
		@return l'instance du controller
	*/
	static Controller *getInstance();

	/**
		m�thode de d�finition de la barri�re virtuelle
		@param virtualBarriere la nouvelle barri�re virtuelle � d�finir 
	*/
	void setVirtualBarriere(osg::ref_ptr<osg::MatrixTransform> virtualBarriere)
	{
		this->virtualBarriere = virtualBarriere;
	}

	/**
		m�thode de d�finition des cases
		@param position la position de la case sur le plateau
		@param c la Matrix de la case
	*/
	void setCase(Point position, osg::ref_ptr<osg::MatrixTransform> c)
	{
		cases[position.getX()][position.getY()] = c;
	}

	/**
		m�thode de d�finition des barri�res
		@param position la position de la barri�re dans le jeu. Le x d�finit � quel joueur appartient la barri�re, le y la position
		@param barriere la barri�re
	*/
	void setBarriere(Point position, osg::ref_ptr<osg::MatrixTransform> barriere)
	{
		if (position.getX() < 1 || position.getX() > 2)
			throw new ArgumentInvalidException("Num�ro de joueur incorrect");

		barrieres[position.getX() - 1][position.getY()] = barriere;
	}

	/**
		m�thode de d�finition des pions
		@param joueur le joueur
		@param pion le pion
	*/
	void setPion(int joueur, osg::ref_ptr<osg::MatrixTransform> pion)
	{
		if (joueur < 1 || joueur > 2)
			throw new ArgumentInvalidException("Num�ro de joueur incorrect");

		pions[joueur - 1] = pion;
	}

	/**
		m�thode d'initialisation du controlleur en ajoutant les diff�rents �l�ments � la cam�ra et en calculant les diff�rents positions
		@param container le container devant contenir les diff�rents �l�ments mobiles
	*/
	void initialize(osg::ref_ptr<osg::MatrixTransform> container);

	/**
		m�thode d'affichage de la barri�re virtuelle
	*/
	void showVirtualBarriere();

	/**
		m�thode de masquage de la barri�re virtuelle
	*/
	void hideVirtualBarriere();

	/**
		m�thode du calcul toutes les positions
	*/
	void computeAllPositions();

	//TODO: doc
	void recommencerPartie();

	//TODO: doc
	void updateFence(osg::ref_ptr<osg::MatrixTransform> mt, bool reset);
};
