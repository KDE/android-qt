/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [0]
QSettings settings("MySoft", "Star Runner");
QColor color = settings.value("DataPump/bgcolor").value<QColor>();
//! [0]


//! [1]
QSettings settings("MySoft", "Star Runner");
QColor color = palette().background().color();
settings.setValue("DataPump/bgcolor", color);
//! [1]


//! [2]
QSettings settings("/home/petra/misc/myapp.ini",
                   QSettings::IniFormat);
//! [2]


//! [3]
QSettings settings("/Users/petra/misc/myapp.plist",
                   QSettings::NativeFormat);
//! [3]


//! [4]
QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Office",
                   QSettings::NativeFormat);
//! [4]


//! [5]
settings.setValue("11.0/Outlook/Security/DontTrustInstalledFiles", 0);
//! [5]


//! [6]
settings.setValue("HKEY_CURRENT_USER\\MySoft\\Star Runner\\Galaxy", "Milkyway");
settings.setValue("HKEY_CURRENT_USER\\MySoft\\Star Runner\\Galaxy\\Sun", "OurStar");
settings.value("HKEY_CURRENT_USER\\MySoft\\Star Runner\\Galaxy\\Default"); // returns "Milkyway"
//! [6]


//! [7]
#ifdef Q_WS_MAC
    QSettings settings("grenoullelogique.fr", "Squash");
#else
    QSettings settings("Grenoulle Logique", "Squash");
#endif
//! [7]


//! [8]
pos = @Point(100 100)
//! [8]


//! [9]
windir = C:\Windows
//! [9]


//! [10]
QSettings settings("Moose Tech", "Facturo-Pro");
//! [10]


//! [11]
QSettings settings("Moose Soft", "Facturo-Pro");
//! [11]


//! [12]
QCoreApplication::setOrganizationName("Moose Soft");
QCoreApplication::setApplicationName("Facturo-Pro");
QSettings settings;
//! [12]


//! [13]
settings.beginGroup("mainwindow");
settings.setValue("size", win->size());
settings.setValue("fullScreen", win->isFullScreen());
settings.endGroup();

settings.beginGroup("outputpanel");
settings.setValue("visible", panel->isVisible());
settings.endGroup();
//! [13]


//! [14]
settings.beginGroup("alpha");
// settings.group() == "alpha"

settings.beginGroup("beta");
// settings.group() == "alpha/beta"

settings.endGroup();
// settings.group() == "alpha"

settings.endGroup();
// settings.group() == ""
//! [14]


//! [15]
struct Login {
    QString userName;
    QString password;
};
QList<Login> logins;
...

QSettings settings;
int size = settings.beginReadArray("logins");
for (int i = 0; i < size; ++i) {
    settings.setArrayIndex(i);
    Login login;
    login.userName = settings.value("userName").toString();
    login.password = settings.value("password").toString();
    logins.append(login);
}
settings.endArray();
//! [15]


//! [16]
struct Login {
    QString userName;
    QString password;
};
QList<Login> logins;
...

QSettings settings;
settings.beginWriteArray("logins");
for (int i = 0; i < logins.size(); ++i) {
    settings.setArrayIndex(i);
    settings.setValue("userName", list.at(i).userName);
    settings.setValue("password", list.at(i).password);
}
settings.endArray();
//! [16]


//! [17]
QSettings settings;
settings.setValue("fridge/color", Qt::white);
settings.setValue("fridge/size", QSize(32, 96));
settings.setValue("sofa", true);
settings.setValue("tv", false);

QStringList keys = settings.allKeys();
// keys: ["fridge/color", "fridge/size", "sofa", "tv"]
//! [17]


//! [18]
settings.beginGroup("fridge");
keys = settings.allKeys();
// keys: ["color", "size"]
//! [18]


//! [19]
QSettings settings;
settings.setValue("fridge/color", Qt::white);
settings.setValue("fridge/size", QSize(32, 96));
settings.setValue("sofa", true);
settings.setValue("tv", false);

QStringList keys = settings.childKeys();
// keys: ["sofa", "tv"]
//! [19]


//! [20]
settings.beginGroup("fridge");
keys = settings.childKeys();
// keys: ["color", "size"]
//! [20]


//! [21]
QSettings settings;
settings.setValue("fridge/color", Qt::white);
settings.setValue("fridge/size", QSize(32, 96));
settings.setValue("sofa", true);
settings.setValue("tv", false);

QStringList groups = settings.childGroups();
// group: ["fridge"]
//! [21]


//! [22]
settings.beginGroup("fridge");
groups = settings.childGroups();
// groups: []
//! [22]


//! [23]
QSettings settings;
settings.setValue("interval", 30);
settings.value("interval").toInt();     // returns 30

settings.setValue("interval", 6.55);
settings.value("interval").toDouble();  // returns 6.55
//! [23]


//! [24]
QSettings settings;
settings.setValue("ape");
settings.setValue("monkey", 1);
settings.setValue("monkey/sea", 2);
settings.setValue("monkey/doe", 4);

settings.remove("monkey");
QStringList keys = settings.allKeys();
// keys: ["ape"]
//! [24]


//! [25]
QSettings settings;
settings.setValue("ape");
settings.setValue("monkey", 1);
settings.setValue("monkey/sea", 2);
settings.setValue("monkey/doe", 4);

settings.beginGroup("monkey");
settings.remove("");
settings.endGroup();

QStringList keys = settings.allKeys();
// keys: ["ape"]
//! [25]


//! [26]
QSettings settings;
settings.setValue("animal/snake", 58);
settings.value("animal/snake", 1024).toInt();   // returns 58
settings.value("animal/zebra", 1024).toInt();   // returns 1024
settings.value("animal/zebra").toInt();         // returns 0
//! [26]


//! [27]
bool myReadFunc(QIODevice &device, QSettings::SettingsMap &map);
//! [27]


//! [28]
bool myWriteFunc(QIODevice &device, const QSettings::SettingsMap &map);
//! [28]


//! [29]
bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);

int main(int argc, char *argv[])
{
    const QSettings::Format XmlFormat =
            QSettings::registerFormat("xml", readXmlFile, writeXmlFile);

    QSettings settings(XmlFormat, QSettings::UserScope, "MySoft",
                       "Star Runner");

    ...
}
//! [29]
