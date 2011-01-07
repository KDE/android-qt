#! /bin/sh

### Edit these 2 values ###

SDK_ROOT=/home/buus/dev/android-sdk-linux_x86
LIGHTHOUSE_ROOT=/home/buus/dev/android-lighthouse

###########################

PROJECT_NAME=
ANDROID_TARGET=4

usage()
{
cat << EOF
usage: $0 options

OPTIONS:
   -h      Show this message
   -n      Project name
   -t	   Android target 	1=android1.5 
				2=android1.6 
				3=android2.1 
			      * 4=android2.2
EOF
}

while getopts ":h:n:t" options; do
case $options in
	h)
		usage
		exit 1
		;;
	n)
		PROJECT_NAME=$OPTARG
		;;
	t)
		ANDROID_TARGET=$OPTARG
		;;
	?)
		usage
		exit
		;;
esac
done

if [ -z "$PROJECT_NAME" ] | [ -z "$PROJECT_NAME" ]
then
	usage 
	exit 1
fi

J_PRO=$PROJECT_NAME-java
Q_PRO=$PROJECT_NAME-cpp

#JAVA SIDE
$SDK_ROOT/tools/android create project -n $PROJECT_NAME -t $ANDROID_TARGET -p $J_PRO -a QtMain -k org.$PROJECT_NAME
if [ $? -ne 0 ] ; then echo "android create project failed"; exit \$?; fi

rm $J_PRO/src/org/$PROJECT_NAME/QtMain.java
mkdir -p $J_PRO/src/org/$PROJECT_NAME/qt
cp $LIGHTHOUSE_ROOT/examples/android/QtAnimatedtiles/src/org/animatedtiles/qt/QtMain.java $J_PRO/src/org/$PROJECT_NAME/qt/QtMain.java
cp $LIGHTHOUSE_ROOT/examples/android/QtAnimatedtiles/AndroidManifest.xml $J_PRO/AndroidManifest.xml
cp -r $LIGHTHOUSE_ROOT/src/android/java/src/com $PROJECT_NAME-java/src
sed -i "s/animatedtiles/$PROJECT_NAME/g" $J_PRO/src/org/$PROJECT_NAME/qt/QtMain.java
sed -i "s/animatedtiles/$PROJECT_NAME/g" $J_PRO/AndroidManifest.xml

#Qt SIDE
mkdir $Q_PRO
cat > $Q_PRO/main.cpp <<EOF
#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
EOF

cat > $Q_PRO/mainwindow.cpp <<EOF
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
EOF

cat > $Q_PRO/mainwindow.h <<EOF
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
EOF

cat > $Q_PRO/$PROJECT_NAME.pro <<EOF
QT       += core gui

TARGET = $PROJECT_NAME
TEMPLATE = app

android-g++ {
    TEMPLATE = lib
    CONFIG += dll
}

SOURCES += main.cpp \\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
EOF

cat > $Q_PRO/mainwindow.ui <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>300</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>MainWindow</string>
  </property>
  <widget class="QWidget" name="centralWidget"/>
  <widget class="QMenuBar" name="menuBar">
   <property name="geometry">
    <rect>
     <x>0</x>
     <y>0</y>
     <width>400</width>
     <height>22</height>
    </rect>
   </property>
  </widget>
  <widget class="QToolBar" name="mainToolBar">
   <attribute name="toolBarArea">
    <enum>TopToolBarArea</enum>
   </attribute>
   <attribute name="toolBarBreak">
    <bool>false</bool>
   </attribute>
  </widget>
  <widget class="QStatusBar" name="statusBar"/>
 </widget>
 <layoutdefault spacing="6" margin="11"/>
 <resources/>
 <connections/>
</ui>
EOF

#Build file
cat > build.sh <<EOF
#!/bin/sh

SDK_ROOT=$SDK_ROOT
ANDROID_LIGHTHOUSE=/data/local/qt
PROJECT_NAME=$PROJECT_NAME
PROJECT_ROOT=\$PWD

Q_PRO=\$PROJECT_NAME-cpp
J_PRO=\$PROJECT_NAME-java

# Build Qt project
cd \$Q_PRO
#QMAKE
\$ANDROID_LIGHTHOUSE/bin/qmake
if [ \$? -ne 0 ] ; then echo "QMake failed"; exit \$?; fi
#MAKE
make
if [ \$? -ne 0 ] ; then echo "Make failed"; exit \$?; fi

cd ..
mkdir -p \$J_PRO/libs/armeabi
cp \$Q_PRO/lib\$PROJECT_NAME.so* \$J_PRO/libs/armeabi

# Build apk
cd \$J_PRO
ant debug
if [ \$? -ne 0 ] ; then echo "Ant failed"; exit \$?; fi

\$SDK_ROOT/tools/adb install -r bin/\$PROJECT_NAME-debug.apk

exit 0
EOF

chmod +x build.sh

#Run file
cat > run.sh <<EOF
#!/bin/sh

SDK_ROOT=$SDK_ROOT
PROJECT_NAME=$PROJECT_NAME

\$SDK_ROOT/tools/adb shell am start -n org.\$PROJECT_NAME.qt/.QtMain

exit 0
EOF

chmod +x run.sh

# Qt creato helper build file
cat > creatorbuild.sh <<EOF
#!/bin/sh

CREATOR_BUILDDIR=$PROJECT_NAME-cpp
SDK_ROOT=$SDK_ROOT
ANDROID_LIGHTHOUSE=/data/local/qt
PROJECT_NAME=$PROJECT_NAME
PROJECT_ROOT=\$PWD

Q_PRO=\$PROJECT_NAME-cpp
J_PRO=\$PROJECT_NAME-java

mkdir -p \$J_PRO/libs/armeabi
cp \$CREATOR_BUILDDIR/lib\$PROJECT_NAME.so \$J_PRO/libs/armeabi

# Build apk
cd \$J_PRO
ant debug
if [ \$? -ne 0 ] ; then echo "Ant failed"; exit \$?; fi

\$SDK_ROOT/tools/adb install -r bin/\$PROJECT_NAME-debug.apk

exit 0
EOF
chmod +x creatorbuild.sh

exit 0

