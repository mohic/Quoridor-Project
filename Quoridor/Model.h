#pragma once

//#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "Config.h"
#include "LoadException.h"
#include "Point.h"

/**
	modèle reprennant les différentes données du programme
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class Model
{
public:
	enum Direction { LEFT, RIGHT, UP, DOWN };	// différente direction possible pour le pion
	enum Mode { BARRIERE, PIONS };				// différent mode de jeu possible

private:
	Point pions[2];								// position du pion de chaque joueur. La première dimension est le joueur, la deuxième la position X et Y
	std::vector<Point> barrierePlacee[2];		// barrières placées pour chaque joueur
	std::vector<bool> sensBarrierePlacee[2];	// sens des barrières placées pour chaque joueur. true = vertical, false = horizontal
	std::vector<std::vector<bool>> placePrises;	// représente une grille en mémoire permettant de savoir si une place de barrière est prise ou pas
	Point virtualBarriere;						// position de la barrière virtuelle si affichée
	bool sensVirtualBarriere;					// sens de la barrière virtuelle. true = vertical, false = horizontal
	bool partieTerminee;						// true si la partie est terminée, sinon false
	int joueurEnCours;							// joueur qui doit jouer
	Mode currentMode;							// mode courant du jeu
	std::string userMessage;					// message à afficher à l'utilisateur
	std::string	errorMessage;					// message d'erreur à afficher à l'utilisateur

	std::vector<Point> caseVerifiee;			// case déjà vérifiée lors de l'analyse du bloquage d'un pion

	/**
		méthode testant si les pions peuvent atteindre l'arrivée
		@return true si tous les pions peuvent atteindre la ligne d'arrivée, sinon false
	*/
	bool testerBlocagePions();

	/**
		méthode servant pour le test si un pion est bloqué de façon récursive
		@param joueur Le joueur à tester
		@parem caseATester case à tester
		@return true si le pion n'est pas bloqué, sinon false
	*/
	bool testerBlocagePion(int joueur, Point caseATester);

	/**
		méthode de teste si une barrière peut être positionnée à un endroit donné
		@param position la position de la barrière
		@param vertical le sens de la barrière. true si vertical, false si horizontal
		@param colision vérifie si la barrière n'est pas en colision avec une autre
		@return true si la barrière peut être positionnée, sinon false
	*/
	bool testerPositionnementBarriere(Point position, bool vertical, bool colision);

	/**
		méthode de teste si un pion peut être déplacé dans la direction choisie
		@param position position actuelle du pion
		@param direction la direction voulue du pion
		@return true si le pion peut être déplacé, sinon false
	*/
	bool testerPassagePion(Point position, Direction direction);

	static Model *instance;	// instance de la vue pour le pattern singleton
	Model();				// constructeur privé

public:
	/**
		méthode de récuperation de l'instance du modèle
		@return l'instance du modèle
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
		méthode de récupération de la position d'un pion
		@param joueur le joueur du pion
		@return le point du pion
	*/
	Point getPion(int joueur)
	{
		return pions[joueur - 1];
	}

	/**
		méthode de déplacement du joueur
		@param joueur le joueur qui bouge
		@param direction la direction où le joueur veut bouger son pion
		@return true si le joueur peut déplacer son pion, false sinon
	*/
	bool deplacerPion(int joueur, Direction direction);

	/**
		méthode de récupération du nombre de barrière à encore placer
		@param joueur le joueur dont on veut le nombre de barrière
		@return le pnombre de barrière quel le joueur peut encore placer
	*/
	int getNbrBarriereAPlacer(int joueur)
	{
		return Config::getInstance()->getNbrBarriereJoueur() - barrierePlacee[joueur - 1].size();
	}

	/**
		méthode de récupération des positions des barrières placées
		@param joueur le joueur dont on veut les barrières placées
		@return les positions des barrières placées
	*/
	std::vector<Point> getBarrierePlacee(int joueur)
	{
		return barrierePlacee[joueur - 1];
	}

	/**
		méthode de récupération du sens des barrières placées
		@param joueur le joueur dont on veut le sens des barrières placées
		@return le sens des barrières placées
	*/
	std::vector<bool> getSensBarrierePlacee(int joueur)
	{
		return sensBarrierePlacee[joueur - 1];
	}

	/**
		méthode de récupération du joueur dont c'est le tour
		@return le joueur dont c'est le tour
	*/
	int getJoueurEnCours()
	{
		return joueurEnCours;
	}
	
	/**
		méthode de définition du joueur dont c'est le tour
		@param joueur le joueur dont c'est le tour
	*/
	void setJoueurEnCours(int joueur)
	{
		joueurEnCours = joueur;

		if (!partieTerminee) {
			userMessage = "Au tour du joueur " + std::to_string(joueur);
			errorMessage = "";
		}
	}

	/**
		méthode de récupération du message d'utilisateur
		@return le message d'utilisateur
	*/
	std::string getUserMessage()
	{
		return userMessage;
	}

	/**
		méthode de récupération du message d'erreur
		@return le message d'erreur
	*/
	std::string getErrorMessage()
	{
		return errorMessage;
	}

	/**
		méthode de définition du message d'utilisateur
		@param nouveauMessage le nouveau message d'utilisateur
	*/
	void setUserMessage(std::string message)
	{
		userMessage = message;
	}

	/**
		méthode de définition du message d'erreur
		@param nouveauMessage le nouveau message d'erreur
	*/
	void setErrorMessage(std::string message)
	{
		errorMessage = message;
	}
	
	/**
		méthode de changement lors d'un placement d'une barrière
		@param joueur le joueur qui place une barrière
		@param position la position de la barrière placée
		@param vertical true pour une barrière vertical, sinon false
		@return true si le joueur peut placer cette barrière, false sinon
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
		définir le mode de jeu
		@param mode le mode de jeu
	*/
	void setMode(Mode mode)
	{
		currentMode = mode;
	}

	/**
		obtenir le sens de la barrière virtuelle
		@return true si la barrière est vertical, false si horizontale
	*/
	bool getSensVirtualBarriere()
	{
		return sensVirtualBarriere;
	}

	/**
		obtenir la position de la barrière virtuelle
		@return la position de la barrière virtuelle
	*/
	Point getVirtualBarriere()
	{
		return virtualBarriere;
	}

	/**
		définir la position de la barrière virtuelle
		@param position la position de la barrière virtuelle
		@return true si la barrière virtuelle peut être définie à cet endroit, sinon false
	*/
	bool setVirtualBarriere(Point position);

	/**
		déplacer une barrière virtuelle dans une direction donnée
		@param direction la direction du déplacement de la barrière virtuelle
		@return true si la barrière virtuelle peut être déplacée à cet endroit, sinon false
	*/
	bool deplacerVirtualBarriere(Direction direction);

	/**
		changer le sens de la barrière virtuelle
		@return true si le sens de la barrière virtuelle peut-être changé, sinon false
	*/
	bool changerSensVirtualBarriere();

	/**
		méthode enregistement de la partie dans un flux
		@param stream le flux où enregistrer la partie
		@param model la partie à enregistrer
		@return le flux avec les données enregistrées
	*/
	friend std::ostream &operator<<(std::ostream &stream, Model const &model);
	
	/**
		méthode de chargement de la partie depuis un flux
		@param stream le flux à lire
		@param model la partie où envoyer les données
		@return le flux sans les données lues
	*/
	friend std::istream &operator>>(std::istream &stream, Model &model);
};
