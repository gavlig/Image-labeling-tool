/*
 * ImageDescriptionForm.cpp
 *
 *  Created on: Oct 11, 2011
 *      Author: Gapchich Vlad
 */

#include "OptionsForm.h"

#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QKeyEvent>

//! A constructor
/*!
 * Constructor is responsible for allocating memory for all the GUI,
 * arranging and connecting them in the right order.
 */
OptionsForm::OptionsForm(QWidget *aParent)
	: QWidget(aParent)
{
	setWindowTitle(tr("Options"));

	PASCALpath_ = 0;

	layout_v_ = new QVBoxLayout(this);
	layout_PASCAL_root_ = new QHBoxLayout;
	layout_h_ = new QHBoxLayout;

	auto_color_generation_box_ = new QCheckBox(this);
	auto_color_generation_box_->setText(tr("Automatic label color generation"));
	button_set_PASCAL_root_ = new QPushButton(this);
	button_set_PASCAL_root_->setText(tr("set PASCAL root path"));
	edit_PASCAL_root_ = new QLineEdit("", this);
	edit_PASCAL_root_->setMinimumWidth(180);
	button_ok_ = new QPushButton(this);
	button_ok_->setText(tr("OK"));
	button_cancel_ = new QPushButton(this);
	button_cancel_->setText(tr("Cancel"));

	layout_v_->addWidget(auto_color_generation_box_);
	layout_v_->addLayout(layout_PASCAL_root_);
	layout_v_->addLayout(layout_h_);

	layout_PASCAL_root_->addWidget(button_set_PASCAL_root_);
	layout_PASCAL_root_->addWidget(edit_PASCAL_root_);

	layout_h_->addWidget(button_ok_);
	layout_h_->addWidget(button_cancel_);

	connect(
		button_set_PASCAL_root_,
		SIGNAL(clicked()),
		this,
		SLOT(newPascalPath())
		);
	connect(
		button_ok_,
		SIGNAL(clicked()),
		this,
		SLOT(setOptions())
		);
	connect(
		button_cancel_,
		SIGNAL(clicked()),
		this,
		SLOT(hide())
		);
	connect(
		edit_PASCAL_root_,
		SIGNAL(editingFinished()),
		this,
		SLOT(onPathEditing())
		);

	/* moving it to the center of the screen */
	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

//! A destructor
OptionsForm::~OptionsForm()
{
	delete auto_color_generation_box_;
	delete button_set_PASCAL_root_;
	delete edit_PASCAL_root_;
	delete button_ok_;
	delete button_cancel_;

	delete layout_v_;
}

//! A slot member confirming all the changes in options
void
OptionsForm::setOptions()
{
	*auto_color_generation_ = auto_color_generation_box_->isChecked();
	hide();
}

//! Sets a pointer on the ImageLabeler::PASCALpath_
void
OptionsForm::setPASCALpath(QString *aPath)
{
	if (!aPath) {
		return;
		/* NOTREACHED */
	}

	PASCALpath_ = aPath;
}

//! Asks user for the new path to the PASCAL "root" folder
void
OptionsForm::newPascalPath()
{
	QString newPath;
	QFileDialog fileDialog(0, tr("root directory for the PASCAL files"));
	fileDialog.setFileMode(QFileDialog::Directory);
	if (fileDialog.exec())
		newPath = fileDialog.selectedFiles().last();
	else {
		return;
		/* NOTREACHED */
	}

	if (newPath.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	*PASCALpath_ = newPath;
	edit_PASCAL_root_->setText(newPath);
}

//! Returns auto_color_generation_box_ status
bool
OptionsForm::autoColorGeneration()
{
	return auto_color_generation_box_->isChecked();
}

//! Sets auto_color_generation_box_ status
void
OptionsForm::setAutoColorGeneration(bool *flag)
{
	auto_color_generation_box_->setChecked(*flag);
	auto_color_generation_ = flag;
}

//! A slot member showing the form and initializing widgets
void
OptionsForm::showOptions()
{
	if (!PASCALpath_) {
		return;
		/* NOTREACHED */
	}

	if (PASCALpath_->isEmpty())
		edit_PASCAL_root_->setText(tr("root path is not set yet"));
	else
		edit_PASCAL_root_->setText(*PASCALpath_);

	show();
}

//! A slot member validating path to the PASCAL "root" folder typed by user
void
OptionsForm::onPathEditing()
{
	QString text = edit_PASCAL_root_->text();
	QDir dir(text);
	if (dir.exists())
		*PASCALpath_ = text;
	else
		edit_PASCAL_root_->setText(*PASCALpath_);
}

//! An event which is automatically called on every key press
void
OptionsForm::keyPressEvent(QKeyEvent *anEvent)
{
	if ((Qt::Key_Enter == anEvent->key() ||
		Qt::Key_Return == anEvent->key()) &&
		!edit_PASCAL_root_->hasFocus()) {
		setOptions();
	}
}

/*
 *
 */
