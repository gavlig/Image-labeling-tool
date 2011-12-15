/*!
 * \file OptionsForm.h
 *
 *  Created on: Oct 11, 2011
 *      Author: Gapchich Vlad
 */

#ifndef __OPTIONSFORM_H__
#define __OPTIONSFORM_H__

#include <QWidget>

class QCheckBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QKeyEvent;
//! A widget for changing options
/*!
 * For now it contains automatic color generation switcher
 * and path to the PASCAL "root" folder setter
 */
class OptionsForm : public QWidget {
	Q_OBJECT
protected:
	void keyPressEvent(QKeyEvent *anEvent);
public:
	OptionsForm(QWidget *aParent = 0);
	virtual ~OptionsForm();

	bool autoColorGeneration();
	void setPASCALpath(QString *aPath);

public slots:
	void setOptions();
	void showOptions();
	void newPascalPath();
	void setAutoColorGeneration(bool *flag);
	void onPathEditing();

signals:

private:
	QCheckBox *auto_color_generation_box_;
	QPushButton *button_set_PASCAL_root_;
	QLineEdit *edit_PASCAL_root_;
	QPushButton *button_ok_;
	QPushButton *button_cancel_;

	QVBoxLayout *layout_v_;
	QHBoxLayout *layout_PASCAL_root_;
	QHBoxLayout *layout_h_;

	/* pointers to variables */
	QString *PASCALpath_;
	bool *auto_color_generation_;
};

#endif /* __OPTIONSFORM_H__ */

/*
 *
 */
