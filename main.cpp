#include <QApplication>
#include <QTextCodec>
#include <QString>
#include <QDebug>

#include "ImageLabeler.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    app.setApplicationName(QObject::tr("Image Labeler"));

    int i = 1;
    QString settingsPath;
    while (i < argc) {
    	if ((QString(argv[i]) == "-config" || QString(argv[i]) == "-c") &&
    		i + 1 < argc)
    	{
    		settingsPath = QString(argv[i + 1]);
    	}
    	i++;
    }

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    ImageLabeler imageLabeler(0, settingsPath);
    imageLabeler.show();

    return app.exec();
}

/*
 *
 */
