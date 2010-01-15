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
QUrlOperator op( "ftp://ftp.qt.nokia.com" );
op.listChildren(); // Asks the server to provide a directory listing
//! [0]


//! [1]
Q3Ftp *ftp = new Q3Ftp( this ); // this is an optional QObject parent
ftp->connectToHost( "ftp.qt.nokia.com" );
ftp->login();
//! [1]


//! [2]
ftp->connectToHost( "ftp.qt.nokia.com" );   // id == 1
ftp->login();                               // id == 2
ftp->cd( "qt" );                            // id == 3
ftp->get( "INSTALL" );                      // id == 4
ftp->close();                               // id == 5
//! [2]


//! [3]
start( 1 )
stateChanged( HostLookup )
stateChanged( Connecting )
stateChanged( Connected )
finished( 1, false )

start( 2 )
stateChanged( LoggedIn )
finished( 2, false )

start( 3 )
finished( 3, false )

start( 4 )
dataTransferProgress( 0, 3798 )
dataTransferProgress( 2896, 3798 )
readyRead()
dataTransferProgress( 3798, 3798 )
readyRead()
finished( 4, false )

start( 5 )
stateChanged( Closing )
stateChanged( Unconnected )
finished( 5, false )

done( false )
//! [3]


//! [4]
start( 1 )
stateChanged( HostLookup )
stateChanged( Connecting )
stateChanged( Connected )
finished( 1, false )

start( 2 )
finished( 2, true )

done( true )
//! [4]
