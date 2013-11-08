#include "InputBoxDialog.h"

using namespace osg;

InputBoxDialog *InputBoxDialog::instance = 0;

InputBoxDialog::InputBoxDialog()
{
	StateSet *stateset;

	// initialisation des variables internes
	dialog = new MatrixTransform();

	// créer le background
	ref_ptr<ShapeDrawable> background = new ShapeDrawable();
	background->setShape(new Box(POSITION_CENTRE, 100, 50, 1));
	background->setColor(Vec4(0, 0, 0, .9));

	ref_ptr<Geode> geodeBackground = new Geode();

	stateset = geodeBackground->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, StateAttribute::ON); // activer la transparence
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeBackground->addDrawable(background.get());

	dialog->addChild(geodeBackground);
	// position dialog z + 1

	//
	//message = new osgText::Text();
	//message->setCharacterSize(3);
	//message->setPosition(Vec3(-98, -1.5, 0));
	//message->setText(Model::getInstance()->getMessage());
	//message->setColor(COLOR_MESSAGE);
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
