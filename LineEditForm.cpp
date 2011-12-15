/*
 * LineEditForm.cpp
 *
 *  Created on: Oct 6, 2011
 *      Author: Gapchich Vlad
 */

#include "LineEditForm.h"

#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>

//! A constructor
/*!
 * Constructor is responsible for allocating memory for all the GUI,
 * arranging and connecting them in the right order.
 */
LineEditForm::LineEditForm(QWidget *aParent)
	: QWidget(aParent)
{
	//setWindowTitle(tr("Image Description"));
	purpose_ = NoPurpose;

	layout_v_ = new QVBoxLayout(this);
	layout_h_ = new QHBoxLayout;

	data_ = new QLineEdit(this);
	button_ok_ = new QPushButton(this);
	button_ok_->setText(tr("OK"));
	button_cancel_ = new QPushButton(this);
	button_cancel_->setText(tr("Cancel"));

	layout_v_->addWidget(data_);
	layout_v_->addLayout(layout_h_);

	layout_h_->addWidget(button_ok_);
	layout_h_->addWidget(button_cancel_);

	connect(
		button_ok_,
		SIGNAL(clicked()),
		this,
		SLOT(setData())
		);
	connect(
		button_cancel_,
		SIGNAL(clicked()),
		this,
		SLOT(hide())
		);

	/* moving form to the center of the screen */
	adjustSize();
	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

//! A destructor
LineEditForm::~LineEditForm()
{
	delete data_;
	delete button_ok_;
	delete button_cancel_;

	delete layout_v_;
}

//! Transmits data to any reciever by dataSet(QString aData)
void
LineEditForm::setData()
{
	if (!data_->text().isEmpty()) {
		emit dataSet(data_->text());
		hide();
	}
	else
		QMessageBox::warning(
			this,
			tr("Warning!"),
			tr("Image description is empty"),
			QMessageBox::Ok,
			QMessageBox::Cancel
			);
}

//! Showing form and setting proper purpose(ImageDescriptionPurpose)
void
LineEditForm::setDescription()
{
	data_->clear();
	purpose_ = ImageDescriptionPurpose;
	setWindowTitle(tr("Image Description"));
	show();
}

//! Showing form and setting proper purpose(TaggingPurpose)
void
LineEditForm::setTags()
{
	data_->clear();
	purpose_ = TaggingPurpose;
	setWindowTitle(tr("Tagging"));
	show();
}

//! Returns current purpose of the form
FormPurpose
LineEditForm::purpose()
{
	return purpose_;
}

//! An event which is automatically called on every key press
void
LineEditForm::keyPressEvent(QKeyEvent *anEvent)
{
	if ((Qt::Key_Enter == anEvent->key() ||
		Qt::Key_Return == anEvent->key())) {
		setData();
	}
}

/*
 *
 */
