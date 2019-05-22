#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QString filename = QLocale::system().name();
	QTranslator translator(&a);
	if(translator.load(filename,"://lang/")) a.installTranslator(&translator);

	MainWindow w;
	w.show();

	return a.exec();
}
