#pragma once

#include <string>
#include <osg\Geode>
#include <osg\MatrixTransform>
#include <osg\ShapeDrawable>

/**
	Classe permettant d'afficher une boîte de dialogue pour saisir un texte
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class InputDialogBox
{
private:
	static InputDialogBox *instance;			// instance de la vue pour le pattern singleton
	osg::ref_ptr<osg::MatrixTransform> dialog;	// contient la fenêtre de dialogue
	std::string title;
	std::string defaultText;
	
	InputDialogBox();							// constructeur privé

public:
	/**
		méthode de récuperation de l'instance de InputDialogBox
		@return l'instance de InputDialogBox
	*/
	static InputDialogBox *getInstance();

	//TODO: documentation
	osg::ref_ptr<osg::MatrixTransform> showDialog(std::string title, std::string defaultText);
};
