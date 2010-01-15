/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

#include "cmdlineparser.h"

QT_BEGIN_NAMESPACE

#define CHECK_NEXT_ARG \
    ++i < arguments.count() && !arguments.at(i).startsWith(QLatin1String("-"))

CmdLineParser::CmdLineParser()
    : m_enableRemoteControl(false),
    m_contents(Untouched),
    m_index(Untouched),
    m_bookmarks(Untouched),
    m_search(Untouched),
    m_register(None),
    m_removeSearchIndex(false),
    m_copy(false),
    m_quiet(false)
{
    m_helpMessage = QLatin1String(
        "Usage: assistant [Options]\n\n"
        "-collectionFile file       Uses the specified collection\n"
        "                           file instead of the default one\n"
        "-showUrl url               Shows the document with the\n"
        "                           url.\n"
        "-enableRemoteControl       Enables Assistant to be\n"
        "                           remotely controlled.\n"
        "-show widget               Shows the specified dockwidget\n"
        "                           which can be \"contents\", \"index\",\n"
        "                           \"bookmarks\" or \"search\".\n"
        "-activate widget           Activates the specified dockwidget\n"
        "                           which can be \"contents\", \"index\",\n"
        "                           \"bookmarks\" or \"search\".\n"
        "-hide widget               Hides the specified dockwidget\n"
        "                           which can be \"contents\", \"index\"\n"
        "                           \"bookmarks\" or \"search\".\n"
        "-register helpFile         Registers the specified help file\n"
        "                           (.qch) in the given collection\n"
        "                           file.\n"
        "-unregister helpFile       Unregisters the specified help file\n"
        "                           (.qch) from the give collection\n"
        "                           file.\n"
        "-setCurrentFilter filter   Set the filter as the active filter.\n"
        "-remove-search-index       Removes the full text search index.\n"
        "-quiet                     Does not display any error or\n"
        "                           status message.\n"
        "-help                      Displays this help.\n"
        );
}

CmdLineParser::Result CmdLineParser::parse(const QStringList &arguments)
{
    QString error;
    bool showHelp = false;

    for (int j=1; j<arguments.count(); ++j) {
        if (arguments.at(j).toLower() == QLatin1String("-quiet")) {
            m_quiet = true;
            break;
        }
    }

    for (int i=1; i<arguments.count(); ++i) {
        QString arg = arguments.at(i).toLower();
        if (arg == QLatin1String("-collectionfile")) {
            if (CHECK_NEXT_ARG) {
                m_collectionFile = getFileName(arguments.at(i));
                if (m_collectionFile.isEmpty()) {
                    error = QObject::tr("The specified collection file does not exist!");
                    break;
                }
            } else {
                error = QObject::tr("Missing collection file!");
                break;
            }
        } else if (arg == QLatin1String("-showurl")) {
            if (CHECK_NEXT_ARG) {
                QUrl url(arguments.at(i));
                if (url.isValid()) {
                    m_url = url;
                } else {
                    error = QObject::tr("Invalid URL!");
                    break;
                }
            } else {
                error = QObject::tr("Missing URL!");
                break;
            }
        } else if (arg == QLatin1String("-enableremotecontrol")) {
            m_enableRemoteControl = true;
        } else if (arg == QLatin1String("-show")) {
            if (CHECK_NEXT_ARG) {
                arg = arguments.at(i).toLower();
                if (arg == QLatin1String("contents")) {
                    m_contents = Show;
                } else if (arg == QLatin1String("index")) {
                    m_index = Show;
                } else if (arg == QLatin1String("bookmarks")) {
                    m_bookmarks = Show;
                } else if (arg == QLatin1String("search")) {
                    m_search = Show;
                } else {
                    error = QObject::tr("Unknown widget: %1").arg(arg);
                    break;
                }
            } else {
                error = QObject::tr("Missing widget!");
                break;
            }
        } else if (arg == QLatin1String("-hide")) {
            if (CHECK_NEXT_ARG) {
                arg = arguments.at(i).toLower();
                if (arg == QLatin1String("contents")) {
                    m_contents = Hide;
                } else if (arg == QLatin1String("index")) {
                    m_index = Hide;
                } else if (arg == QLatin1String("bookmarks")) {
                    m_bookmarks = Hide;
                } else if (arg == QLatin1String("search")) {
                    m_search = Hide;
                } else {
                    error = QObject::tr("Unknown widget: %1").arg(arg);
                    break;
                }
            } else {
                error = QObject::tr("Missing widget!");
                break;
            }
        } else if (arg == QLatin1String("-activate")) {
            if (CHECK_NEXT_ARG) {
                arg = arguments.at(i).toLower();
                if (arg == QLatin1String("contents")) {
                    m_contents = Activate;
                } else if (arg == QLatin1String("index")) {
                    m_index = Activate;
                } else if (arg == QLatin1String("bookmarks")) {
                    m_bookmarks = Activate;
                } else if (arg == QLatin1String("search")) {
                    m_search = Activate;
                } else {
                    error = QObject::tr("Unknown widget: %1").arg(arg);
                    break;
                }
            } else {
                error = QObject::tr("Missing widget!");
                break;
            }
        } else if (arg == QLatin1String("-register")) {
            if (CHECK_NEXT_ARG) {
                m_helpFile = getFileName(arguments.at(i));
                if (m_helpFile.isEmpty()) {
                    error = QObject::tr("The specified Qt help file does not exist!");
                    break;
                }
                m_register = Register;
            } else {
                error = QObject::tr("Missing help file!");
                break;
            }
        } else if (arg == QLatin1String("-unregister")) {
            if (CHECK_NEXT_ARG) {
                m_helpFile = getFileName(arguments.at(i));
                if (m_helpFile.isEmpty()) {
                    error = QObject::tr("The specified Qt help file does not exist!");
                    break;
                }
                m_register = Unregister;
            } else {
                error = QObject::tr("Missing help file!");
                break;
            }
        } else if (arg == QLatin1String("-setcurrentfilter")) {
            if (CHECK_NEXT_ARG) {
                m_currentFilter = arguments.at(i);
            } else {
                error = QObject::tr("Missing filter argument!");
                break;
            }
        } else if (arg == QLatin1String("-remove-search-index")) {
            m_removeSearchIndex = true;
        } else if (arg == QLatin1String("-quiet")) {
            continue;
        } else if (arg == QLatin1String("-help")) {
            showHelp = true;
        } else if (arg == QLatin1String("-copy")) {
            m_copy = true;
        } else {
            error = QObject::tr("Unknown option: %1").arg(arg);
            break;
        }
    }

    if (!error.isEmpty()) {
        showMessage(error + QLatin1String("\n\n\n") + m_helpMessage, true);
        return Error;
    } else if (showHelp) {
        showMessage(m_helpMessage, false);
        return Help;
    }
    return Ok;
}

QString CmdLineParser::getFileName(const QString &fileName)
{
    QFileInfo fi(fileName);
    if (!fi.exists())
        return QString();
    return fi.absoluteFilePath();
}

void CmdLineParser::showMessage(const QString &msg, bool error)
{
    if (m_quiet)
        return;
#ifdef Q_OS_WIN
    QString s = QLatin1String("<pre>") + msg + QLatin1String("</pre>");
    if (error)
        QMessageBox::critical(0, QObject::tr("Qt Assistant"), s);
    else
        QMessageBox::information(0, QObject::tr("Qt Assistant"), s);
#else
    fprintf(error ? stderr : stdout, "%s\n", qPrintable(msg));
#endif
}

void CmdLineParser::setCollectionFile(const QString &file)
{
    m_collectionFile = file;
}

QString CmdLineParser::collectionFile() const
{
    return m_collectionFile;
}

QUrl CmdLineParser::url() const
{
    return m_url;
}

bool CmdLineParser::enableRemoteControl() const
{
    return m_enableRemoteControl;
}

CmdLineParser::ShowState CmdLineParser::contents() const
{
    return m_contents;
}

CmdLineParser::ShowState CmdLineParser::index() const
{
    return m_index;
}

CmdLineParser::ShowState CmdLineParser::bookmarks() const
{
    return m_bookmarks;
}

CmdLineParser::ShowState CmdLineParser::search() const
{
    return m_search;
}

QString CmdLineParser::currentFilter() const
{
    return m_currentFilter;
}

bool CmdLineParser::removeSearchIndex() const
{
    return m_removeSearchIndex;
}

CmdLineParser::RegisterState CmdLineParser::registerRequest() const
{
    return m_register;
}

QString CmdLineParser::helpFile() const
{
    return m_helpFile;
}

QT_END_NAMESPACE
