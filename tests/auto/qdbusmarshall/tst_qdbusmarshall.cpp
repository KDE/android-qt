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
#include <QtCore/QtCore>
#include <QtTest/QtTest>
#include <QtDBus/QtDBus>
#include <QtDBus/private/qdbusutil_p.h>
#include <QtDBus/private/qdbusconnection_p.h>

#include "common.h"
#include <limits>

#include <dbus/dbus.h>

static const char serviceName[] = "com.trolltech.autotests.qpong";
static const char objectPath[] = "/com/trolltech/qpong";
static const char *interfaceName = serviceName;

class tst_QDBusMarshall: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void sendBasic_data();
    void sendBasic();

    void sendVariant_data();
    void sendVariant();

    void sendArrays_data();
    void sendArrays();

    void sendArrayOfArrays_data();
    void sendArrayOfArrays();

    void sendMaps_data();
    void sendMaps();

    void sendStructs_data();
    void sendStructs();

    void sendComplex_data();
    void sendComplex();

    void sendArgument_data();
    void sendArgument();

    void sendSignalErrors();
    void sendCallErrors_data();
    void sendCallErrors();

    void receiveUnknownType_data();
    void receiveUnknownType();

private:
    int fileDescriptorForTest();

    QProcess proc;
    QTemporaryFile tempFile;
    bool fileDescriptorPassing;
};

class QDBusMessageSpy: public QObject
{
    Q_OBJECT
public slots:
    Q_SCRIPTABLE int theSlot(const QDBusMessage &msg)
    {
        list << msg;
        return 42;
    }
public:
    QList<QDBusMessage> list;
};

struct UnregisteredType { };
Q_DECLARE_METATYPE(UnregisteredType)

void tst_QDBusMarshall::initTestCase()
{
    commonInit();
    QDBusConnection con = QDBusConnection::sessionBus();
    fileDescriptorPassing = con.connectionCapabilities() & QDBusConnection::UnixFileDescriptorPassing;
#ifdef Q_OS_WIN
    proc.start("qpong");
#else
    proc.start("./qpong/qpong");
#endif
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(serviceName)) {
        QVERIFY(proc.waitForStarted());

        QVERIFY(con.isConnected());
        con.connect("org.freedesktop.DBus", QString(), "org.freedesktop.DBus", "NameOwnerChanged",
                    QStringList() << serviceName << QString(""), QString(),
                    &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(2);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(QDBusConnection::sessionBus().interface()->isServiceRegistered(serviceName));
        con.disconnect("org.freedesktop.DBus", QString(), "org.freedesktop.DBus", "NameOwnerChanged",
                       QStringList() << serviceName << QString(""), QString(),
                       &QTestEventLoop::instance(), SLOT(exitLoop()));
    }
}

void tst_QDBusMarshall::cleanupTestCase()
{
    proc.close();
    proc.terminate();
    proc.waitForFinished(200);
}

int tst_QDBusMarshall::fileDescriptorForTest()
{
    if (!tempFile.isOpen()) {
        tempFile.setFileTemplate(QDir::tempPath() + "/qdbusmarshalltestXXXXXX.tmp");
        tempFile.open();
    }
    return tempFile.handle();
}

void tst_QDBusMarshall::sendBasic_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<QString>("stringResult");

    // basic types:
    QTest::newRow("bool") << QVariant(false) << "b" << "false";
#if 1
    QTest::newRow("bool2") << QVariant(true) << "b" << "true";
    QTest::newRow("byte") << qVariantFromValue(uchar(1)) << "y" << "1";
    QTest::newRow("int16") << qVariantFromValue(short(2)) << "n" << "2";
    QTest::newRow("uint16") << qVariantFromValue(ushort(3)) << "q" << "3";
    QTest::newRow("int") << QVariant(1) << "i" << "1";
    QTest::newRow("uint") << QVariant(2U) << "u" << "2";
    QTest::newRow("int64") << QVariant(Q_INT64_C(3)) << "x" << "3";
    QTest::newRow("uint64") << QVariant(Q_UINT64_C(4)) << "t" << "4";
    QTest::newRow("double") << QVariant(42.5) << "d" << "42.5";
    QTest::newRow("string") << QVariant("ping") << "s" << "\"ping\"";
    QTest::newRow("objectpath") << qVariantFromValue(QDBusObjectPath("/org/kde")) << "o" << "[ObjectPath: /org/kde]";
    QTest::newRow("signature") << qVariantFromValue(QDBusSignature("g")) << "g" << "[Signature: g]";
    QTest::newRow("emptystring") << QVariant("") << "s" << "\"\"";
    QTest::newRow("nullstring") << QVariant(QString()) << "s" << "\"\"";

    if (fileDescriptorPassing)
        QTest::newRow("file-descriptor") << qVariantFromValue(QDBusUnixFileDescriptor(fileDescriptorForTest())) << "h" << "[Unix FD: valid]";
#endif
}

void tst_QDBusMarshall::sendVariant_data()
{
    sendBasic_data();

    QTest::newRow("variant") << qVariantFromValue(QDBusVariant(1)) << "v" << "[Variant(int): 1]";

    QDBusVariant nested(1);
    QTest::newRow("variant-variant") << qVariantFromValue(QDBusVariant(qVariantFromValue(nested))) << "v"
                                     << "[Variant(QDBusVariant): [Variant(int): 1]]";
}

void tst_QDBusMarshall::sendArrays_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<QString>("stringResult");

    // arrays
    QStringList strings;
    QTest::newRow("emptystringlist") << QVariant(strings) << "as" << "{}";
    strings << "hello" << "world";
    QTest::newRow("stringlist") << QVariant(strings) << "as" << "{\"hello\", \"world\"}";

    strings.clear();
    strings << "" << "" << "";
    QTest::newRow("list-of-emptystrings") << QVariant(strings) << "as" << "{\"\", \"\", \"\"}";

    strings.clear();
    strings << QString() << QString() << QString() << QString();
    QTest::newRow("list-of-nullstrings") << QVariant(strings) << "as" << "{\"\", \"\", \"\", \"\"}";

    QByteArray bytearray;
    QTest::newRow("nullbytearray") << QVariant(bytearray) << "ay" << "{}";
    bytearray = "";             // empty, not null
    QTest::newRow("emptybytearray") << QVariant(bytearray) << "ay" << "{}";
    bytearray = "foo";
    QTest::newRow("bytearray") << QVariant(bytearray) << "ay" << "{102, 111, 111}";

    QList<bool> bools;
    QTest::newRow("emptyboollist") << qVariantFromValue(bools) << "ab" << "[Argument: ab {}]";
    bools << false << true << false;
    QTest::newRow("boollist") << qVariantFromValue(bools) << "ab" << "[Argument: ab {false, true, false}]";

    QList<short> shorts;
    QTest::newRow("emptyshortlist") << qVariantFromValue(shorts) << "an" << "[Argument: an {}]";
    shorts << 42 << -43 << 44 << 45 << -32768 << 32767;
    QTest::newRow("shortlist") << qVariantFromValue(shorts) << "an"
                               << "[Argument: an {42, -43, 44, 45, -32768, 32767}]";

    QList<ushort> ushorts;
    QTest::newRow("emptyushortlist") << qVariantFromValue(ushorts) << "aq" << "[Argument: aq {}]";
    ushorts << 12u << 13u << 14u << 15 << 65535;
    QTest::newRow("ushortlist") << qVariantFromValue(ushorts) << "aq" << "[Argument: aq {12, 13, 14, 15, 65535}]";

    QList<int> ints;
    QTest::newRow("emptyintlist") << qVariantFromValue(ints) << "ai" << "[Argument: ai {}]";
    ints << 42 << -43 << 44 << 45 << 2147483647 << -2147483647-1;
    QTest::newRow("intlist") << qVariantFromValue(ints) << "ai" << "[Argument: ai {42, -43, 44, 45, 2147483647, -2147483648}]";

    QList<uint> uints;
    QTest::newRow("emptyuintlist") << qVariantFromValue(uints) << "au" << "[Argument: au {}]";
    uints << uint(12) << uint(13) << uint(14) << 4294967295U;
    QTest::newRow("uintlist") << qVariantFromValue(uints) << "au" << "[Argument: au {12, 13, 14, 4294967295}]";

    QList<qlonglong> llints;
    QTest::newRow("emptyllintlist") << qVariantFromValue(llints) << "ax" << "[Argument: ax {}]";
    llints << Q_INT64_C(99) << Q_INT64_C(-100)
           << Q_INT64_C(-9223372036854775807)-1 << Q_INT64_C(9223372036854775807);
    QTest::newRow("llintlist") << qVariantFromValue(llints) << "ax"
                               << "[Argument: ax {99, -100, -9223372036854775808, 9223372036854775807}]";

    QList<qulonglong> ullints;
    QTest::newRow("emptyullintlist") << qVariantFromValue(ullints) << "at" << "[Argument: at {}]";
    ullints << Q_UINT64_C(66) << Q_UINT64_C(67)
            << Q_UINT64_C(18446744073709551615);
    QTest::newRow("ullintlist") << qVariantFromValue(ullints) << "at" << "[Argument: at {66, 67, 18446744073709551615}]";

    QList<double> doubles;
    QTest::newRow("emptydoublelist") << qVariantFromValue(doubles) << "ad" << "[Argument: ad {}]";
    doubles << 1.2 << 2.2 << 4.4
            << -std::numeric_limits<double>::infinity()
            << std::numeric_limits<double>::infinity()
            << std::numeric_limits<double>::quiet_NaN();
    QTest::newRow("doublelist") << qVariantFromValue(doubles) << "ad" << "[Argument: ad {1.2, 2.2, 4.4, -inf, inf, nan}]";

    QList<QDBusObjectPath> objectPaths;
    QTest::newRow("emptyobjectpathlist") << qVariantFromValue(objectPaths) << "ao" << "[Argument: ao {}]";
    objectPaths << QDBusObjectPath("/") << QDBusObjectPath("/foo");
    QTest::newRow("objectpathlist") << qVariantFromValue(objectPaths) << "ao" << "[Argument: ao {[ObjectPath: /], [ObjectPath: /foo]}]";

    if (fileDescriptorPassing) {
        QList<QDBusUnixFileDescriptor> fileDescriptors;
        QTest::newRow("emptyfiledescriptorlist") << qVariantFromValue(fileDescriptors) << "ah" << "[Argument: ah {}]";
        fileDescriptors << QDBusUnixFileDescriptor(fileDescriptorForTest()) << QDBusUnixFileDescriptor(1);
        QTest::newRow("filedescriptorlist") << qVariantFromValue(fileDescriptors) << "ah" << "[Argument: ah {[Unix FD: valid], [Unix FD: valid]}]";
    }

    QVariantList variants;
    QTest::newRow("emptyvariantlist") << QVariant(variants) << "av" << "[Argument: av {}]";
    variants << QString("Hello") << QByteArray("World") << 42 << -43.0 << 44U << Q_INT64_C(-45)
             << Q_UINT64_C(46) << true << qVariantFromValue(short(-47))
             << qVariantFromValue(QDBusSignature("av"))
             << qVariantFromValue(QDBusVariant(qVariantFromValue(QDBusObjectPath("/"))));
    QTest::newRow("variantlist") << QVariant(variants) << "av"
        << "[Argument: av {[Variant(QString): \"Hello\"], [Variant(QByteArray): {87, 111, 114, 108, 100}], [Variant(int): 42], [Variant(double): -43], [Variant(uint): 44], [Variant(qlonglong): -45], [Variant(qulonglong): 46], [Variant(bool): true], [Variant(short): -47], [Variant: [Signature: av]], [Variant: [Variant: [ObjectPath: /]]]}]";
}

void tst_QDBusMarshall::sendArrayOfArrays_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<QString>("stringResult");

    // arrays:
    QList<QStringList> strings;
    QTest::newRow("empty-list-of-stringlist") << qVariantFromValue(strings) << "aas"
            << "[Argument: aas {}]";
    strings << QStringList();
    QTest::newRow("list-of-emptystringlist") << qVariantFromValue(strings) << "aas"
            << "[Argument: aas {{}}]";
    strings << (QStringList() << "hello" << "world")
            << (QStringList() << "hi" << "there")
            << (QStringList() << QString());
    QTest::newRow("stringlist") << qVariantFromValue(strings) << "aas"
            << "[Argument: aas {{}, {\"hello\", \"world\"}, {\"hi\", \"there\"}, {\"\"}}]";

    QList<QByteArray> bytearray;
    QTest::newRow("empty-list-of-bytearray") << qVariantFromValue(bytearray) << "aay"
            << "[Argument: aay {}]";
    bytearray << QByteArray();
    QTest::newRow("list-of-emptybytearray") << qVariantFromValue(bytearray) << "aay"
            << "[Argument: aay {{}}]";
    bytearray << "foo" << "bar" << "baz" << "" << QByteArray();
    QTest::newRow("bytearray") << qVariantFromValue(bytearray) << "aay"
            << "[Argument: aay {{}, {102, 111, 111}, {98, 97, 114}, {98, 97, 122}, {}, {}}]";

    QList<QList<bool> > bools;
    QTest::newRow("empty-list-of-boollist") << qVariantFromValue(bools) << "aab"
            << "[Argument: aab {}]";
    bools << QList<bool>();
    QTest::newRow("list-of-emptyboollist") << qVariantFromValue(bools) << "aab"
            << "[Argument: aab {[Argument: ab {}]}]";
    bools << (QList<bool>() << false << true) << (QList<bool>() << false) << (QList<bool>());
    QTest::newRow("boollist") << qVariantFromValue(bools) << "aab"
            << "[Argument: aab {[Argument: ab {}], [Argument: ab {false, true}], [Argument: ab {false}], [Argument: ab {}]}]";
    QList<QList<short> > shorts;
    QTest::newRow("empty-list-of-shortlist") << qVariantFromValue(shorts) << "aan"
            << "[Argument: aan {}]";
    shorts << QList<short>();
    QTest::newRow("list-of-emptyshortlist") << qVariantFromValue(shorts) << "aan"
            << "[Argument: aan {[Argument: an {}]}]";
    shorts << (QList<short>() << 42 << -43 << 44 << 45)
           << (QList<short>() << -32768 << 32767)
           << (QList<short>());
    QTest::newRow("shortlist") << qVariantFromValue(shorts) << "aan"
            << "[Argument: aan {[Argument: an {}], [Argument: an {42, -43, 44, 45}], [Argument: an {-32768, 32767}], [Argument: an {}]}]";

    QList<QList<ushort> > ushorts;
    QTest::newRow("empty-list-of-ushortlist") << qVariantFromValue(ushorts) << "aaq"
            << "[Argument: aaq {}]";
    ushorts << QList<ushort>();
    QTest::newRow("list-of-emptyushortlist") << qVariantFromValue(ushorts) << "aaq"
            << "[Argument: aaq {[Argument: aq {}]}]";
    ushorts << (QList<ushort>() << 12u << 13u << 14u << 15)
            << (QList<ushort>() << 65535)
            << (QList<ushort>());
    QTest::newRow("ushortlist") << qVariantFromValue(ushorts) << "aaq"
            << "[Argument: aaq {[Argument: aq {}], [Argument: aq {12, 13, 14, 15}], [Argument: aq {65535}], [Argument: aq {}]}]";

    QList<QList<int> > ints;
    QTest::newRow("empty-list-of-intlist") << qVariantFromValue(ints) << "aai"
            << "[Argument: aai {}]";
    ints << QList<int>();
    QTest::newRow("list-of-emptyintlist") << qVariantFromValue(ints) << "aai"
            << "[Argument: aai {[Argument: ai {}]}]";
    ints << (QList<int>() << 42 << -43 << 44 << 45)
         << (QList<int>() << 2147483647 << -2147483647-1)
         << (QList<int>());
    QTest::newRow("intlist") << qVariantFromValue(ints) << "aai"
            << "[Argument: aai {[Argument: ai {}], [Argument: ai {42, -43, 44, 45}], [Argument: ai {2147483647, -2147483648}], [Argument: ai {}]}]";

    QList<QList<uint> > uints;
    QTest::newRow("empty-list-of-uintlist") << qVariantFromValue(uints) << "aau"
            << "[Argument: aau {}]";
    uints << QList<uint>();
    QTest::newRow("list-of-emptyuintlist") << qVariantFromValue(uints) << "aau"
            << "[Argument: aau {[Argument: au {}]}]";
    uints << (QList<uint>() << uint(12) << uint(13) << uint(14))
          << (QList<uint>() << 4294967295U)
          << (QList<uint>());
    QTest::newRow("uintlist") << qVariantFromValue(uints) << "aau"
            << "[Argument: aau {[Argument: au {}], [Argument: au {12, 13, 14}], [Argument: au {4294967295}], [Argument: au {}]}]";

    QList<QList<qlonglong> > llints;
    QTest::newRow("empty-list-of-llintlist") << qVariantFromValue(llints) << "aax"
            << "[Argument: aax {}]";
    llints << QList<qlonglong>();
    QTest::newRow("list-of-emptyllintlist") << qVariantFromValue(llints) << "aax"
            << "[Argument: aax {[Argument: ax {}]}]";
    llints << (QList<qlonglong>() << Q_INT64_C(99) << Q_INT64_C(-100))
           << (QList<qlonglong>() << Q_INT64_C(-9223372036854775807)-1 << Q_INT64_C(9223372036854775807))
           << (QList<qlonglong>());
    QTest::newRow("llintlist") << qVariantFromValue(llints) << "aax"
            << "[Argument: aax {[Argument: ax {}], [Argument: ax {99, -100}], [Argument: ax {-9223372036854775808, 9223372036854775807}], [Argument: ax {}]}]";

    QList<QList<qulonglong> > ullints;
    QTest::newRow("empty-list-of-ullintlist") << qVariantFromValue(ullints) << "aat"
            << "[Argument: aat {}]";
    ullints << QList<qulonglong>();
    QTest::newRow("list-of-emptyullintlist") << qVariantFromValue(ullints) << "aat"
            << "[Argument: aat {[Argument: at {}]}]";
    ullints << (QList<qulonglong>() << Q_UINT64_C(66) << Q_UINT64_C(67))
            << (QList<qulonglong>() << Q_UINT64_C(18446744073709551615))
            << (QList<qulonglong>());
    QTest::newRow("ullintlist") << qVariantFromValue(ullints) << "aat"
            << "[Argument: aat {[Argument: at {}], [Argument: at {66, 67}], [Argument: at {18446744073709551615}], [Argument: at {}]}]";

    QList<QList<double> > doubles;
    QTest::newRow("empty-list-ofdoublelist") << qVariantFromValue(doubles) << "aad"
            << "[Argument: aad {}]";
    doubles << QList<double>();
    QTest::newRow("list-of-emptydoublelist") << qVariantFromValue(doubles) << "aad"
            << "[Argument: aad {[Argument: ad {}]}]";
    doubles << (QList<double>() << 1.2 << 2.2 << 4.4)
            << (QList<double>() << -std::numeric_limits<double>::infinity()
                << std::numeric_limits<double>::infinity()
                << std::numeric_limits<double>::quiet_NaN())
            << (QList<double>());
    QTest::newRow("doublelist") << qVariantFromValue(doubles) << "aad"
            << "[Argument: aad {[Argument: ad {}], [Argument: ad {1.2, 2.2, 4.4}], [Argument: ad {-inf, inf, nan}], [Argument: ad {}]}]";

    QList<QVariantList> variants;
    QTest::newRow("emptyvariantlist") << qVariantFromValue(variants) << "aav"
            << "[Argument: aav {}]";
    variants << QVariantList();
    QTest::newRow("emptyvariantlist") << qVariantFromValue(variants) << "aav"
            << "[Argument: aav {[Argument: av {}]}]";
    variants << (QVariantList() << QString("Hello") << QByteArray("World"))
             << (QVariantList() << 42 << -43.0 << 44U << Q_INT64_C(-45))
             << (QVariantList() << Q_UINT64_C(46) << true << qVariantFromValue(short(-47)));
    QTest::newRow("variantlist") << qVariantFromValue(variants) << "aav"
            << "[Argument: aav {[Argument: av {}], [Argument: av {[Variant(QString): \"Hello\"], [Variant(QByteArray): {87, 111, 114, 108, 100}]}], [Argument: av {[Variant(int): 42], [Variant(double): -43], [Variant(uint): 44], [Variant(qlonglong): -45]}], [Argument: av {[Variant(qulonglong): 46], [Variant(bool): true], [Variant(short): -47]}]}]";
}

void tst_QDBusMarshall::sendMaps_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<QString>("stringResult");

    QMap<int, QString> ismap;
    QTest::newRow("empty-is-map") << qVariantFromValue(ismap) << "a{is}"
            << "[Argument: a{is} {}]";
    ismap[1] = "a";
    ismap[2000] = "b";
    ismap[-47] = "c";
    QTest::newRow("is-map") << qVariantFromValue(ismap) << "a{is}"
            << "[Argument: a{is} {-47 = \"c\", 1 = \"a\", 2000 = \"b\"}]";

    QMap<QString, QString> ssmap;
    QTest::newRow("empty-ss-map") << qVariantFromValue(ssmap) << "a{ss}"
            << "[Argument: a{ss} {}]";
    ssmap["a"] = "a";
    ssmap["c"] = "b";
    ssmap["b"] = "c";
    QTest::newRow("ss-map") << qVariantFromValue(ssmap) << "a{ss}"
            << "[Argument: a{ss} {\"a\" = \"a\", \"b\" = \"c\", \"c\" = \"b\"}]";

    QVariantMap svmap;
    QTest::newRow("empty-sv-map") << qVariantFromValue(svmap) << "a{sv}"
            << "[Argument: a{sv} {}]";
    svmap["a"] = 1;
    svmap["c"] = "b";
    svmap["b"] = QByteArray("c");
    svmap["d"] = 42U;
    svmap["e"] = qVariantFromValue(short(-47));
    svmap["f"] = qVariantFromValue(QDBusVariant(0));
    QTest::newRow("sv-map1") << qVariantFromValue(svmap) << "a{sv}"
            << "[Argument: a{sv} {\"a\" = [Variant(int): 1], \"b\" = [Variant(QByteArray): {99}], \"c\" = [Variant(QString): \"b\"], \"d\" = [Variant(uint): 42], \"e\" = [Variant(short): -47], \"f\" = [Variant: [Variant(int): 0]]}]";

    QMap<QDBusObjectPath, QString> osmap;
    QTest::newRow("empty-os-map") << qVariantFromValue(osmap) << "a{os}"
            << "[Argument: a{os} {}]";
    osmap[QDBusObjectPath("/")] = "root";
    osmap[QDBusObjectPath("/foo")] = "foo";
    osmap[QDBusObjectPath("/bar/baz")] = "bar and baz";
    QTest::newRow("os-map") << qVariantFromValue(osmap) << "a{os}"
            << "[Argument: a{os} {[ObjectPath: /] = \"root\", [ObjectPath: /bar/baz] = \"bar and baz\", [ObjectPath: /foo] = \"foo\"}]";

    QHash<QDBusSignature, QString> gsmap;
    QTest::newRow("empty-gs-map") << qVariantFromValue(gsmap) << "a{gs}"
            << "[Argument: a{gs} {}]";
    gsmap[QDBusSignature("i")] = "int32";
    gsmap[QDBusSignature("s")] = "string";
    gsmap[QDBusSignature("a{gs}")] = "array of dict_entry of (signature, string)";
    QTest::newRow("gs-map") << qVariantFromValue(gsmap) << "a{gs}"
            << "[Argument: a{gs} {[Signature: a{gs}] = \"array of dict_entry of (signature, string)\", [Signature: i] = \"int32\", [Signature: s] = \"string\"}]";

    if (fileDescriptorPassing) {
        svmap["zzfiledescriptor"] = qVariantFromValue(QDBusUnixFileDescriptor(fileDescriptorForTest()));
        QTest::newRow("sv-map1-fd") << qVariantFromValue(svmap) << "a{sv}"
                                    << "[Argument: a{sv} {\"a\" = [Variant(int): 1], \"b\" = [Variant(QByteArray): {99}], \"c\" = [Variant(QString): \"b\"], \"d\" = [Variant(uint): 42], \"e\" = [Variant(short): -47], \"f\" = [Variant: [Variant(int): 0]], \"zzfiledescriptor\" = [Variant(QDBusUnixFileDescriptor): [Unix FD: valid]]}]";
    }

    svmap.clear();
    svmap["ismap"] = qVariantFromValue(ismap);
    svmap["ssmap"] = qVariantFromValue(ssmap);
    svmap["osmap"] = qVariantFromValue(osmap);
    svmap["gsmap"] = qVariantFromValue(gsmap);
    QTest::newRow("sv-map2") << qVariantFromValue(svmap) << "a{sv}"
            << "[Argument: a{sv} {\"gsmap\" = [Variant: [Argument: a{gs} {[Signature: a{gs}] = \"array of dict_entry of (signature, string)\", [Signature: i] = \"int32\", [Signature: s] = \"string\"}]], \"ismap\" = [Variant: [Argument: a{is} {-47 = \"c\", 1 = \"a\", 2000 = \"b\"}]], \"osmap\" = [Variant: [Argument: a{os} {[ObjectPath: /] = \"root\", [ObjectPath: /bar/baz] = \"bar and baz\", [ObjectPath: /foo] = \"foo\"}]], \"ssmap\" = [Variant: [Argument: a{ss} {\"a\" = \"a\", \"b\" = \"c\", \"c\" = \"b\"}]]}]";
}

void tst_QDBusMarshall::sendStructs_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<QString>("stringResult");

    QTest::newRow("point") << QVariant(QPoint(1, 2)) << "(ii)" << "[Argument: (ii) 1, 2]";
    QTest::newRow("pointf") << QVariant(QPointF(1.5, -1.5)) << "(dd)" << "[Argument: (dd) 1.5, -1.5]";

    QTest::newRow("size") << QVariant(QSize(1, 2)) << "(ii)" << "[Argument: (ii) 1, 2]";
    QTest::newRow("sizef") << QVariant(QSizeF(1.5, 1.5)) << "(dd)" << "[Argument: (dd) 1.5, 1.5]";

    QTest::newRow("rect") << QVariant(QRect(1, 2, 3, 4)) << "(iiii)" << "[Argument: (iiii) 1, 2, 3, 4]";
    QTest::newRow("rectf") << QVariant(QRectF(0.5, 0.5, 1.5, 1.5)) << "(dddd)" << "[Argument: (dddd) 0.5, 0.5, 1.5, 1.5]";

    QTest::newRow("line") << QVariant(QLine(1, 2, 3, 4)) << "((ii)(ii))"
                          << "[Argument: ((ii)(ii)) [Argument: (ii) 1, 2], [Argument: (ii) 3, 4]]";
    QTest::newRow("linef") << QVariant(QLineF(0.5, 0.5, 1.5, 1.5)) << "((dd)(dd))"
                           << "[Argument: ((dd)(dd)) [Argument: (dd) 0.5, 0.5], [Argument: (dd) 1.5, 1.5]]";

    QDate date(2006, 6, 18);
    QTime time(12, 25, 00);     // the date I wrote this test on :-)
    QTest::newRow("date") << QVariant(date) << "(iii)" << "[Argument: (iii) 2006, 6, 18]";
    QTest::newRow("time") << QVariant(time) << "(iiii)" << "[Argument: (iiii) 12, 25, 0, 0]";
    QTest::newRow("datetime") << QVariant(QDateTime(date, time)) << "((iii)(iiii)i)"
        << "[Argument: ((iii)(iiii)i) [Argument: (iii) 2006, 6, 18], [Argument: (iiii) 12, 25, 0, 0], 0]";

    MyStruct ms = { 1, "Hello, World" };
    QTest::newRow("int-string") << qVariantFromValue(ms) << "(is)" << "[Argument: (is) 1, \"Hello, World\"]";

    MyVariantMapStruct mvms = { "Hello, World", QVariantMap() };
    QTest::newRow("string-variantmap") << qVariantFromValue(mvms) << "(sa{sv})" << "[Argument: (sa{sv}) \"Hello, World\", [Argument: a{sv} {}]]";

    // use only basic types, otherwise comparison will fail
    mvms.map["int"] = 42;
    mvms.map["uint"] = 42u;
    mvms.map["short"] = qVariantFromValue<short>(-47);
    mvms.map["bytearray"] = QByteArray("Hello, world");
    QTest::newRow("string-variantmap2") << qVariantFromValue(mvms) << "(sa{sv})" << "[Argument: (sa{sv}) \"Hello, World\", [Argument: a{sv} {\"bytearray\" = [Variant(QByteArray): {72, 101, 108, 108, 111, 44, 32, 119, 111, 114, 108, 100}], \"int\" = [Variant(int): 42], \"short\" = [Variant(short): -47], \"uint\" = [Variant(uint): 42]}]]";

    QList<MyVariantMapStruct> list;
    QTest::newRow("empty-list-of-string-variantmap") << qVariantFromValue(list) << "a(sa{sv})" << "[Argument: a(sa{sv}) {}]";
    list << mvms;
    QTest::newRow("list-of-string-variantmap") << qVariantFromValue(list) << "a(sa{sv})" << "[Argument: a(sa{sv}) {[Argument: (sa{sv}) \"Hello, World\", [Argument: a{sv} {\"bytearray\" = [Variant(QByteArray): {72, 101, 108, 108, 111, 44, 32, 119, 111, 114, 108, 100}], \"int\" = [Variant(int): 42], \"short\" = [Variant(short): -47], \"uint\" = [Variant(uint): 42]}]]}]";

    if (fileDescriptorPassing) {
        MyFileDescriptorStruct fds;
        fds.fd = QDBusUnixFileDescriptor(fileDescriptorForTest());
        QTest::newRow("fdstruct") << qVariantFromValue(fds) << "(h)" << "[Argument: (h) [Unix FD: valid]]";

        QList<MyFileDescriptorStruct> fdlist;
        QTest::newRow("empty-list-of-fdstruct") << qVariantFromValue(fdlist) << "a(h)" << "[Argument: a(h) {}]";

        fdlist << fds;
        QTest::newRow("list-of-fdstruct") << qVariantFromValue(fdlist) << "a(h)" << "[Argument: a(h) {[Argument: (h) [Unix FD: valid]]}]";
    }
}

void tst_QDBusMarshall::sendComplex_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<QString>("stringResult");

    QList<QDateTime> dtlist;
    QTest::newRow("empty-datetimelist") << qVariantFromValue(dtlist) << "a((iii)(iiii)i)"
            << "[Argument: a((iii)(iiii)i) {}]";
    dtlist << QDateTime();
    QTest::newRow("list-of-emptydatetime") << qVariantFromValue(dtlist) << "a((iii)(iiii)i)"
            << "[Argument: a((iii)(iiii)i) {[Argument: ((iii)(iiii)i) [Argument: (iii) 0, 0, 0], [Argument: (iiii) -1, -1, -1, -1], 0]}]";
    dtlist << QDateTime(QDate(1977, 9, 13), QTime(0, 0, 0))
           << QDateTime(QDate(2006, 6, 18), QTime(13, 14, 0));
    QTest::newRow("datetimelist") << qVariantFromValue(dtlist) << "a((iii)(iiii)i)"
            << "[Argument: a((iii)(iiii)i) {[Argument: ((iii)(iiii)i) [Argument: (iii) 0, 0, 0], [Argument: (iiii) -1, -1, -1, -1], 0], [Argument: ((iii)(iiii)i) [Argument: (iii) 1977, 9, 13], [Argument: (iiii) 0, 0, 0, 0], 0], [Argument: ((iii)(iiii)i) [Argument: (iii) 2006, 6, 18], [Argument: (iiii) 13, 14, 0, 0], 0]}]";

    QHash<qlonglong, QDateTime> lldtmap;
    QTest::newRow("empty-lldtmap") << qVariantFromValue(lldtmap) << "a{x((iii)(iiii)i)}"
            << "[Argument: a{x((iii)(iiii)i)} {}]";
    lldtmap[0] = QDateTime();
    lldtmap[1] = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 1), Qt::UTC);
    lldtmap[1150629776] = QDateTime(QDate(2006, 6, 18), QTime(11, 22, 56), Qt::UTC);
    QTest::newRow("lldtmap") << qVariantFromValue(lldtmap) << "a{x((iii)(iiii)i)}"
            << "[Argument: a{x((iii)(iiii)i)} {0 = [Argument: ((iii)(iiii)i) [Argument: (iii) 0, 0, 0], [Argument: (iiii) -1, -1, -1, -1], 0], 1 = [Argument: ((iii)(iiii)i) [Argument: (iii) 1970, 1, 1], [Argument: (iiii) 0, 0, 1, 0], 1], 1150629776 = [Argument: ((iii)(iiii)i) [Argument: (iii) 2006, 6, 18], [Argument: (iiii) 11, 22, 56, 0], 1]}]";


    QMap<int, QString> ismap;
    ismap[1] = "a";
    ismap[2000] = "b";
    ismap[-47] = "c";

    QMap<QString, QString> ssmap;
    ssmap["a"] = "a";
    ssmap["c"] = "b";
    ssmap["b"] = "c";

    QHash<QDBusSignature, QString> gsmap;
    gsmap[QDBusSignature("i")] = "int32";
    gsmap[QDBusSignature("s")] = "string";
    gsmap[QDBusSignature("a{gs}")] = "array of dict_entry of (signature, string)";

    QVariantMap svmap;
    svmap["a"] = 1;
    svmap["c"] = "b";
    svmap["b"] = QByteArray("c");
    svmap["d"] = 42U;
    svmap["e"] = qVariantFromValue(short(-47));
    svmap["f"] = qVariantFromValue(QDBusVariant(0));
    svmap["date"] = QDate(1977, 1, 1);
    svmap["time"] = QTime(8, 58, 0);
    svmap["datetime"] = QDateTime(QDate(13, 9, 2008), QTime(8, 59, 31));
    svmap["pointf"] = QPointF(0.5, -0.5);
    svmap["ismap"] = qVariantFromValue(ismap);
    svmap["ssmap"] = qVariantFromValue(ssmap);
    svmap["gsmap"] = qVariantFromValue(gsmap);
    svmap["dtlist"] = qVariantFromValue(dtlist);
    svmap["lldtmap"] = qVariantFromValue(lldtmap);
    QTest::newRow("sv-map") << qVariantFromValue(svmap) << "a{sv}"
            << "[Argument: a{sv} {\"a\" = [Variant(int): 1], \"b\" = [Variant(QByteArray): {99}], \"c\" = [Variant(QString): \"b\"], \"d\" = [Variant(uint): 42], \"date\" = [Variant: [Argument: (iii) 1977, 1, 1]], \"datetime\" = [Variant: [Argument: ((iii)(iiii)i) [Argument: (iii) 0, 0, 0], [Argument: (iiii) 8, 59, 31, 0], 0]], \"dtlist\" = [Variant: [Argument: a((iii)(iiii)i) {[Argument: ((iii)(iiii)i) [Argument: (iii) 0, 0, 0], [Argument: (iiii) -1, -1, -1, -1], 0], [Argument: ((iii)(iiii)i) [Argument: (iii) 1977, 9, 13], [Argument: (iiii) 0, 0, 0, 0], 0], [Argument: ((iii)(iiii)i) [Argument: (iii) 2006, 6, 18], [Argument: (iiii) 13, 14, 0, 0], 0]}]], \"e\" = [Variant(short): -47], \"f\" = [Variant: [Variant(int): 0]], \"gsmap\" = [Variant: [Argument: a{gs} {[Signature: a{gs}] = \"array of dict_entry of (signature, string)\", [Signature: i] = \"int32\", [Signature: s] = \"string\"}]], \"ismap\" = [Variant: [Argument: a{is} {-47 = \"c\", 1 = \"a\", 2000 = \"b\"}]], \"lldtmap\" = [Variant: [Argument: a{x((iii)(iiii)i)} {0 = [Argument: ((iii)(iiii)i) [Argument: (iii) 0, 0, 0], [Argument: (iiii) -1, -1, -1, -1], 0], 1 = [Argument: ((iii)(iiii)i) [Argument: (iii) 1970, 1, 1], [Argument: (iiii) 0, 0, 1, 0], 1], 1150629776 = [Argument: ((iii)(iiii)i) [Argument: (iii) 2006, 6, 18], [Argument: (iiii) 11, 22, 56, 0], 1]}]], \"pointf\" = [Variant: [Argument: (dd) 0.5, -0.5]], \"ssmap\" = [Variant: [Argument: a{ss} {\"a\" = \"a\", \"b\" = \"c\", \"c\" = \"b\"}]], \"time\" = [Variant: [Argument: (iiii) 8, 58, 0, 0]]}]";
}

void tst_QDBusMarshall::sendArgument_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("sig");
    QTest::addColumn<int>("classification");

    QDBusArgument();
    QDBusArgument arg;

    arg = QDBusArgument();
    arg << true;
    QTest::newRow("bool") << qVariantFromValue(arg) << "b" << int(QDBusArgument::BasicType);;

    arg = QDBusArgument();
    arg << false;
    QTest::newRow("bool2") << qVariantFromValue(arg) << "b" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << uchar(1);
    QTest::newRow("byte") << qVariantFromValue(arg) << "y" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << short(2);
    QTest::newRow("int16") << qVariantFromValue(arg) << "n" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << ushort(3);
    QTest::newRow("uint16") << qVariantFromValue(arg) << "q" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << 1;
    QTest::newRow("int32") << qVariantFromValue(arg) << "i" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << 2U;
    QTest::newRow("uint32") << qVariantFromValue(arg) << "u" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << Q_INT64_C(3);
    QTest::newRow("int64") << qVariantFromValue(arg) << "x" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << Q_UINT64_C(4);
    QTest::newRow("uint64") << qVariantFromValue(arg) << "t" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << 42.5;
    QTest::newRow("double") << qVariantFromValue(arg) << "d" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << QLatin1String("ping");
    QTest::newRow("string") << qVariantFromValue(arg) << "s" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << QDBusObjectPath("/org/kde");
    QTest::newRow("objectpath") << qVariantFromValue(arg) << "o" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << QDBusSignature("g");
    QTest::newRow("signature") << qVariantFromValue(arg) << "g" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << QLatin1String("");
    QTest::newRow("emptystring") << qVariantFromValue(arg) << "s" << int(QDBusArgument::BasicType);

    arg = QDBusArgument();
    arg << QString();
    QTest::newRow("nullstring") << qVariantFromValue(arg) << "s" << int(QDBusArgument::BasicType);

    if (fileDescriptorPassing) {
        arg = QDBusArgument();
        arg << QDBusUnixFileDescriptor(fileDescriptorForTest());
        QTest::newRow("filedescriptor") << qVariantFromValue(arg) << "h" << int(QDBusArgument::BasicType);
    }

    arg = QDBusArgument();
    arg << QDBusVariant(1);
    QTest::newRow("variant") << qVariantFromValue(arg) << "v" << int(QDBusArgument::VariantType);

    arg = QDBusArgument();
    arg << QDBusVariant(qVariantFromValue(QDBusVariant(1)));
    QTest::newRow("variant-variant") << qVariantFromValue(arg) << "v" << int(QDBusArgument::VariantType);

    arg = QDBusArgument();
    arg.beginArray(QVariant::Int);
    arg << 1 << 2 << 3 << -4;
    arg.endArray();
    QTest::newRow("array-of-int") << qVariantFromValue(arg) << "ai" << int(QDBusArgument::ArrayType);

    arg = QDBusArgument();
    arg.beginMap(QVariant::Int, QVariant::UInt);
    arg.beginMapEntry();
    arg << 1 << 2U;
    arg.endMapEntry();
    arg.beginMapEntry();
    arg << 3 << 4U;
    arg.endMapEntry();
    arg.endMap();
    QTest::newRow("map") << qVariantFromValue(arg) << "a{iu}" << int(QDBusArgument::MapType);

    arg = QDBusArgument();
    arg.beginStructure();
    arg << 1 << 2U << short(-3) << ushort(4) << 5.0 << false;
    arg.endStructure();
    QTest::newRow("structure") << qVariantFromValue(arg) << "(iunqdb)" << int(QDBusArgument::StructureType);

#if 0
    // this is now unsupported
    arg << 1 << 2U << short(-3) << ushort(4) << 5.0 << false;
    QTest::newRow("many-args") << qVariantFromValue(arg) << "(iunqdb)iunqdb";
#endif
}

void tst_QDBusMarshall::sendBasic()
{
    QFETCH(QVariant, value);
    QFETCH(QString, stringResult);

    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());

    QDBusMessage msg = QDBusMessage::createMethodCall(serviceName,
                                                      objectPath, interfaceName, "ping");
    msg << value;

    QDBusMessage reply = con.call(msg);
    QVERIFY2(reply.type() == QDBusMessage::ReplyMessage,
             qPrintable(reply.errorName() + ": " + reply.errorMessage()));
    //qDebug() << reply;

    QCOMPARE(reply.arguments().count(), msg.arguments().count());
    QTEST(reply.signature(), "sig");
    for (int i = 0; i < reply.arguments().count(); ++i) {
        QVERIFY(compare(reply.arguments().at(i), msg.arguments().at(i)));
        //printf("\n! %s\n* %s\n", qPrintable(qDBusArgumentToString(reply.arguments().at(i))), qPrintable(stringResult));
        QCOMPARE(QDBusUtil::argumentToString(reply.arguments().at(i)), stringResult);
    }
}

void tst_QDBusMarshall::sendVariant()
{
    QFETCH(QVariant, value);

    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());

    QDBusMessage msg = QDBusMessage::createMethodCall(serviceName,
                                                      objectPath, interfaceName, "ping");
    msg << qVariantFromValue(QDBusVariant(value));

    QDBusMessage reply = con.call(msg);
 //   qDebug() << reply;

    QCOMPARE(reply.arguments().count(), msg.arguments().count());
    QCOMPARE(reply.signature(), QString("v"));
    for (int i = 0; i < reply.arguments().count(); ++i)
        QVERIFY(compare(reply.arguments().at(i), msg.arguments().at(i)));
}

void tst_QDBusMarshall::sendArrays()
{
    sendBasic();
}

void tst_QDBusMarshall::sendArrayOfArrays()
{
    sendBasic();
}

void tst_QDBusMarshall::sendMaps()
{
    sendBasic();
}

void tst_QDBusMarshall::sendStructs()
{
    sendBasic();
}

void tst_QDBusMarshall::sendComplex()
{
    sendBasic();
}

void tst_QDBusMarshall::sendArgument()
{
    QFETCH(QVariant, value);
    QFETCH(QString, sig);

    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());

    QDBusMessage msg = QDBusMessage::createMethodCall(serviceName, objectPath,
                                                      interfaceName, "ping");
    msg << value;

    QDBusMessage reply = con.call(msg);

//    QCOMPARE(reply.arguments().count(), msg.arguments().count());
    QCOMPARE(reply.signature(), sig);
//    for (int i = 0; i < reply.arguments().count(); ++i)
//        QVERIFY(compare(reply.arguments().at(i), msg.arguments().at(i)));

    // do it again inside a STRUCT now
    QDBusArgument sendArg;
    sendArg.beginStructure();
    sendArg.appendVariant(value);
    sendArg.endStructure();
    msg.setArguments(QVariantList() << qVariantFromValue(sendArg));
    reply = con.call(msg);

    QCOMPARE(reply.signature(), QString("(%1)").arg(sig));
    QCOMPARE(reply.arguments().at(0).userType(), qMetaTypeId<QDBusArgument>());

    const QDBusArgument arg = qvariant_cast<QDBusArgument>(reply.arguments().at(0));
    QCOMPARE(int(arg.currentType()), int(QDBusArgument::StructureType));

    arg.beginStructure();
    QVERIFY(!arg.atEnd());
    QCOMPARE(arg.currentSignature(), sig);
    QTEST(int(arg.currentType()), "classification");

    QVariant extracted = arg.asVariant();
    QVERIFY(arg.atEnd());

    arg.endStructure();
    QVERIFY(arg.atEnd());
    QCOMPARE(arg.currentType(), QDBusArgument::UnknownType);

    if (value.type() != QVariant::UserType)
        QCOMPARE(extracted, value);
}

void tst_QDBusMarshall::sendSignalErrors()
{
    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());
    QDBusMessage msg = QDBusMessage::createSignal("/foo", "local.interfaceName",
                                                  "signalName");
    msg << qVariantFromValue(QDBusObjectPath());

    QTest::ignoreMessage(QtWarningMsg, "QDBusConnection: error: could not send signal path \"/foo\" interface \"local.interfaceName\" member \"signalName\": Marshalling failed: Invalid object path passed in arguments");
    QVERIFY(!con.send(msg));

    msg.setArguments(QVariantList());
    QDBusObjectPath path;

    QTest::ignoreMessage(QtWarningMsg, "QDBusObjectPath: invalid path \"abc\"");
    path.setPath("abc");
    msg << qVariantFromValue(path);

    QTest::ignoreMessage(QtWarningMsg, "QDBusConnection: error: could not send signal path \"/foo\" interface \"local.interfaceName\" member \"signalName\": Marshalling failed: Invalid object path passed in arguments");
    QVERIFY(!con.send(msg));

    QDBusSignature sig;
    msg.setArguments(QVariantList() << qVariantFromValue(sig));
    QTest::ignoreMessage(QtWarningMsg, "QDBusConnection: error: could not send signal path \"/foo\" interface \"local.interfaceName\" member \"signalName\": Marshalling failed: Invalid signature passed in arguments");
    QVERIFY(!con.send(msg));

    QTest::ignoreMessage(QtWarningMsg, "QDBusSignature: invalid signature \"a\"");
    sig.setSignature("a");
    msg.setArguments(QVariantList());
    msg << qVariantFromValue(sig);
    QTest::ignoreMessage(QtWarningMsg, "QDBusConnection: error: could not send signal path \"/foo\" interface \"local.interfaceName\" member \"signalName\": Marshalling failed: Invalid signature passed in arguments");
    QVERIFY(!con.send(msg));
}

void tst_QDBusMarshall::sendCallErrors_data()
{
    QTest::addColumn<QString>("service");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("interface");
    QTest::addColumn<QString>("method");
    QTest::addColumn<QVariantList>("arguments");
    QTest::addColumn<QString>("errorName");
    QTest::addColumn<QString>("errorMsg");
    QTest::addColumn<QString>("ignoreMsg");

    // this error comes from the bus server
    QTest::newRow("empty-service") << "" << objectPath << interfaceName << "ping" << QVariantList()
            << "org.freedesktop.DBus.Error.UnknownMethod"
            << "Method \"ping\" with signature \"\" on interface \"com.trolltech.autotests.qpong\" doesn't exist\n" << (const char*)0;

    QTest::newRow("invalid-service") << "this isn't valid" << objectPath << interfaceName << "ping" << QVariantList()
            << "com.trolltech.QtDBus.Error.InvalidService"
            << "Invalid service name: this isn't valid" << "";

    QTest::newRow("empty-path") << serviceName << "" << interfaceName << "ping" << QVariantList()
            << "com.trolltech.QtDBus.Error.InvalidObjectPath"
            << "Object path cannot be empty" << "";
    QTest::newRow("invalid-path") << serviceName << "//" << interfaceName << "ping" << QVariantList()
            << "com.trolltech.QtDBus.Error.InvalidObjectPath"
            << "Invalid object path: //" << "";

    // empty interfaces are valid
    QTest::newRow("invalid-interface") << serviceName << objectPath << "this isn't valid" << "ping" << QVariantList()
            << "com.trolltech.QtDBus.Error.InvalidInterface"
            << "Invalid interface class: this isn't valid" << "";

    QTest::newRow("empty-method") << serviceName << objectPath << interfaceName << "" << QVariantList()
            << "com.trolltech.QtDBus.Error.InvalidMember"
            << "method name cannot be empty" << "";
    QTest::newRow("invalid-method") << serviceName << objectPath << interfaceName << "this isn't valid" << QVariantList()
            << "com.trolltech.QtDBus.Error.InvalidMember"
            << "Invalid method name: this isn't valid" << "";

    QTest::newRow("invalid-variant1") << serviceName << objectPath << interfaceName << "ping"
            << (QVariantList() << QVariant())
            << "org.freedesktop.DBus.Error.Failed"
            << "Marshalling failed: Variant containing QVariant::Invalid passed in arguments"
            << "QDBusMarshaller: cannot add an invalid QVariant";
    QTest::newRow("invalid-variant1") << serviceName << objectPath << interfaceName << "ping"
            << (QVariantList() << qVariantFromValue(QDBusVariant()))
            << "org.freedesktop.DBus.Error.Failed"
            << "Marshalling failed: Variant containing QVariant::Invalid passed in arguments"
            << "QDBusMarshaller: cannot add a null QDBusVariant";

    QTest::newRow("builtin-unregistered") << serviceName << objectPath << interfaceName << "ping"
            << (QVariantList() << QLocale::c())
            << "org.freedesktop.DBus.Error.Failed"
            << "Marshalling failed: Unregistered type QLocale passed in arguments"
            << "QDBusMarshaller: type `QLocale' (18) is not registered with D-BUS. Use qDBusRegisterMetaType to register it";

    // this type is known to the meta type system, but not registered with D-Bus
    qRegisterMetaType<UnregisteredType>();
    QTest::newRow("extra-unregistered") << serviceName << objectPath << interfaceName << "ping"
            << (QVariantList() << qVariantFromValue(UnregisteredType()))
            << "org.freedesktop.DBus.Error.Failed"
            << "Marshalling failed: Unregistered type UnregisteredType passed in arguments"
            << QString("QDBusMarshaller: type `UnregisteredType' (%1) is not registered with D-BUS. Use qDBusRegisterMetaType to register it")
            .arg(qMetaTypeId<UnregisteredType>());

    QTest::newRow("invalid-object-path-arg") << serviceName << objectPath << interfaceName << "ping"
            << (QVariantList() << qVariantFromValue(QDBusObjectPath()))
            << "org.freedesktop.DBus.Error.Failed"
            << "Marshalling failed: Invalid object path passed in arguments"
            << "";

    QTest::newRow("invalid-signature-arg") << serviceName << objectPath << interfaceName << "ping"
            << (QVariantList() << qVariantFromValue(QDBusSignature()))
            << "org.freedesktop.DBus.Error.Failed"
            << "Marshalling failed: Invalid signature passed in arguments"
            << "";

    // invalid file descriptor
    if (fileDescriptorPassing) {
        QTest::newRow("invalid-file-descriptor") << serviceName << objectPath << interfaceName << "ping"
                << (QVariantList() << qVariantFromValue(QDBusUnixFileDescriptor(-1)))
                << "org.freedesktop.DBus.Error.Failed"
                << "Marshalling failed: Invalid file descriptor passed in arguments"
                << "";
    }
}

void tst_QDBusMarshall::sendCallErrors()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    QFETCH(QString, service);
    QFETCH(QString, path);
    QFETCH(QString, interface);
    QFETCH(QString, method);
    QFETCH(QVariantList, arguments);
    QFETCH(QString, errorMsg);

    QFETCH(QString, ignoreMsg);
    if (!ignoreMsg.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, ignoreMsg.toLatin1());
    if (!ignoreMsg.isNull())
        QTest::ignoreMessage(QtWarningMsg,
                             QString("QDBusConnection: error: could not send message to service \"%1\" path \"%2\" interface \"%3\" member \"%4\": %5")
                             .arg(service, path, interface, method, errorMsg)
                             .toLatin1());

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, method);
    msg.setArguments(arguments);

    QDBusMessage reply = con.call(msg, QDBus::Block);
    QCOMPARE(reply.type(), QDBusMessage::ErrorMessage);
    QTEST(reply.errorName(), "errorName");
    QCOMPARE(reply.errorMessage(), errorMsg);
}

void tst_QDBusMarshall::receiveUnknownType_data()
{
    QTest::addColumn<int>("receivedTypeId");
    QTest::newRow("in-call") << qMetaTypeId<void*>();
    QTest::newRow("type-variant") << qMetaTypeId<QDBusVariant>();
    QTest::newRow("type-array") << qMetaTypeId<QDBusArgument>();
    QTest::newRow("type-struct") << qMetaTypeId<QDBusArgument>();
    QTest::newRow("type-naked") << qMetaTypeId<void *>();
}

struct DisconnectRawDBus {
    static void cleanup(DBusConnection *connection)
    {
        if (!connection)
            return;
        dbus_connection_close(connection);
        dbus_connection_unref(connection);
    }
};
template <typename T, void (*unref)(T *)> struct GenericUnref
{
    static void cleanup(T *type)
    {
        if (!type) return;
        unref(type);
    }
};

// use these scoped types to avoid memory leaks if QVERIFY or QCOMPARE fails
typedef QScopedPointer<DBusConnection, DisconnectRawDBus> ScopedDBusConnection;
typedef QScopedPointer<DBusMessage, GenericUnref<DBusMessage, dbus_message_unref> > ScopedDBusMessage;
typedef QScopedPointer<DBusPendingCall, GenericUnref<DBusPendingCall, dbus_pending_call_unref> > ScopedDBusPendingCall;

template <typename T> struct SetResetValue
{
    const T oldValue;
    T &value;
public:
    SetResetValue(T &v, T newValue) : oldValue(v), value(v)
    {
        value = newValue;
    }
    ~SetResetValue()
    {
        value = oldValue;
    }
};

void tst_QDBusMarshall::receiveUnknownType()
{
#ifndef DBUS_TYPE_UNIX_FD
    QSKIP("Your system's D-Bus library is too old for this test", SkipAll);
#else
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    // this needs to be implemented in raw
    // open a new connection to the bus daemon
    DBusError error;
    dbus_error_init(&error);
    ScopedDBusConnection rawcon(dbus_bus_get_private(DBUS_BUS_SESSION, &error));
    QVERIFY2(rawcon.data(), error.name);

    // check if this bus supports passing file descriptors
    if (!dbus_connection_can_send_type(rawcon.data(), DBUS_TYPE_UNIX_FD))
        QSKIP("Your session bus does not allow sending Unix file descriptors", SkipAll);

    // make sure this QDBusConnection won't handle Unix file descriptors
    QDBusConnection::ConnectionCapabilities &capabRef = QDBusConnectionPrivate::d(con)->capabilities;
    SetResetValue<QDBusConnection::ConnectionCapabilities> resetter(capabRef, capabRef & ~QDBusConnection::UnixFileDescriptorPassing);

    if (qstrcmp(QTest::currentDataTag(), "in-call") == 0) {
        // create a call back to us containing a file descriptor
        QDBusMessageSpy spy;
        con.registerObject("/spyObject", &spy, QDBusConnection::ExportAllSlots);
        ScopedDBusMessage msg(dbus_message_new_method_call(con.baseService().toLatin1(), "/spyObject", NULL, "theSlot"));

        int fd = fileno(stdout);
        dbus_message_append_args(msg.data(), DBUS_TYPE_UNIX_FD, &fd, DBUS_TYPE_INVALID);

        // try to send to us
        DBusPendingCall *pending_ptr;
        dbus_connection_send_with_reply(rawcon.data(), msg.data(), &pending_ptr, 1000);
        ScopedDBusPendingCall pending(pending_ptr);

        // check that it got sent
        while (dbus_connection_dispatch(rawcon.data()) == DBUS_DISPATCH_DATA_REMAINS)
            ;

        // now spin our event loop. We don't catch this call, so let's get the reply
        QEventLoop loop;
        QTimer::singleShot(200, &loop, SLOT(quit()));
        loop.exec();

        // now try to receive the reply
        dbus_pending_call_block(pending.data());

        // check that the spy received what it was supposed to receive
        QCOMPARE(spy.list.size(), 1);
        QCOMPARE(spy.list.at(0).arguments().size(), 1);
        QFETCH(int, receivedTypeId);
        QCOMPARE(spy.list.at(0).arguments().at(0).userType(), receivedTypeId);

        msg.reset(dbus_pending_call_steal_reply(pending.data()));
        QVERIFY(msg);
        QCOMPARE(dbus_message_get_type(msg.data()), DBUS_MESSAGE_TYPE_METHOD_RETURN);
        QCOMPARE(dbus_message_get_signature(msg.data()), DBUS_TYPE_INT32_AS_STRING);

        int retval;
        QVERIFY(dbus_message_get_args(msg.data(), &error, DBUS_TYPE_INT32, &retval, DBUS_TYPE_INVALID));
        QCOMPARE(retval, 42);
    } else {
        // create a signal that we'll emit
        static const char signalName[] = "signalName";
        static const char interfaceName[] = "local.interface.name";
        ScopedDBusMessage msg(dbus_message_new_signal("/", interfaceName, signalName));
        con.connect(dbus_bus_get_unique_name(rawcon.data()), QString(), interfaceName, signalName, &QTestEventLoop::instance(), SLOT(exitLoop()));

        QDBusMessageSpy spy;
        con.connect(dbus_bus_get_unique_name(rawcon.data()), QString(), interfaceName, signalName, &spy, SLOT(theSlot(QDBusMessage)));

        DBusMessageIter iter;
        dbus_message_iter_init_append(msg.data(), &iter);
        int fd = fileno(stdout);

        if (qstrcmp(QTest::currentDataTag(), "type-naked") == 0) {
            // send naked
            dbus_message_iter_append_basic(&iter, DBUS_TYPE_UNIX_FD, &fd);
        } else {
            DBusMessageIter subiter;
            if (qstrcmp(QTest::currentDataTag(), "type-variant") == 0)
                dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, DBUS_TYPE_UNIX_FD_AS_STRING, &subiter);
            else if (qstrcmp(QTest::currentDataTag(), "type-array") == 0)
                dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, DBUS_TYPE_UNIX_FD_AS_STRING, &subiter);
            else if (qstrcmp(QTest::currentDataTag(), "type-struct") == 0)
                dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, 0, &subiter);
            dbus_message_iter_append_basic(&subiter, DBUS_TYPE_UNIX_FD, &fd);
            dbus_message_iter_close_container(&iter, &subiter);
        }

        // send it
        dbus_connection_send(rawcon.data(), msg.data(), 0);

        // check that it got sent
        while (dbus_connection_dispatch(rawcon.data()) == DBUS_DISPATCH_DATA_REMAINS)
            ;

        // now let's see what happens
        QTestEventLoop::instance().enterLoop(1);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QCOMPARE(spy.list.size(), 1);
        QCOMPARE(spy.list.at(0).arguments().count(), 1);
        QFETCH(int, receivedTypeId);
        //qDebug() << spy.list.at(0).arguments().at(0).typeName();
        QCOMPARE(spy.list.at(0).arguments().at(0).userType(), receivedTypeId);
    }
#endif
}

QTEST_MAIN(tst_QDBusMarshall)
#include "tst_qdbusmarshall.moc"
