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
	background->setShape(new Box(POSITION_CENTRE, 150, 75, 1));
	background->setColor(Vec4(0, 0, 0, .9));

	ref_ptr<Geode> geodeBackground = new Geode();

	stateset = geodeBackground->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, StateAttribute::ON); // activer la transparence
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeBackground->addDrawable(background.get());

	dialog->addChild(geodeBackground.get());

	// création du titre
	title = new osgText::Text();
	title->setCharacterSize(30);
	title->setPosition(Vec3(0, 0, 0));
	title->setText("Coucou");
	title->setColor(RED);

	ref_ptr<Geode> geodeTitle = new Geode();
	
	stateset = geodeTitle->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);

	geodeTitle->addDrawable(title);

	ref_ptr<MatrixTransform> transformTitle = new MatrixTransform();
	transformTitle->addChild(geodeTitle.get());
	transformTitle->setMatrix(Matrix::identity());
	transformTitle->postMult(Matrix::translate(Vec3(0, 0, 2)));

	dialog->addChild(transformTitle.get());
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
