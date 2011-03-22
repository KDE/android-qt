/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>

#include "tokenizers/basic/basic.h"
#include "tokenizers/basicNamespace/basicNamespace.h"
#include "tokenizers/boilerplate/boilerplate.h"
#include "tokenizers/noToString/noToString.h"

/*!
 \class tst_QTokenAutomaton
 \internal
 \brief Tests the QTokenAutomaton generator.

TODO:
    - Verify high Unicode codepoints
    - Verify that signatures are static.
 */
class tst_QTokenAutomaton : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void tokenizerBasic() const;
};

void tst_QTokenAutomaton::tokenizerBasic() const
{
    typedef QPair<Basic::Token, QString> TokenPair;
    typedef QList<TokenPair> TokenPairList;

    /* Tokenizer basic. */
    {
        /* All the tokens matching tokens. */
        {
            TokenPairList tokens;
            tokens.append(qMakePair(Basic::_, QString::fromLatin1("abc")));
            tokens.append(qMakePair(Basic::_111, QString::fromLatin1("def")));
            tokens.append(qMakePair(Basic::wobbly, QString::fromLatin1("ghi")));
            tokens.append(qMakePair(Basic::FFFF, QString::fromLatin1("FFFF")));
            tokens.append(qMakePair(Basic::FFFG, QString::fromLatin1("FFFG")));
            tokens.append(qMakePair(Basic::FFGG, QString::fromLatin1("FFGG")));
            tokens.append(qMakePair(Basic::FFGF, QString::fromLatin1("FFGF")));
            tokens.append(qMakePair(Basic::FFLM, QString::fromLatin1("FFLM")));
            tokens.append(qMakePair(Basic::AReallyLongTokenIFreakinMeanItUKnowUKnowKnow, QString::fromLatin1("aReallyLongTokenIFreakinMeanItUKnowUKnowKnow")));
            tokens.append(qMakePair(Basic::WeHaveDashes, QString::fromLatin1("we-have-dashes")));
            tokens.append(qMakePair(Basic::WeHaveDashes2, QString::fromLatin1("we-have-dashes-")));

            /* toToken(). */
            for(int i = 0; i < tokens.count(); ++i)
            {
                const TokenPair &at = tokens.at(i);
                /* QString. */
                QCOMPARE(Basic::toToken(at.second), at.first);

                /* const QChar *, int. */
                QCOMPARE(Basic::toToken(at.second.constData(), at.second.length()), at.first);

                /* QStringRef. */
                const QStringRef ref(&at.second);
                QCOMPARE(Basic::toToken(ref), at.first);
            }

            /* toString(). */
            for(int i = 0; i < tokens.count(); ++i)
            {
                const TokenPair &at = tokens.at(i);
                QCOMPARE(Basic::toString(at.first), at.second);
            }
        }

        /* Tokens that we don't recognize. */
        {
            QStringList tokens;
            tokens.append(QLatin1String("NO-MATCH"));
            tokens.append(QLatin1String("NoKeyword"));
            tokens.append(QString());

            for(int i = 0; i < tokens.count(); ++i)
                QCOMPARE(Basic::toToken(tokens.at(i)), Basic::NoKeyword);
        }

        /* Weird values for toToken(). */
        {
            QCOMPARE(Basic::toString(Basic::Token(5000)), QString());
            QCOMPARE(Basic::toString(Basic::NoKeyword), QString());
        }

    }
}

QTEST_MAIN(tst_QTokenAutomaton)

#include "tst_qtokenautomaton.moc"
