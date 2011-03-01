/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDESKTOPICON_P_H
#define QDESKTOPICON_P_H

#ifndef QT_NO_ICON
//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/QIcon>
#include <QtGui/QIconEngine>
#include <QtGui/QPixmapCache>
#include <private/qicon_p.h>
#include <private/qfactoryloader_p.h>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

class QIconLoader;

struct QIconDirInfo
{
    enum Type { Fixed, Scalable, Threshold };
    QIconDirInfo(const QString &_path = QString()) :
            path(_path),
            size(0),
            maxSize(0),
            minSize(0),
            threshold(0),
            type(Threshold) {}
    QString path;
    short size;
    short maxSize;
    short minSize;
    short threshold;
    Type type : 4;
};

class QIconLoaderEngineEntry
 {
public:
    virtual ~QIconLoaderEngineEntry() {}
    virtual QPixmap pixmap(const QSize &size,
                           QIcon::Mode mode,
                           QIcon::State state) = 0;
    QString filename;
    QIconDirInfo dir;
    static int count;
};

struct ScalableEntry : public QIconLoaderEngineEntry
{
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QIcon svgIcon;
};

struct PixmapEntry : public QIconLoaderEngineEntry
{
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QPixmap basePixmap;
};

typedef QList<QIconLoaderEngineEntry*> QThemeIconEntries;

class QIconLoaderEngine : public QIconEngineV2
{
public:
    QIconLoaderEngine(const QString& iconName = QString());
    ~QIconLoaderEngine();

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QIconEngineV2 *clone() const;
    bool read(QDataStream &in);
    bool write(QDataStream &out) const;

private:
    QString key() const;
    bool hasIcon() const;
    void ensureLoaded();
    void virtual_hook(int id, void *data);
    QIconLoaderEngineEntry *entryForSize(const QSize &size);
    QIconLoaderEngine(const QIconLoaderEngine &other);
    QThemeIconEntries m_entries;
    QString m_iconName;
    uint m_key;

    friend class QIconLoader;
};

class QIconTheme
{
public:
    QIconTheme(const QString &name);
    QIconTheme() : m_valid(false) {}
    QStringList parents() { return m_parents; }
    QList <QIconDirInfo> keyList() { return m_keyList; }
    QString contentDir() { return m_contentDir; }
    bool isValid() { return m_valid; }

private:
    QString m_contentDir;
    QList <QIconDirInfo> m_keyList;
    QStringList m_parents;
    bool m_valid;
};

class QIconLoader : public QObject
{
public:
    QIconLoader();
    QThemeIconEntries loadIcon(const QString &iconName) const;
    uint themeKey() const { return m_themeKey; }

    QString themeName() const { return m_userTheme.isEmpty() ? m_systemTheme : m_userTheme; }
    void setThemeName(const QString &themeName);
    QIconTheme theme() { return themeList.value(themeName()); }
    void setThemeSearchPath(const QStringList &searchPaths);
    QStringList themeSearchPaths() const;
    QIconDirInfo dirInfo(int dirindex);
    static QIconLoader *instance();
    void updateSystemTheme();
    void invalidateKey() { m_themeKey++; }
    void ensureInitialized();

private:
    QThemeIconEntries findIconHelper(const QString &themeName,
                                     const QString &iconName,
                                     QStringList &visited) const;
    uint m_themeKey;
    bool m_supportsSvg;
    bool m_initialized;

    mutable QString m_userTheme;
    mutable QString m_systemTheme;
    mutable QStringList m_iconDirs;
    mutable QHash <QString, QIconTheme> themeList;
};

QT_END_NAMESPACE

#endif // QDESKTOPICON_P_H

#endif //QT_NO_ICON
