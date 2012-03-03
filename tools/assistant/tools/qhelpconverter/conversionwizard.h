/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONVERSIONWIZARD_H
#define CONVERSIONWIZARD_H

#include <QtGui/QWizard>
#include "adpreader.h"

QT_BEGIN_NAMESPACE

class InputPage;
class GeneralPage;
class FilterPage;
class IdentifierPage;
class PathPage;
class FilesPage;
class OutputPage;
class FinishPage;
class HelpWindow;

class ConversionWizard : public QWizard
{
    Q_OBJECT

public:
    ConversionWizard();
    void setAdpFileName(const QString &fileName);

private slots:
    void pageChanged(int id);
    void showHelp(bool toggle);
    void convert();

private:
    enum Pages {Input_Page, General_Page, Filter_Page,
        Identifier_Page, Path_Page, Files_Page, Output_Page,
        Finish_Page};
    void initializePage(int id);
    QStringList getUnreferencedFiles(const QStringList &files);
    bool eventFilter(QObject *obj, QEvent *e);
    
    AdpReader m_adpReader;
    InputPage *m_inputPage;
    GeneralPage *m_generalPage;
    FilterPage *m_filterPage;
    IdentifierPage *m_identifierPage;
    PathPage *m_pathPage;
    FilesPage *m_filesPage;
    OutputPage *m_outputPage;
    FinishPage *m_finishPage;
    QStringList m_files;
    HelpWindow *m_helpWindow;
};

QT_END_NAMESPACE

#endif
