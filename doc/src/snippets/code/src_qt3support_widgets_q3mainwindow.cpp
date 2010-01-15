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
Q3MainWindow *mw = new Q3MainWindow;
QTextEdit *edit = new QTextEdit(mw, "editor");
edit->setFocus();
mw->setWindowTitle("Main Window");
mw->setCentralWidget(edit);
mw->show();
//! [0]


//! [1]
Q3ToolBar *tb = new Q3ToolBar(this);
addDockWindow(tb, tr("Menubar"), Top, false);
QMenuBar *mb = new QMenuBar(tb);
mb->setFrameStyle(QFrame::NoFrame);
tb->setStretchableWidget(mb);
setDockEnabled(tb, Left, false);
setDockEnabled(tb, Right, false);
//! [1]


//! [2]
QFile file(filename);
if (file.open(IO_WriteOnly)) {
    QTextStream stream(&file);
    stream << *mainWindow;
    file.close();
}
//! [2]


//! [3]
QFile file(filename);
if (file.open(IO_ReadOnly)) {
    QTextStream stream(&file);
    stream >> *mainWindow;
    file.close();
}
//! [3]


//! [4]
Q3PopupMenu * help = new Q3PopupMenu(this);
help->insertItem("What's &This", this , SLOT(enterWhatsThis()), Qt::SHIFT+Qt::Key_F1);
//! [4]
