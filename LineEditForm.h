/*!
 * \file ImageDescriptionForm.h
 *
 *  Created on: Oct 6, 2011
 *      Author: Gapchich Vlad
 */

#ifndef __LINEEDITFORM_H__
#define __LINEEDITFORM_H__

#include <QWidget>

class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QKeyEvent;

//! Enum indicating the purpose why the form was called
enum FormPurpose {
	NoPurpose,
	ImageDescriptionPurpose,
	TaggingPurpose
};

//! \brief A widget with one-line text editor which can be used both for
//! image description or adding tags for the image
/*!
 * The number of purposes can be increased by simple adding another purpose
 * to the FormPurpose enum and managing it in the code of this class
 */
class LineEditForm : public QWidget {
	Q_OBJECT
protected:
	void keyPressEvent(QKeyEvent *anEvent);
public:
	LineEditForm(QWidget *aParent = 0);
	virtual ~LineEditForm();
	FormPurpose purpose();

public slots:
	void setData();
	void setDescription();
	void setTags();

signals:
	void dataSet(QString aData);

private:
	//! one-line text editor
	QLineEdit *data_;
	QPushButton *button_ok_;
	QPushButton *button_cancel_;

	QVBoxLayout *layout_v_;
	QHBoxLayout *layout_h_;

	//! keeps the purpose why LineEditForm was called
	FormPurpose purpose_;
};

#endif /* __LINEEDITFORM_H__ */

/*
 *
 */
