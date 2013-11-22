#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Config.h"
#include "LoadException.h"
#include "Point.h"

/**
	mod�le reprennant les diff�rentes donn�es du programme
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Model
{
	//TODO: faire 2 messages: 1 pour donner � qui c'est le tour et l'autre pour dire les diff�rentes erreurs
public:
	enum Direction { LEFT, RIGHT, UP, DOWN };	// diff�rente direction possible pour le pion
	enum Mode { BARRIERE, PIONS };				// diff�rent mode de jeu possible

private:
	Point pions[2];								// position du pion de chaque joueur. La premi�re dimension est le joueur, la deuxi�me la position X et Y
	std::vector<Point> barrierePlacee[2];		// barri�res plac�es pour chaque joueur
	std::vector<bool> sensBarrierePlacee[2];	// sens des barri�res plac�es pour chaque joueur. true = vertical, false = horizontal
	std::vector<std::vector<bool>> placePrises;	// repr�sente une grille en m�moire permettant de savoir si une place de barri�re est prise ou pas
	Point virtualBarriere;						// position de la barri�re virtuelle si affich�e
	bool sensVirtualBarriere;					// sens de la barri�re virtuelle. true = vertical, false = horizontal
	bool partieTerminee;						// true si la partie est termin�e, sinon false
	int joueurEnCours;							// joueur qui doit jouer
	Mode currentMode;							// mode courant du jeu
	std::string message;						// message � afficher � l'utilisateur

	/**
		m�thode de teste si une barri�re peut �tre positionn�e � un endroit donn�
		@param position la position de la barri�re
		@param vertical le sens de la barri�re. true si vertical, false si horizontal
		@param colision v�rifie si la barri�re n'est pas en colision avec une autre
		@return true si la barri�re peut �tre positionn�e, sinon false
	*/
	bool testerPositionnementBarriere(Point position, bool vertical, bool colision);

	/**
		m�thode de teste si un pion peut �tre d�plac� dans la direction choisie
		@param position position actuelle du pion
		@param direction la direction voulue du pion
		@return true si le pion peut �tre d�plac�, sinon false
	*/
	bool testerPassagePion(Point position, Direction direction);

	static Model *instance;	// instance de la vue pour le pattern singleton
	Model();				// constructeur priv�

public:
	/**
		m�thode de r�cuperation de l'instance du mod�le
		@return l'instance du mod�le
	*/
	static Model *getInstance();

	//TODO documentation
	bool getPartieTerminee()
	{
		return partieTerminee;
	}

	//TODO documentation
	void recommencerPartie()
	{
		delete instance;
		instance = new Model();
	}

	/**
		m�thode de r�cup�ration de la position d'un pion
		@param joueur le joueur du pion
		@return le point du pion
	*/
	Point getPion(int joueur)
	{
		return pions[joueur - 1];
	}

	/**
		m�thode de d�placement du joueur
		@param joueur le joueur qui bouge
		@param direction la direction o� le joueur veut bouger son pion
		@return true si le joueur peut d�placer son pion, false sinon
	*/
	bool deplacerPion(int joueur, Direction direction);

	/**
		m�thode de r�cup�ration du nombre de barri�re � encore placer
		@param joueur le joueur dont on veut le nombre de barri�re
		@return le pnombre de barri�re quel le joueur peut encore placer
	*/
	int getNbrBarriereAPlacer(int joueur)
	{
		return Config::getInstance()->getNbrBarriereJoueur() - barrierePlacee[joueur - 1].size();
	}

	/**
		m�thode de r�cup�ration des positions des barri�res plac�es
		@param joueur le joueur dont on veut les barri�res plac�es
		@return les positions des barri�res plac�es
	*/
	std::vector<Point> getBarrierePlacee(int joueur)
	{
		return barrierePlacee[joueur - 1];
	}

	/**
		m�thode de r�cup�ration du sens des barri�res plac�es
		@param joueur le joueur dont on veut le sens des barri�res plac�es
		@return le sens des barri�res plac�es
	*/
	std::vector<bool> getSensBarrierePlacee(int joueur)
	{
		return sensBarrierePlacee[joueur - 1];
	}

	/**
		m�thode de r�cup�ration du joueur dont c'est le tour
		@return le joueur dont c'est le tour
	*/
	int getJoueurEnCours()
	{
		return joueurEnCours;
	}
	
	/**
		m�thode de d�finition du joueur dont c'est le tour
		@param joueur le joueur dont c'est le tour
	*/
	void setJoueurEnCours(int joueur)
	{
		joueurEnCours = joueur;
		message = "Au tour du joueur " + std::to_string(joueur);
	}

	/**
		m�thode de r�cup�ration du message d'utilisateur
		@return le message d'utilisateur
	*/
	std::string getMessage()
	{
		return message;
	}

	/**
		m�thode de d�finition du message d'utilisateur
		@param nouveauMessage le nouveau message d'utilisateur
	*/
	void setMessage(std::string nouveauMessage)
	{
		message = nouveauMessage;
	}
	
	/**
		m�thode de changement lors d'un placement d'une barri�re
		@param joueur le joueur qui place une barri�re
		@param position la position de la barri�re plac�e
		@param vertical true pour une barri�re vertical, sinon false
		@return true si le joueur peut placer cette barri�re, false sinon
	*/
	bool placerBarriere(int joueur, Point position, bool vertical);

	/**
		obtenir le mode de jeu en cours
		@return le mode de jeu en cours
	*/
	Mode getMode()
	{
		return currentMode;
	}

	/**
		d�finir le mode de jeu
		@param mode le mode de jeu
	*/
	void setMode(Mode mode)
	{
		currentMode = mode;
	}

	/**
		obtenir le sens de la barri�re virtuelle
		@return true si la barri�re est vertical, false si horizontale
	*/
	bool getSensVirtualBarriere()
	{
		return sensVirtualBarriere;
	}

	/**
		obtenir la position de la barri�re virtuelle
		@return la position de la barri�re virtuelle
	*/
	Point getVirtualBarriere()
	{
		return virtualBarriere;
	}

	/**
		d�finir la position de la barri�re virtuelle
		@param position la position de la barri�re virtuelle
		@return true si la barri�re virtuelle peut �tre d�finie � cet endroit, sinon false
	*/
	bool setVirtualBarriere(Point position);

	/**
		d�placer une barri�re virtuelle dans une direction donn�e
		@param direction la direction du d�placement de la barri�re virtuelle
		@return true si la barri�re virtuelle peut �tre d�plac�e � cet endroit, sinon false
	*/
	bool deplacerVirtualBarriere(Direction direction);

	/**
		changer le sens de la barri�re virtuelle
		@return true si le sens de la barri�re virtuelle peut-�tre chang�, sinon false
	*/
	bool changerSensVirtualBarriere();

	/**
		m�thode enregistement de la partie dans un flux
		@param stream le flux o� enregistrer la partie
		@param model la partie � enregistrer
		@return le flux avec les donn�es enregistr�es
	*/
	friend std::ostream &operator<<(std::ostream &stream, Model const &model);
	
	/**
		m�thode de chargement de la partie depuis un flux
		@param stream le flux � lire
		@param model la partie o� envoyer les donn�es
		@return le flux sans les donn�es lues
	*/
	friend std::istream &operator>>(std::istream &stream, Model &model);
};
