/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef PROPARSERUTILS_H
#define PROPARSERUTILS_H

#include <QtCore/QDir>
#ifndef QT_BOOTSTRAPPED
#include <QtCore/QLibraryInfo>
#endif

QT_BEGIN_NAMESPACE

#ifdef QT_BOOTSTRAPPED
// this is a stripped down version of the one found in QtCore
class QLibraryInfo
{
public:
    enum LibraryLocation
    {
        PrefixPath,
        DocumentationPath,
        HeadersPath,
        LibrariesPath,
        BinariesPath,
        PluginsPath,
        DataPath,
        TranslationsPath,
        SettingsPath,
        DemosPath,
        ExamplesPath
    };
    static QString location(LibraryLocation);
};
#endif

// Pre- and postcondition macros
#define PRE(cond) do {if (!(cond))qt_assert(#cond,__FILE__,__LINE__);} while (0)
#define POST(cond) do {if (!(cond))qt_assert(#cond,__FILE__,__LINE__);} while (0)

// This struct is from qmake, but we are not using everything.
struct Option
{
    //simply global convenience
    //static QString libtool_ext;
    //static QString pkgcfg_ext;
    //static QString prf_ext;
    //static QString prl_ext;
    //static QString ui_ext;
    //static QStringList h_ext;
    //static QStringList cpp_ext;
    //static QString h_moc_ext;
    //static QString cpp_moc_ext;
    //static QString obj_ext;
    //static QString lex_ext;
    //static QString yacc_ext;
    //static QString h_moc_mod;
    //static QString cpp_moc_mod;
    //static QString lex_mod;
    //static QString yacc_mod;
    static QString dir_sep;
    static QString dirlist_sep;
    static QString qmakespec;
    static QChar field_sep;

    enum TARG_MODE { TARG_UNIX_MODE, TARG_WIN_MODE, TARG_MACX_MODE, TARG_MAC9_MODE, TARG_QNX6_MODE };
    static TARG_MODE target_mode;
    //static QString pro_ext;
    //static QString res_ext;

    static void init()
    {
#ifdef Q_OS_WIN
        Option::dirlist_sep = QLatin1Char(';');
        Option::dir_sep = QLatin1Char('\\');
#else
        Option::dirlist_sep = QLatin1Char(':');
        Option::dir_sep = QLatin1Char(QLatin1Char('/'));
#endif
        Option::qmakespec = QString::fromLatin1(qgetenv("QMAKESPEC").data());
        Option::field_sep = QLatin1Char(' ');
    }

    enum StringFixFlags {
        FixNone                 = 0x00,
        FixEnvVars              = 0x01,
        FixPathCanonicalize     = 0x02,
        FixPathToLocalSeparators  = 0x04,
        FixPathToTargetSeparators = 0x08
    };
    static QString fixString(QString string, uchar flags);

    inline static QString fixPathToLocalOS(const QString &in, bool fix_env = true, bool canonical = true)
    {
        uchar flags = FixPathToLocalSeparators;
        if (fix_env)
            flags |= FixEnvVars;
        if (canonical)
            flags |= FixPathCanonicalize;
        return fixString(in, flags);
    }
};
#if defined(Q_OS_WIN32)
Option::TARG_MODE Option::target_mode = Option::TARG_WIN_MODE;
#elif defined(Q_OS_MAC)
Option::TARG_MODE Option::target_mode = Option::TARG_MACX_MODE;
#elif defined(Q_OS_QNX6)
Option::TARG_MODE Option::target_mode = Option::TARG_QNX6_MODE;
#else
Option::TARG_MODE Option::target_mode = Option::TARG_UNIX_MODE;
#endif

QString Option::qmakespec;
QString Option::dirlist_sep;
QString Option::dir_sep;
QChar Option::field_sep;

static void insertUnique(QHash<QString, QStringList> *map,
    const QString &key, const QStringList &value)
{
    QStringList &sl = (*map)[key];
    foreach (const QString &str, value)
        if (!sl.contains(str))
            sl.append(str);
}

static void removeEach(QHash<QString, QStringList> *map,
    const QString &key, const QStringList &value)
{
    QStringList &sl = (*map)[key];
    foreach (const QString &str, value)
        sl.removeAll(str);
}

/*
  See ProFileEvaluator::Private::visitProValue(...)

static QStringList replaceInList(const QStringList &varList, const QRegExp &regexp,
                           const QString &replace, bool global)
{
    QStringList resultList = varList;

    for (QStringList::Iterator varit = resultList.begin(); varit != resultList.end();) {
        if (varit->contains(regexp)) {
            *varit = varit->replace(regexp, replace);
            if (varit->isEmpty())
                varit = resultList.erase(varit);
            else
                ++varit;
            if (!global)
                break;
        } else {
            ++varit;
        }
    }
    return resultList;
}
*/

inline QString fixEnvVariables(const QString &x)
{
    return Option::fixString(x, Option::FixEnvVars);
}

inline QStringList splitPathList(const QString &paths)
{
    return paths.split(Option::dirlist_sep);
}

static QStringList split_arg_list(QString params)
{
    int quote = 0;
    QStringList args;

    const ushort LPAREN = '(';
    const ushort RPAREN = ')';
    const ushort SINGLEQUOTE = '\'';
    const ushort DOUBLEQUOTE = '"';
    const ushort COMMA = ',';
    const ushort SPACE = ' ';
    //const ushort TAB = '\t';

    ushort unicode;
    const QChar *params_data = params.data();
    const int params_len = params.length();
    int last = 0;
    while (last < params_len && ((params_data+last)->unicode() == SPACE
                                /*|| (params_data+last)->unicode() == TAB*/))
        ++last;
    for (int x = last, parens = 0; x <= params_len; x++) {
        unicode = (params_data+x)->unicode();
        if (x == params_len) {
            while (x && (params_data+(x-1))->unicode() == SPACE)
                --x;
            QString mid(params_data+last, x-last);
            if (quote) {
                if (mid[0] == quote && mid[(int)mid.length()-1] == quote)
                    mid = mid.mid(1, mid.length()-2);
                quote = 0;
            }
            args << mid;
            break;
        }
        if (unicode == LPAREN) {
            --parens;
        } else if (unicode == RPAREN) {
            ++parens;
        } else if (quote && unicode == quote) {
            quote = 0;
        } else if (!quote && (unicode == SINGLEQUOTE || unicode == DOUBLEQUOTE)) {
            quote = unicode;
        }
        if (!parens && !quote && unicode == COMMA) {
            QString mid = params.mid(last, x - last).trimmed();
            args << mid;
            last = x+1;
            while (last < params_len && ((params_data+last)->unicode() == SPACE
                                        /*|| (params_data+last)->unicode() == TAB*/))
                ++last;
        }
    }
    return args;
}

static QStringList split_value_list(const QString &vals, bool do_semicolon=false)
{
    QString build;
    QStringList ret;
    QStack<char> quote;

    const ushort LPAREN = '(';
    const ushort RPAREN = ')';
    const ushort SINGLEQUOTE = '\'';
    const ushort DOUBLEQUOTE = '"';
    const ushort BACKSLASH = '\\';
    const ushort SEMICOLON = ';';

    ushort unicode;
    const QChar *vals_data = vals.data();
    const int vals_len = vals.length();
    for (int x = 0, parens = 0; x < vals_len; x++) {
        unicode = vals_data[x].unicode();
        if (x != (int)vals_len-1 && unicode == BACKSLASH &&
            (vals_data[x+1].unicode() == SINGLEQUOTE || vals_data[x+1].unicode() == DOUBLEQUOTE)) {
            build += vals_data[x++]; //get that 'escape'
        } else if (!quote.isEmpty() && unicode == quote.top()) {
            quote.pop();
        } else if (unicode == SINGLEQUOTE || unicode == DOUBLEQUOTE) {
            quote.push(unicode);
        } else if (unicode == RPAREN) {
            --parens;
        } else if (unicode == LPAREN) {
            ++parens;
        }

        if (!parens && quote.isEmpty() && ((do_semicolon && unicode == SEMICOLON) ||
                                           vals_data[x] == Option::field_sep)) {
            ret << build;
            build.clear();
        } else {
            build += vals_data[x];
        }
    }
    if (!build.isEmpty())
        ret << build;
    return ret;
}

static QStringList qmake_mkspec_paths()
{
    QStringList ret;
    const QString concat = QDir::separator() + QLatin1String("mkspecs");
    QByteArray qmakepath = qgetenv("QMAKEPATH");
    if (!qmakepath.isEmpty()) {
        const QStringList lst = splitPathList(QString::fromLocal8Bit(qmakepath));
        for (QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it)
            ret << ((*it) + concat);
    }
    ret << QLibraryInfo::location(QLibraryInfo::DataPath) + concat;

    return ret;
}

QT_END_NAMESPACE

#endif // PROPARSERUTILS_H
