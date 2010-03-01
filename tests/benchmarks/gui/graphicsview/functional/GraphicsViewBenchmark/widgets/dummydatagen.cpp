/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QFile>
#include "theme.h"

#include "dummydatagen.h"

DummyDataGenerator::DummyDataGenerator() : m_isMale(false)
{
    QFile countryCodeFile(":/contact/areacodes.txt");
    countryCodeFile.open(QIODevice::ReadOnly);
    while (!countryCodeFile.atEnd()) {
        m_countryCodes << QString(countryCodeFile.readLine()).remove("\n");
    }
    
    QFile firstNameFFile(":/contact/firstnamesF.txt");
    firstNameFFile.open(QIODevice::ReadOnly);
    while (!firstNameFFile.atEnd()) {
        m_firstNamesF << QString(firstNameFFile.readLine()).remove("\n");
    }
  
    QFile firstNameMFile(":/contact/firstnamesM.txt");
    firstNameMFile.open(QIODevice::ReadOnly);
    while (!firstNameMFile.atEnd()) {
        m_firstNamesM << QString(firstNameMFile.readLine()).remove("\n");
    }
    
    QFile lastNameFile(":/contact/lastnames.txt");
    lastNameFile.open(QIODevice::ReadOnly);
    while (!lastNameFile.atEnd()) {
        m_lastNames << QString(lastNameFile.readLine()).remove("\n");
    }
    Reset();
}

DummyDataGenerator::~DummyDataGenerator()
{
    
}

void DummyDataGenerator::Reset()
{
    qsrand(100);
}

QString DummyDataGenerator::randomPhoneNumber(QString indexNumber)
{
    int index = qrand()%m_countryCodes.count();
    QString countryCode = m_countryCodes.at(index);
    QString areaCode = QString::number(index) + QString("0").repeated(2-QString::number(index).length());
    QString beginNumber = QString::number(555-index*2);
    QString endNumber = QString("0").repeated(4-indexNumber.length()) + indexNumber;
    
    return countryCode +" " + areaCode +" " + beginNumber +" " + endNumber;
}

QString DummyDataGenerator::randomFirstName()
{
    m_isMale = !m_isMale;
    if (m_isMale)
        return m_firstNamesM.at(qrand()%m_firstNamesM.count());
    return m_firstNamesF.at(qrand()%m_firstNamesF.count());
}

QString DummyDataGenerator::randomLastName()
{
    return m_lastNames.at(qrand()%m_lastNames.count());
}

QString DummyDataGenerator::randomName()
{
    return QString(randomFirstName()+QString(", ")+randomLastName());
}

QString DummyDataGenerator::randomIconItem()
{
    QString avatar = Theme::p()->pixmapPath() + "contact_default_icon.svg";
    if (qrand()%4) {
      int randVal = 1+qrand()%25;
      
      if(m_isMale && randVal > 15) {
          randVal -= 15;    
      }
      if(!m_isMale && randVal <= 10) {
          randVal += 10;
      }
      
      avatar = QString(":/avatars/avatar_%1.png").arg(randVal, 3, 10, QChar('0'));
    }
    return avatar;
}

QString DummyDataGenerator::randomStatusItem()
{
    switch ( qrand()%3 )
    {
        case 0: return Theme::p()->pixmapPath() + "contact_status_online.svg";
        case 1: return Theme::p()->pixmapPath() + "contact_status_offline.svg";
        case 2: return Theme::p()->pixmapPath() + "contact_status_idle.svg";
    }
    return 0;
}
