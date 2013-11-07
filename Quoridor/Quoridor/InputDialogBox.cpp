#include "InputDialogBox.h"

using namespace osg;

static InputDialogBox *instance = 0;

InputDialogBox::InputDialogBox()
{
	StateSet *stateset;

	// initialisation des variables internes
	dialog = new MatrixTransform();
	title = "";
	defaultText = "";

	// créer le background
	ref_ptr<ShapeDrawable> background = new ShapeDrawable();
	background->setShape(new Box(Vec3(0, 0, 0), 100, 50, 1));
	background->setColor(Vec4(0, 0, 0, .9));

	ref_ptr<Geode> geodeBackground = new Geode();

	stateset = geodeBackground->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, StateAttribute::ON); // activer la transparence
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeBackground->addDrawable(background.get());

	dialog->addChild(geodeBackground);
}

InputDialogBox *InputDialogBox::getInstance()
{
	if (instance == 0)
		instance = new InputDialogBox();

	return instance;
}
