/*
 * \file ImageLabeler.h
 *
 *  Created on: Oct 4, 2011
 *      Author: Gapchich Vladislav
 */

#ifndef __IMAGELABELER_H__
#define __IMAGELABELER_H__

#include "ImageHolder.h"
#include "LineEditForm.h"
#include "OptionsForm.h"

#include <QMainWindow>
#include <QDir>

/* forward declarations */
class QMenuBar;
class QMenu;
class QAction;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QPushButton;
class QPixmap;
class QLabel;
class QScrollArea;
class QFrame;
class QListWidget;
class QListWidgetItem;
class QButtonGroup;
class QDomDocument;
class QDomElement;
class QSettings;

//! Structure keeps path to the image and it's flags
/*
 * \see loadInfo(QString filename)
 * \see loadPascalFile(QString aFilename, QString aPath)
 *
 * Image could be labeled before so you can load all the info about it
 * using loadInfo(QString filename)
 * pas_ means it was loaded from the file with PASCAL format
 */
struct Image {
	QString image_;
	bool labeled_;
	bool pas_;
};

//! \brief Main widget which contains all GUI elements
//! and connect them with each other.
/*
 * \\see ImageHolder
 */
class ImageLabeler : public QMainWindow
{
	Q_OBJECT
protected:
	/* events */
	void resizeEvent(QResizeEvent *anEvent);
	void mousePressEvent(QMouseEvent *anEvent);
	void mouseMoveEvent(QMouseEvent *anEvent);
	void wheelEvent(QWheelEvent *anEvent);
	void keyPressEvent(QKeyEvent *anEvent);
	void keyReleaseEvent(QKeyEvent *anEvent);
	void closeEvent(QCloseEvent *anEvent);

	bool loadPixmap(const QString &aPath);
	bool readSettings(QSettings *aSettings);
	bool writeSettings(QSettings *aSettings);
	void getImagesFromDir(const QDir &dir);
	void showWarning(const QString &text);
	bool askForUnsavedData();
	void loadLegendFromNode(QDomElement *anElement);
	void addLabel(
		int aLabelID,
		bool isMain,
		QString aLabel
		);
	void addPoly(Polygon *poly);
	void addBBox(BoundingBox *bbox);
	void addPolyFromData(
		QString *aPolyData,
		int *labelID
		);
	void addBBoxFromData(
		QString *aBBoxData,
		int *labelID
		);
	Polygon polyFromData(
		QString *aPolyData
		);
	Polygon	polyFromListItemText(
		QString *aString,
		int *oldID
		);
	BoundingBox BBoxFromData(
		QString *aBBoxData
		);
	BoundingBox	BBoxFromListItemText(
		QString *aString,
		int *oldID
		);
	void addBBoxArea(
		int anID,
		BoundingBox aBBox,
		int itemID = -1
		);
	void addPolyArea(
		int aPolyID,
		Polygon aPoly,
		int itemID = -1
		);
	bool deleteArea(QListWidgetItem *anItem);
	bool toggleLabelPriority(QListWidgetItem *anItem);
	void enableTools();
	void disableTools();
	void legendToXml(QDomDocument *aDoc, QDomElement *aRoot);
	void objectsToXml(QDomDocument *aDoc, QDomElement *aRoot);
	void addImage(Image *anImage);
	bool loadInfo(QString filename);
	bool loadPascalFile(QString aFilename, QString aPath = QString());
	bool loadPascalPolys(QString aFilename);
	bool selectImage(int anImageID);
	void setLabelColor(int anID, QColor aColor);

public:
	ImageLabeler(QWidget *aParent = 0, QString aSettingsPath = QString());
	virtual ~ImageLabeler();

public slots:
	void addLabel();
	void editLabel(QListWidgetItem *anItem);
	void removeLabel();
	void removeLabel(int aLabelID);
	void setLabelID(QListWidgetItem *anItem);
	void toggleLabelPriority();

	void focusOnArea();
	void editArea();
	void deleteArea();

	void saveAllInfo();
	void saveSegmentedPicture();
	void saveLegend();
	void loadImage();
	void loadImages();
	void loadInfo();
	void loadPascalFile();
	void loadPascalPolys();
	void loadLegendFromFile();
	void nextImage();
	void prevImage();
	void setBoundingBoxTool(bool aButtonPressed);
	void setPolygonTool(bool aButtonPressed);
	void generateColors();
	void confirmSelection();
	void clearAll();
	void clearAllTool();
	void clearLabelList();
	void clearLabelColorList();
	void areaListPopupMenu(const QPoint &aPos);
	void labelListPopupMenu(const QPoint &aPos);
	void imageListPopupMenu(const QPoint &);
	void setDataFromForm(QString aData);
	void onSelectionStarted();
	void onAreaItemChange(QListWidgetItem *);
	void onAreaEdit();
	void setPureData();
	void setLabelColor();
	void viewNormal();
	void viewSegmented();
	void interruptSearch();
	void selectImage(QListWidgetItem *);
	void removeImage();
	void writeSettings();
	void readSettings();

private:
	/*
	 *  menu
	 */

	//! main menu bar contains all other menus
	QMenuBar *menu_bar_;

	//! \brief contains all actions connected with file loading and saving.
	//! also contains menu_pascal_
	QMenu *menu_file_;

	//! \brief contains all actions connected with file saving.
	QMenu *menu_save_;

	//! \brief contains all actions connected with file saving.
	QMenu *menu_load_;

	//! contains actions for loading PASCAL xml file and PASCAL polygon
	QMenu *menu_pascal_;

	//! \brief contains action_view_normal_ and action_view_segmented_
	//! \see action_view_normal_ \see action_view_segmented_
	QMenu *menu_view_;

	/*! \brief contains tools, undo, redo, description adding and options
	 * \see action_undo_
	 * \see action_redo_
	 * \see action_bound_box_tool_
	 * \see action_polygon_tool_
	 * \see action_tagging_tool_
	 * \see action_add_description_
	 * \see action_options_
	 */
	QMenu *menu_edit_;

	//! empty yet
	QMenu *menu_help_;

	/* menu file */
	//! \see loadImages()
	QAction *action_open_images_;

	//! \see loadImage()
	QAction *action_open_image_;

	//! \see loadInfo()
	QAction *action_open_labeled_image_;

	//! \see loadLegendFromFile()
	QAction *action_load_legend_;

	//! \see saveAllInfo()
	QAction *action_save_all_;

	//! \see saveSegmentedPicture()
	QAction *action_save_segmented_;

	//! \see saveLegend()
	QAction *action_save_legend_;

	//! closes the application
	QAction *action_quit_;

	/* menu pascal */
	//! \see loadPascalFile();
	QAction *action_load_pascal_file_;

	//! \see loadPascalPolys();
	QAction *action_load_pascal_poly_;

	/* menu view */
	//! loads an image from current_image_ \see viewNormal()
	QAction *action_view_normal_;

	//! loads an image from segmented_image_ \see viewSegmented()
	QAction *action_view_segmented_;

	/* menu edit */
	//! \see ImageHolder::undo()
	QAction *action_undo_;

	//! \see ImageHolder::redo()
	QAction *action_redo_;

	//! \see setBoundingBoxTool(bool aButtonPressed)
	QAction *action_bound_box_tool_;

	//! \see setPolygonTool(bool aButtonPressed)
	QAction *action_polygon_tool_;

	//! \see setTags()
	QAction *action_tagging_tool_;

	/*! \see setDataFromForm(QString aData)
	 *  \see image_description_
	 */
	QAction *action_add_description_;

	//! \see OptionsForm
	QAction *action_options_;

	//! empty yet
	QAction *action_about_;

	//! empty yet
	QAction *action_help_content_;


	/* popup menu */
	//! \brief popup menu for area_list_
	//! \see list_areas_
	QMenu *popup_area_list_;

	//! \see deleteArea()
	QAction *action_delete_area_;

	//! \see editArea()
	QAction *action_edit_area_;

	//! \brief popup menu for list_label_
	//! \see list_label_
	QMenu *popup_label_list_;

	//! \see toggleLabelPriority()
	QAction *action_toggle_priority_;

	//! \see setLabelColor(int anID, QColor aColor)
	QAction *action_set_color_;

	//! \see removeLabel()
	QAction *action_delete_label_;

	//! \brief popup menu for list_images_
	//! \see list_images
	QMenu *popup_images_list_;

	//! \see removeImage()
	QAction *action_remove_image_;

	/* layouts */
	QHBoxLayout *layout_main_;
	QVBoxLayout *layout_left_;
	QVBoxLayout *layout_toolbox_;
	QHBoxLayout *layout_imagelist_buttons_;
	QVBoxLayout *layout_center_;
	QVBoxLayout *layout_frame_image_;
	QGridLayout *layout_image_widget_;
	QHBoxLayout *layout_center_buttons_;
	QVBoxLayout *layout_right_;
	QVBoxLayout *layout_labelbox_;
	QHBoxLayout *layout_labelbox_buttons_;
	QVBoxLayout *layout_areabox_buttons_;

	/* widgets */
	//!	parent for all widgets
	QWidget *central_widget_;

	//! \brief container(parent) of the image_holder_
	//! \see image_holder_
	QScrollArea *frame_image_;

	//! frame containing all the widgets in the center
	QFrame *frame_center_;

	//! frame containing all the left part
	QFrame *frame_toolbox_;

	//! frame containing all the right part
	QFrame *frame_labelbox_;

	//! \brief object containing current loaded image
	//! \see image_holder_
	QPixmap *image_;

	//! widget containing the image_(inherited from QLabel)
	ImageHolder *image_holder_;

	//! just an information label
	QLabel *label_toolbox_;

	//! just an information label
	QLabel *label_list_label_;

	//! just an information label
	QLabel *label_list_areas_;

	//! just an information label
	QLabel *label_list_images_;

	//! list widget containing labels
	QListWidget *list_label_;

	//! list widget containing selected areas data
	QListWidget *list_areas_;

	//! \brief list widget containing loaded images names
	//! \see list_images_
	QListWidget *list_images_widget_;

	//! \brief widget for editing tags and image description
	//! \see tags_
	//! \see image_description_
	LineEditForm line_edit_form_;

	//! widget containing possible options
	OptionsForm options_form_;

	//! \brief button switching to the bounding box tool
	//! \see BoundingBox
	//! \see setBoundingBoxTool(bool aButtonPressed)
	QPushButton *button_bound_box_tool_;

	//! \brief button switching to the bounding box tool
	//! \see Polygon
	//! \see setPolygonTool(bool aButtonPressed)
	QPushButton *button_polygon_tool_;

	//! \brief button calling LineEditForm to edit tags
	//! \see tags_
	//! \see LineEditForm
	QPushButton *button_tagging_tool_;

	//! \brief button clearing all selection
	//! \see clearAllTool()
	QPushButton *button_clear_selection_tool_;

	//! \brief button deleting all labels(except BACKGROUND)
	//! \see clearLabelList()
	QPushButton *button_delete_all_labels_;

	//! \brief button generating colors for all labels(except BACKGROUND)
	//! \see generateColors()
	QPushButton *button_generate_colors_;

	//! \brief button adding new label
	//! \see addLabel()
	QPushButton *button_add_label_;

	//! \brief button removing current label
	//! \see removeLabel()
	QPushButton *button_remove_label_;

	//! \brief button deleting current area
	//! \see deleteArea()
	QPushButton *button_delete_area_;

	//! \brief button allowing area text changing
	//! \see editArea()
	QPushButton *button_change_area_text_;

	//! \brief button allowing area modifying
	//! \see focusOnArea()
	QPushButton *button_change_area_;

	//! \brief button removing current image
	//! \see loadImage()
	QPushButton *button_add_image_;

	//! \brief button removing current image
	//! \see removeImage()
	QPushButton *button_remove_image_;

	//! \brief button switching to the next image
	//! \see nextImage()
	QPushButton *button_prev_image_;

	//! \brief button switching to the previous image
	//! \see prevImage()
	QPushButton *button_next_image_;

	//! \brief button confirming current selection(selected area)
	//! \see configrmSelection()
	QPushButton *button_confirm_selection_;

	//! abstract group manages tool buttons state
	QButtonGroup *group_tools_;

	/* variables */
	//! keeps current keyboard modifier(like ctrl, shift, alt etc)
	Qt::KeyboardModifiers keyboard_modifier_;

	//! number of the main label
	int main_label_;

	//! \brief 2d array for the segmented image
	//! \see setPureData()
	int **pure_data_;

	//! \brief number of selected label in the list_label_
	//! \see list_label_
	int label_ID_;

	//! \brief number of current image in the list_images_
	//! \see list_images_
	int image_ID_;

	//! contains current image description
	QString image_description_;

	//! contains tags for the current image
	QString tags_;

	//! "root" path to the directory where all the PASCAL data is located
	QString PASCALpath_;

	//! path to the currenlty loaded image
	QString current_image_;

	//! path to the segmented image
	QString segmented_image_;

	//! \brief list of all selected and confirmed rectangular areas
	//! \see addBBox(BoundingBox *bbox)
	QList< BoundingBox * > list_bounding_box_;

	//! \brief list of all selected and confirmed polygon areas
	//! \see addPoly(Polygon *poly)
	QList< Polygon * > list_polygon_;

	//! contains the paths for all loaded images
	QList< Image > *list_images_;

	//! list of label colors
	QList< uint > list_label_colors_;

	//! \brief buffer for manual list_areas_ items editing
	//! \see onAreaItemChange(QListWidgetItem *anItem)
	QString old_area_string_;

	/* options */
	//! enables/disables automatic color generation before image segmenting
	bool auto_color_generation_;

	/* flags */
	//! \brief flag used to interrupt recursive search of the images
	//! \see interruptSearch()
	//! \see loadImages()
	//! \see getImagesFromDir(const QDir &dir)
	bool interrupt_search_;

	//! flag indicating whether there is an unsaved data or not
	bool unsaved_data_;

	//! pointer to object used to read and write application settings
	QSettings *settings_;
};

#endif /* __IMAGELABELER_H__ */

/*
 *
 */
