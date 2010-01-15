/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFONTDATABASE_H
#define QFONTDATABASE_H

#include <QtGui/qwindowdefs.h>
#include <QtCore/qstring.h>
#include <QtGui/qfont.h>
#ifdef QT3_SUPPORT
#include <QtCore/qstringlist.h>
#include <QtCore/qlist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QStringList;
template <class T> class QList;
struct QFontDef;
class QFontEngine;

class QFontDatabasePrivate;

class Q_GUI_EXPORT QFontDatabase
{
    Q_GADGET
    Q_ENUMS(WritingSystem)
public:
    // do not re-order or delete entries from this enum without updating the
    // QPF2 format and makeqpf!!
    enum WritingSystem {
        Any,

        Latin,
        Greek,
        Cyrillic,
        Armenian,
        Hebrew,
        Arabic,
        Syriac,
        Thaana,
        Devanagari,
        Bengali,
        Gurmukhi,
        Gujarati,
        Oriya,
        Tamil,
        Telugu,
        Kannada,
        Malayalam,
        Sinhala,
        Thai,
        Lao,
        Tibetan,
        Myanmar,
        Georgian,
        Khmer,
        SimplifiedChinese,
        TraditionalChinese,
        Japanese,
        Korean,
        Vietnamese,

        Symbol,
        Other = Symbol,

        Ogham,
        Runic,
        Nko,

        WritingSystemsCount
    };

    static QList<int> standardSizes();

    QFontDatabase();

    QList<WritingSystem> writingSystems() const;
    QList<WritingSystem> writingSystems(const QString &family) const;

    QStringList families(WritingSystem writingSystem = Any) const;
    QStringList styles(const QString &family) const;
    QList<int> pointSizes(const QString &family, const QString &style = QString());
    QList<int> smoothSizes(const QString &family, const QString &style);
    QString styleString(const QFont &font);
    QString styleString(const QFontInfo &fontInfo);

    QFont font(const QString &family, const QString &style, int pointSize) const;

    bool isBitmapScalable(const QString &family, const QString &style = QString()) const;
    bool isSmoothlyScalable(const QString &family, const QString &style = QString()) const;
    bool isScalable(const QString &family, const QString &style = QString()) const;
    bool isFixedPitch(const QString &family, const QString &style = QString()) const;

    bool italic(const QString &family, const QString &style) const;
    bool bold(const QString &family, const QString &style) const;
    int weight(const QString &family, const QString &style) const;

    static QString writingSystemName(WritingSystem writingSystem);
    static QString writingSystemSample(WritingSystem writingSystem);

    static int addApplicationFont(const QString &fileName);
    static int addApplicationFontFromData(const QByteArray &fontData);
    static QStringList applicationFontFamilies(int id);
    static bool removeApplicationFont(int id);
    static bool removeAllApplicationFonts();

    static bool supportsThreadedFontRendering();

private:
    static void createDatabase();
    static void parseFontName(const QString &name, QString &foundry, QString &family);
#if defined(Q_WS_QWS) || defined(Q_WS_LITE) || defined(Q_OS_SYMBIAN)
    static QFontEngine *findFont(int script, const QFontPrivate *fp, const QFontDef &request);
#endif
    static void load(const QFontPrivate *d, int script);
#ifdef Q_WS_X11
    static QFontEngine *loadXlfd(int screen, int script, const QFontDef &request, int force_encoding_id = -1);
#endif

    friend struct QFontDef;
    friend class QFontPrivate;
    friend class QFontDialog;
    friend class QFontDialogPrivate;
    friend class QFontEngineMultiXLFD;
    friend class QFontEngineMultiQWS;
    friend class QFontEngineMultiS60;

    QFontDatabasePrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFONTDATABASE_H
