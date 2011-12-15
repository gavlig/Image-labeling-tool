/*
 * functions.h
 *
 *  Created on: Oct 6, 2011
 *      Author: Gapchich Vladislav
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

class QString;
class QChar;
class QDomDocument;
class QPoint;
class QLine;

QString getDirFromPath(
	const QString *aPath
	);
QString getFilenameFromPath(
	const QString *aPath
	);
int getNumFromString(
	QString *aString,
	const QString &aFirstStr,
	const QString &aSecondStr,
	bool *anOkFlag
	);
QString alterFileName(
	const QString &aFilename,
	const QString &aSuffix
	);
QString removePath(
	const QString &aFilename
	);
QString getPathFromFilename(
	const QString &aFilename
	);
void calcLineCoeff(
	const QPoint &p1,
	const QPoint &p2,
	int *a,
	int *b,
	int *c
	);
int pointToLineDistance(
	const QLine &aLine,
	const QPoint &aPoint
	);

#endif /* __FUNCTIONS_H__ */

/*
 *
 */
