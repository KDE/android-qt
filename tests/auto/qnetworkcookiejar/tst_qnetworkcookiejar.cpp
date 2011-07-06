/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>
#include <QtNetwork/QNetworkCookieJar>
#include "private/qtldurl_p.h"

class tst_QNetworkCookieJar: public QObject
{
    Q_OBJECT

private slots:
    void getterSetter();
    void setCookiesFromUrl_data();
    void setCookiesFromUrl();
    void setCookiesFromUrl_50CookiesLimitPerDomain();
    void cookiesForUrl_data();
    void cookiesForUrl();
    void effectiveTLDs_data();
    void effectiveTLDs();
};

QT_BEGIN_NAMESPACE

namespace QTest {
    template<>
    char *toString(const QNetworkCookie &cookie)
    {
        return qstrdup(cookie.toRawForm());
    }

    template<>
    char *toString(const QList<QNetworkCookie> &list)
    {
        QString result = "QList(";
        bool first = true;
        foreach (QNetworkCookie cookie, list) {
            if (!first)
                result += ", ";
            first = false;
            result += QString::fromLatin1("QNetworkCookie(%1)").arg(QLatin1String(cookie.toRawForm()));
        }

        return qstrdup(result.append(')').toLocal8Bit());
    }
}

QT_END_NAMESPACE

class MyCookieJar: public QNetworkCookieJar
{
public:
    inline QList<QNetworkCookie> allCookies() const
        { return QNetworkCookieJar::allCookies(); }
    inline void setAllCookies(const QList<QNetworkCookie> &cookieList)
        { QNetworkCookieJar::setAllCookies(cookieList); }
};

void tst_QNetworkCookieJar::getterSetter()
{
    MyCookieJar jar;

    QVERIFY(jar.allCookies().isEmpty());

    QList<QNetworkCookie> list;
    QNetworkCookie cookie;
    cookie.setName("a");
    list << cookie;

    jar.setAllCookies(list);
    QCOMPARE(jar.allCookies(), list);
}

void tst_QNetworkCookieJar::setCookiesFromUrl_data()
{
    QTest::addColumn<QList<QNetworkCookie> >("preset");
    QTest::addColumn<QNetworkCookie>("newCookie");
    QTest::addColumn<QString>("referenceUrl");
    QTest::addColumn<QList<QNetworkCookie> >("expectedResult");
    QTest::addColumn<bool>("setCookies");

    QList<QNetworkCookie> preset;
    QList<QNetworkCookie> result;
    QNetworkCookie cookie;

    cookie.setName("a");
    cookie.setPath("/");
    cookie.setDomain(".foo.tld");
    result += cookie;
    QTest::newRow("just-add") << preset << cookie << "http://www.foo.tld" << result << true;

    preset = result;
    QTest::newRow("replace-1") << preset << cookie << "http://www.foo.tld" << result << true;

    cookie.setValue("bc");
    result.clear();
    result += cookie;
    QTest::newRow("replace-2") << preset << cookie << "http://www.foo.tld" << result << true;

    preset = result;
    cookie.setName("d");
    result += cookie;
    QTest::newRow("append") << preset << cookie << "http://www.foo.tld" << result << true;

    cookie = preset.at(0);
    result = preset;
    cookie.setPath("/something");
    result += cookie;
    QTest::newRow("diff-path") << preset << cookie << "http://www.foo.tld/something" << result << true;

    preset.clear();
    preset += cookie;
    cookie.setPath("/");
    QTest::newRow("diff-path-order") << preset << cookie << "http://www.foo.tld" << result << true;

    preset.clear();
    result.clear();
    QNetworkCookie finalCookie = cookie;
    cookie.setDomain("foo.tld");
    finalCookie.setDomain(".foo.tld");
    result += finalCookie;
    QTest::newRow("should-add-dot-prefix") << preset << cookie << "http://www.foo.tld" << result << true;

    result.clear();
    cookie.setDomain("");
    finalCookie.setDomain("www.foo.tld");
    result += finalCookie;
    QTest::newRow("should-set-default-domain") << preset << cookie << "http://www.foo.tld" << result << true;

    // security test:
    result.clear();
    preset.clear();
    cookie.setDomain("something.completely.different");
    QTest::newRow("security-domain-1") << preset << cookie << "http://www.foo.tld" << result << false;

    // we want the cookie to be accepted although the path does not match, see QTBUG-5815
    cookie.setDomain(".foo.tld");
    cookie.setPath("/something");
    result += cookie;
    QTest::newRow("security-path-1") << preset << cookie << "http://www.foo.tld" << result << true;

    // check effective TLDs
    // 1. co.uk is an effective TLD, should be denied
    result.clear();
    preset.clear();
    cookie.setPath("/");
    cookie.setDomain(".co.uk");
    QTest::newRow("effective-tld1-denied") << preset << cookie << "http://something.co.uk" << result << false;
    cookie.setDomain("co.uk");
    QTest::newRow("effective-tld1-denied2") << preset << cookie << "http://something.co.uk" << result << false;
    cookie.setDomain(".something.co.uk");
    result += cookie;
    QTest::newRow("effective-tld1-accepted") << preset << cookie << "http://something.co.uk" << result << true;

    // 2. anything .ar is an effective TLD ('*.ar'), but 'gobiernoelectronico.ar' is an exception
    result.clear();
    preset.clear();
    cookie.setDomain(".farmacia.ar");
    QTest::newRow("effective-tld2-denied") << preset << cookie << "http://farmacia.ar" << result << false;
    QTest::newRow("effective-tld2-denied2") << preset << cookie << "http://www.farmacia.ar" << result << false;
    QTest::newRow("effective-tld2-denied3") << preset << cookie << "http://www.anything.farmacia.ar" << result << false;
    cookie.setDomain(".gobiernoelectronico.ar");
    result += cookie;
    QTest::newRow("effective-tld2-accepted") << preset << cookie << "http://www.gobiernoelectronico.ar" << result << true;


    // setting the defaults:
    finalCookie = cookie;
    finalCookie.setPath("/something/");
    finalCookie.setDomain("www.foo.tld");
    cookie.setPath("");
    cookie.setDomain("");
    result.clear();
    result += finalCookie;
    QTest::newRow("defaults-1") << preset << cookie << "http://www.foo.tld/something/" << result << true;

    finalCookie.setPath("/");
    result.clear();
    result += finalCookie;
    QTest::newRow("defaults-2") << preset << cookie << "http://www.foo.tld" << result << true;

    // security test: do not accept cookie domains like ".com" nor ".com." (see RFC 2109 section 4.3.2)
    result.clear();
    preset.clear();
    cookie.setDomain(".com");
    QTest::newRow("rfc2109-4.3.2-ex3") << preset << cookie << "http://x.foo.com" << result << false;

    result.clear();
    preset.clear();
    cookie.setDomain(".com.");
    QTest::newRow("rfc2109-4.3.2-ex3-2") << preset << cookie << "http://x.foo.com" << result << false;
}

void tst_QNetworkCookieJar::setCookiesFromUrl()
{
    QFETCH(QList<QNetworkCookie>, preset);
    QFETCH(QNetworkCookie, newCookie);
    QFETCH(QString, referenceUrl);
    QFETCH(QList<QNetworkCookie>, expectedResult);
    QFETCH(bool, setCookies);

    QList<QNetworkCookie> cookieList;
    cookieList += newCookie;
    MyCookieJar jar;
    jar.setAllCookies(preset);
    QCOMPARE(jar.setCookiesFromUrl(cookieList, referenceUrl), setCookies);

    QList<QNetworkCookie> result = jar.allCookies();
    foreach (QNetworkCookie cookie, expectedResult) {
        QVERIFY2(result.contains(cookie), cookie.toRawForm());
        result.removeAll(cookie);
    }
    QVERIFY2(result.isEmpty(), QTest::toString(result));
}

static bool findCookieName(const QList<QNetworkCookie> &cookieList, const QString &name)
{
    foreach(QNetworkCookie cookie, cookieList)
        if (cookie.name() == name)
            return true;
    return false;
}

void tst_QNetworkCookieJar::setCookiesFromUrl_50CookiesLimitPerDomain()
{
    QNetworkCookie cookie;
    cookie.setValue("value");
    MyCookieJar jar;
    QUrl url("http://a.b.c.com");

    for (int i = 0; i < 20; ++i) {
        // Add a list of 3 domain-matched cookies on each iteration for a total of 60 cookies.
        QList<QNetworkCookie> cookieList;
        cookie.setName(QString("CookieNo%1").arg(i*3+1).toAscii());
        cookie.setDomain("a.b.c.com");
        cookieList += cookie;
        cookie.setName(QString("CookieNo%1").arg(i*3+2).toAscii());
        cookie.setDomain(".b.c.com");
        cookieList += cookie;
        cookie.setName(QString("CookieNo%1").arg(i*3+3).toAscii());
        cookie.setDomain(".c.com");
        cookieList += cookie;
        jar.setCookiesFromUrl(cookieList, url);

        int expectedNumCookies = std::min((i+1)*3, 50);
        QCOMPARE(jar.allCookies().size(), expectedNumCookies);
    }

    // Verify that the oldest cookies were the ones overwritten.
    QVERIFY(!findCookieName(jar.allCookies(), "CookieNo1"));
    QVERIFY(!findCookieName(jar.allCookies(), "CookieNo10"));
    QVERIFY(findCookieName(jar.allCookies(), "CookieNo11"));
    QVERIFY(findCookieName(jar.allCookies(), "CookieNo60"));
}

void tst_QNetworkCookieJar::cookiesForUrl_data()
{
    QTest::addColumn<QList<QNetworkCookie> >("allCookies");
    QTest::addColumn<QString>("url");
    QTest::addColumn<QList<QNetworkCookie> >("expectedResult");

    QList<QNetworkCookie> allCookies;
    QList<QNetworkCookie> result;

    QTest::newRow("no-cookies") << allCookies << "http://foo.bar/" << result;

    QNetworkCookie cookie;
    cookie.setName("a");
    cookie.setPath("/web");
    cookie.setDomain(".nokia.com");
    allCookies += cookie;

    QTest::newRow("no-match-1") << allCookies << "http://foo.bar/" << result;
    QTest::newRow("no-match-2") << allCookies << "http://foo.bar/web" << result;
    QTest::newRow("no-match-3") << allCookies << "http://foo.bar/web/wiki" << result;
    QTest::newRow("no-match-4") << allCookies << "http://nokia.com" << result;
    QTest::newRow("no-match-5") << allCookies << "http://qt.nokia.com" << result;
    QTest::newRow("no-match-6") << allCookies << "http://nokia.com/webinar" << result;
    QTest::newRow("no-match-7") << allCookies << "http://qt.nokia.com/webinar" << result;

    result = allCookies;
    QTest::newRow("match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("match-3") << allCookies << "http://nokia.com/web/content" << result;
    QTest::newRow("match-4") << allCookies << "http://qt.nokia.com/web" << result;
    QTest::newRow("match-4") << allCookies << "http://qt.nokia.com/web/" << result;
    QTest::newRow("match-6") << allCookies << "http://qt.nokia.com/web/content" << result;

    cookie.setPath("/web/wiki");
    allCookies += cookie;

    // exact same results as before:
    QTest::newRow("one-match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("one-match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("one-match-3") << allCookies << "http://nokia.com/web/content" << result;
    QTest::newRow("one-match-4") << allCookies << "http://qt.nokia.com/web" << result;
    QTest::newRow("one-match-4") << allCookies << "http://qt.nokia.com/web/" << result;
    QTest::newRow("one-match-6") << allCookies << "http://qt.nokia.com/web/content" << result;

    result.prepend(cookie);     // longer path, it must match first
    QTest::newRow("two-matches-1") << allCookies << "http://nokia.com/web/wiki" << result;
    QTest::newRow("two-matches-2") << allCookies << "http://qt.nokia.com/web/wiki" << result;

    // invert the order;
    allCookies.clear();
    allCookies << result.at(1) << result.at(0);
    QTest::newRow("two-matches-3") << allCookies << "http://nokia.com/web/wiki" << result;
    QTest::newRow("two-matches-4") << allCookies << "http://qt.nokia.com/web/wiki" << result;

    // expired cookie
    allCookies.clear();
    cookie.setExpirationDate(QDateTime::fromString("09-Nov-1999", "dd-MMM-yyyy"));
    allCookies += cookie;
    result.clear();
    QTest::newRow("exp-match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("exp-match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("exp-match-3") << allCookies << "http://nokia.com/web/content" << result;
    QTest::newRow("exp-match-4") << allCookies << "http://qt.nokia.com/web" << result;
    QTest::newRow("exp-match-4") << allCookies << "http://qt.nokia.com/web/" << result;
    QTest::newRow("exp-match-6") << allCookies << "http://qt.nokia.com/web/content" << result;

    // path matching
    allCookies.clear();
    QNetworkCookie anotherCookie;
    anotherCookie.setName("a");
    anotherCookie.setPath("/web");
    anotherCookie.setDomain(".nokia.com");
    allCookies += anotherCookie;
    result.clear();
    QTest::newRow("path-unmatch-1") << allCookies << "http://nokia.com/" << result;
    QTest::newRow("path-unmatch-2") << allCookies << "http://nokia.com/something/else" << result;
    result += anotherCookie;
    QTest::newRow("path-match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("path-match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("path-match-3") << allCookies << "http://nokia.com/web/content" << result;

    // secure cookies
    allCookies.clear();
    result.clear();
    QNetworkCookie secureCookie;
    secureCookie.setName("a");
    secureCookie.setPath("/web");
    secureCookie.setDomain(".nokia.com");
    secureCookie.setSecure(true);
    allCookies += secureCookie;
    QTest::newRow("no-match-secure-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("no-match-secure-2") << allCookies << "http://qt.nokia.com/web" << result;
    result += secureCookie;
    QTest::newRow("match-secure-1") << allCookies << "https://nokia.com/web" << result;
    QTest::newRow("match-secure-2") << allCookies << "https://qt.nokia.com/web" << result;

}

void tst_QNetworkCookieJar::cookiesForUrl()
{
    QFETCH(QList<QNetworkCookie>, allCookies);
    QFETCH(QString, url);
    QFETCH(QList<QNetworkCookie>, expectedResult);

    MyCookieJar jar;
    jar.setAllCookies(allCookies);

    QList<QNetworkCookie> result = jar.cookiesForUrl(url);
    QCOMPARE(result, expectedResult);
}

void tst_QNetworkCookieJar::effectiveTLDs_data()
{
    QTest::addColumn<QString>("domain");
    QTest::addColumn<bool>("isTLD");

    QTest::newRow("yes1") << "com" << true;
    QTest::newRow("yes2") << "de" << true;
    QTest::newRow("yes3") << "ulm.museum" << true;
    QTest::newRow("yes4") << "krodsherad.no" << true;
    QTest::newRow("yes5") << "1.bg" << true;
    QTest::newRow("yes6") << "com.cn" << true;
    QTest::newRow("yes7") << "org.ws" << true;
    QTest::newRow("yes8") << "co.uk" << true;
    QTest::newRow("yes9") << "wallonie.museum" << true;

    QTest::newRow("no1") << "anything.com" << false;
    QTest::newRow("no2") << "anything.de" << false;
    QTest::newRow("no3") << "eselsberg.ulm.museum" << false;
    QTest::newRow("no4") << "noe.krodsherad.no" << false;
    QTest::newRow("no5") << "2.1.bg" << false;
    QTest::newRow("no6") << "foo.com.cn" << false;
    QTest::newRow("no7") << "something.org.ws" << false;
    QTest::newRow("no8") << "teatime.co.uk" << false;
    QTest::newRow("no9") << "bla" << false;
    QTest::newRow("no10") << "bla.bla" << false;

    const ushort s1[] = {0x74, 0x72, 0x61, 0x6e, 0xf8, 0x79, 0x2e, 0x6e, 0x6f, 0x00}; // xn--trany-yua.no
    const ushort s2[] = {0x5d9, 0x5e8, 0x5d5, 0x5e9, 0x5dc, 0x5d9, 0x5dd, 0x2e, 0x6d, 0x75, 0x73, 0x65, 0x75, 0x6d, 0x00}; // xn--9dbhblg6di.museum
    const ushort s3[] = {0x7ec4, 0x7e54, 0x2e, 0x68, 0x6b, 0x00}; // xn--mk0axi.hk
    const ushort s4[] = {0x7f51, 0x7edc, 0x2e, 0x63, 0x6e, 0x00}; // xn--io0a7i.cn
    const ushort s5[] = {0x72, 0xe1, 0x68, 0x6b, 0x6b, 0x65, 0x72, 0xe1, 0x76, 0x6a, 0x75, 0x2e, 0x6e, 0x6f, 0x00}; // xn--rhkkervju-01af.no
    const ushort s6[] = {0xb9a, 0xbbf, 0xb99, 0xbcd, 0xb95, 0xbaa, 0xbcd, 0xbaa, 0xbc2, 0xbb0, 0xbcd, 0x00}; // xn--clchc0ea0b2g2a9gcd
    const ushort s7[] = {0x627, 0x644, 0x627, 0x631, 0x62f, 0x646, 0x00}; // xn--mgbayh7gpa
    const ushort s8[] = {0x63, 0x6f, 0x72, 0x72, 0x65, 0x69, 0x6f, 0x73, 0x2d, 0x65, 0x2d, 0x74, 0x65, 0x6c, 0x65,
                         0x63, 0x6f, 0x6d, 0x75, 0x6e, 0x69, 0x63, 0x61, 0xe7, 0xf5, 0x65, 0x73, 0x2e, 0x6d, 0x75,
                         0x73, 0x65, 0x75, 0x6d, 0x00}; // xn--correios-e-telecomunicaes-ghc29a.museum
    QTest::newRow("yes-specialchars1") << QString::fromUtf16(s1) << true;
    QTest::newRow("yes-specialchars2") << QString::fromUtf16(s2) << true;
    QTest::newRow("yes-specialchars3") << QString::fromUtf16(s3) << true;
    QTest::newRow("yes-specialchars4") << QString::fromUtf16(s4) << true;
    QTest::newRow("yes-specialchars5") << QString::fromUtf16(s5) << true;
    QTest::newRow("yes-specialchars6") << QString::fromUtf16(s6) << true;
    QTest::newRow("yes-specialchars7") << QString::fromUtf16(s7) << true;
    QTest::newRow("yes-specialchars8") << QString::fromUtf16(s8) << true;

    QTest::newRow("no-specialchars1") << QString::fromUtf16(s1).prepend("something") << false;
    QTest::newRow("no-specialchars2") << QString::fromUtf16(s2).prepend(QString::fromUtf16(s2)) << false;
    QTest::newRow("no-specialchars2.5") << QString::fromUtf16(s2).prepend("whatever") << false;
    QTest::newRow("no-specialchars3") << QString::fromUtf16(s3).prepend("foo") << false;
    QTest::newRow("no-specialchars4") << QString::fromUtf16(s4).prepend("bar") << false;
    QTest::newRow("no-specialchars5") << QString::fromUtf16(s5).prepend(QString::fromUtf16(s2)) << false;
    QTest::newRow("no-specialchars6") << QString::fromUtf16(s6).prepend(QLatin1Char('.') + QString::fromUtf16(s6)) << false;
    QTest::newRow("no-specialchars7") << QString::fromUtf16(s7).prepend("bla") << false;
    QTest::newRow("no-specialchars8") << QString::fromUtf16(s8).append("foo") << false;

    QTest::newRow("exception1") << "pref.iwate.jp" << false;
    QTest::newRow("exception2") << "omanpost.om" << false;
    QTest::newRow("exception3") << "omantel.om" << false;
    QTest::newRow("exception4") << "gobiernoelectronico.ar" << false;
    QTest::newRow("exception5") << "pref.ishikawa.jp" << false;

    QTest::newRow("yes-wildcard1") << "*.jm" << true;
    QTest::newRow("yes-wildcard1.5") << "anything.jm" << true;
    QTest::newRow("yes-wildcard2") << "something.kh" << true;
    QTest::newRow("yes-wildcard3") << "whatever.uk" << true;
    QTest::newRow("yes-wildcard4") << "anything.shizuoka.jp" << true;
    QTest::newRow("yes-wildcard5") << "foo.sch.uk" << true;
}

void tst_QNetworkCookieJar::effectiveTLDs()
{
#ifndef QT_BUILD_INTERNAL
    QSKIP("Test requires private API", SkipAll);
#endif
    QFETCH(QString, domain);
    QFETCH(bool, isTLD);
    QCOMPARE(qIsEffectiveTLD(domain), isTLD);
}

QTEST_MAIN(tst_QNetworkCookieJar)
#include "tst_qnetworkcookiejar.moc"

