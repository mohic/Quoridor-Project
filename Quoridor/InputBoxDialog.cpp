#include "InputBoxDialog.h"

using namespace osg;

InputBoxDialog *InputBoxDialog::instance = 0;

InputBoxDialog::InputBoxDialog()
{
	StateSet *stateset;

	// initialisation des variables internes
	dialog = new MatrixTransform();
	//title = "";
	//defaultText = "";

	// créer le background
	ref_ptr<ShapeDrawable> background = new ShapeDrawable();
	background->setShape(new Box(Vec3(0, 0, 1), 100, 50, 1));
	background->setColor(Vec4(0, 0, 0, .9));

	ref_ptr<Geode> geodeBackground = new Geode();

	stateset = geodeBackground->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, StateAttribute::ON); // activer la transparence
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeBackground->addDrawable(background.get());

	dialog->addChild(geodeBackground);
}

ref_ptr<MatrixTransform> InputBoxDialog::showDialog(std::string title, std::string defaultText)
{
	//TODO: __
	return dialog;
}

InputBoxDialog *InputBoxDialog::getInstance()
{
	if (instance == 0)
		instance = new InputBoxDialog();

	return instance;
}
