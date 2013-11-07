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
	Controller();														// constructeur privé
	
	osg::ref_ptr<osg::Camera> camera;									// camera devant afficher les différents éléments mobile
	std::vector<std::vector<osg::ref_ptr<osg::MatrixTransform>>> cases;	// emplacement des cases. La première dimension donne l'emplacement X, la deuxième Y
	std::vector<osg::ref_ptr<osg::MatrixTransform>> barrieres[2];		// MatrixTransform des différentes barrières par joueurs
	osg::ref_ptr<osg::MatrixTransform> pions[2];						// pions sur le jeu
	osg::ref_ptr<osg::MatrixTransform> virtualBarriere;					// sert à afficher une barrière servant à choisir où placer la barrière

public:
	/**
		méthode de récuperation de l'instance du controller
		@return l'instance du controller
	*/
	static Controller *getInstance();

	/**
		méthode de définition de la barrière virtuelle
		@param node la nouvelle barrière virtuelle à définir 
	*/
	void setVirtualBarriere(osg::ref_ptr<osg::Node> node);

	/**
		méthode de définition de la caméra
		@param camera la nouvelle caméra à définir 
	*/
	void setCamera(osg::ref_ptr<osg::Camera> camera);

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
		méthode de récupération des pion
		@return le vecteur de la MatrixTransform des pions
	*/
	std::vector<osg::ref_ptr<osg::MatrixTransform>> getPions();

	/**
		méthode de récupération des barrières
		@return le vecteur de la MatrixTransform des barrières
	*/
	std::vector<osg::ref_ptr<osg::MatrixTransform>> getBarrieres();

	/**
		méthode de récupération de la barrière virtuelle
		@return le MatrixTransform de la barrière virtuelle
	*/
	osg::ref_ptr<osg::MatrixTransform> getVirtualBarriere();

	/**
		méthode de suppression d'une barrière virtuelle de l'affichage
	*/
	void removeVirtualBarriere();
};
