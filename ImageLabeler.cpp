/*!
 * \file ImageLabeler.cpp
 * \brief implementation of the ImageLabeler class
 *
 * Main widget which contains all GUI elements
 * and connects them with each other.
 *
 *  Created on: Oct 4, 2011
 *      Author: Gapchich Vladislav
 */

#include "ImageLabeler.h"
#include "functions.h"

#include <QApplication>
#include <QFrame>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QCheckBox>
#include <QScrollArea>
#include <QPushButton>
#include <QButtonGroup>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QColorDialog>
#include <QDir>
#include <QMessageBox>
#include <QListIterator>
#include <QDomDocument>
#include <QFile>
#include <QKeyEvent>
#include <QSettings>
#include <QDebug>

//! A constructor of the main class
/*!
 *	\param[in,out] aParent a pointer to the parent widget.
 *	If aParent is 0, then ImageLabeler becomes a window, otherwise
 *	it becomes a child of some other widget
 *	\param[in] aSettingsPath alters the path to the file with settings
 *	(default path is /path/to/binary/ImageLabeler.ini)
 *
 *	A constructor is responsible for allocating memory for all the
 *	GUI objects, arranging and connecting them in the right order.
 */
ImageLabeler::ImageLabeler(QWidget *aParent, QString aSettingsPath) :
	QMainWindow(aParent)
{
	setFocusPolicy(Qt::StrongFocus);

	/*
	 * Variables
	 */
	list_images_ = new QList< Image >;

	main_label_ = -1;
	pure_data_ = 0;
	//label_ID_ = -1;

	/* options */
	auto_color_generation_ = 0;

	/* flags */
	interrupt_search_ = 0;
	unsaved_data_ = 0;

	setMouseTracking(true);

	/*
	 * menu bar part begins
	 */

	menu_bar_ = new QMenuBar(this);
	setMenuBar(menu_bar_);

	menu_file_ = new QMenu(menu_bar_);
	menu_file_->setTitle(tr("&File"));

	menu_save_ = new QMenu(menu_bar_);
	menu_save_->setTitle(tr("&Save"));
	menu_load_ = new QMenu(menu_bar_);
	menu_load_->setTitle(tr("&Load"));

	menu_pascal_ = new QMenu(menu_bar_);
	menu_pascal_->setTitle(tr("&Pascal"));
	menu_view_ = new QMenu(menu_bar_);
	menu_view_->setTitle(tr("&View"));
	menu_edit_ = new QMenu(menu_bar_);
	menu_edit_->setTitle(tr("&Edit"));
	menu_help_ = new QMenu(menu_bar_);
	menu_help_->setTitle(tr("&Help"));

	/* menu file */
	action_open_image_ = new QAction(this);
	action_open_image_->setText(tr("&Load image"));
	action_open_images_ = new QAction(this);
	action_open_images_->setText(tr("&Load images(recursively)"));
	action_open_labeled_image_ = new QAction(this);
	action_open_labeled_image_->setText(tr("&Load labeled image"));
	action_load_legend_ = new QAction(this);
	action_load_legend_->setText(tr("&Load legend"));
	action_save_all_ = new QAction(this);
	action_save_all_->setText(tr("&Save all info"));
	action_save_all_->setEnabled(false);
	action_save_segmented_ = new QAction(this);
	action_save_segmented_->setText(tr("Save segmented &picture"));
	action_save_segmented_->setEnabled(false);
	action_save_legend_ = new QAction(this);
	action_save_legend_->setText(tr("Save &legend"));
	action_save_legend_->setEnabled(false);
	action_quit_ = new QAction(this);
	action_quit_->setText(tr("&Quit"));
	/* menu pascal */
	action_load_pascal_file_ = new QAction(this);
	action_load_pascal_file_->setText(tr("&Load pascal file"));
	action_load_pascal_poly_ = new QAction(this);
	action_load_pascal_poly_->setText(tr("&Load poly info"));
	/* menu view */
	action_view_normal_ = new QAction(this);
	action_view_normal_->setText(tr("&Normal"));
	action_view_normal_->setEnabled(false);
	action_view_segmented_ = new QAction(this);
	action_view_segmented_->setText(tr("&Segmented"));
	action_view_segmented_->setEnabled(false);
	/* menu edit */
	action_undo_ = new QAction(this);
	action_undo_->setText(tr("&Undo"));
	action_undo_->setEnabled(false);
	action_redo_ = new QAction(this);
	action_redo_->setText(tr("&Redo"));
	action_redo_->setEnabled(false);
	action_bound_box_tool_ = new QAction(this);
	action_bound_box_tool_->setText(tr("Bounding box tool"));
	action_bound_box_tool_->setEnabled(false);
	action_polygon_tool_ = new QAction(this);
	action_polygon_tool_->setText(tr("&Polygon tool"));
	action_polygon_tool_->setEnabled(false);
	action_tagging_tool_ = new QAction(this);
	action_tagging_tool_->setText(tr("&Tagging tool"));
	action_tagging_tool_->setEnabled(false);
	action_add_description_ = new QAction(this);
	action_add_description_->setText(tr("&Add image description"));
	action_add_description_->setEnabled(false);
	action_options_ = new QAction(this);
	action_options_->setText(tr("&Options"));
	/* menu help */
	action_help_content_ = new QAction(this);
	action_help_content_->setText(tr("&Help content"));
	action_help_content_->setEnabled(false);
	action_about_ = new QAction(this);
	action_about_->setText(tr("&About"));
	action_about_->setEnabled(false);
	/* ------------------ */

	menu_load_->addAction(action_open_image_);
	menu_load_->addAction(action_open_images_);
	menu_load_->addAction(action_open_labeled_image_);
	menu_load_->addAction(action_load_legend_);
	menu_save_->addAction(action_save_segmented_);
	menu_save_->addAction(action_save_legend_);
	menu_save_->addAction(action_save_all_);
	menu_file_->addAction(menu_load_->menuAction());
	menu_file_->addAction(menu_save_->menuAction());
	menu_file_->addAction(menu_pascal_->menuAction());
	//menu_file_->addSeparator();

	menu_file_->addSeparator();
	menu_file_->addAction(action_quit_);

	menu_pascal_->addAction(action_load_pascal_file_);
	menu_pascal_->addAction(action_load_pascal_poly_);

	menu_view_->addAction(action_view_normal_);
	menu_view_->addAction(action_view_segmented_);

	menu_edit_->addAction(action_undo_);
	menu_edit_->addAction(action_redo_);
	menu_edit_->addSeparator();
	menu_edit_->addAction(action_bound_box_tool_);
	menu_edit_->addAction(action_polygon_tool_);
	menu_edit_->addAction(action_tagging_tool_);
	menu_edit_->addSeparator();
	menu_edit_->addAction(action_add_description_);
	menu_edit_->addAction(action_options_);

	menu_help_->addAction(action_help_content_);
	menu_help_->addAction(action_about_);

	menu_bar_->addAction(menu_file_->menuAction());
	menu_bar_->addAction(menu_view_->menuAction());
	menu_bar_->addAction(menu_edit_->menuAction());
	menu_bar_->addAction(menu_help_->menuAction());

	/*
	 * popup menu part begins
	 */
	popup_area_list_ = new QMenu;

	action_delete_area_ = new QAction(this);
	action_delete_area_->setText(tr("&Delete area"));
	action_edit_area_ = new QAction(this);
	action_edit_area_->setText(tr("&Change area"));

	popup_area_list_->addAction(action_delete_area_);
	popup_area_list_->addAction(action_edit_area_);

	popup_label_list_ = new QMenu;

	action_set_color_ = new QAction(this);
	action_set_color_->setText(tr("Set &color"));
	action_toggle_priority_ = new QAction(this);
	action_toggle_priority_->setText(tr("Toggle &priority"));
	action_delete_label_ = new QAction(this);
	action_delete_label_->setText(tr("&Delete"));

	popup_label_list_->addAction(action_set_color_);
	popup_label_list_->addAction(action_toggle_priority_);
	popup_label_list_->addAction(action_delete_label_);

	popup_images_list_ = new QMenu;

	action_remove_image_ = new QAction(this);
	action_remove_image_->setText(tr("&remove image"));

	popup_images_list_->addAction(action_remove_image_);

	/*
	 * widgets part begins
	 */
	central_widget_ = new QWidget(this);
	setCentralWidget(central_widget_);

	frame_toolbox_ = new QFrame(central_widget_);
	frame_toolbox_->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	frame_toolbox_->setLineWidth(0);
	frame_toolbox_->setMidLineWidth(0);
	frame_center_ = new QFrame(central_widget_);
	frame_image_ = new QScrollArea(frame_center_);
	frame_image_->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	frame_image_->setLineWidth(0);
	frame_image_->setMidLineWidth(0);
	frame_image_->setWidgetResizable(false);
	frame_image_->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	frame_image_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	frame_labelbox_ = new QFrame(central_widget_);
	frame_labelbox_->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	frame_labelbox_->setLineWidth(0);
	frame_labelbox_->setMidLineWidth(0);

	/* just dummy */
	image_ = new QPixmap(500, 500);
	image_->fill(QColor(Qt::white));

	image_holder_ = new ImageHolder;
	image_holder_->setPixmap(*image_);
	image_holder_->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	image_holder_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	image_holder_->setScaledContents(true);
	image_holder_->setFrameStyle(QFrame::Box | QFrame::Plain);
	image_holder_->setLineWidth(0);
	image_holder_->setFocusPolicy(Qt::StrongFocus);

	frame_image_->setWidget(image_holder_);

	list_label_ = new QListWidget(central_widget_);
	list_label_->setContextMenuPolicy(Qt::CustomContextMenu);
	list_areas_ = new QListWidget(central_widget_);
	list_areas_->setContextMenuPolicy(Qt::CustomContextMenu);
	list_images_widget_ = new QListWidget(central_widget_);
	list_images_widget_->setContextMenuPolicy(Qt::CustomContextMenu);

	label_toolbox_ = new QLabel(tr("Tool box"), frame_toolbox_);
	label_list_label_ = new QLabel(tr("Object labels:"), central_widget_);
	label_list_areas_ = new QLabel(tr("Selected areas:"), central_widget_);
	label_list_images_ = new QLabel(tr("Loaded images:"), central_widget_);

	/* buttons */
	button_bound_box_tool_ = new QPushButton(frame_toolbox_);
	button_bound_box_tool_->setText(tr("bbox"));
	button_bound_box_tool_->setEnabled(false);
	button_bound_box_tool_->setCheckable(true);
	button_polygon_tool_ = new QPushButton(frame_toolbox_);
	button_polygon_tool_->setText(tr("poly tool"));
	button_polygon_tool_->setEnabled(false);
	button_polygon_tool_->setCheckable(true);
	button_tagging_tool_ = new QPushButton(frame_toolbox_);
	button_tagging_tool_->setText(tr("tagging"));
	button_tagging_tool_->setEnabled(false);
	button_clear_selection_tool_ = new QPushButton(frame_toolbox_);
	button_clear_selection_tool_->setText(tr("clear selection"));
	button_clear_selection_tool_->setEnabled(false);
	button_generate_colors_ = new QPushButton(frame_toolbox_);
	button_generate_colors_->setText(tr("generate label colors"));
	button_generate_colors_->setEnabled(false);
	button_delete_all_labels_ = new QPushButton(frame_toolbox_);
	button_delete_all_labels_->setText(tr("delete all labels"));
	button_delete_all_labels_->setEnabled(false);

	group_tools_ = new QButtonGroup;
	group_tools_->addButton(button_bound_box_tool_);
	group_tools_->addButton(button_polygon_tool_);

	button_confirm_selection_ = new QPushButton(central_widget_);
	button_confirm_selection_->setText(tr("Confirm selection"));
	button_confirm_selection_->setEnabled(false);

	button_add_label_ = new QPushButton(frame_labelbox_);
	button_add_label_->setText(tr("Add label"));
	button_add_label_->setEnabled(false);
	button_remove_label_ = new QPushButton(frame_labelbox_);
	button_remove_label_->setText(tr("Remove label"));
	button_remove_label_->setEnabled(false);

	button_delete_area_ = new QPushButton(frame_labelbox_);
	button_delete_area_->setText(tr("Delete area"));
	button_delete_area_->setEnabled(false);
	button_change_area_ = new QPushButton(frame_labelbox_);
	button_change_area_->setText(tr("Change area"));
	button_change_area_->setEnabled(false);
	button_change_area_text_ = new QPushButton(frame_labelbox_);
	button_change_area_text_->setText(tr("Change area text"));
	button_change_area_text_->setEnabled(false);

	button_add_image_ = new QPushButton(frame_toolbox_);
	button_add_image_->setText(tr("Add"));
	button_remove_image_ = new QPushButton(frame_toolbox_);
	button_remove_image_->setText(tr("Remove"));
	button_remove_image_->setEnabled(false);

	button_prev_image_ = new QPushButton(central_widget_);
	button_prev_image_->setText("←");
	button_next_image_ = new QPushButton(central_widget_);
	button_next_image_->setText("→");

	/*
	 * layouts part begins
	 */
	layout_main_ = new QHBoxLayout(central_widget_);
	layout_left_ = new QVBoxLayout();
	layout_toolbox_ = new QVBoxLayout();
	layout_imagelist_buttons_ = new QHBoxLayout();
	layout_center_ = new QVBoxLayout();
	layout_frame_image_ = new QVBoxLayout();
	layout_image_widget_ = new QGridLayout();
	layout_center_buttons_ = new QHBoxLayout();
	layout_right_ = new QVBoxLayout();
	layout_labelbox_ = new QVBoxLayout();
	layout_labelbox_buttons_ = new QHBoxLayout();
	layout_areabox_buttons_ = new QVBoxLayout();

	layout_main_->addLayout(layout_left_);
	layout_main_->addLayout(layout_center_);
	layout_main_->addLayout(layout_right_);

	/* making the center part stretchable */
	layout_main_->setStretch(1, 1);

	/* left part */
	layout_left_->addWidget(frame_toolbox_);
	frame_toolbox_->setLayout(layout_toolbox_);

	layout_toolbox_->addWidget(label_toolbox_);
	layout_toolbox_->addWidget(button_bound_box_tool_);
	layout_toolbox_->addWidget(button_polygon_tool_);
	layout_toolbox_->addSpacing(10);
	layout_toolbox_->addWidget(button_tagging_tool_);
	layout_toolbox_->addSpacing(10);
	layout_toolbox_->addWidget(button_clear_selection_tool_);
	layout_toolbox_->addWidget(button_delete_all_labels_);
	layout_toolbox_->addSpacing(10);
	layout_toolbox_->addWidget(button_generate_colors_);
	/* stretch is for making toolbox as small as it can be */
	layout_toolbox_->addSpacing(10);
	layout_left_->addWidget(label_list_images_);

	layout_left_->addLayout(layout_imagelist_buttons_);
	layout_imagelist_buttons_->addWidget(button_add_image_);
	layout_imagelist_buttons_->addWidget(button_remove_image_);

	layout_left_->addWidget(list_images_widget_);
	list_images_widget_->setFixedWidth(200);
	layout_left_->addStretch(1);
	layout_left_->addWidget(button_confirm_selection_);


	/* central part */
	layout_center_->addWidget(frame_center_);
	frame_center_->setLayout(layout_frame_image_);

	layout_frame_image_->setContentsMargins(0, 0, 0, 0);
	layout_frame_image_->addWidget(frame_image_);

	layout_frame_image_->addLayout(layout_center_buttons_);
	layout_center_buttons_->addWidget(button_prev_image_);
	layout_center_buttons_->addWidget(button_next_image_);

	/* right part */
	layout_right_->addWidget(frame_labelbox_);
	frame_labelbox_->setFixedWidth(200);
	frame_labelbox_->setLayout(layout_labelbox_);

	layout_labelbox_->addWidget(label_list_label_);

	layout_labelbox_->addLayout(layout_labelbox_buttons_);
	layout_labelbox_buttons_->addWidget(button_add_label_);
	layout_labelbox_buttons_->addWidget(button_remove_label_);

	layout_labelbox_->addWidget(list_label_);
	layout_labelbox_->addWidget(label_list_areas_);

	layout_labelbox_->addLayout(layout_areabox_buttons_);
	layout_areabox_buttons_->addWidget(button_delete_area_);
	layout_areabox_buttons_->addWidget(button_change_area_);
	layout_areabox_buttons_->addWidget(button_change_area_text_);
	layout_areabox_buttons_->setSpacing(0);

	layout_labelbox_->addWidget(list_areas_);

	connect(
		action_quit_,
		SIGNAL(triggered()),
		this,
		SLOT(close())
		);
	connect(
		action_open_images_,
		SIGNAL(triggered()),
		this,
		SLOT(loadImages())
		);
	connect(
		action_open_image_,
		SIGNAL(triggered()),
		this,
		SLOT(loadImage())
		);
	connect(
		action_open_labeled_image_,
		SIGNAL(triggered()),
		this,
		SLOT(loadInfo())
		);
	connect(
		action_load_legend_,
		SIGNAL(triggered()),
		this,
		SLOT(loadLegendFromFile())
		);
	connect(
		action_load_pascal_file_,
		SIGNAL(triggered()),
		this,
		SLOT(loadPascalFile())
		);
	connect(
		action_load_pascal_poly_,
		SIGNAL(triggered()),
		this,
		SLOT(loadPascalPolys())
		);
	connect(
		action_save_legend_,
		SIGNAL(triggered()),
		this,
		SLOT(saveLegend())
		);
	connect(
		action_save_segmented_,
		SIGNAL(triggered()),
		this,
		SLOT(saveSegmentedPicture())
		);
	connect(
		action_save_all_,
		SIGNAL(triggered()),
		this,
		SLOT(saveAllInfo())
		);
	connect(
		action_view_normal_,
		SIGNAL(triggered()),
		this,
		SLOT(viewNormal())
		);
	connect(
		action_view_segmented_,
		SIGNAL(triggered()),
		this,
		SLOT(viewSegmented())
		);
	connect(
		action_undo_,
		SIGNAL(triggered()),
		image_holder_,
		SLOT(undo())
		);
	connect(
		action_redo_,
		SIGNAL(triggered()),
		image_holder_,
		SLOT(redo())
		);
	connect(
		action_tagging_tool_,
		SIGNAL(triggered()),
		&line_edit_form_,
		SLOT(setTags())
		);
	connect(
		action_add_description_,
		SIGNAL(triggered()),
		&line_edit_form_,
		SLOT(setDescription())
		);
	connect(
		action_options_,
		SIGNAL(triggered()),
		&options_form_,
		SLOT(showOptions())
		);
	connect(
		button_add_label_,
		SIGNAL(clicked()),
		this,
		SLOT(addLabel())
		);
	connect(
		button_remove_label_,
		SIGNAL(clicked()),
		this,
		SLOT(removeLabel())
		);
	connect(
		button_delete_area_,
		SIGNAL(clicked()),
		this,
		SLOT(deleteArea())
		);
	connect(
		button_change_area_,
		SIGNAL(clicked()),
		this,
		SLOT(focusOnArea())
		);
	connect(
		button_change_area_text_,
		SIGNAL(clicked()),
		this,
		SLOT(editArea())
		);
	connect(
		button_next_image_,
		SIGNAL(clicked()),
		this,
		SLOT(nextImage())
		);
	connect(
		button_prev_image_,
		SIGNAL(clicked()),
		this,
		SLOT(prevImage())
		);
	connect(
		button_bound_box_tool_,
		SIGNAL(toggled(bool)),
		this,
		SLOT(setBoundingBoxTool(bool))
		);
	connect(
		button_polygon_tool_,
		SIGNAL(toggled(bool)),
		this,
		SLOT(setPolygonTool(bool))
		);
	connect(
		button_tagging_tool_,
		SIGNAL(clicked()),
		&line_edit_form_,
		SLOT(setTags())
		);
	connect(
		button_clear_selection_tool_,
		SIGNAL(clicked()),
		this,
		SLOT(clearAllTool())
		);
	connect(
		button_generate_colors_,
		SIGNAL(clicked()),
		this,
		SLOT(generateColors())
		);
	connect(
		button_delete_all_labels_,
		SIGNAL(clicked()),
		this,
		SLOT(clearLabelList())
		);
	connect(
		button_add_image_,
		SIGNAL(clicked()),
		this,
		SLOT(loadImage())
		);
	connect(
		button_remove_image_,
		SIGNAL(clicked()),
		this,
		SLOT(removeImage())
		);
	connect(
		button_confirm_selection_,
		SIGNAL(clicked()),
		this,
		SLOT(confirmSelection())
		);
	connect(
		list_label_,
		SIGNAL(itemChanged(QListWidgetItem *)),
		this,
		SLOT(editLabel(QListWidgetItem *))
		);
	connect(
		list_label_,
		SIGNAL(itemClicked(QListWidgetItem *)),
		this,
		SLOT(setLabelID(QListWidgetItem *))
		);
	connect(
		list_areas_,
		SIGNAL(itemDoubleClicked(QListWidgetItem *)),
		image_holder_,
		SLOT(focusOnArea(QListWidgetItem *))
		);
	connect(
		list_areas_,
		SIGNAL(customContextMenuRequested(const QPoint &)),
		this,
		SLOT(areaListPopupMenu(const QPoint &))
		);
	connect(
		list_areas_,
		SIGNAL(itemChanged(QListWidgetItem *)),
		this,
		SLOT(onAreaItemChange(QListWidgetItem *))
		);
	connect(
		list_label_,
		SIGNAL(customContextMenuRequested(const QPoint &)),
		this,
		SLOT(labelListPopupMenu(const QPoint &))
		);
	connect(
		list_images_widget_,
		SIGNAL(itemDoubleClicked(QListWidgetItem *)),
		this,
		SLOT(selectImage(QListWidgetItem *))
		);
	connect(
		list_images_widget_,
		SIGNAL(customContextMenuRequested(const QPoint &)),
		this,
		SLOT(imageListPopupMenu(const QPoint &))
		);
	connect(
		action_delete_area_,
		SIGNAL(triggered()),
		this,
		SLOT(deleteArea())
		);
	connect(
		action_edit_area_,
		SIGNAL(triggered()),
		this,
		SLOT(editArea())
		);
	connect(
		action_set_color_,
		SIGNAL(triggered()),
		this,
		SLOT(setLabelColor())
		);
	connect(
		action_toggle_priority_,
		SIGNAL(triggered()),
		this,
		SLOT(toggleLabelPriority())
		);
	connect(
		action_delete_label_,
		SIGNAL(triggered()),
		this,
		SLOT(removeLabel())
		);
	connect(
		action_remove_image_,
		SIGNAL(triggered()),
		this,
		SLOT(removeImage())
		);
	connect(
		image_holder_,
		SIGNAL(selectionStarted()),
		this,
		SLOT(onSelectionStarted())
		);
	connect(
		&line_edit_form_,
		SIGNAL(dataSet(QString)),
		this,
		SLOT(setDataFromForm(QString))
		);
	connect(
		image_holder_,
		SIGNAL(areaEdited()),
		this,
		SLOT(onAreaEdit())
		);

	QString settingsPath = aSettingsPath;
	if (settingsPath.isEmpty())
		settingsPath = QString("ImageLabeler.ini");

	settings_ = new QSettings(settingsPath, QSettings::IniFormat, this);

	readSettings(settings_);

	/* giving the pointers to some properties for image_holder_ */
	image_holder_->setBoundingBoxList(&list_bounding_box_);
	image_holder_->setPolygonList(&list_polygon_);
	image_holder_->setLabelColorList(&list_label_colors_);
	image_holder_->setMainLabelNum(&main_label_);
	image_holder_->setImage(image_);
	image_holder_->setScrollArea(frame_image_);

	/* the same for options_form_ */
	options_form_.setPASCALpath(&PASCALpath_);
	options_form_.setAutoColorGeneration(&auto_color_generation_);
}

//! A destructor of the ImageLabeler class
/*!
 *	Simply frees all the allocated memory
 *	in the right order
 */
ImageLabeler::~ImageLabeler()
{
	delete action_quit_;
	delete action_open_labeled_image_;
	delete action_open_image_;
	delete action_open_images_;
	delete action_load_legend_;
	delete action_load_pascal_file_;
	delete action_load_pascal_poly_;
	delete action_save_legend_;
	delete action_save_segmented_;
	delete action_save_all_;
	delete action_view_normal_;
	delete action_view_segmented_;
	delete action_undo_;
	delete action_redo_;
	delete action_bound_box_tool_;
	delete action_polygon_tool_;
	delete action_tagging_tool_;
	delete action_add_description_;
	delete action_options_;
	delete action_about_;
	delete action_help_content_;

	delete menu_load_;
	delete menu_save_;
	delete menu_pascal_;
	delete menu_file_;
	delete menu_view_;
	delete menu_edit_;
	delete menu_help_;

	delete menu_bar_;

	delete action_delete_area_;
	delete action_edit_area_;

	delete popup_area_list_;

	delete action_toggle_priority_;
	delete action_set_color_;
	delete action_delete_label_;

	delete popup_label_list_;

	delete action_remove_image_;

	delete popup_images_list_;

	delete image_;
	delete image_holder_;

	delete button_add_label_;
	delete button_remove_label_;
	delete button_bound_box_tool_;
	delete button_delete_area_;
	delete button_change_area_;
	delete button_change_area_text_;
	delete button_polygon_tool_;
	delete button_tagging_tool_;
	delete button_clear_selection_tool_;
	delete button_generate_colors_;
	delete button_delete_all_labels_;
	delete button_next_image_;
	delete button_prev_image_;

	delete button_confirm_selection_;

	delete label_list_label_;
	delete label_list_areas_;
	delete label_toolbox_;
	delete label_list_images_;
	delete list_areas_;
	delete list_label_;
	delete list_images_widget_;

	delete layout_imagelist_buttons_;
	delete layout_toolbox_;
	delete layout_right_;
	delete layout_center_buttons_;
	delete layout_frame_image_;
	delete layout_center_;
	delete layout_labelbox_buttons_;
	delete layout_areabox_buttons_;
	delete layout_labelbox_;
	delete layout_left_;
	delete layout_main_;

	delete frame_labelbox_;
	delete frame_toolbox_;
	delete frame_image_;
	delete frame_center_;

	delete central_widget_;

	if (pure_data_) {
		delete[] *pure_data_;
		delete pure_data_;
	}

	delete list_images_;
	delete settings_;
}

//! A member which reads the settings from file to RAM
/*!
 *  \see writeSettings(QSettings *aSettings)
 *	\param[in] aSettings a pointer to the QSettings object
 *	which should be previously created and set to the right
 *	file with settings.
 */
bool
ImageLabeler::readSettings(QSettings *aSettings)
{
	aSettings->beginGroup("/global");
	auto_color_generation_ =
		aSettings->value("/auto_label_color_generation", 0).toBool();
	options_form_.setAutoColorGeneration(&auto_color_generation_);
	PASCALpath_ = aSettings->value("/PASCAL_root_path", "").toString();
	aSettings->endGroup();

	return true;
}

//! A member which reads the settings from file to RAM
/*!
 *	\see readSettings(QSettings *aSettings)
 */
void
ImageLabeler::readSettings()
{
	readSettings(settings_);
}

//! A member which writes settings from RAM to file
/*!
 *  \see readSettings(QSettings *aSettings)
 *	\param[in] aSettings a pointer to the QSettings object
 *	which should be previously created and set to the right
 *	file with settings
 */
bool
ImageLabeler::writeSettings(QSettings *aSettings)
{
	aSettings->beginGroup("/global");
	aSettings->setValue("/auto_label_color_generation", auto_color_generation_);
	aSettings->setValue("/PASCAL_root_path", PASCALpath_);
	aSettings->endGroup();

	return true;
}

//! A member which writes settings from RAM to file
/*!
 * \see writeSettings(QSettings *aSettings)
 */
void
ImageLabeler::writeSettings()
{
	writeSettings(settings_);
}

//! \brief A member taking a pointer to the Image struct and adding it to all necessary
//! containers and widgets
/*!
 * \param[in] anImage a pointer to the Image struct which should contain path to
 * the image, and two boolean flags:
 * labeled_ indicates whether it is labeled or not
 * pas_ indicates whether it was read from the PASCAL file or not
 */
void
ImageLabeler::addImage(Image *anImage)
{
	QListWidgetItem *newItem = new QListWidgetItem;

	QString itemText = QString("%1: %2").
		arg(list_images_widget_->count()).
		arg(removePath(anImage->image_));
	if (anImage->labeled_)
		itemText.append(" #labeled");
	if (anImage->pas_)
		itemText.append(" #pas");
	newItem->setText(itemText);

	list_images_widget_->addItem(newItem);
	list_images_->append(*anImage);

	button_remove_image_->setEnabled(true);
}

//! A slot member creating new label with default parameters
/*!
 *	New label will be added to the list_label_ widget and
 *	it's color will be added to the list_label_colors_ container.
 *	By default the color of new label is white and it's name is
 *	"New Label".
 *	If there was no labels before and the new one get number 0 then
 *	it becomes a background label width black color and name "BACKGROUND"
 */
void
ImageLabeler::addLabel()
{
	QListWidgetItem *newItem = new QListWidgetItem;

	QString label;
	int itemNum = list_label_->count();
	label.append(QString("%1: ").arg(itemNum));

	if (itemNum) {
		newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
		label.append("New label");
	}
	/* label #0 is always BACKGROUND */
	else {
		newItem->setFlags(
				Qt::ItemIsUserCheckable | Qt::ItemIsSelectable |
				Qt::ItemIsEnabled
				);
		label.append("BACKGROUND");
	}
	newItem->setText(label);

	QPixmap iconPix = QPixmap(20, 20);
	QColor color;
	/* white by default */
	if (0 != itemNum)
		color = Qt::white;
	/* label #0 is always black */
	else
		color = Qt::black;
	iconPix.fill(color);
	QIcon icon(iconPix);
	list_label_colors_.append(color.rgb());

	newItem->setIcon(icon);

	list_label_->addItem(newItem);
	list_label_->setItemSelected(newItem, true);
	label_ID_ = list_label_->count() - 1;

	/* adding new label means changing file data */
	unsaved_data_ = 1;
}

/* TODO: label color could be specified also */
//! A member creating new specified label
/*!
 * \see addLabel()
 * \param[in] aLabelID an integer specifying label position(ID also) in the list
 * \param[in] isMain a bool flag indicates whether this label is main or not
 * \param[in] aLabel a QString object containing new label's name
 */
void
ImageLabeler::addLabel(
	int aLabelID,
	bool isMain,
	QString aLabel
)
{
	QListWidgetItem *newItem = new QListWidgetItem;

	/* label #0 is always BACKGROUND */
	if (0 == aLabelID) {
		aLabel = QString("BACKGROUND");
		newItem->setFlags(
				Qt::ItemIsUserCheckable | Qt::ItemIsSelectable |
				Qt::ItemIsEnabled
				);
	}

	QString label;
	label.append(QString("%1: %2").
		arg(aLabelID).
		arg(aLabel)
		);

	/* TODO: check if there is another main label and make it common */
	if (isMain) {
		main_label_ = aLabelID;
	}

	QPixmap iconPix = QPixmap(20, 20);
	QColor color;
	if (0 != aLabelID)
		color = Qt::white;
	else
		color = Qt::black;
	iconPix.fill(color);
	QIcon icon(iconPix);
	list_label_colors_.append(color.rgb());

	newItem->setIcon(icon);

	newItem->setText(label);
	newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
	list_label_->addItem(newItem);
	list_label_->setItemSelected(newItem, true);

	unsaved_data_ = 1;
}

//! A slot member being called after a label gets changed
/*!
 *	\param[in] anItem a pointer to the QListWidgetItem object
 *	which contains a text of just edited label
 *
 *	Checking label if it has a label ID and puting a mark #main
 *	if it is so
 */
void
ImageLabeler::editLabel(QListWidgetItem *anItem)
{
	QString label = anItem->text();
	int itemRow = list_label_->row(anItem);

	QString prefix = QString("%1: ").arg(itemRow);
	if (-1 == label.indexOf(prefix))
		label.prepend(QString("%1: ").arg(itemRow));

	if (main_label_ == itemRow) {
		label.append(" #main");
	}

	list_label_->blockSignals(true);
	anItem->setText(label);
	list_label_->blockSignals(false);

	unsaved_data_ = 1;
}

//! A slot member removing selected label
/*!
 * \see removeLabel(int aLabelID)
 *
 * Simply removing selected label. Label no 0 can not be removed
 */
void
ImageLabeler::removeLabel()
{
	removeLabel(label_ID_);
}

//! A slot member removing selected label
/*!
 * \see removeLabel()
 * \param[in] aLabelID a integer specifying a label to remove
 *
 * Removing label number aLabelID. Label no 0 can not be removed
 */
void
ImageLabeler::removeLabel(int aLabelID)
{
	/* we need to keep BACKGROUND category */
	if (0 == list_label_->count() || aLabelID < 1) {
		return;
		/* NOTREACHED */
	}

	if (list_label_->count() <= aLabelID ||
		list_label_colors_.count() <= aLabelID) {
		return;
		/* NOTREACHED */
	}
	list_label_->takeItem(aLabelID);
	list_label_colors_.takeAt(aLabelID);

	unsaved_data_ = 1;
}

//! \brief a slot Member setting label_ID_ variable according to the selected row
//! in the list_label_ widget
/*!
 *	\param[in] anItem a pointer to the QListWidgetItem object which can be
 *	used to get current selected label in the list
 *
 *	This slot just duplicates QListWidget::row() functionality to get
 *	previously selected label even if it is not focused.
 */
void
ImageLabeler::setLabelID(
	QListWidgetItem *anItem
)
{
	if (!list_label_->count()) {
		return;
		/* NOTREACHED */
	}

	label_ID_ = list_label_->row(anItem);
}

//! A protected member adding a bbox area to the QListWidget list_areas_
/*!
 * \param[in] anID a integer which indicates bbox id in the
 * list_bounding_boxes_ container
 * \param[in] aBBox a BoundingBox struct containing
 * a rectangle data and it's label ID
 * \param[in] itemID an integer indicates a position
 * of a new item in the list_areas_
 *
 * itemID is -1 by default so the new area item will be placed in the
 * end of the list
 */
void
ImageLabeler::addBBoxArea(
	int anID,
	BoundingBox aBBox,
	int itemID
)
{
	QListWidgetItem *newItem = new QListWidgetItem;

	if (itemID < 0)
		itemID = list_areas_->count();

	QString label;
	label.append(QString("%1: ").arg(itemID));

	label.append(QString("BBox #%1; ").arg(anID));
	label.append(QString("LabelID: %1; ").arg(aBBox.label_ID_));
	label.append(
		QString("data:%1;%2;%3;%4; ").
		arg(aBBox.rect.topLeft().x()).
		arg(aBBox.rect.topLeft().y()).
		arg(aBBox.rect.width()).
		arg(aBBox.rect.height())
		);

	newItem->setText(label);
	//newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
	list_areas_->insertItem(itemID, newItem);
	list_areas_->setItemSelected(newItem, true);
}

//! A protected member adding a poly area to the QListWidget list_areas_
/*!
 * \param[in] aPolyID a integer which indicates poly id in the
 * list_polygon_ container
 * \param[in] aPoly a Polygon struct containing
 * a polygon data(QPolygon) and it's label ID
 * \param[in] itemID an integer indicates a position
 * of a new item in the list_areas_
 *
 * itemID is -1 by default so the new area item will be placed in the
 * end of the list
 */
void
ImageLabeler::addPolyArea(
	int aPolyID,
	Polygon aPoly,
	int itemID
)
{
	QListWidgetItem *newItem = new QListWidgetItem;

	if (-1 == itemID)
		itemID = list_areas_->count();

	QString label;
	label.append(QString("%1: ").arg(itemID));

	label.append(QString("Poly #%1; ").arg(aPolyID));
	label.append(QString("LabelID: %1; ").arg(aPoly.label_ID_));
	label.append("points:");
	for (int i = 0; i < aPoly.poly.count(); i++) {
		label.append(
			QString("%1;%2;").
			arg(aPoly.poly.point(i).x()).
			arg(aPoly.poly.point(i).y())
			);
	}

	newItem->setText(label);
	list_areas_->insertItem(itemID, newItem);
	list_areas_->setItemSelected(newItem, true);
}

//! A slot member allowing manual editing the selected item in the list_areas_
/*!
 *	Slot being called after "change area" item in the popup menu of the list_areas_
 *	widget was clicked
 */
void
ImageLabeler::editArea()
{
	/* signals are blocked to avoid recursive call of the same slot */
	bool oldState = list_areas_->blockSignals(true);
	if (!list_areas_->currentItem() || !list_areas_->count()) {
		return;
		/* NOTREACHED */
	}

	QListWidgetItem *current = list_areas_->currentItem();

	old_area_string_ = current->text();
	current->setFlags(current->flags() | Qt::ItemIsEditable);
	list_areas_->editItem(current);
	list_areas_->blockSignals(oldState);
}

void
ImageLabeler::focusOnArea()
{
	QListWidgetItem *current = list_areas_->currentItem();
	if (!current || !list_areas_->count()) {
		return;
		/* NOTREACHED */
	}

	image_holder_->focusOnArea(current);
}

//! \brief A slot member parsing a text of the changed item
//! and tries to edit the corresponding area
/*!
 *  \see polyFromListItemText(QString *aString,int *oldID)
 *  \see BBoxFromListItemText(QString *aString,int *oldID)
 *	\param[in] anItem a pointer to the QListWidgetItem object which contains
 *	area's changed data
 *
 *	Slot usually being called after item manual changing is done
 */
void
ImageLabeler::onAreaItemChange(QListWidgetItem *anItem)
{
	/* signals are blocked to avoid recursive call of the same slot */
	list_areas_->blockSignals(true);
	QString areaString = anItem->text();
	int oldID = -1;
	if (-1 != areaString.indexOf("Poly")) {
		Polygon *poly = new Polygon;
		*poly = polyFromListItemText(&areaString, &oldID);

		if (-1 < poly->label_ID_ && !poly->poly.isEmpty() &&
			-1 < oldID) {
			list_polygon_.takeAt(oldID);
			list_polygon_.insert(oldID, poly);
		}
		else
			anItem->setText(old_area_string_);
	}
	else if (-1 != areaString.indexOf("BBox")) {
		BoundingBox *bbox = new BoundingBox;
		*bbox = BBoxFromListItemText(&areaString, &oldID);

		if (-1 < bbox->label_ID_ && -1 < oldID) {
			list_bounding_box_.takeAt(oldID);
			list_bounding_box_.insert(oldID, bbox);
		}
		else
			anItem->setText(old_area_string_);
	}
	else {
		showWarning(tr("record format is corrupted, try again"));
		anItem->setText(old_area_string_);
	}

	anItem->setFlags(anItem->flags() ^ Qt::ItemIsEditable);
	list_areas_->blockSignals(false);
	image_holder_->update();

	unsaved_data_ = 1;
}

//! \brief A slot member changing the item in the list_areas_ to meet the changes
//! being made during area editing(where editing means moving the points of
//! an area)
/*!
 *	Slot usually being called on every change which was done with any area.
 */
void
ImageLabeler::onAreaEdit()
{
	if (!list_areas_->count() ||
		-1 == image_holder_->focusedSelection()) {
		//showWarning(tr("You haven't added any label"));
		return;
		/* NOTREACHED */
	}

	/* a figure was focused by double click on the list_areas_ widget
	 * see ImageHolder::focusOnArea(QListWidgetItem *) */
	int figureID = image_holder_->focusedSelection();
	Figure figure = image_holder_->focusedSelectionType();

	/* looking for the corresponding item in the list_areas_ */
	for (int i = 0; i < list_areas_->count(); i++) {
		QListWidgetItem *item = list_areas_->item(i);
		QString text = item->text();
		if (RectFigure == figure && -1 != text.indexOf("BBox")) {
			bool ok = 0;
			int num = getNumFromString(&text, "BBox #", ";", &ok);
			if (ok && num == figureID) {
				list_areas_->takeItem(i);
				addBBoxArea(num, *list_bounding_box_.at(num), i);
			}
		}
		else if (PolyFigure == figure && -1 != text.indexOf("Poly")) {
			bool ok = 0;
			int num = getNumFromString(&text, "Poly #", ";", &ok);
			if (ok && num == figureID) {
				list_areas_->takeItem(i);
				addPolyArea(num, *list_polygon_.at(num), i);
			}
		}
	}

	unsaved_data_ = 1;
}

//! \brief A slot member deleting selected area in the list_areas_ widget
//! and all other corresponding containers
/*!
 * \see deleteArea(QListWidgetItem *anItem)
 */
void
ImageLabeler::deleteArea()
{
	deleteArea(list_areas_->currentItem());
	/*if (!deleteArea(list_areas_->currentItem()))
		showWarning(tr("An error occurred while deleting the area"));*/
}

//! A slot member deleting certain area in the list_areas_ widget
//! and all other corresponding containers
/*!
 * \param[in] anItem a pointer to the QListWidgetItem which corresponds to
 * some certain area(bbox or poly)
 *
 * Slot parses the text from anItem and deletes area from list_areas_ and
 * from list_bounding_box_ or list_polygon depends on which type of area it
 * was
 *
 * returns true on success
 */
bool
ImageLabeler::deleteArea(QListWidgetItem *anItem)
{
	if (!anItem || !list_areas_->count()) {
		return false;
		/* NOTREACHED */
	}

	QString text = anItem->text();

	bool ok = 0;

	/*
	 * 0 - bbox
	 * 1 - poly
	 */
	QString shape;
	if (-1 != text.indexOf("BBox"))
		shape = QString("BBox");
	else if (-1 != text.indexOf("Poly"))
		shape = QString("Poly");
	else {
		return false;
		/* NOTREACHED */
	}

	int areaNum = getNumFromString(&text, "#", ";", &ok);

	if (!ok) {
		return false;
		/* NOTREACHED */
	}

	int currentItemRow = list_areas_->row(anItem);

	/* changing all shapes(depends on current) which are next in the list */
	for (int i = list_areas_->count() - 1; i > currentItemRow; i--) {
		QListWidgetItem item = *(list_areas_->item(i));
		QString newText = item.text();

		if (-1 == newText.indexOf(shape))
			continue;

		int num = getNumFromString(&newText, "#", ";", &ok);
		num--;
		QString numString = QString("%1").arg(num);

		int numPos = newText.indexOf("#") + 1;
		newText.replace(numPos, numString.size(), numString);

		list_areas_->takeItem(i);
		list_areas_->insertItem(i, newText);
	}

	list_areas_->takeItem(currentItemRow);
	if (shape == "BBox")
		list_bounding_box_.removeAt(areaNum);
	else
		list_polygon_.removeAt(areaNum);

	image_holder_->update();

	unsaved_data_ = 1;

	if (!list_areas_->count()) {
		button_delete_area_->setEnabled(false);
		button_change_area_->setEnabled(false);
		button_change_area_text_->setEnabled(false);
	}

	return true;
}

//! A slot member toggling the priority of current item(main/not main)
/*!
 * \see toggleLabelPriority(QListWidgetItem *anItem)
 * Slot not just only toggles current label priority but also checks other labels
 * if any of them are also main. If so it just makes them common(not main)
 */
void
ImageLabeler::toggleLabelPriority()
{
	if (!toggleLabelPriority(list_label_->currentItem()))
		showWarning(tr("An error occurred while toggling a label priority"));
}

//! A slot member toggling the priority of current item(main/not main)
/*!
 * \param[in] anItem a pointer to the QListWidgetItem object which corresponds to
 * some certain label
 *
 * Slot not just only toggles current label priority but also checks other labels
 * if any of the are also main. If so it just makes them common(not main)
 *
 * returns true on success
 */
bool
ImageLabeler::toggleLabelPriority(QListWidgetItem *anItem)
{
	if (!list_label_->count()) {
		return false;
		/* NOTREACHED */
	}

	/* because we need to keep BACKGROUND category */
	if (0 == list_label_->row(anItem)) {
		return false;
		/* NOTREACHED */
	}

	int itemRow = list_label_->row(anItem);
	QString text = anItem->text();

	/* cleaning previous " #main" mark */
	if (-1 != main_label_) {
		QListWidgetItem *lastMain = list_label_->item(main_label_);
		QString lastMainText = lastMain->text();
		int mainPos = lastMainText.indexOf(" #main");
		lastMainText = lastMainText.mid(0, mainPos);
		list_label_->blockSignals(true);
		lastMain->setText(lastMainText);
		list_label_->blockSignals(false);
	}

	if (main_label_ == itemRow) {
		int mainPos = text.indexOf(" #main");
		text = text.mid(0, mainPos);
		main_label_ = -1;
	}
	else {
		text.append(" #main");
		main_label_ = list_label_->row(anItem);
	}

	list_label_->blockSignals(true);
	anItem->setText(text);
	list_label_->blockSignals(false);

	image_holder_->update();

	unsaved_data_ = 1;

	return true;
}

//! A protected member recursively getting all images from directory dir
/*!
 * \param[in] dir a const QDir object indicating current directory
 *
 * Looking for images in the directory and adding them to the list at once
 *
 * Recursive search can be interrupted by interrupt_search flag being true;
 *
 * If there are any labeled images in the folder they will be loaded with all
 * corresponding data.
 */
void
ImageLabeler::getImagesFromDir(const QDir &dir)
{
	/* avoiding freezing during recursive search for files */
	QApplication::processEvents();

	/* interrupting */
	if (interrupt_search_) {
		clearAll();
		return;
	}

	QStringList filenameFilter;
	/* *.dat are included also but only to indicate labeled images */
	filenameFilter <<
		"*.jpeg" <<
		"*.jpg" <<
		"*.gif" <<
		"*.png" <<
		"*.bmp" <<
		"*.tiff" <<
		"*.dat"
		;

	QStringList listImages =
		dir.entryList(filenameFilter, QDir::Files);

	foreach (QString file, listImages) {
		/* ignoring segmented images and .dat files */
		if (file.contains("_segmented", Qt::CaseInsensitive) ||
			file.contains(".dat", Qt::CaseInsensitive)) {
			continue;
		}

		Image newImage;
		newImage.image_ = dir.absoluteFilePath(file);
		/* TODO: think about loading pascal files */
		newImage.pas_ = 0;

		/* checking if there is a data for current image */
		QString labeled = alterFileName(file, "_labeled");
		labeled = removePath(labeled);
		labeled.append(".dat");
		if (listImages.contains(labeled, Qt::CaseInsensitive))
			newImage.labeled_ = 1;
		else
			newImage.labeled_ = 0;

		addImage(&newImage);
	}

	/* recursively going into subdirectory */
	QStringList listDir = dir.entryList(QDir::Dirs);
	foreach (QString subdir, listDir) {
		if ("." == subdir || ".." == subdir)
			continue;
		getImagesFromDir(QDir(dir.absoluteFilePath(subdir)));
	}
}

//! \brief A slot member changing current image to the next one
//! and clearing all the data(except legend)
/*!
 * Asks about unsaved data if there is any. If current picture was the
 * last in the image list then it goes to the first one.
 * Adds to filename of the current image to the window title
 */
void
ImageLabeler::nextImage()
{
	if (list_images_->isEmpty()) {
		return;
		/* NOTREACHED */
	}

	if (!askForUnsavedData()) {
		return;
		/* NOTREACHED */
	}

	if (list_images_widget_->count() - 1 == image_ID_) {
		image_ID_ = 0;
	}
	else {
		image_ID_ ++;
	}

	if (!selectImage(image_ID_)) {
		showWarning(tr("Next image is not available"));
		return;
		/* NOTREACHED */
	}

	list_images_widget_->setCurrentRow(image_ID_);

	if (current_image_.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	QString winTitle;
	winTitle.append("ImageLabeler - ");
	winTitle.append(current_image_);
	setWindowTitle(winTitle);

	image_->load(current_image_);
	image_holder_->resize(image_->size());
	image_holder_->setPixmap(*image_);
	list_bounding_box_.clear();
	list_polygon_.clear();
	list_areas_->clear();
	image_holder_->clearAll();
	segmented_image_.clear();
	//clearLabelColorList();
}

//! \brief A slot member changing current image to the previous one
//! and clearing all the data(except legend)
/*!
 * Asks about unsaved data if there is any. If current picture was the
 * first in the image list then it goes to the last one.
 * Adds to filename of the current image to the window title
 */
void
ImageLabeler::prevImage()
{
	if (!list_images_widget_->count()) {
		return;
		/* NOTREACHED */
	}

	if (!askForUnsavedData()) {
		return;
		/* NOTREACHED */
	}

	if (!image_ID_) {
		image_ID_ = list_images_widget_->count() - 1;
	}
	else {
		image_ID_--;
	}

	list_images_widget_->setCurrentRow(image_ID_);

	if (!selectImage(image_ID_)) {
		showWarning(tr("Next image is not available"));
		return;
		/* NOTREACHED */
	}

	QString winTitle;
	winTitle.append("ImageLabeler - ");
	winTitle.append(current_image_);
	setWindowTitle(winTitle);

	image_->load(current_image_);
	image_holder_->resize(image_->size());
	image_holder_->setPixmap(*image_);
	list_bounding_box_.clear();
	list_polygon_.clear();
	list_areas_->clear();
	image_holder_->clearAll();
	segmented_image_.clear();
	//clearLabelColorList();
}

//! A Slot member saving all info about labeled image
/*!
 * It saves a file in xml format which contains:
 * - path to the original image
 * - path to the segmented image(if any)
 * - image description
 * - tags
 * - labels
 * - objects data
 * - segmented representation of the image as 2-dimensional array of label ids
 */
void
ImageLabeler::saveAllInfo()
{
	if (!list_images_widget_->count()) {
		showWarning("You have not opened any image yet");
		return;
		/* NOTREACHED */
	}

	/* ------------------------------------------------------------------------
	 * XML part
	 */
	QDomDocument doc(tr("ImageLabeler"));
	QDomElement root = doc.createElement(tr("pixelwise_labeling"));
	doc.appendChild(root);

	QDomElement image = doc.createElement(tr("image"));
	root.appendChild(image);

	QDomText pathToImage = doc.createTextNode(current_image_);
	image.appendChild(pathToImage);

	if (!segmented_image_.isEmpty()) {
		QDomElement segmentedImage = doc.createElement(tr("segmented"));
		root.appendChild(segmentedImage);

		QDomText pathToSegmented = doc.createTextNode(segmented_image_);
		segmentedImage.appendChild(pathToSegmented);
	}

	QDomElement description = doc.createElement(tr("description"));
	root.appendChild(description);

	QDomText descriptionText = doc.createTextNode(image_description_);
	description.appendChild(descriptionText);

	QDomElement tags = doc.createElement(tr("tags"));
	root.appendChild(tags);

	QDomText tagsText = doc.createTextNode(tags_);
	tags.appendChild(tagsText);

	legendToXml(&doc, &root);

	objectsToXml(&doc, &root);

	/* image size */
	QSize imageSize = image_->size();
	QString imageSizeString;
	imageSizeString.append(QString("%1;%2").
		arg(imageSize.width()).
		arg(imageSize.height()));

	QDomElement imageSizeElement = doc.createElement(tr("image_size"));
	QDomText imageSizeText = doc.createTextNode(imageSizeString);
	imageSizeElement.appendChild(imageSizeText);
	root.appendChild(imageSizeElement);

	/* pure data */
	setPureData();
	QString pixelValues;
	for (int i = 0; i < imageSize.height(); i++) {
		for (int j = 0; j < imageSize.width(); j++) {
			pixelValues.append(QString("%1;").arg(pure_data_[i][j]));
		}
		pixelValues.append("\n");
	}

	QDomElement pureData = doc.createElement(tr("pure_data"));
	QDomText pureDataText = doc.createTextNode(pixelValues);
	pureData.appendChild(pureDataText);
	root.appendChild(pureData);

	QString xml = doc.toString();
	/* ------------------------------------------------------------------------
	 * XML part ends
	 */

	QFileDialog fileDialog(0, tr("Save all info"));
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setDefaultSuffix("dat");
	fileDialog.setFileMode(QFileDialog::AnyFile);
	QString dir = getPathFromFilename(current_image_);

	/* altering the name of a new file */
	QString newFileName = alterFileName(current_image_, "_labeled");
	newFileName = removePath(newFileName);

	fileDialog.selectFile(newFileName);

	fileDialog.setDirectory(dir);

	QString filename;
	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Can not open file dialog"));
		return;
		/* NOTREACHED */
	}

	if (filename.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		showWarning(tr("Can not open file for writing"));
		return;
		/* NOTREACHED */
	}

	file.write(xml.toLocal8Bit());
	file.close();

	unsaved_data_ = 0;
}

//! A slot member saving a segmented image from pure_data_ array
/*!
 * \see setPureData()
 *
 * If label colors are not set it asks user about automatic color generation.
 * New image format is .png
 */
void
ImageLabeler::saveSegmentedPicture()
{
	if (list_bounding_box_.isEmpty() && list_polygon_.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	setPureData();

	QFileDialog fileDialog(0, tr("Save segmented picture"));
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setDefaultSuffix("png");
	fileDialog.setFileMode(QFileDialog::AnyFile);
	QString dir = getPathFromFilename(current_image_);

	/* altering the name of a new file */
	QString newFileName = alterFileName(current_image_, "_segmented");
	newFileName = removePath(newFileName);

	fileDialog.selectFile(newFileName);

	fileDialog.setDirectory(dir);

	QString filename;
	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		return;
		/* NOTREACHED */
	}

	QSize imageSize = image_holder_->pixmap()->size();
	QImage newImage(imageSize, QImage::Format_RGB32);
	bool generateColorsFlag = auto_color_generation_;
	bool flag = 0;


	/* checking if all the colors are are different from white(default) */
	if (!generateColorsFlag) {
		for (int i = 1; i < list_label_->count(); i++) {
			if (list_label_colors_.at(i) == 0xffffffff && !flag)
				flag = 1;
			else if (list_label_colors_.at(i) == 0xffffffff && flag) {
				QMessageBox msgBox;
				msgBox.setText(tr("There are few labels with default white color."));
				msgBox.setInformativeText(tr("Do you want to generate all colors automatically? Otherwise you'll have to do it manually."));
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				msgBox.setDefaultButton(QMessageBox::Yes);
				msgBox.setIcon(QMessageBox::Question);
				int ret = msgBox.exec();

				if (QMessageBox::Yes == ret) {
					generateColorsFlag = 1;
					break;
				}
				else {
					return;
					/* NOTREACHED */
				}
			}
		}
	}

	/* generating colors for labels */
	if (list_label_colors_.count() < list_label_->count() ||
		generateColorsFlag) {
		generateColors();
	}

	for (int i = 0; i < imageSize.height(); i++)
		for (int j = 0; j < imageSize.width(); j++) {
			newImage.setPixel(j, i, list_label_colors_.at(pure_data_[i][j]));
		}

	if (!newImage.save(filename, "png", 100)) {
		showWarning(tr("An error occurred while saving the segmented image"));
		return;
		/* NOTREACHED */
	}

	segmented_image_ = filename;
	action_view_segmented_->setEnabled(true);
}

//! A slot member saving only labels(legend) to the separate xml file
/*!
 * \see legendToXml(QDomDocument *aDoc, QDomElement *aRoot)
 *
 * Slot asks user where to save a file with the legend and saves it.
 */
void
ImageLabeler::saveLegend()
{
	if (!list_label_->count()) {
			showWarning("You have not added any label yet");
			return;
			/* NOTREACHED */
		}

	/* ------------------------------------------------------------------------
	 * XML part
	 */
	QDomDocument doc(tr("ImageLabeler"));
	QDomElement root = doc.createElement(tr("root"));
	doc.appendChild(root);

	legendToXml(&doc, &root);

	QString xml = doc.toString();
	/* ------------------------------------------------------------------------
	 * XML part ends
	 */

	QFileDialog fileDialog(0, tr("Save legend"));
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setDefaultSuffix("dat");
	fileDialog.setFileMode(QFileDialog::AnyFile);
	QString dir = getPathFromFilename(current_image_);

	/* altering the name of a new file */
	QString newFileName = alterFileName(current_image_, "_legend");
	newFileName = removePath(newFileName);

	fileDialog.selectFile(newFileName);

	fileDialog.setDirectory(dir);

	QString filename;
	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Can not open file dialog"));
		return;
		/* NOTREACHED */
	}

	if (filename.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		showWarning(tr("Can not open file for writing"));
		return;
		/* NOTREACHED */
	}

	file.write(xml.toLocal8Bit());
	file.close();
}

//! A slot member loading labeled image from formatted xml file.
/*!
 * \see loadInfo(QString filename)
 */
void
ImageLabeler::loadInfo()
{
	if (!askForUnsavedData()) {
		return;
		/* NOTREACHED */
	}

	QFileDialog fileDialog(0, tr("Load file with info"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setDefaultSuffix("dat");
	fileDialog.setFileMode(QFileDialog::AnyFile);

	QString filename;
	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Can not open file dialog"));
		return;
		/* NOTREACHED */
	}

	clearAllTool();
	clearLabelList();
	if (loadInfo(filename)) {
		enableTools();
		Image newImage;
		newImage.image_ = current_image_;
		newImage.labeled_ = 1;
		newImage.pas_ = 0;
		addImage(&newImage);
		image_ID_ = list_images_widget_->count() - 1;
		list_images_widget_->setCurrentRow(image_ID_);
	}

	unsaved_data_ = 0;
}

//! A slot member loading labeled image from formatted xml file.
/*!
 * \param[in] filename a QString object containing a path to the file
 * we need to load data from
 *
 * \see loadLegendFromNode(QDomElement *)
 * \see addBBoxFromData(QString *aBBoxData, int *ID)
 * \see addPolyFromData(QString *aPolyData, int *labelID)
 */
bool
ImageLabeler::loadInfo(QString filename)
{
	QDomDocument doc("Image Labeler");
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		showWarning(tr("Can not open such file"));
		return false;
		/* NOTREACHED */
	}

	QString errMsg;
	if (!doc.setContent(&file, &errMsg)) {
		showWarning(errMsg);
		file.close();
		return false;
		/* NOTREACHED */
	}

	file.close();

	/* getting all info */
	QDomElement elements = doc.documentElement();
	QDomNode rootNode = elements.firstChild();
	QString string;

	while(!rootNode.isNull()) {
		QDomElement element = rootNode.toElement();
		if(!element.isNull()) {
			/* path to the image */
			if (element.tagName() == "image") {
				string = element.text();
				if (string.isEmpty()) {
					showWarning(
						tr(
						"The file with data doesn't contain path to the image"
						)
						);
					return false;
					/* NOTREACHED */
				}
				if (!image_->load(string)) {
					return false;
					/* NOTREACHED */
				}
				current_image_ = string;

				QString winTitle;
				winTitle.append("ImageLabeler - ");
				winTitle.append(current_image_);
				setWindowTitle(winTitle);


				image_holder_->resize(image_->size());
				image_holder_->setPixmap(*image_);
			}
			/* path to the segmented image */
			if (element.tagName() == "segmented") {
				string = element.text();
				if (string.isEmpty()) {
					continue;
				}
				segmented_image_ = string;
				action_view_segmented_->setEnabled(true);
			}
			/* image description */
			else if (element.tagName() == "description" &&
				!element.text().isEmpty()) {
				image_description_ = element.text();
			}
			/* tags */
			else if (element.tagName() == "tags" &&
				!element.text().isEmpty()) {
				tags_ = element.text();
			}
			/* legend */
			else if (element.tagName() == "legend") {
				list_label_->clear();
				loadLegendFromNode(&element);
			}
			/* objects */
			else if (element.tagName() == "objects") {
				QDomNode subNode = element.firstChild();
				QDomElement subElement;

				button_delete_area_->setEnabled(false);
				button_change_area_->setEnabled(false);
				button_change_area_text_->setEnabled(false);

				while(!subNode.isNull()) {
					subElement = subNode.toElement();

					if (subElement.isNull() || subElement.text().isEmpty()) {
						subNode = subNode.nextSibling();
						continue;
					}

					string = subElement.attribute("id");
					bool ok = 1;
					int id = string.toInt(&ok, 10);

					if (!ok) {
						qDebug() <<
							"loadInfo: "
							"poly id format is corrupted";
						subNode = subNode.nextSibling();
						continue;
					}

					string = subElement.text();

					if (subElement.tagName() == "bbox")
						addBBoxFromData(&string, &id);
					else if (subElement.tagName() == "poly")
						addPolyFromData(&string, &id);
					else {
						qDebug() <<
							"loadInfo: "
							"poly id format is corrupted";
						subNode = subNode.nextSibling();
						continue;
					}

					button_delete_area_->setEnabled(true);
					button_change_area_->setEnabled(true);
					button_change_area_text_->setEnabled(true);

					subNode = subNode.nextSibling();
				}
			}
		}
		rootNode = rootNode.nextSibling();
	}

	unsaved_data_ = 0;
	return true;
}

//! A slot member loading info about labeled image from PASCAL file(xml)
/*!
 * \see loadPascalFile(QString aFilename, QString aPath)
 *
 * Slot asks user to set default "root" directory for the PASCAL files if
 * it wan't set before
 */
void
ImageLabeler::loadPascalFile()
{
	if (!askForUnsavedData()) {
		return;
		/* NOTREACHED */
	}

	if (PASCALpath_.isEmpty()) {
		showWarning(tr("before opening first PASCAL file please choose \"root\" directory"
			" where a folder with segmentations,"
			" a folder with polygons,"
			" a folder with image descriptions and"
			" a folder with the pure images are."));

		QFileDialog fileDialog(0, tr("root directory for the PASCAL files"));
		fileDialog.setFileMode(QFileDialog::Directory);
		if (fileDialog.exec())
			PASCALpath_ = fileDialog.selectedFiles().last();
		else {
			return;
			/* NOTREACHED */
		}
	}

	QFileDialog fileDialog(0, tr("Load pascal file"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setDefaultSuffix("xml");
	fileDialog.setFileMode(QFileDialog::AnyFile);
	fileDialog.setDirectory(PASCALpath_);
	QStringList filters;
	filters << "PASCAL xml data (*.xml)"
			<< "Any files (*)";

	fileDialog.setNameFilters(filters);

	QString filename;
	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Can not open file dialog"));
		return;
		/* NOTREACHED */
	}

	clearAllTool();
	clearLabelList();
	if (loadPascalFile(filename, PASCALpath_)) {
		enableTools();
		Image newImage;
		newImage.image_ = current_image_;
		newImage.labeled_ = 1;
		newImage.pas_ = 1;
		addImage(&newImage);
		image_ID_ = list_images_widget_->count() - 1;
		list_images_widget_->setCurrentRow(image_ID_);
	}

	unsaved_data_ = 0;
}

//! A slot member loading info about labeled image from PASCAL file(xml)
/*!
 * Slot parses xml file and gets all needed info out of it
 */
bool
ImageLabeler::loadPascalFile(QString aFilename, QString aPath)
{
	QDomDocument doc;
	QFile file(aFilename);
	if (!file.open(QIODevice::ReadOnly)) {
		showWarning(tr("Can not open such file"));
		return false;
		/* NOTREACHED */
	}

	QString errMsg;
	if (!doc.setContent(&file, &errMsg)) {
		showWarning(errMsg);
		file.close();
		return false;
		/* NOTREACHED */
	}

	file.close();

	//clearAll();
	//enableTools();

	/* getting all info */
	QDomElement elements = doc.documentElement();
	QDomNode rootNode = elements.firstChild();
	QString string;
	QString path;
	if (aPath.isEmpty())
		path = getPathFromFilename(aFilename);
	else
		path = aPath + "/";
	QString filename;
	QStringList labels;
	labels << "BACKGROUND";
	int labelID;

	while(!rootNode.isNull()) {
		QDomElement element = rootNode.toElement();
		if(!element.isNull()) {
			/* folder */
			if (element.tagName() == "folder") {
				string = element.text();
				if (!string.isEmpty()) {
					path.append(string);
				}
			}
			/* filename */
			else if (element.tagName() == "filename") {
				string = element.text();
				if (!string.isEmpty()) {
					filename = string;
				}
			}
			/* object */
			else if (element.tagName() == "object") {
				QDomNode subNode = element.firstChild();
				QDomElement subElement;

				while(!subNode.isNull()) {
					subElement = subNode.toElement();

					if (subElement.isNull() || subElement.text().isEmpty()) {
						subNode = subNode.nextSibling();
						continue;
					}

					/* label */
					if (subElement.tagName() == "name") {
						string = subElement.text();
						if (!string.isEmpty() &&
							!labels.contains(string, Qt::CaseInsensitive))
						{
							addLabel(list_label_->count(), 0, string);
							labelID = labels.count();
							labels << string;
						}
						else if (labels.contains(string, Qt::CaseInsensitive)) {
							for (int i = 0; i < labels.count(); i++) {
								if (labels.at(i) == string)
									labelID = i;
							}

						}
					}

					if (subElement.tagName() == "bndbox") {
						/* 2 points */
						QPoint topLeft;
						QPoint bottomRight;
						QDomNode bboxNode = subElement.firstChild();
						QDomElement bboxElement;
						while(!bboxNode.isNull()) {
							bboxElement = bboxNode.toElement();
							string.clear();
							bool ok = 1;
							if (bboxElement.tagName() == "xmin") {
								string = bboxElement.text();
								if (string.isEmpty())
									ok = 0;
								double xmin = string.toDouble(&ok);
								if (ok)
									topLeft.setX(qRound(xmin));

							}
							else if (bboxElement.tagName() == "ymin") {
								string = bboxElement.text();
								if (string.isEmpty())
									ok = 0;
								double ymin = string.toDouble(&ok);
								if (ok)
									topLeft.setY(qRound(ymin));
							}
							else if (bboxElement.tagName() == "xmax") {
								string = bboxElement.text();
								if (string.isEmpty())
									ok = 0;
								double xmax = string.toDouble(&ok);
								if (ok)
									bottomRight.setX(qRound(xmax));
							}
							else if (bboxElement.tagName() == "ymax") {
								string = bboxElement.text();
								if (string.isEmpty())
									ok = 0;
								double ymax = string.toDouble(&ok);
								if (ok)
									bottomRight.setY(qRound(ymax));
							}
							if (string.isEmpty() || !ok)
								break;

							bboxNode = bboxNode.nextSibling();

							if (bboxNode.isNull()) {
								BoundingBox *bbox = new BoundingBox;
								bbox->rect.setTopLeft(topLeft);
								bbox->rect.setBottomRight(bottomRight);
								bbox->label_ID_ = labelID;

								addBBox(bbox);
							}
						}
					}
					subNode = subNode.nextSibling();
				}
			}
		}

		rootNode = rootNode.nextSibling();
	}

	if (!image_->load(path + "/JPEGImages/" + filename)) {
		return false;
		/* NOTREACHED */
	}

	current_image_ = path + "/JPEGImages/" + filename;

	QString winTitle;
	winTitle.append("ImageLabeler - ");
	winTitle.append(current_image_);
	setWindowTitle(winTitle);

	image_holder_->resize(image_->size());
	image_holder_->setPixmap(*image_);

	unsaved_data_ = 0;
	return true;
}

//! A slot member loading only information about polygons on the image
/*!
 * \see loadPascalPolys(QString aFilename)
 * Slot parses text file with .polygon suffix
 */
void
ImageLabeler::loadPascalPolys()
{
	if (current_image_.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	QFileDialog fileDialog(0, tr("Load pascal polygons"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setDefaultSuffix("polygon");
	fileDialog.setFileMode(QFileDialog::AnyFile);
	QStringList filters;
	filters << "PASCAL polygons (*.polygon)"
			<< "Any files (*)";

	fileDialog.setNameFilters(filters);

	QString filename;
	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Can not open file dialog"));
		return;
		/* NOTREACHED */
	}

	//clearAllTool();
	//clearLabelList();
	if (!loadPascalPolys(filename)) {
		showWarning(tr("File format is corrupted."));
	}

	unsaved_data_ = 0;
	image_holder_->update();
}

//! A slot member loading only information about polygons on the image
/*!
 * Slot parses text file with .polygon suffix
 *
 * returns true on success
 */
bool
ImageLabeler::loadPascalPolys(QString aFilename)
{
	QFile file(aFilename);
	if (!file.open(QIODevice::ReadOnly)) {
		showWarning(tr("Can not open such file"));
		return false;
		/* NOTREACHED */
	}

	QByteArray data = file.readAll();

	file.close();

	QString label;
	int lastSpace = 0;
	int pointCount = 0;
	Polygon *poly = 0;
	QPoint point;
	bool evenFlag = 0;
	//for (int i = 0; i < data.length(); i++) {
	int i = 0;
	while (i < data.length()) {
		if (data.at(i) == ' ' && label.isEmpty() && !pointCount) {
			label = QString(data.mid(lastSpace, i - lastSpace));
			lastSpace = i + 1;
		}
		else if (data.at(i) == ' ' && !label.isEmpty() && !pointCount) {
			QString num = QString(data.mid(lastSpace, i - lastSpace));
			bool ok = 1;
			pointCount = num.toInt(&ok, 10);
			if (!ok) {
				return false;
				/* NOTREACHED */
			}
			poly = new Polygon;
			poly->label_ID_ = -1;
			lastSpace = i + 1;
		}
		else if (data.at(i) == ' ' &&
			!label.isEmpty() &&
			pointCount &&
			!evenFlag)
		{
			evenFlag = 1;
			QString num = QString(data.mid(lastSpace, i - lastSpace));
			bool ok = 1;
			int coor = qRound(num.toDouble(&ok));
			if (!ok) {
				return false;
				/* NOTREACHED */
			}
			point.setX(coor);
			lastSpace = i + 1;
		}
		else if (data.at(i) == ' ' &&
			!label.isEmpty() &&
			pointCount &&
			evenFlag)
		{
			evenFlag = 0;
			pointCount--;
			QString num = QString(data.mid(lastSpace, i - lastSpace));
			bool ok = 1;
			int coor = qRound(num.toDouble(&ok));
			if (!ok) {
				return false;
				/* NOTREACHED */
			}
			point.setY(coor);
			lastSpace = i + 1;
			poly->poly << point;
		}
		else if (data.at(i) == '\n') {
			if (pointCount || !poly) {
				return false;
				/* NOTREACHED */
			}
			lastSpace = i + 1;
			for (int j = 0; j < list_label_->count(); j++) {
				if (list_label_->item(j)->text().contains(label, Qt::CaseInsensitive)) {
					poly->label_ID_ = j;
				}
			}
			if (-1 == poly->label_ID_) {
				int labelID = list_label_->count();
				addLabel(labelID, 0, label);
				poly->label_ID_ = labelID;
			}
			addPoly(poly);
			label.clear();
		}

		i++;
	}

	return true;
}

//! A Slot member loading single image.
/*!
 * \see loadImages()
 *
 * Slot asks for unsaved data and loads an image.
 * If selected image was previously labeled and the file with data
 * was named like imagename_labeled.dat then it would be loaded
 * with all data
 */
void
ImageLabeler::loadImage()
{
	if (!askForUnsavedData()) {
		return;
		/* NOTREACHED */
	}

	QFileDialog fileDialog(0, tr("Load image"));
	fileDialog.setFileMode(QFileDialog::ExistingFile);
	QStringList filters;
	filters << "Image files (*.jpeg *.jpg *.gif *.png *.bmp *.tiff)"
			<< "Any files (*)";

	fileDialog.setNameFilters(filters);
	QString filename;

	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Could not open file dialog"));
		return;
		/* NOTREACHED */
	}

	if (filename.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	clearAllTool();

	/* checking if it was previously labeled */
	QString dirPath = getPathFromFilename(filename);
	QDir dir(dirPath);
	QStringList filter;
	filter << "*.dat";
	QStringList fileList = dir.entryList(filter, QDir::Files);
	QString labeled = alterFileName(filename, "_labeled");
	labeled = removePath(labeled);
	labeled.append(".dat");
	if (fileList.contains(labeled, Qt::CaseInsensitive)) {
		labeled = dir.absoluteFilePath(labeled);
		loadInfo(labeled);
		Image *newImage = new Image;
		newImage->image_ = filename;
		newImage->labeled_ = 1;
		newImage->pas_ = 0;
		addImage(newImage);
		enableTools();
		return;
		/* NOTREACHED */
	}

	QString winTitle;
	winTitle.append("ImageLabeler - ");
	winTitle.append(current_image_);
	setWindowTitle(winTitle);

	if (!loadPixmap(filename)) {
		return;
		/* NOTREACHED */
	}
//	if (!image_->load(filename)) {
//		return;
//		/* NOTREACHED */
//	}
//	image_holder_->resize(image_->size());
//	image_holder_->setPixmap(*image_);

	current_image_ = filename;
	image_ID_ = list_images_widget_->count() - 1;
	list_images_widget_->setCurrentRow(image_ID_);

	Image *newImage = new Image;
	newImage->image_ = filename;
	newImage->labeled_ = 0;
	newImage->pas_ = 0;
	addImage(newImage);

	if (!list_images_widget_->count()) {
		return;
		/* NOTREACHED */
	}

	enableTools();
}

//! A slot member loading images recursively
/*!
 * \see getImagesFromDir(const QDir &dir)
 *
 * Slot asks for unsaved data.
 * It gives user a possibility to break a recursive search(with the widget
 * and a button "cancel" on it.)
 *
 */
void
ImageLabeler::loadImages()
{
	if (!askForUnsavedData()) {
		return;
		/* NOTREACHED */
	}

	QFileDialog fileDialog(0, tr("Load images"));
	fileDialog.setFileMode(QFileDialog::Directory);
	QString dirName("");

	if (fileDialog.exec()) {
		/* TODO: make it possible to select multiple folders */
		dirName = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Could not open file dialog"));
		return;
		/* NOTREACHED */
	}

	clearAllTool();

	/* creating a widget with a "cancel" button to
	 * give user an opportunity to stop the recursive search */
	QWidget *searchInProgress = new QWidget(0);
	searchInProgress->setWindowTitle(tr("Loading images"));
	QLabel *info = new QLabel(searchInProgress);
	info->setText(
		tr("Program is looking for all image files in your directory recursively."));
	QPushButton *cancel = new QPushButton(tr("Cancel"), searchInProgress);
	connect(cancel, SIGNAL(clicked()), this, SLOT(interruptSearch()));
	QVBoxLayout *layout = new QVBoxLayout(searchInProgress);
	searchInProgress->setLayout(layout);
	layout->addWidget(info);
	layout->addWidget(cancel);
	searchInProgress->adjustSize();
	searchInProgress->move(QApplication::desktop()->screen()->rect().center() - rect().center());
	searchInProgress->show();

	getImagesFromDir(QDir(dirName));

	/* deleting everything related to searchInProgress widget */
	cancel->disconnect();
	searchInProgress->hide();
	delete info;
	delete cancel;
	delete layout;
	delete searchInProgress;

	if (interrupt_search_) {
		interrupt_search_ = 0;
		return;
		/* NOTREACHED */
	}

	if (!list_images_widget_->count()) {
		showWarning(tr("The folder you selected contains no images"));
		return;
		/* NOTREACHED */
	}

	bool ret = 0;
	if (list_images_->at(0).labeled_) {
		QString labeled =
			alterFileName(list_images_->at(0).image_, "_labeled");
		labeled.append(".dat");
		ret = loadInfo(labeled);
	}
	else
		ret = image_->load(list_images_->at(0).image_);

	if (!ret) {
		return;
		/* NOTREACHED */
	}
	current_image_ = list_images_->at(0).image_;
	image_ID_ = 0;
	list_images_widget_->setCurrentRow(image_ID_);

	QString winTitle;
	winTitle.append("ImageLabeler - ");
	winTitle.append(current_image_);
	setWindowTitle(winTitle);

	image_holder_->resize(image_->size());
	image_holder_->setPixmap(*image_);

	enableTools();
}

//! A slot member loading legend(labels) from xml file
/*!
 * \see loadLegendFromNode(QDomElement *anElement)
 */
void
ImageLabeler::loadLegendFromFile()
{
	QFileDialog fileDialog(0, tr("Load legend"));
	fileDialog.setFileMode(QFileDialog::AnyFile);
	QString filename;

	if (fileDialog.exec()) {
		filename = fileDialog.selectedFiles().last();
	}
	else {
		//showWarning(tr("Could not open file dialog"));
		return;
		/* NOTREACHED */
	}

	QDomDocument doc("Image Labeler");
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		showWarning(tr("Can not open such file"));
		return;
		/* NOTREACHED */
	}

	QString errMsg;
	if (!doc.setContent(&file, &errMsg)) {
		showWarning(errMsg);
		file.close();
		return;
		/* NOTREACHED */
	}

	file.close();

	list_label_->clear();

	/* getting legend */
	QDomElement elements = doc.documentElement();
	QDomNode rootNode = elements.firstChild();
	QString string;

	while(!rootNode.isNull()) {
		QDomElement element = rootNode.toElement();
		if(!element.isNull()) {
			if (element.tagName() == "legend") {
				loadLegendFromNode(&element);
			}
		}
		rootNode = rootNode.nextSibling();
	}
}

//! A protected member loading legend from xml node
/*!
 * \see addLabel(int aLabelID, bool isMain, QString aLabel)
 * \see setLabelColor(int anID, QColor aColor)
 *
 * Slot gets all label information from xml node.
 */
void
ImageLabeler::loadLegendFromNode(QDomElement *anElement)
{
	if (!anElement) {
		return;
		/* NOTREACHED */
	}
	QDomNode subNode = anElement->firstChild();
	QDomElement subElement;
	QString string;
	int id = -1;
	bool isMain;
	uint color = 0xff000000;

	while(!subNode.isNull()) {
		subElement = subNode.toElement();

		if (!subElement.isNull() && !subElement.text().isEmpty()) {
			/* id attribute */
			string = subElement.attribute("id");
			bool ok = 0;
			id = string.toInt(&ok, 10);

			if (!ok) {
				qDebug() <<
					"loadLegendFromNode: "
					"label id format is corrupted";
				subNode = subNode.nextSibling();
				continue;
			}

			/* isMain attribute */
			string = subElement.attribute("isMain");
			isMain = string.toInt(&ok, 2);

			if (!ok) {
				qDebug() <<
					"loadLegendFromNode: "
					"label isMain flag format is corrupted";
				subNode = subNode.nextSibling();
				continue;
			}

			/* color attribute */
			string = subElement.attribute("color");
			color = string.toUInt(&ok, 16);

			if (!ok) {
				qDebug() <<
					"loadLegendFromNode: "
					"label color format is corrupted";
				subNode = subNode.nextSibling();
				continue;
			}

			/* label name */
			string = subElement.text();

			addLabel(id, isMain, string);
			setLabelColor(id, color);
		}

		subNode = subNode.nextSibling();
	}
}

//! A protected member converting all the label information into xml format
/*!
 * \see objectsToXml(QDomDocument *aDoc, QDomElement *aRoot)
 * \param[in] aDoc a pointer to QDomDocument object - represents an xml document
 * \param[out] aRoot a pointer to QDomElement object - contains all the results from
 * the converting
 */
void
ImageLabeler::legendToXml(QDomDocument *aDoc, QDomElement *aRoot)
{
	QDomElement legend = aDoc->createElement(tr("legend"));
	aRoot->appendChild(legend);

	/* storing all labels made by user */
	int labelCount = list_label_->count();
	for (int i = 0; i < labelCount; i++) {
		QDomElement label = aDoc->createElement(tr("label"));
		label.setAttribute("color", QString("%1").arg(list_label_colors_.at(i), 0, 16));
		label.setAttribute("id", i);

		QString priority;
		if (main_label_ == i)
			priority.append("1");
		else
			priority.append("0");
		label.setAttribute("isMain", priority);

		QString labelText = list_label_->item(i)->text();

		/* removing the number prefix of label */
		if (-1 != labelText.indexOf(QString("%1: ").arg(i))) {
			labelText = labelText.mid(3, labelText.size() - 3);
		}

		QDomText labelName = aDoc->createTextNode(labelText);
		label.appendChild(labelName);
		legend.appendChild(label);
	}

	/* in case we have no labels */
	if (0 == labelCount) {
		QDomElement label = aDoc->createElement(tr("label"));
		label.setAttribute(tr("id"), -1);
		legend.appendChild(label);
	}
}

//! A protected member converting all the objects information into xml format
/*!
 * \see legendToXml(QDomDocument *aDoc, QDomElement *aRoot)
 * \param[in] aDoc a pointer to the QDomDocument object - represents an xml document
 * \param[out] aRoot a pointer to the QDomElement object - contains all the results from
 * the converting
 */
void
ImageLabeler::objectsToXml(QDomDocument *aDoc, QDomElement *aRoot)
{
	QDomElement objects = aDoc->createElement(tr("objects"));
	aRoot->appendChild(objects);

	/* rects first */
	for (int i = 0; i < list_bounding_box_.size(); i++) {
		QDomElement rectData = aDoc->createElement(tr("bbox"));

		rectData.setAttribute("id", list_bounding_box_.at(i)->label_ID_);

		QRect rect = list_bounding_box_.at(i)->rect.normalized();

		QString rectDataString =
			QString("%1;%2;%3;%4;").
				arg(rect.x()).
				arg(rect.y()).
				arg(rect.width()).
				arg(rect.height());

		QDomText rectDataText = aDoc->createTextNode(rectDataString);
		rectData.appendChild(rectDataText);
		objects.appendChild(rectData);
	}

	/* polys next */
	for (int i = 0; i < list_polygon_.size(); i++) {
		QDomElement polyData = aDoc->createElement(tr("poly"));

		polyData.setAttribute("id", list_polygon_.at(i)->label_ID_);

		QPolygon poly = list_polygon_.at(i)->poly;

		QString polyDataString;
		for (int j = 0; j < poly.count(); j++)
			polyDataString.append(
				QString("%1;%2;").
					arg(poly.point(j).x()).
					arg(poly.point(j).y())
				);


		QDomText polyDataText = aDoc->createTextNode(polyDataString);
		polyData.appendChild(polyDataText);
		objects.appendChild(polyData);
	}
}

//! A slot member generating color for all labels
/*!
 * A very primitive temporary solution for color generation
 */
void
ImageLabeler::generateColors()
{
	int labelCount = list_label_->count();

	if (!labelCount) {
		showWarning(tr("you have not added any labels yet"));
		return;
		/* NOTREACHED */
	}

	int coeff = (0xff / labelCount) * 3;
	list_label_colors_.clear();
	list_label_colors_.append(0);
	uchar red = 0xff;
	uchar green = 0xff;
	uchar blue = 0xff;
	uchar iterationColor = coeff;
	uint color = 0xffffffff;
	int j = 1;
	for (int i = 1; i < labelCount; i++) {
		if (6 == j) {
			iterationColor += coeff;
			j = 1;
		}

		if (5 == j) {
			red = 0xff - iterationColor;
			green = 0xff - iterationColor;
			blue = 0xff;
		}
		else if (4 == j) {
			red = 0xff - iterationColor;
			green = 0xff;
			blue = 0xff - iterationColor;
		}
		else if (3 == j) {
			red = 0xff - iterationColor;
			green = 0xff;
			blue = 0xff;
		}
		else if (2 == j) {
			red = 0xff;
			green = 0xff - iterationColor;
			blue = 0xff;
		}
		else if (1 == j){
			red = 0xff;
			green = 0xff;
			blue = 0xff - iterationColor;
		}
		j++;
		color = red + (green * 0x100) + (blue * 0x10000) + 0xff000000;

		int itemNo = list_label_colors_.count();

		QPixmap iconPix = QPixmap(20, 20);
		iconPix.fill(color);
		QIcon icon(iconPix);

		list_label_->item(itemNo)->setIcon(icon);
		list_label_colors_.append(color);
	}

	image_holder_->update();
}

//! A protected member creating new boundary box from text data
/*!
 * \see addBBoxArea(int anID, BoundingBox aBBox, int itemID)
 * \see BBoxFromData(QString *aBBoxData)
 *
 * It creates bbox both in the container and in the list widget
 */
void
ImageLabeler::addBBoxFromData(
	QString *aBBoxData,
	int *ID
)
{
	BoundingBox *bbox = new BoundingBox;
	*bbox = BBoxFromData(aBBoxData);
	if (!bbox->rect.isValid() || !ID) {
		return;
		/* NOTREACHED */
	}

	bbox->label_ID_ = *ID;
	list_bounding_box_.append(bbox);
	addBBoxArea(list_bounding_box_.count() - 1, *bbox);
}

//! A protected member parsing string data and returning a BoundingBox from it
/*!
 * \see addBBoxArea(int anID, BoundingBox aBBox, int itemID)
 *
 * format is x;y;w;h where w - width and h - height
 */
BoundingBox
ImageLabeler::BBoxFromData(
	QString *aBBoxData
)
{
	BoundingBox bbox;
	QString buffer;
	bbox.rect.setRect(-1, -1, -1, -1);
	int startPos = 0;
	bool ok = 1;

	for (int i = 0; i < aBBoxData->size(); i++) {
		if (';' != aBBoxData->at(i))
			continue;

		buffer = aBBoxData->mid(startPos, i - startPos);

		int bboxData = buffer.toInt(&ok, 10);
		if (!ok) {
			qDebug() <<
				"BBoxFromData: "
				"bbox format is corrupted";
			break;
		}

		if (-1 == bbox.rect.x()) {
			bbox.rect.setX(bboxData);
			bbox.rect.setWidth(-1);
		}
		else if (-1 == bbox.rect.y()) {
			bbox.rect.setY(bboxData);
			bbox.rect.setHeight(-1);
		}
		else if (-1 == bbox.rect.width()) {
			bbox.rect.setWidth(bboxData);
		}
		else if (-1 == bbox.rect.height()) {
			bbox.rect.setHeight(bboxData);
		}

		startPos = i + 1;
	}

	if (!bbox.rect.isValid()) {
		qDebug() <<
			"BBoxFromData: "
			"bbox format is corrupted";
		bbox.rect.setRect(-1, -1, -1, -1);
	}
	else if (!ok) {
		bbox.rect.setRect(-1, -1, -1, -1);
	}

	return bbox;
}

//! A protected member getting a BoundaryBox from text of the list_areas_ item
/*!
 * \see BBoxFromData(QString *aBBoxData)
 *
 * Slot gets label ID an points from the string data. BBox ID can not be changed.
 */
BoundingBox
ImageLabeler::BBoxFromListItemText(
	QString *aString,
	int *oldID
)
{
	BoundingBox bbox;
	bbox.label_ID_ = -1;
	*oldID = -1;

	if (!aString) {
		return bbox;
		/* NOTREACHED */
	}

	if (-1 == aString->indexOf("BBox")) {
		return bbox;
		/* NOTREACHED */
	}

	/* getting bbox id in the list(it cannot be changed) */
	bool ok = 0;
	int bboxID = getNumFromString(aString, "BBox #", ";", &ok);
	if (!ok || bboxID <= -1) {
		qDebug() <<
			"BBoxFromListItemText: bboxID is corrupted";
		return bbox;
		/* NOTREACHED */
	}

	/* getting new label id */
	int labelID = getNumFromString(aString, "LabelID: ", ";", &ok);
	if (!ok || labelID <= -1) {
		showWarning(
			tr("new LabelID is wrong, area can not be changed")
				);
		return bbox;
		/* NOTREACHED */
	}

	/* getting new points */
	int pointsPos = aString->indexOf("data:") + 5;
	int pointsLen = aString->size() - pointsPos;
	if (pointsLen <= 0) {
		showWarning(
			tr("new data is wrong, area can not be changed")
				);
		return bbox;
		/* NOTREACHED */
	}
	QString pointsData = aString->mid(pointsPos, pointsLen);
	bbox = BBoxFromData(&pointsData);
	bbox.label_ID_ = labelID;


	*oldID = bboxID;
	return bbox;
}

//! A protected member adding poly to the list widget and to the list container
/*!
 * \see addBBox(BoundingBox *bbox)
 * \param[in] poly a pointer to the Polygon structure
 *  containing new poly information
 *
 */
void
ImageLabeler::addPoly(Polygon *poly)
{
	if (poly->poly.isEmpty() || poly->label_ID_ < 0) {
		return;
		/* NOTREACHED */
	}

	list_polygon_.append(poly);
	addPolyArea(list_polygon_.count() - 1, *poly);

}

//! A protected member adding bounding box to the list widget
//! and to the list container
/*!
 * \see addPoly(Polygon *poly)
 * \param[in] bbox a pointer to the BoundingBox structure
 *  containing new bounding box information
 *
 */
void
ImageLabeler::addBBox(BoundingBox *bbox)
{
	if (bbox->rect.isEmpty() || bbox->label_ID_ < 0) {
		return;
		/* NOTREACHED */
	}

	list_bounding_box_.append(bbox);
	addBBoxArea(list_bounding_box_.count() - 1, *bbox);
}

//! A protected member creating new polygon from text data
/*!
 * \see addPolyArea(int anID, Polygon aPoly, int itemID)
 * \see polyFromData(QString *aPolyData)
 *
 * It creates polygon both in the container and in the list widget
 */
void
ImageLabeler::addPolyFromData(
	QString *aPolyData,
	int *labelID
)
{
	Polygon *poly = new Polygon;
	*poly = polyFromData(aPolyData);

	if (poly->poly.isEmpty() || !labelID) {
		return;
		/* NOTREACHED */
	}

	poly->label_ID_ = *labelID;
	list_polygon_.append(poly);
	addPolyArea(list_polygon_.count() - 1, *poly);
}

//! A protected member parsing string data and returning a Polygon from it
/*!
 * \see addPolyArea(int anID, Polygon aPoly, int itemID)
 *
 * format is x0;y0;x1;y1;...
 */
Polygon
ImageLabeler::polyFromData(
	QString *aPolyData
)
{
	Polygon poly;
	poly.label_ID_ = -1;
	QPoint point;
	QString buffer;
	int startPos = 0;
	bool ok = 1;
	/* indicates whether coordinate x or y */
	bool evenFlag = 0;

	for (int i = 0; i < aPolyData->size(); i++) {
		/* ";" is a separator */
		if (';' != aPolyData->at(i))
			continue;

		buffer = aPolyData->mid(startPos, i - startPos);

		int polyCoor = buffer.toInt(&ok, 10);
		if (!ok) {
			qDebug() <<
				"polyFromData: "
				"poly format is corrupted";
			break;
		}

		if (!evenFlag) {
			point.setX(polyCoor);
			evenFlag = 1;
		}
		else {
			point.setY(polyCoor);
			poly.poly.append(point);
			evenFlag = 0;
		}
		startPos = i + 1;
	}

	/* last coordinate was Xi what means an error */
	if (evenFlag) {
		qDebug() <<
			"polyFromData: "
			"poly format is corrupted";
		poly.poly.clear();
	}
	/* last converting from string was not successful */
	else if (!ok) {
		poly.poly.clear();
	}

	return poly;
}

//! A protected member getting a Polygon from text of the list_areas_ item
/*!
 * \see polyFromData(QString *aPolyData)
 *
 * Slot gets label ID an points from the string data. Poly ID can not be changed.
 */
Polygon
ImageLabeler::polyFromListItemText(
	QString *aString,
	int *oldID
)
{
	Polygon poly;
	poly.label_ID_ = -1;
	*oldID = -1;

	if (!aString) {
		return poly;
		/* NOTREACHED */
	}

	if (-1 == aString->indexOf("Poly")) {
		return poly;
		/* NOTREACHED */
	}

	/* getting poly id in the list(it cannot be changed) */
	bool ok = 0;
	int polyID = getNumFromString(aString, "Poly #", ";", &ok);
	if (!ok || polyID <= -1) {
		qDebug() <<
			"polyFromListItemText: poly ID is corrupted";
		return poly;
		/* NOTREACHED */
	}

	/* getting new label id */
	int labelID = getNumFromString(aString, "LabelID: ", ";", &ok);
	if (!ok || labelID <= -1) {
		showWarning(
			tr("new LabelID is wrong, area can not be changed")
				);
		return poly;
		/* NOTREACHED */
	}

	/* getting new points */
	int pointsPos = aString->indexOf("points:") + 7;
	int pointsLen = aString->size() - pointsPos;
	if (pointsLen <= 0) {
		showWarning(
			tr("new points data is wrong, area can not be changed")
				);
		return poly;
		/* NOTREACHED */
	}
	QString pointsData = aString->mid(pointsPos, pointsLen);
	poly = polyFromData(&pointsData);
	poly.label_ID_ = labelID;


	*oldID = polyID;
	return poly;
}

//! A protected member only for internal use. Showing a message box with warning
/*!
 * \param[in] text a QString object containing message you want
 *  to deliver to the user
 */
void
ImageLabeler::showWarning(
		const QString &text
)
{
	if (text.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.exec();
}

//! A protected member only for internal use.
//! Shows a message box asking about unsaved progress
/*!
 * It depends on object containers(list_bounding_box_ and list_polygon_)
 * and on unsaved_data_ flag which must be true to make this dialog appear
 */
bool
ImageLabeler::askForUnsavedData()
{
	if ((!list_bounding_box_.isEmpty() ||
		!list_polygon_.isEmpty()) &&
		unsaved_data_)
	{
		QMessageBox msgBox;
		msgBox.setText(tr("There is some unsaved data"));
		msgBox.setInformativeText(tr("Do you want to save your progress?"));
		msgBox.setStandardButtons(
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setIcon(QMessageBox::Question);
		int ret = msgBox.exec();

		if (QMessageBox::Save == ret)
			saveAllInfo();
		else if (QMessageBox::Cancel == ret)
			return true;
	}
	return true;
}

//! A slot member setting bbox tool active
/*!
 * \param[in] aButtonPressed is a bool flag which is needed for toggled buttons
 */
void
ImageLabeler::setBoundingBoxTool(bool aButtonPressed)
{
	if (aButtonPressed)
		image_holder_->setTool(ImageHolder::BoundingBoxTool);
	else {
		image_holder_->setTool(ImageHolder::NoTool);
		image_holder_->clearLast();
	}
}

//! A slot member setting polygon tool active
/*!
 * \param[in] aButtonPressed is a bool flag which is needed for toggled buttons
 */
void
ImageLabeler::setPolygonTool(bool aButtonPressed)
{
	if (aButtonPressed) {
		image_holder_->setTool(ImageHolder::PolygonTool);
		image_holder_->setFocus();
	}
	else {
		image_holder_->setTool(ImageHolder::NoTool);
		image_holder_->clearLast();
	}
}

//! A slot member enables button_confirm_selection_ button
void
ImageLabeler::onSelectionStarted()
{
	button_confirm_selection_->setEnabled(true);
}

//! A slot member adding information about new selected area
/*!
 * \see addBBoxArea(int anID, BoundingBox aBBox, int itemID)
 * \see addPolyArea(int aPolyID, Polygon aPoly, int itemID)
 *
 * After user has created a new selection area it should be confirmed or else it
 * wouldn't be added to the item list and containers. This slot takes all data from
 * unconfirmed selection and places it into list widget and corresponding container
 * (list_bounding_box_ or list_polygon_)
 */
void
ImageLabeler::confirmSelection()
{
	if (!list_label_->count()) {
		showWarning(tr("You haven't added any label"));
		return;
		/* NOTREACHED */
	}

	image_holder_->confirmSelection();

	if (label_ID_ < 0)
		label_ID_ = 0;

	ImageHolder::Tool tool = image_holder_->tool();
	switch (tool) {
	case ImageHolder::BoundingBoxTool:
		list_bounding_box_.last()->label_ID_ = label_ID_;
		addBBoxArea(
			list_bounding_box_.count() - 1,
			*(list_bounding_box_.last())
			);
		break;
	case ImageHolder::PolygonTool:
		list_polygon_.last()->label_ID_ = label_ID_;
		addPolyArea(
			list_polygon_.count() - 1,
			*(list_polygon_.last())
			);
		break;
	default:
		break;
	}
	button_confirm_selection_->setEnabled(false);
	button_delete_area_->setEnabled(true);
	button_change_area_->setEnabled(true);
	button_change_area_text_->setEnabled(true);

	unsaved_data_ = 1;
}

//! \brief A slot member clears all widgets and containers
//! (not recommended to use without necessity)
/*!
 * \see clearAllTool()
 * \see clearLabelList()
 * \see clearLabelColorList()
 */
void
ImageLabeler::clearAll()
{
	clearLabelList();
	list_areas_->clear();
	list_bounding_box_.clear();
	list_polygon_.clear();
	list_images_->clear();
	list_images_widget_->clear();
	main_label_ = -1;
	image_holder_->clearAll();
	segmented_image_.clear();
	action_view_normal_->setEnabled(false);
	action_view_segmented_->setEnabled(false);
}

//! A slot member clears only selections made by user
/*!
 * \see clearAll()
 * \see clearLabelList()
 * \see clearLabelColorList()
 */
void
ImageLabeler::clearAllTool()
{
	list_areas_->clear();
	list_bounding_box_.clear();
	list_polygon_.clear();
	main_label_ = -1;
	image_holder_->clearAll();
}

//! A slot member clears label list
/*!
 * \see clearAll()
 * \see clearAllTool()
 * \see clearLabelColorList()
 *
 * As far as BACKGROUND label should always be present slot adds
 * it after clearing all the label list
 */
void ImageLabeler::clearLabelList()
{
	list_label_->clear();
	addLabel(0, false, "BACKGROUND");
	list_label_->item(0)->setFlags(
		Qt::ItemIsUserCheckable | Qt::ItemIsSelectable |
		Qt::ItemIsEnabled
		);
}

//! A slot member clears the list of label colors
/*!
 * \see clearAll()
 * \see clearAllTool()
 * \see clearLabelList()
 *
 * As far as BACKGROUND label should always be present slot adds
 * it's color(black by default) after clearing all the label color list
 */
void ImageLabeler::clearLabelColorList()
{
	list_label_colors_.clear();
	list_label_colors_.append(0x0);
}

//! A slot member enabling all the tools
/*!
 * \see disableTools()
 *
 * By default the initial state of all tools is disabled
 * so this slot is recommended to use when at least one image
 * has been loaded
 */
void
ImageLabeler::enableTools()
{
	action_save_all_->setEnabled(true);
	action_save_segmented_->setEnabled(true);
	action_save_legend_->setEnabled(true);
	action_bound_box_tool_->setEnabled(true);
	action_polygon_tool_->setEnabled(true);
	action_tagging_tool_->setEnabled(true);
	action_add_description_->setEnabled(true);
	action_undo_->setEnabled(true);
	action_redo_->setEnabled(true);
	button_bound_box_tool_->setEnabled(true);
	button_polygon_tool_->setEnabled(true);
	button_tagging_tool_->setEnabled(true);
	button_add_label_->setEnabled(true);
	button_remove_label_->setEnabled(true);
	button_prev_image_->setEnabled(true);
	button_next_image_->setEnabled(true);
	button_clear_selection_tool_->setEnabled(true);
	button_generate_colors_->setEnabled(true);
	button_delete_all_labels_->setEnabled(true);
}

//! A slot member disabling all the tools
/*!
 * \see enableTools()
 */
void
ImageLabeler::disableTools()
{
	action_save_all_->setEnabled(false);
	action_save_segmented_->setEnabled(false);
	action_save_legend_->setEnabled(false);
	action_bound_box_tool_->setEnabled(false);
	action_polygon_tool_->setEnabled(false);
	action_tagging_tool_->setEnabled(false);
	button_tagging_tool_->setEnabled(false);
	action_add_description_->setEnabled(false);
	action_undo_->setEnabled(false);
	action_redo_->setEnabled(false);
	button_bound_box_tool_->setEnabled(false);
	button_polygon_tool_->setEnabled(false);
	button_add_label_->setEnabled(false);
	button_remove_label_->setEnabled(false);
	button_prev_image_->setEnabled(false);
	button_next_image_->setEnabled(false);
	button_clear_selection_tool_->setEnabled(false);
	button_generate_colors_->setEnabled(false);
	button_delete_all_labels_->setEnabled(false);
}

//! \brief A slot member triggering popup_area_list_ menu with the coordinates
//! of the mouse pointer
/*!
 * \see deleteArea()
 * \see editArea()
 */
void
ImageLabeler::areaListPopupMenu(const QPoint &aPos)
{
	QPoint globalPos = list_areas_->mapToGlobal(aPos);
	QModelIndex index = list_areas_->indexAt(aPos);

	if (-1 == index.row()) {
		return;
		/* NOTREACHED */
	}

	list_areas_->item(index.row())->setSelected(true);

	popup_area_list_->exec(globalPos);
}

//! \brief A slot member triggering popup_label_list_ menu width the coordinates
//! of the mouse pointer
/*!
 * \see setLabelColor()
 * \see toggleLabelPriority()
 * \see removeLabel()
 */
void
ImageLabeler::labelListPopupMenu(const QPoint &aPos)
{
	QPoint globalPos = list_label_->mapToGlobal(aPos);
	QModelIndex index = list_label_->indexAt(aPos);

	if (-1 == index.row() || !index.row()) {
		return;
		/* NOTREACHED */
	}

	list_label_->item(index.row())->setSelected(true);

	popup_label_list_->exec(globalPos);
}

//! \brief A slot member triggering popup_images_list_ menu width the coordinates
//! of the mouse pointer
/*!
 * \see removeImage()
 */
void
ImageLabeler::imageListPopupMenu(const QPoint &aPos)
{
	QPoint globalPos = list_images_widget_->mapToGlobal(aPos);
	QModelIndex index = list_images_widget_->indexAt(aPos);

	if (-1 == index.row()) {
		return;
		/* NOTREACHED */
	}

	list_images_widget_->item(index.row())->setSelected(true);

	popup_images_list_->exec(globalPos);
}

//! \brief A slot member gets image description or tags from line_edit_form_
//! depends on line_edit_form_.purpose()
/*!
 * \see LineEditForm
 */
void
ImageLabeler::setDataFromForm(QString aData)
{
	if (ImageDescriptionPurpose == line_edit_form_.purpose()) {
		image_description_ = aData;
		setWindowTitle(image_description_);
	}
	else if (TaggingPurpose == line_edit_form_.purpose()) {
		tags_ = aData;
	}
}

//! A slot member creating 2-dimensional array for the segmented image
/*!
 * Array size is equal to a size of the current image loaded
 * and each pixel has the label id which corresponds to the objects in
 * list_bounding_box_ and list_polygon_.
 */
void
ImageLabeler::setPureData()
{
	/* initializing array */
	if (pure_data_) {
		delete[] *pure_data_;
		delete pure_data_;
	}

	/* getting image size */
	QSize imageSize = image_->size();
	pure_data_ = new int *[imageSize.height()];
	if (!pure_data_) {
		return;
		/* NOTREACHED */
	}
	/* allocating memory */
	for (int i = 0; i < imageSize.height(); i++) {
		pure_data_[i] = new int[imageSize.width()];
		if (!pure_data_[i]) {
			return;
			/* NOTREACHED */
		}
	}

	/* getting the number of all selections */
	int bboxCnt = list_bounding_box_.count();
	int polyCnt = list_polygon_.count();
	/* checking if each coordinate(pixel) belongs to any object */
	for (int i = 0; i < imageSize.height(); i++)
		for (int j = 0; j < imageSize.width(); j++) {
			pure_data_[i][j] = 0;
			/* bboxes first */
			for (int cnt = 0; cnt < bboxCnt; cnt++) {
				BoundingBox *bbox = list_bounding_box_.at(cnt);
				if (bbox->rect.contains(j, i)) {
					pure_data_[i][j] = bbox->label_ID_;
				}
			}
			/* polys next */
			for (int cnt = 0; cnt < polyCnt; cnt++) {
				Polygon *poly = list_polygon_.at(cnt);
				if (poly->poly.containsPoint(QPoint(j, i), Qt::OddEvenFill)) {
					pure_data_[i][j] = poly->label_ID_;
				}
			}
		}
}

//! \brief A slot member setting new color for
//! the current label from QColorDialog
/*!
 * \see setLabelColor(int anID, QColor aColor)
 */
void
ImageLabeler::setLabelColor()
{
	if (list_label_colors_.count() < list_label_->count()) {
		generateColors();
	}

	/* getting current item */
	QListWidgetItem *current = list_label_->currentItem();

	int labelID = list_label_->row(current);
	QColor defaultColor;
	defaultColor.setRgb(list_label_colors_.at(labelID));
	QColor newColor = QColorDialog::getColor(
		defaultColor,
		list_label_
		);

	if (!newColor.isValid()) {
		return;
		/* NOTREACHED */
	}
	list_label_colors_.takeAt(labelID);
	list_label_colors_.insert(labelID, newColor.rgb());

	QPixmap iconPix = QPixmap(20, 20);
	iconPix.fill(newColor);
	QIcon icon(iconPix);

	current->setIcon(icon);

	image_holder_->update();
}

//! A protected member setting color aColor for the label number anID
/*!
 * \param[in] anID an integer indicating label number in the list_label_ widget
 * \param[in] aColor a QColor object contains color you want to set for the label
 */
void
ImageLabeler::setLabelColor(int anID, QColor aColor)
{
	if (anID < 0 || list_label_->count() < anID) {
		return;
		/* NOTREACHED */
	}

	if (list_label_colors_.count() < list_label_->count()) {
		generateColors();
	}

	QListWidgetItem *item = list_label_->item(anID);
	list_label_colors_.takeAt(anID);
	list_label_colors_.insert(anID, aColor.rgb());

	QPixmap iconPix = QPixmap(20, 20);
	iconPix.fill(aColor);
	QIcon icon(iconPix);

	item->setIcon(icon);

	image_holder_->update();
}

//! A slot member loading normal image which path is in the current_image_
/*!
 * \see viewSegmented()
 */
void
ImageLabeler::viewNormal()
{
	if (current_image_.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	image_->load(current_image_);
	image_holder_->setPixmap(*image_);

	action_view_segmented_->setEnabled(true);
	action_view_normal_->setEnabled(false);
}

//! A slot member loading segmented image which path is in the segmented_image_
/*!
 * \see viewNormal()
 */
void
ImageLabeler::viewSegmented()
{
	if (segmented_image_.isEmpty()) {
		return;
		/* NOTREACHED */
	}

	image_->load(segmented_image_);
	image_holder_->setPixmap(*image_);

	action_view_segmented_->setEnabled(false);
	action_view_normal_->setEnabled(true);
}

//! \brief A slot member setting interrupt_search_ to true for interrupting
//! the recursive search
/*!
 * \see loadImages()
 * \see getImagesFromDir(const QDir &dir)
 */
void
ImageLabeler::interruptSearch()
{
	interrupt_search_ = 1;
}

//! \brief A slot member selecting image corresponding to the item
//! in the list_images_widget_
/*!
 * \see selectImage(int anImageID)
 * \param[in] anItem a pointer to the QListWidgetItem object which indicates
 * certain image in the image_list_
 */
void
ImageLabeler::selectImage(QListWidgetItem *anItem)
{
	if (!anItem || list_images_widget_->row(anItem) < 0 ||
		list_images_->isEmpty())
	{
		return;
		/* NOTREACHED */
	}
	clearAllTool();
	//clearLabelList();
	//clearLabelColorList();

	image_ID_ = list_images_widget_->row(anItem);

	selectImage(image_ID_);
}

//! A protected member loading image from list_images_
/*!
 * \see loadInfo(QString filename)
 * \param[in] anImageID an integer indicates certain
 * image in the image_list_
 *
 * if that image was labeled before, selectImage(int anImageID)
 * will try to load all info
 */
bool
ImageLabeler::selectImage(int anImageID)
{
	if (anImageID < 0 || list_images_->isEmpty() ||
		list_images_->count() <= anImageID)
	{
		return false;
		/* NOTREACHED */
	}

	action_view_segmented_->setEnabled(false);

	/* checking if it was labeled before */
	if (list_images_->at(anImageID).labeled_ &&
		!list_images_->at(anImageID).pas_)
	{
		list_label_colors_.clear();
		list_label_->clear();
		QString labeled =
			alterFileName(list_images_->at(anImageID).image_, "_labeled");
		labeled.append(".dat");
		loadInfo(labeled);
	}
	/* if it was loaded from PASCAL file then we're in trouble */
	else if (list_images_->at(anImageID).labeled_ &&
		list_images_->at(anImageID).pas_)
	{
		/* TODO: do the pascal file selecting */
		showWarning("this function doesn't work at the moment, sorry.");
	}
	/* loading clean unlabeled image */
	else {
		current_image_ = list_images_->at(anImageID).image_;
		image_->load(current_image_);
		image_holder_->setPixmap(*image_);
		image_holder_->resize(image_->size());
	}

	return true;
}

//! \brief A slot member removing selected image from list
//! (image will still be loaded in the image_holder_)
/*!
 * Image will be removed from list so it will not be possible to load
 * it again without using loadImage() or other slots for image loading.
 * if you want to remove image from image_holder_ just call
 * image_holder_->clear()
 */
void
ImageLabeler::removeImage()
{
	int imageCount = list_images_widget_->count();
	int num = list_images_widget_->currentRow();

	if (!imageCount || num < 0 || imageCount <= num ||
		list_images_->count() != imageCount)
	{
		return;
		/* NOTREACHED */
	}

	list_images_widget_->takeItem(num);
	list_images_->takeAt(num);

	for (int i = num ; i < list_images_widget_->count(); i++) {
		QString newStr = removePath(list_images_->at(i).image_);
		newStr.prepend(QString("%1: ").arg(i));
		list_images_widget_->item(i)->setText(newStr);
	}

	if (1 == imageCount)
		button_remove_image_->setEnabled(false);
}

//!
bool
ImageLabeler::loadPixmap(const QString &aPath)
{
	if (!image_->load(aPath)) {
		return false;
		/* NOTREACHED */
	}

	QSize holderSize = image_holder_->size();
	QSize imageSize = image_->size();
	double scale = 1;

	while (holderSize.width() * scale < imageSize.width() ||
		holderSize.height() * scale < imageSize.height())
	{
		scale *= 1.1;
	}

	image_holder_->resize(imageSize);
	image_holder_->setPixmap(*image_);
	image_holder_->scaleImage(ZoomOut, scale);

	return true;
}

//! A protected member which is being automatically called on every image resize
/*!
 *
 */
void
ImageLabeler::resizeEvent (QResizeEvent *anEvent)
{
	QWidget::resizeEvent(anEvent);
}

void
ImageLabeler::mouseMoveEvent(QMouseEvent *anEvent)
{
	if ((anEvent->buttons() & Qt::MiddleButton))// &&
		//image_holder_->hasFocus())// &&
//		(frame_image_->size().height() < image_holder_->size().height() ||
//		frame_image_->size().width() < image_holder_->size().width()))
	{
		qDebug() << "yahooo!";
	}
}

//! A protected member which is being automatically called on every mouse press
/*!
 *
 */
void
ImageLabeler::mousePressEvent(QMouseEvent *anEvent)
{
	QWidget::mousePressEvent(anEvent);
}

//! A protected member which is being automatically called on every key press
/*!
 * current list of available commands:
 * - key left/right - prevImage() / nextImage()
 * - enter/return - confirmSelection()
 * - esc - clear current selection
 * - ctrl+z - image_holder_->undo()
 * - ctrl+y - iamge_holder_->redo()
 */
void
ImageLabeler::keyPressEvent(QKeyEvent *anEvent)
{
	keyboard_modifier_ = anEvent->modifiers();

	if (Qt::Key_Left == anEvent->key() &&
		Qt::ControlModifier == keyboard_modifier_) {
		prevImage();
	}

	if (Qt::Key_Right == anEvent->key() &&
		Qt::ControlModifier == keyboard_modifier_) {
		nextImage();
	}

	if ((Qt::Key_Enter == anEvent->key() ||
		Qt::Key_Return == anEvent->key()) &&
	//	Qt::NoModifier == anEvent->modifiers() &&
		ImageHolder::NewSelection == image_holder_->state()) {
		confirmSelection();
	}

	if ((Qt::Key_Enter == anEvent->key() ||
		Qt::Key_Return == anEvent->key()) &&
	//	Qt::NoModifier == anEvent->modifiers() &&
		-1 != image_holder_->focusedSelection()) {
		image_holder_->clearFocusOnArea();
		image_holder_->clearHoveredPoint();
		image_holder_->update();
	}

	if (Qt::Key_Escape == anEvent->key()) {
		image_holder_->clearLast();
		image_holder_->clearFocusOnArea();
		image_holder_->clearHoveredPoint();
		image_holder_->update();
	}

	if (Qt::Key_Z == anEvent->key() &&
		Qt::ControlModifier == anEvent->modifiers()) {
		image_holder_->undo();
	}

	if (Qt::Key_Y == anEvent->key() &&
		Qt::ControlModifier == anEvent->modifiers()) {
		image_holder_->redo();
	}

	if (Qt::Key_Delete == anEvent->key() &&
		list_areas_->hasFocus()) {
		deleteArea();
	}

	if (Qt::Key_Delete == anEvent->key() &&
		list_label_->hasFocus()) {
		removeLabel();
	}

	if (Qt::Key_Delete == anEvent->key() &&
		list_images_widget_->hasFocus()) {
		removeImage();
	}

	if (Qt::Key_Delete == anEvent->key() &&
		image_holder_->hasFocus()) {
		image_holder_->removeSelectedPoint();
	}

	QWidget::keyPressEvent(anEvent);
}

//!
/*!
 *
 */
void
ImageLabeler::keyReleaseEvent(QKeyEvent *anEvent)
{
	keyboard_modifier_ = anEvent->modifiers();
}

//! A protected member which is being automatically called on every mouse wheel turn
/*!
 * implements zooming in and out on ctrl+wheel
 */
void
ImageLabeler::wheelEvent(QWheelEvent *anEvent)
{
	/* zoomin */
	if (0 < anEvent->delta() &&
		Qt::ControlModifier == keyboard_modifier_) {
		image_holder_->scaleImage(ZoomIn, 1.1);
	}
	/* zoomout */
	else if (anEvent->delta() < 0 &&
		Qt::ControlModifier == keyboard_modifier_) {
		image_holder_->scaleImage(ZoomOut, 1.1);
	}
}

//!
/*!
 *
 */
void
ImageLabeler::closeEvent(QCloseEvent *anEvent)
{
	Q_UNUSED(anEvent)
	writeSettings();

	askForUnsavedData();
}

/*
 *
 */
