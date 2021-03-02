#include "QtOpencvSet.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtOpencvSet w;
	w.show();
	return a.exec();
}
