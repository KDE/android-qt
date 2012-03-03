#ifndef QPLATFORMMENU_QPA_H
#define QPLATFORMMENU_QPA_H

#include <QList>

class QAction;
class QMenu;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QPlatformMenu
{
public:
    virtual ~QPlatformMenu() { }
    virtual void showMenuBar(const QList<QAction*> & menuBarActionList) = 0;
    virtual void hideMenuBar() = 0;

    virtual void showMenu(const QList<QAction*> & menuBarActionList) = 0;
    virtual void hideMenu() = 0;
};

QT_END_NAMESPACE

QT_END_HEADER


#endif // QPLATFORMMENU_QPA_H
