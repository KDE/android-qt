#include <QApplication>
#include <qmlviewer.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QmlViewer view;
    view.show();
    return a.exec();
}
