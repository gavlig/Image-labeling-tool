/*
 * functions.cpp
 *
 *  Created on: Oct 6, 2011
 *      Author: Gapchich Vladislav
 */

#include "functions.h"

#include <QString>
#include <QChar>
#include <QDomDocument>
#include <QDomNode>
#include <QDomText>
#include <QPoint>
#include <QLine>
#include <qmath.h>
#include <QDebug>

//! Gets number from a string which is located between aFirstStr and aSecondStr
/*!
 * \param[in] aString pointer to the source string containing the number we need
 * \param[in] aFirstStr a string which is located to the left side of the number
 * \param[in] aSecondStr a string which is located to the right side of the number
 * \param[in,out] anOkFlag a pointer to the bool flag indicating the conversation errors
 *
 * example: aString contains "0: Poly #0; LabelID: 1; points:..."
 * aFirstStr "LabelID: "
 * aSecondStr ";"
 * function returns 1
 */
int
getNumFromString(
	QString *aString,
	const QString &aFirstStr,
	const QString &aSecondStr,
	bool *anOkFlag
)
{
	int numPos = aString->indexOf(aFirstStr) + aFirstStr.size();

	if (numPos < 0) {
		*anOkFlag = 0;
		return -1;
		/* NOTREACHED */
	}

	int numLength = -1;
	for (int i = numPos; i < aString->size(); i++) {
		if (aSecondStr == aString->at(i)) {
			numLength = i - numPos;
			break;
		}
	}

	if (numLength <= 0) {
		*anOkFlag = 0;
		return -1;
		/* NOTREACHED */
	}

	QString numString = aString->mid(numPos, numLength);
	if (numString.isEmpty()) {
		*anOkFlag = 0;
		return -1;
		/* NOTREACHED */
	}

	bool ok = 0;
	int num = numString.toInt(&ok, 10);

	if (!ok) {
		*anOkFlag = 0;
		return -1;
		/* NOTREACHED */
	}

	*anOkFlag = 1;
	return num;
}


//! Adds given suffix to the file name
/*
 * example: /home/user/file.dot -> /home/user/file_altered.dot
 */
QString
alterFileName(const QString &aFilename, const QString &aSuffix)
{
	/* altering the name of a new file */
	QString newFileName = aFilename;
	int dotPos = newFileName.lastIndexOf('.');

	if (-1 == dotPos)
		dotPos = newFileName.size();
	else
		newFileName.remove(dotPos, newFileName.size() - dotPos);

	newFileName.insert(dotPos, aSuffix);

	return newFileName;
}

//! Removes the path from filename
/*!
 * example: /home/user/file -> file
 */
QString
removePath(const QString &aFilename)
{
	QString newFileName = aFilename;

	int slashPos = newFileName.lastIndexOf('/');
	newFileName.remove(0, slashPos + 1);

	return newFileName;
}

//! Gets path from filename
/*!
 *  example: /home/user/file.dot -> /home/user
 */
QString
getPathFromFilename(const QString &aFilename)
{
	QString path = aFilename;

	int slashPos = path.lastIndexOf('/');
	path = path.mid(0, slashPos + 1);

	return path;
}

//! Calculates coefficients a,b,c (ax + by + c = 0) for the straight line
/*!
 * \see ImageHolder::posInPolygon(QPoint *aPos,QPolygon *aPoly)
 * \param[in] p1 first point defining the line
 * \param[in] p2 second point defining the line
 * \param[out] a pointer to the coefficient
 * \param[out] b pointer to the coefficient
 * \param[out] c pointer to the coefficient
 */
void
calcLineCoeff(
	const QPoint &p1,
	const QPoint &p2,
	int *a,
	int *b,
	int *c
)
{
	*a = p1.y() - p2.y();
	*b = p2.x() - p1.x();
	*c = (p1.x() * p2.y()) - (p2.x() * p1.y());
}

//! Returns the distance between point and a line
/*!
 * \see calcLineCoeff(
	const QPoint &p1,
	const QPoint &p2,
	int *a,
	int *b,
	int *c
	)
	\see ImageHolder::posInPolygon(QPoint *aPos,QPolygon *aPoly)
 * \param[in] aLine line
 * \param[in] aPoint point
 */
int
pointToLineDistance(
	const QLine &aLine,
	const QPoint &aPoint
)
{
	int a, b, c;
	int distance = 0;

	calcLineCoeff(
		aLine.p1(),
		aLine.p2(),
		&a,
		&b,
		&c
		);

	distance = qAbs((a * aPoint.x()) + (b * aPoint.y()) + c);

	return distance;
}

/*
 *
 */
