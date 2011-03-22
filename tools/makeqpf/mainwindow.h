/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "qpf2.h"

QT_BEGIN_NAMESPACE

class QListWidgetItem;

class MainWindow : public QMainWindow, Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow(const QString &customFont);

private Q_SLOTS:
    void on_actionAdd_Custom_Font_triggered();
    void on_selectAll_clicked();
    void on_deselectAll_clicked();
    void on_invertSelection_clicked();
    void fontChanged();
    void on_browsePath_clicked();
    void on_browseSampleFile_clicked();
    void on_generate_clicked();
    void on_sampleFile_editingFinished();

private:
    void populateCharacterRanges();
    void addCustomFont(const QString &fontFile);

private:
    QList<QPF::CharacterRange> sampleFileRanges;
};

QT_END_NAMESPACE

#endif // MAINWINDOW_H
