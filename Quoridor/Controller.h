#pragma once

#include <vector>
#include <osg\MatrixTransform>
#include <osg\Camera>
#include "Config.h"
#include "Model.h"
#include "Point.h"

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
	
	osg::ref_ptr<osg::Camera> camera;									// camera devant afficher les diff�rents �l�ments mobile
	std::vector<std::vector<osg::ref_ptr<osg::MatrixTransform>>> cases;	// emplacement des cases. La premi�re dimension donne l'emplacement X, la deuxi�me Y
	std::vector<osg::ref_ptr<osg::MatrixTransform>> barrieres[2];		// MatrixTransform des diff�rentes barri�res par joueurs
	osg::ref_ptr<osg::MatrixTransform> pions[2];						// pions sur le jeu
	osg::ref_ptr<osg::MatrixTransform> virtualBarriere;					// sert � afficher une barri�re servant � choisir o� placer la barri�re

public:
	/**
		m�thode de r�cuperation de l'instance du controller
		@return l'instance du controller
	*/
	static Controller *getInstance();

	/**
		m�thode de d�finition de la barri�re virtuelle
		@param node la nouvelle barri�re virtuelle � d�finir 
	*/
	void setVirtualBarriere(osg::ref_ptr<osg::Node> node);

	/**
		m�thode de d�finition de la cam�ra
		@param camera la nouvelle cam�ra � d�finir 
	*/
	void setCamera(osg::ref_ptr<osg::Camera> camera);

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
		m�thode de r�cup�ration des pion
		@return le vecteur de la MatrixTransform des pions
	*/
	std::vector<osg::ref_ptr<osg::MatrixTransform>> getPions();

	/**
		m�thode de r�cup�ration des barri�res
		@return le vecteur de la MatrixTransform des barri�res
	*/
	std::vector<osg::ref_ptr<osg::MatrixTransform>> getBarrieres();

	/**
		m�thode de r�cup�ration de la barri�re virtuelle
		@return le MatrixTransform de la barri�re virtuelle
	*/
	osg::ref_ptr<osg::MatrixTransform> getVirtualBarriere();

	/**
		m�thode de suppression d'une barri�re virtuelle de l'affichage
	*/
	void removeVirtualBarriere();
};
