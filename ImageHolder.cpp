/*
 * ImageHolder.cpp
 *
 *  Created on: Oct 5, 2011
 *      Author: gapchich
 */

#include "ImageHolder.h"
#include "functions.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QListWidgetItem>
#include <qmath.h>
#include <QScrollArea>
#include <QScrollBar>
#include <QDebug>

//! A constructor initializing some variables
ImageHolder::ImageHolder(QWidget *aParent)
	: QLabel(aParent)
{
	repaint_needed_ = 0;
	tool_ = NoTool;
	state_ = StandBy;
	keyboard_modifier_ = Qt::NoModifier;

	focused_selection_ = -1;
	focused_selection_type_ = NoFigure;

	hovered_point_.figure = NoFigure;
	hovered_point_.figureID = -1;
	hovered_point_.pointID = -1;

	selected_point_ = -1;

	list_bounding_box_ = 0;
	main_label_ = 0;
	//list_bounding_box_ = new QList< QRect >;

	scale_ = 1;

	point_radius_ = 6;

	setScaledContents(true);
	setMouseTracking(true);
}
//! An empty destructor
ImageHolder::~ImageHolder()
{

}

//! An event which being automatically called after any change of the widget
/*!
 * \see drawBoundingBoxes(QPainter *aPainter, QPen *aPen)
 * \see drawPolygons(QPainter *aPainter, QPen *aPen)
 *
 * It contains drawing of the confirmed and not confirmed selections either.
 */
void
ImageHolder::paintEvent(QPaintEvent *anEvent)
{
	QLabel::paintEvent(anEvent);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	//painter.setRenderHint(QPainter::SmoothPixmapTransform);
	QPen pen;

	if (NoTool != tool_) {
		pen.setWidth(1);
		pen.setColor(QColor(Qt::black));
		pen.setStyle(Qt::DashLine);
		painter.setPen(pen);

		if (BoundingBoxTool == tool_) {
			/* scaling */
			QRect bbox = bounding_box_.rect;
			QPoint bboxTopLeft = bbox.topLeft() * scale_;
			QPoint bboxBottomRight = bbox.bottomRight() * scale_;

			bbox.setTopLeft(bboxTopLeft);
			bbox.setBottomRight(bboxBottomRight);

			painter.drawRect(bbox);
		}
		else if (PolygonTool == tool_) {
			/* scaling */
			QPoint point;
			QPolygon poly = polygon_.poly;
			for (int i = 0; i < poly.size(); i++) {
				point.setX(poly.at(i).x());
				point.setY(poly.at(i).y());
				point *= scale_;
				poly.remove(i);
				poly.insert(i, point);
			}
			painter.drawPolygon(poly);
		}
	}

	/* drawing bounding boxes */
	drawBoundingBoxes(&painter, &pen);
	drawPolygons(&painter, &pen);
}

//! draws only confirmed bounding boxes
/*!
 * parameters of bboxes may vary depending on whether bbox is selected or not or
 * whether it's label is main or not.
 */
void
ImageHolder::drawBoundingBoxes(
	QPainter *aPainter,
	QPen *aPen
) const
{
	if (0 == list_bounding_box_)
	{
		return;
		/* NOTREACHED */
	}

	Qt::PenStyle penStyle;
	/* default width is hardcoded */
	int width = 2;
	/* drawing all the bboxes */
	for (int i = 0; i < list_bounding_box_->size(); i++) {
		penStyle = Qt::SolidLine;
		int labelID = list_bounding_box_->at(i)->label_ID_;

		/* setting color for the label of current bbox */
		if (labelID < list_label_color_->count())
			aPen->setColor(QColor(list_label_color_->at(labelID)));
		/* in case there is no color for such label */
		else
			aPen->setColor(QColor(Qt::white));

		/* checking whether labeled area is of main label or not */
		if (labelID == *main_label_)
			width = 3;
		else
			width = 2;

		/* changing the line style and width if current area is selected(focused) */
		if (RectFigure == focused_selection_type_ &&
			focused_selection_ == i) {
			penStyle = Qt::DotLine;
			width = 3;
		}

		/* scaling */
		QRect rect = list_bounding_box_->at(i)->rect.normalized();
		QPoint topLeft = rect.topLeft() * scale_;
		QPoint bottomRight = rect.bottomRight() * scale_;

		rect.setTopLeft(topLeft);
		rect.setBottomRight(bottomRight);

		if (focused_selection_ == i &&
			focused_selection_type_ == RectFigure) {
			QPen circPen;
			circPen.setWidth(2);
			circPen.setStyle(Qt::SolidLine);
			circPen.setColor(aPen->color());
			aPainter->setPen(circPen);
			for (int j = 0; j < 4; j++) {
				QPoint point;
				/* getting the number of point mouse pointer hovered on */
				if (!j) {
					point = rect.topLeft();
				}
				else if (1 == j)
				{
					point = rect.topRight();
				}
				else if (2 == j)
				{
					point = rect.bottomRight();
				}
				else if (3 == j)
				{
					point = rect.bottomLeft();
				}
				/* if current point is hovered then fill it */
				if (i == hovered_point_.figureID &&
					j == hovered_point_.pointID &&
					RectFigure == hovered_point_.figure) {
					QBrush brush;
					brush.setColor(aPen->color());
					brush.setStyle(Qt::SolidPattern);
					aPainter->setBrush(brush);
				}
				aPainter->drawEllipse(point, point_radius_, point_radius_);
				aPainter->setBrush(Qt::NoBrush);
			}
		}

		aPen->setWidth(width);
		aPen->setStyle(penStyle);
		aPainter->setPen(*aPen);

		aPainter->drawRect(rect);

		/* drawing label ids of these boxes */
		QString labelIDText =
			QString("%1").arg(labelID);

		aPainter->drawText(
			rect.left() + 5,
			rect.top() + 5,
			20,
			20,
			Qt::AlignLeft,
			labelIDText
			);
	}

}

//! draws only confirmed polygons
/*!
 * parameters of polygons may vary depending on whether poly is selected or not or
 * whether it's label is main or not.
 */
void
ImageHolder::drawPolygons(
	QPainter *aPainter,
	QPen *aPen
) const
{
	if (0 == list_polygon_)
	{
		return;
		/* NOTREACHED */
	}

	Qt::PenStyle penStyle = Qt::SolidLine;
	/* default width is hardcoded */
	int width = 2;
	/* drawing all the polygons */
	for (int i = 0; i < list_polygon_->size(); i++) {
		penStyle = Qt::SolidLine;
		int labelID = list_polygon_->at(i)->label_ID_;

		/* setting color for the label of current bbox */
		if (labelID < list_label_color_->count())
			aPen->setColor(QColor(list_label_color_->at(labelID)));
		/* in case there is no color for such label */
		else
			aPen->setColor(QColor(Qt::white));

		/* checking whether labeled area is of main object or not */
		if (labelID == *main_label_)
			width = 3;
		else
			width = 2;

		/* changing the line style and width if current area is selected(focused) */
		if (PolyFigure == focused_selection_type_ &&
			focused_selection_ == i) {
			penStyle = Qt::DotLine;
			width = 3;
		}

		QPoint point;
		QPolygon poly = list_polygon_->at(i)->poly;
		for (int j = 0; j < poly.size(); j++) {
			point.setX(poly.at(j).x());
			point.setY(poly.at(j).y());
			/* scaling */
			point *= scale_;
			poly.remove(j);
			poly.insert(j, point);

			/* in case if it's focused */
			if (focused_selection_ == i &&
				focused_selection_type_ == PolyFigure) {
				QPen circPen;
				circPen.setWidth(2);
				circPen.setStyle(Qt::SolidLine);
				circPen.setColor(aPen->color());
				aPainter->setPen(circPen);
				/* filling the point if it is hovered */
				if ((j == hovered_point_.pointID &&
					i == hovered_point_.figureID &&
					PolyFigure == hovered_point_.figure) ||
					j == selected_point_) {
					QBrush brush;
					brush.setColor(aPen->color());
					brush.setStyle(Qt::SolidPattern);
					aPainter->setBrush(brush);
				}
				aPainter->drawEllipse(point, point_radius_, point_radius_);
				aPainter->setBrush(Qt::NoBrush);
			}
		}

		aPen->setWidth(width);
		aPen->setStyle(penStyle);
		aPainter->setPen(*aPen);

		aPainter->drawPolygon(poly);
		/* drawing label IDs of these polygons */
		QString labelIDText =
			QString("%1").arg(labelID);
		QRect rect = poly.boundingRect();
		int x = rect.center().x();
		int y = rect.center().y();

		aPainter->drawText(
			x,
			y,
			20,
			20,
			Qt::AlignHCenter,
			labelIDText
			);
	}

}

//! Changes current state and setting new coordinates for the bbox
/*!
 * \see mouseMoveEvent(QMouseEvent *anEvent)
 * \param[in] aNewPos a second point for the bbox(rect)
 * \param[in] anOldPos a first point for the bbox
 * \param[in,out] aNewRect a pointer to the rectangle
 *  which is being currently changed
 */
void
ImageHolder::triggerBoundBox(
	const QPoint &aNewPos,
	const QPoint &anOldPos,
	QRect *aNewRect
	)
{
	aNewRect->setCoords(
			 anOldPos.x(),
			 anOldPos.y(),
			 aNewPos.x(),
			 aNewPos.y()
	);

	state_ = NewSelection;
	repaint_needed_ = 1;
}

//! \brief Changes current state and adding a new point to the
//! current not confirmed polygon
/*!
 * \see mouseMoveEvent(QMouseEvent *anEvent)
 * \param[in] aPoint a point going to be added to the polygon
 * \param[in,out] aNewPoly a pointer to the polygon
 *  which is being currently changed
 */
void
ImageHolder::triggerPolygon(
	const QPoint &aPoint,
	QPolygon *aNewPoly
	)
{
	*aNewPoly << aPoint;

	repaint_needed_ = 1;
}

//! \brief Puts focus on some of the selections(selected areas)
/*!
 * \param[in] anItem should be a pointer to the item of list_areas_ which
 * has a string with certain format
 * \see ImageLabeler::addPoly(Polygon *poly)
 * \see ImageLabeler::list_areas_
 * \see focused_selection_
 * \see focused_selection_type_
 * Parses the string from anItem and gets the selected object out of it
 */
void
ImageHolder::focusOnArea(QListWidgetItem *anItem)
{
	QString text = anItem->text();

	Tool tool = NoTool;
	if (-1 != text.indexOf("BBox"))
		tool = BoundingBoxTool;
	else if (-1 != text.indexOf("Poly"))
		tool = PolygonTool;

	/* looking for a number of selected area */
	if (NoTool != tool) {
		bool ok = 0;
		focused_selection_ = getNumFromString(&text, "#", ";", &ok);

		if (!ok) {
			focused_selection_ = -1;
			focused_selection_type_ = NoFigure;
			return;
			/* NOTREACHED */
		}

		switch (tool) {
		case BoundingBoxTool:
			focused_selection_type_ = RectFigure;
			break;
		case PolygonTool:
			focused_selection_type_ = PolyFigure;
			break;
		default:
			focused_selection_type_ = NoFigure;
			break;
		}

	}
	update();
}

//! Changes scale_ variable depending on zoom direction and scale factor
/*!
 * \param[in] aDirection an enum indicating the zoom direction
 * \param[in] scaleFactor a speed of zooming
 */
void
ImageHolder::scaleImage(
	ZoomDirection aDirection,
	const double &scaleFactor
)
{
	QSize size = this->size();

	/* zoomin */
	if (ZoomIn == aDirection) {
		size *= scaleFactor;
		scale_ *= scaleFactor;
	}
	/* zoomout */
	else if (ZoomOut == aDirection) {
		size /= scaleFactor;
		scale_ /= scaleFactor;
	}
	this->resize(size);
}

//! Removes the focus on an object
/*!
 * \see paintEvent(QPaintEvent *)
 */
void
ImageHolder::clearFocusOnArea()
{
	focused_selection_ = -1;
	focused_selection_type_ = NoFigure;
}

//! Clears the data of hovered_point_
void
ImageHolder::clearHoveredPoint()
{
	hovered_point_.figure = NoFigure;
	hovered_point_.figureID = -1;
	hovered_point_.pointID = -1;
}

//! Sets current tool
void
ImageHolder::setTool(Tool aTool)
{
	switch(aTool) {
	case BoundingBoxTool:
		tool_ = BoundingBoxTool;
		break;
	case PolygonTool:
		tool_ = PolygonTool;
		break;
	default:
		tool_ = NoTool;
		break;
	}
}

//! Sets a pointer on the ImageLabeler::list_bounding_box_
void
ImageHolder::setBoundingBoxList(QList< BoundingBox * > *aBBoxList)
{
	if (0 == aBBoxList) {
		return;
		/* NOTREACHED */
	}

	list_bounding_box_ = aBBoxList;
}

//! Sets a pointer on the ImageLabeler::list_polygon_
void
ImageHolder::setPolygonList(QList< Polygon * > *aPolygonList)
{
	if (0 == aPolygonList) {
		return;
		/* NOTREACHED */
	}

	list_polygon_ = aPolygonList;
}

//! Sets a pointer on the ImageLabeler::list_label_color
void
ImageHolder::setLabelColorList(QList< uint > *aLabelColorList)
{
	if (0 == aLabelColorList) {
		return;
		/* NOTREACHED */
	}

	list_label_color_ = aLabelColorList;
}

//! Sets a pointer on the ImageLabeler::main_label_
void
ImageHolder::setMainLabelNum(int *aNum)
{
	if (0 == aNum) {
		return;
		/* NOTREACHED */
	}

	main_label_ = aNum;
}

//! Sets a pointer on the ImageLabeler::image_
void
ImageHolder::setImage(QPixmap *anImage)
{
	if (0 == anImage) {
		return;
		/* NOTREACHED */
	}

	image_ = anImage;
}

//! Sets a pointer to scroll area containing this widget
void
ImageHolder::setScrollArea(QScrollArea *aPointer)
{
	if (0 == aPointer) {
		return;
		/* NOTREACHED */
	}

	scroll_area_ = aPointer;
}

//! Clears scale, state, bounding_box_ and polygon_
void
ImageHolder::clearAll()
{
	//list_bounding_box_->clear();
	bounding_box_.rect.setRect(-1, -1, 0, 0);
	//list_polygon_->clear();
	polygon_.poly.clear();
	clearFocusOnArea();
	state_ = StandBy;
	scale_ = 1;

	update();
}

//! Clears las selection(selected area)
void
ImageHolder::clearLast()
{
	switch (tool_) {
	case BoundingBoxTool:
		bounding_box_.rect.setRect(-1, -1, 0, 0);
		break;
	case PolygonTool:
		polygon_.poly.clear();
		break;
	default:
		break;
	}
	bounding_box_.rect.setRect(-1, -1, 0, 0);

	state_ = StandBy;
	update();
}

//! Moves bounding_box_ or polygon_ to the corresponding list
/*!
 * \see ImageLabeler::confirmSelection()
 */
void
ImageHolder::confirmSelection()
{
	if (NewSelection != state_ || NoTool == tool_) {
		return;
		/* NOTREACHED */
	}

	if (BoundingBoxTool == tool_) {
		BoundingBox *bbox = new BoundingBox;
		bounding_box_.rect = bounding_box_.rect.normalized();
		*bbox = bounding_box_;
		list_bounding_box_->append(bbox);
		bounding_box_.rect.setRect(-1, -1, 0, 0);
	}
	else if (PolygonTool == tool_) {
		Polygon *poly = new Polygon;
		*poly = polygon_;
		list_polygon_->append(poly);
		polygon_.poly.clear();
	}

	list_poly_history_.clear();

	state_ = StandBy;
	update();
}

//! returns state
ImageHolder::State
ImageHolder::state() const
{
	return state_;
}

//! returns tool
ImageHolder::Tool
ImageHolder::tool() const
{
	return tool_;
}

//! returns focused_selection_
int
ImageHolder::focusedSelection() const
{
	return focused_selection_;
}

//! returns focused_selection_type_
Figure
ImageHolder::focusedSelectionType() const
{
	return focused_selection_type_;
}

//! removes last added point form the polygon_
void
ImageHolder::undo()
{
	if (PolygonTool == tool_ &&
		NewSelection == state_ &&
		!polygon_.poly.isEmpty())
	{
		list_poly_history_.append(polygon_.poly.last());
		polygon_.poly.pop_back();
		repaint_needed_ = 1;
	}

	if (repaint_needed_) {
		update();
		repaint_needed_ = 0;
	}
}

//! Brings back the last removed by undo() point
void
ImageHolder::redo()
{
	if (PolygonTool == tool_ &&
		NewSelection == state_ &&
		!list_poly_history_.isEmpty())
	{
		polygon_.poly.append(list_poly_history_.last());
		list_poly_history_.pop_back();
		repaint_needed_ = 1;
	}

	if (repaint_needed_) {
		update();
		repaint_needed_ = 0;
	}
}

//! Checks whether mouse pointer is on some point of any object or not
/*!
 * \see hovered_point_
 * \see drawBoundingBoxes(QPainter *aPainter, QPen *aPen)
 * \see drawPolygons(QPainter *aPainter, QPen *aPen)
 *
 * It simply checks all the points of all objects if mouse
 * pointer is hovered above any of them
 */
void
ImageHolder::checkForPoints(QPoint *aPos)
{
	if ((!list_polygon_->count() &&
		!list_bounding_box_->count()) ||
		!aPos) {
		return;
		/* NOTREACHED */
	}

	int newRadius = 0;
	int x = aPos->x();
	int y = aPos->y();
	/* center coordinates */
	int xc = 0;
	int yc = 0;
	for (int i = 0; i < list_polygon_->count(); i++) {
		QPolygon poly = list_polygon_->at(i)->poly;
		for (int j = 0; j < poly.count(); j++) {
			xc = poly.at(j).x();
			yc = poly.at(j).y();
			newRadius = qSqrt(qPow(x - xc, 2) + qPow(y - yc, 2));
			if (newRadius <= point_radius_) {
				hovered_point_.figure = PolyFigure;
				hovered_point_.figureID = i;
				hovered_point_.pointID = j;
				repaint_needed_ = 1;
				return;
				/* NOTREACHED */
			}
		}
	}

	for (int i = 0; i < list_bounding_box_->count(); i++) {
		QRect rect = list_bounding_box_->at(i)->rect;

		for (int j = 0; j < 4; j++) {
			if (!j) {
				xc = rect.left();
				yc = rect.top();
			}
			else if (1 == j)
			{
				xc = rect.right();
				yc = rect.top();
			}
			else if (2 == j)
			{
				xc = rect.right();
				yc = rect.bottom();
			}
			else if (3 == j)
			{
				xc = rect.left();
				yc = rect.bottom();
			}


			newRadius = qSqrt(qPow(x - xc, 2) + qPow(y - yc, 2));
			if (newRadius <= point_radius_) {
				hovered_point_.figure = RectFigure;
				hovered_point_.figureID = i;
				hovered_point_.pointID = j;
				repaint_needed_ = 1;
				return;
				/* NOTREACHED */
			}
		}
	}

	hovered_point_.figure = NoFigure;
	hovered_point_.figureID = -1;
	hovered_point_.pointID = -1;
	repaint_needed_ = 1;
}

//! Returns position index of the point in polygon
/*!
 * \param[in] aPos point to insert into the polygon
 * \param[in] aPoly polygon
 */
int
ImageHolder::posInPolygon(QPoint *aPos, QPolygon *aPoly) const
{
	if (!aPos || !aPoly || aPoly->count() < 2 || aPos->isNull()) {
		return -1;
		/* NOTREACHED */
	}

	int x = aPos->x();
	int y = aPos->y();

	int index = 0;
	int dist = 100000;
	int temp = 0;
	int count = aPoly->count();
	QRect rect;

	for (int i = 0; i < count - 1; i++) {
		temp = pointToLineDistance(
			QLine(aPoly->at(i), aPoly->at(i + 1)),
			*aPos
			);
		rect.setTopLeft(aPoly->at(i));
		rect.setBottomRight(aPoly->at(i + 1));
		rect = rect.normalized();
		if (temp < dist &&
			((x < rect.right() && rect.left() < x) ||
			(y < rect.bottom() && rect.top() < y)))
		{
			dist = temp;
			index = i + 1;
		}
	}

	/* first and last points */
	temp = pointToLineDistance(
		QLine(aPoly->at(0), aPoly->at(count - 1)),
		*aPos
		);

	rect.setTopLeft(aPoly->at(0));
	rect.setBottomRight(aPoly->at(count - 1));
	rect = rect.normalized();
	if (temp < dist &&
		((x < rect.right() && rect.left() < x) ||
		(y < rect.bottom() && rect.top() < y)))
	{
		index = 0;
	}

	return index;
}

//! Removes selected point from focused polygon
void
ImageHolder::removeSelectedPoint()
{
	if (-1 == selected_point_ ||
		-1 == focused_selection_ ||
		focused_selection_type_ != PolyFigure)
	{
		return;
		/* NOTREACHED */
	}

	list_polygon_->at(focused_selection_)->poly.remove(selected_point_);
	selected_point_ = -1;
	update();
}

void
ImageHolder::keyPressEvent(QKeyEvent *anEvent)
{
	QLabel::keyPressEvent(anEvent);
}

//! Event is automatically called on every mouse move
/*!
 * With the help of this event all the mouse movements are being tracked.
 */
void
ImageHolder::mouseMoveEvent(QMouseEvent *anEvent)
{
	QPoint pos = anEvent->pos() / scale_;
	if (anEvent->pos().x() < 0)
		pos.setX(0);

	if (width() < anEvent->pos().x())
		pos.setX(width() - 1);

	if (anEvent->pos().y() < 0)
		pos.setY(0);


	if (height() < anEvent->pos().y())
		pos.setY(height() - 1);

	/* modifying rectangle  */
	if ((anEvent->buttons() & Qt::LeftButton) &&
		BoundingBoxTool == tool_ &&
		NewSelection == state_ &&
		Qt::NoModifier == keyboard_modifier_)
	{
		triggerBoundBox(pos, prev_cursor_pos_, &(bounding_box_.rect));
	}

	/* moving last point of the poly during creating a new one */
	if (PolygonTool == tool_ &&
		NewSelection == state_ &&
		(anEvent->buttons() & Qt::LeftButton))
	{
		polygon_.poly.setPoint(polygon_.poly.count() - 1, pos);
		repaint_needed_ = 1;
	}

	if (-1 != focused_selection_ &&
		!(anEvent->buttons() & Qt::LeftButton)) {
		checkForPoints(&pos);
	}

	/* editing polygons */
	if (-1 != hovered_point_.figureID &&
		!list_polygon_->isEmpty() &&
		PolyFigure == hovered_point_.figure &&
		(anEvent->buttons() & Qt::LeftButton) &&
		hovered_point_.figureID == focused_selection_)
	{
		Polygon *poly = list_polygon_->at(hovered_point_.figureID);
		poly->poly.setPoint(hovered_point_.pointID, pos);
		repaint_needed_ = 1;
	}

	/* editing bounding boxes */
	if (-1 != hovered_point_.figureID &&
		!list_bounding_box_->isEmpty() &&
		RectFigure == hovered_point_.figure &&
		(anEvent->buttons() & Qt::LeftButton))
	{
		BoundingBox *rect = list_bounding_box_->at(hovered_point_.figureID);
		if (0 == hovered_point_.pointID)
			rect->rect.setTopLeft(pos);
		else if (1 == hovered_point_.pointID)
			rect->rect.setTopRight(pos);
		else if (2 == hovered_point_.pointID)
				rect->rect.setBottomRight(pos);
		else if (3 == hovered_point_.pointID)
				rect->rect.setBottomLeft(pos);

		repaint_needed_ = 1;
	}

	/* moving image when it's too big */
	if ((anEvent->buttons() & Qt::MiddleButton) &&
		(scroll_area_->size().height() < size().height() ||
		scroll_area_->size().width() < size().width()))
	{
		QPoint globPos = mapToGlobal(pos);
		QPoint prev = mapToGlobal(prev_cursor_pos_);
		int horValue = scroll_area_->horizontalScrollBar()->value();
		int verValue = scroll_area_->verticalScrollBar()->value();

		QPoint delta = globPos - prev;
		prev_cursor_pos_ = pos;

		horValue += delta.x();
		verValue += delta.y();

		scroll_area_->horizontalScrollBar()->setValue(horValue);
		scroll_area_->verticalScrollBar()->setValue(verValue);
	}

	if (repaint_needed_) {
		update();
		repaint_needed_ = 0;
	}
}

//! Event is automatically called on every mouse click
/*!
 * Every time mouse was clicked it's position is stored in the prev_cursor_pos_
 * Depending on current state and selected tool it creates new points or
 * starts new rects
 */
void
ImageHolder::mousePressEvent(QMouseEvent *anEvent)
{
	/* remembering coordinates of the click */
	prev_cursor_pos_ = anEvent->pos() / scale_;

	QPoint pos = anEvent->pos() / scale_;

	if (anEvent->buttons() & Qt::LeftButton) {
		/* clearing the selected area if it is not confirmed */
		if (NewSelection == state_ && BoundingBoxTool == tool_) {
			bounding_box_.rect.setRect(-1, -1, 0, 0);
			state_ = StandBy;
		}

		/* making new points for poly */
		if (PolygonTool == tool_ &&
			NewSelection == state_ &&
			Qt::NoModifier == keyboard_modifier_)
		{
			triggerPolygon(pos, &(polygon_.poly));
		}

		/* starting new selection by click */
		if (StandBy == state_ && NoTool != tool_ &&
			-1 == focused_selection_) {
			state_ = NewSelection;
			emit selectionStarted();

			polygon_.poly.clear();
			if (PolygonTool == tool_) {
				polygon_.poly << prev_cursor_pos_;
			}
		}

		/* selecting a point */
		selected_point_ = -1;
		repaint_needed_ = 1;
		if (-1 != hovered_point_.figureID &&
			!list_polygon_->isEmpty() &&
			PolyFigure == hovered_point_.figure &&
			hovered_point_.figureID == focused_selection_)
		{
			selected_point_ = hovered_point_.pointID;
		}
	}

	if (repaint_needed_) {
		update();
		repaint_needed_ = 0;
	}
}

void
ImageHolder::mouseDoubleClickEvent(QMouseEvent *anEvent)
{
	QPoint pos = anEvent->pos() / scale_;

	/* new point for polygon */
	if (-1 == hovered_point_.figureID &&
		!list_polygon_->isEmpty() &&
		-1 != focused_selection_ &&
		PolyFigure == focused_selection_type_ &&
		(anEvent->buttons() & Qt::LeftButton))
	{
		Polygon *poly = list_polygon_->at(focused_selection_);
		int index = posInPolygon(&pos, &(poly->poly));

		if (index < 0 || poly->poly.count() <= index) {
			return;
			/* NOTREACHED */
		}

		poly->poly.insert(index, pos);
		repaint_needed_ = 1;
	}

	if (repaint_needed_) {
		update();
		repaint_needed_ = 0;
	}
}

//! Event is automatically called on every mouse release
void
ImageHolder::mouseReleaseEvent(QMouseEvent *anEvent)
{
	Q_UNUSED(anEvent)

	if (-1 != hovered_point_.figureID)
		emit areaEdited();

	if (RectFigure == hovered_point_.figure &&
		-1 != hovered_point_.figureID &&
		!list_bounding_box_->
			at(hovered_point_.figureID)->
			rect.isValid()
		)
	{
		BoundingBox *rect = list_bounding_box_->at(hovered_point_.figureID);
		rect->rect = rect->rect.normalized();
	}
}

/*
 *
 */
