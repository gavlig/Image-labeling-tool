/*!
 * \file ImageHolder.h
 *
 *  Created on: Oct 5, 2011
 *      Author: Gapchich Vladislav
 */

#ifndef __IMAGEHOLDER_H__
#define __IMAGEHOLDER_H__

#include <QLabel>

//! enum indicating the figure of selection
enum Figure {
	NoFigure,
	RectFigure,
	PolyFigure
};

//! structure containing rectangle and it's label ID
struct BoundingBox {
	QRect rect;
	int label_ID_;
};

//! structure containing list of the polygon points and polygon's label ID
struct Polygon {
	QPolygon poly;
	int label_ID_;
};

//! structure indicates hovered point and the object which belongs to this hovered point
struct HoveredPoint {
	Figure figure; /*!< figure of the object which belongs to the hovered point */
	int figureID; /*!< ID of the object in list_bounding_box_ or list_polygon_ depend on figure */
	int pointID; /*!< number of the point in an object(rect or poly) */
};

//! enum indicating the direction of zooming
enum ZoomDirection {
	NoZoom,
	ZoomIn,
	ZoomOut
};

class QListWidgetItem;
class QPixmap;
class QScrollArea;

//! \brief Widget containing loaded image.
//! It makes drawing rectangles and polygons on the image possible.
/* \see paintEvent(QPaintEvent *)
 * \see ImageLabeler
 */
class ImageHolder : public QLabel
{
	Q_OBJECT
protected:
	void keyPressEvent(QKeyEvent *anEvent);
	void mouseMoveEvent(QMouseEvent *anEvent);
	void mousePressEvent(QMouseEvent *anEvent);
	void mouseDoubleClickEvent(QMouseEvent *anEvent);
	void mouseReleaseEvent(QMouseEvent *anEvent);
	void paintEvent (QPaintEvent *anEvent);

	void triggerBoundBox(
		const QPoint &aNewPos,
		const QPoint &anOldPos,
		QRect *aNewBox
		);
	void triggerPolygon(
		const QPoint &aPoint,
		QPolygon *aNewPoly
		);
	void drawBoundingBoxes(
		QPainter *aPainter,
		QPen *aPen
		) const;
	void drawPolygons(
		QPainter *aPainter,
		QPen *aPen
		) const;
	void checkForPoints(QPoint *aPos);
	int posInPolygon(
		QPoint *aPos,
		QPolygon *aPoly
		) const;

public:
	enum Tool {
		NoTool,
		BoundingBoxTool,
		PolygonTool
	};

	enum State {
		StandBy,
		NewSelection
	};

	ImageHolder(QWidget *aParent = 0);
	virtual ~ImageHolder();

	void setTool(Tool aTool);
	void setBoundingBoxList(QList< BoundingBox * > *aBBoxList);
	void setPolygonList(QList< Polygon * > *aPolyList);
	void setLabelColorList(QList< uint > *aLabelColorList);
	void setScrollArea(QScrollArea *aPointer);
	void setMainLabelNum(int *aNum);
	void setImage(QPixmap *anImage);
	void scaleImage(ZoomDirection aDirection, const double &scaleFactor);
	int focusedSelection() const;
	Figure focusedSelectionType() const;
	State state() const;
	Tool tool() const;


public slots:
	void clearAll();
	void clearLast();
	void confirmSelection();
	void focusOnArea(QListWidgetItem *anItem);
	void clearFocusOnArea();
	void clearHoveredPoint();
	void undo();
	void redo();
	void removeSelectedPoint();

signals:
	void selectionStarted();
	void areaEdited();

private:
	//! flag for the internal use
	bool repaint_needed_;

	//! \brief pointer to the list of ImageLabeler
	//! \see ImageLabeler::list_bounding_box_
	QList< BoundingBox * > *list_bounding_box_;

	//! \brief pointer to the list of ImageLabeler
	//! \see ImageLabeler::list_polygon_
	QList< Polygon * > *list_polygon_;

	//! \brief pointer to the list of ImageLabeler
	//! \see ImageLabeler::list_label_color_
	QList< uint > *list_label_color_;

	//! \brief pointer to scroll area containing this widget
	//! \see ImageLabeler::frame_image_
	QScrollArea *scroll_area_;

	//! \brief list of points for the undo/redo methods
	//! \see undo()
	//! \see redo()
	QList< QPoint > list_poly_history_;

	//! \brief pointer to the object of ImageLabeler
	//! \see ImageLabeler::image_
	QPixmap *image_;

	//! \brief pointer to the variable of ImageLabeler
	//! \see ImageLabeler::main_label_
	int *main_label_;

	//! \brief a structure that keeps not confirmed rectangle selection
	//! \see paintEvent(QPaintEvent *)
	//! \see ImageLabeler::confirmSelection()
	BoundingBox bounding_box_;

	//! \brief a structure that keeps not confirmed polygon selection
	//! \see paintEvent(QPaintEvent *)
	//! \see ImageLabeler::confirmSelection()
	Polygon polygon_;

	//! keeps current keyboard modifier(like ctrl, shift, alt etc)
	int keyboard_modifier_;

	//! \brief keeps an information about current hovered point
	//! \see checkForPoints(QPoint *aPos)
	HoveredPoint hovered_point_;

	//! keeps the selected point number
	int selected_point_;

	//! \brief keeps the position of a mouse when it was clicked
	QPoint prev_cursor_pos_;

	//! enum indicating tool user applying currently
	Tool tool_;

	//! enum indicating the current state of image_holder_
	State state_;

	//! indicates which selection(selected area) is focused at the moment
	int focused_selection_;

	//! indicates the type of currently selected figure
	Figure focused_selection_type_;

	//! \brief indicates current scale
	//! \see scaleImage(ZoomDirection aDirection,double scaleFactor)
	double scale_;

	//! \brief declares the radius of the selecltable point
	//! \see drawBoundingBoxes(QPainter *aPainter,QPen *aPen)
	//! \see drawPolygons(QPainter *aPainter,QPen *aPen)
	int point_radius_;
};

#endif /* IMAGEHOLDER_H_ */

/*
 *
 */
