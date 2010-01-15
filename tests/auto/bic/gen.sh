#!/bin/sh
#############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is the build configuration utility of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## No Commercial Usage
## This file contains pre-release code and may not be distributed.
## You may use this file in accordance with the terms and conditions
## contained in the Technology Preview License Agreement accompanying
## this package.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
##
##
##
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################

if [ "$#" -lt 2 ]; then
    echo "$0 - Generates reference files for b/c autotest"
    echo "Usage: $0 [module|-all] [platform]"
    echo "Examples: $0 -all 4.1.0.macx-gcc-ppc32"
    echo "          $0 QtGui 4.0.0.linux-gcc-ia32"
    exit 1
fi

if [ "$1" = "-all" ]; then
    modules="QtCore QtGui QtSql QtSvg QtNetwork QtScript QtXml QtXmlPatterns QtOpenGL Qt3Support QtTest QtDBus QtDesigner QtXmlPatterns"
else
    modules="$1"
fi

for module in $modules; do
    echo "#include <$module/$module>" >test.cpp
    g++ -c -I$QTDIR/include -DQT_NO_STL -DQT3_SUPPORT -fdump-class-hierarchy test.cpp
    mv test.cpp*.class $module.$2.txt
    # Remove template classes from the output
    perl -pi -e '$skip = 0 if (/^\n/);
        $skip = 1 if (/^(Class|Vtable).*</);
        $_ = "" if ($skip);' $module.$2.txt
done

