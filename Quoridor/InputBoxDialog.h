#pragma once

#include <string>
#include <osg\Geode>
#include <osg\MatrixTransform>
#include <osg\ShapeDrawable>
#include <osgText\Text>
#include "Config.h"

/**
	Classe permettant d'afficher une bo�te de dialogue pour saisir un texte
	@author LECAT Maxime
	@author MOHIMONT Christophe
*/
class InputBoxDialog
{
private:
	static InputBoxDialog *instance;			// instance de la vue pour le pattern singleton

	osg::ref_ptr<osg::MatrixTransform> dialog;	// contient la fen�tre de dialogue
	osg::ref_ptr<osgText::Text> title;			// objet texte du titre
	osg::ref_ptr<osgText::Text> defaultText;	// objet texte du texte par d�faut
	
	InputBoxDialog();							// constructeur priv�

public:
	/**
		m�thode de r�cuperation de l'instance de InputDialogBox
		@return l'instance de InputDialogBox
	*/
	static InputBoxDialog *getInstance();

	//TODO: documentation
	osg::ref_ptr<osg::MatrixTransform> showDialog(std::string title, std::string defaultText);
};
