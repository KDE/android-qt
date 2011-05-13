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
/* -*- C++ -*-
 */
#include <qcoreapplication.h>
#include <qmetatype.h>
#include <QtTest/QtTest>
#include <QtDBus/QtDBus>

#define USE_PRIVATE_CODE
#include "../qdbusmarshall/common.h"

class tst_QDBusXmlParser: public QObject
{
    Q_OBJECT

private:
    void parsing_common(const QString&);

private slots:
    void parsing_data();
    void parsing();
    void parsingWithDoctype_data();
    void parsingWithDoctype();

    void objectWithContent_data();
    void objectWithContent();

    void methods_data();
    void methods();
    void signals__data();
    void signals_();
    void properties_data();
    void properties();
};

void tst_QDBusXmlParser::parsing_data()
{
    QTest::addColumn<QString>("xmlData");
    QTest::addColumn<int>("interfaceCount");
    QTest::addColumn<int>("objectCount");

    QTest::newRow("null") << QString() << 0 << 0;
    QTest::newRow("empty") << QString("") << 0 << 0;
    
    QTest::newRow("junk") << "<junk/>" << 0 << 0;
    QTest::newRow("interface-inside-junk") << "<junk><interface name=\"iface.iface1\" /></junk>"
                                           << 0 << 0;
    QTest::newRow("object-inside-junk") << "<junk><node name=\"obj1\" /></junk>"
                                        << 0 << 0;

    QTest::newRow("zero-interfaces") << "<node/>" << 0 << 0;
    QTest::newRow("one-interface") << "<node><interface name=\"iface.iface1\" /></node>" << 1 << 0;

    
    QTest::newRow("two-interfaces") << "<node><interface name=\"iface.iface1\" />"
                                       "<interface name=\"iface.iface2\"></node>"
                                    << 2 << 0;        


    QTest::newRow("one-object") << "<node><node name=\"obj1\"/></node>" << 0 << 1;
    QTest::newRow("two-objects") << "<node><node name=\"obj1\"/><node name=\"obj2\"></node>" << 0 << 2;

    QTest::newRow("i1o1") << "<node><interface name=\"iface.iface1\"><node name=\"obj1\"></node>" << 1 << 1;

}

void tst_QDBusXmlParser::parsing_common(const QString &xmlData)
{
    QDBusIntrospection::ObjectTree obj =
        QDBusIntrospection::parseObjectTree(xmlData, "local.testing", "/");
    QFETCH(int, interfaceCount);
    QFETCH(int, objectCount);
    QCOMPARE(obj.interfaces.count(), interfaceCount);
    QCOMPARE(obj.childObjects.count(), objectCount);

    // also verify the naming
    int i = 0;
    foreach (QString name, obj.interfaces)
        QCOMPARE(name, QString("iface.iface%1").arg(++i));

    i = 0;
    foreach (QString name, obj.childObjects)
        QCOMPARE(name, QString("obj%1").arg(++i));
}

void tst_QDBusXmlParser::parsing()
{
    QFETCH(QString, xmlData);

    parsing_common(xmlData);
}

void tst_QDBusXmlParser::parsingWithDoctype_data()
{
    parsing_data();
}

void tst_QDBusXmlParser::parsingWithDoctype()
{
    QString docType = "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
                      "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n";
    QFETCH(QString, xmlData);

    parsing_common(docType + xmlData);
}    

void tst_QDBusXmlParser::objectWithContent_data()
{
    QTest::addColumn<QString>("xmlData");
    QTest::addColumn<QString>("probedObject");
    QTest::addColumn<int>("interfaceCount");
    QTest::addColumn<int>("objectCount");

    QTest::newRow("zero") << "<node><node name=\"obj\"/></node>" << "obj" << 0 << 0;

    QString xmlData = "<node><node name=\"obj\">"
                      "<interface name=\"iface.iface1\" />"
                      "</node></node>";
    QTest::newRow("one-interface") << xmlData << "obj" << 1 << 0;
    QTest::newRow("one-interface2") << xmlData << "obj2" << 0 << 0;

    xmlData = "<node><node name=\"obj\">"
              "<interface name=\"iface.iface1\" />"
              "<interface name=\"iface.iface2\" />"
              "</node></node>";
    QTest::newRow("two-interfaces") << xmlData << "obj" << 2 << 0;
    QTest::newRow("two-interfaces2") << xmlData << "obj2" << 0 << 0;

    xmlData = "<node><node name=\"obj\">"
              "<interface name=\"iface.iface1\" />"
              "<interface name=\"iface.iface2\" />"
              "</node><node name=\"obj2\">"
              "<interface name=\"iface.iface1\" />"
              "</node></node>";
    QTest::newRow("two-nodes-two-interfaces") << xmlData << "obj" << 2 << 0;
    QTest::newRow("two-nodes-one-interface") << xmlData << "obj2" << 1 << 0;

    xmlData = "<node><node name=\"obj\">"
              "<node name=\"obj1\" />"
              "</node></node>";
    QTest::newRow("one-object") << xmlData << "obj" << 0 << 1;
    QTest::newRow("one-object2") << xmlData << "obj2" << 0 << 0;

    xmlData = "<node><node name=\"obj\">"
              "<node name=\"obj1\" />"
              "<node name=\"obj2\" />"
              "</node></node>";
    QTest::newRow("two-objects") << xmlData << "obj" << 0 << 2;
    QTest::newRow("two-objects2") << xmlData << "obj2" << 0 << 0;

    xmlData = "<node><node name=\"obj\">"
              "<node name=\"obj1\" />"
              "<node name=\"obj2\" />"
              "</node><node name=\"obj2\">"
              "<node name=\"obj1\" />"
              "</node></node>";
    QTest::newRow("two-nodes-two-objects") << xmlData << "obj" << 0 << 2;
    QTest::newRow("two-nodes-one-object") << xmlData << "obj2" << 0 << 1;
}

void tst_QDBusXmlParser::objectWithContent()
{
    QFETCH(QString, xmlData);
    QFETCH(QString, probedObject);

    QDBusIntrospection::ObjectTree tree =
        QDBusIntrospection::parseObjectTree(xmlData, "local.testing", "/");

    const ObjectMap &om = tree.childObjectData;

    if (om.contains(probedObject)) {
        const QSharedDataPointer<QDBusIntrospection::ObjectTree>& obj = om.value(probedObject);
        QVERIFY(obj != 0);
    
        QFETCH(int, interfaceCount);
        QFETCH(int, objectCount);

        QCOMPARE(obj->interfaces.count(), interfaceCount);
        QCOMPARE(obj->childObjects.count(), objectCount);

        // verify the object names
        int i = 0;
        foreach (QString name, obj->interfaces)
            QCOMPARE(name, QString("iface.iface%1").arg(++i));

        i = 0;
        foreach (QString name, obj->childObjects)
            QCOMPARE(name, QString("obj%1").arg(++i));
    }
}

void tst_QDBusXmlParser::methods_data()
{
    QTest::addColumn<QString>("xmlDataFragment");
    QTest::addColumn<MethodMap>("methodMap");

    MethodMap map;
    QTest::newRow("no-methods") << QString() << map;

    // one method without arguments
    QDBusIntrospection::Method method;
    method.name = "Foo";
    map << method;
    QTest::newRow("one-method") << "<method name=\"Foo\"/>" << map;

    // add another method without arguments
    method.name = "Bar";
    map << method;
    QTest::newRow("two-methods") << "<method name=\"Foo\"/>"
                                    "<method name=\"Bar\"/>"
                                 << map;

    // invert the order of the XML declaration
    QTest::newRow("two-methods-inverse") << "<method name=\"Bar\"/>"
                                            "<method name=\"Foo\"/>"
                                         << map;

    // add a third, with annotations
    method.name = "Baz";
    method.annotations.insert("foo.testing", "nothing to see here");
    map << method;
    QTest::newRow("method-with-annotation") <<
        "<method name=\"Foo\"/>"
        "<method name=\"Bar\"/>"
        "<method name=\"Baz\"><annotation name=\"foo.testing\" value=\"nothing to see here\"></method>"
                                            << map;

    // arguments
    map.clear();
    method.annotations.clear();

    method.name = "Method";
    method.inputArgs << arg("s");
    map << method;
    QTest::newRow("one-in") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" direction=\"in\"/>"
        "</method>" << map;

    // two arguments
    method.inputArgs << arg("v");
    map.clear();
    map << method;
    QTest::newRow("two-in") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" direction=\"in\"/>"
        "<arg type=\"v\" direction=\"in\"/>"
        "</method>" << map;

    // one invalid arg
    method.inputArgs << arg("~", "invalid");
    map.clear();
    map << method;
    QTest::newRow("two-in-one-invalid") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" direction=\"in\"/>"
        "<arg type=\"v\" direction=\"in\"/>"
        "<arg type=\"~\" name=\"invalid\" direction=\"in\"/>"
        "</method>" << map;

    // one out argument
    method.inputArgs.clear();
    method.outputArgs << arg("s");
    map.clear();
    map << method;
    QTest::newRow("one-out") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" direction=\"out\"/>"
        "</method>" << map;

    // two in and one out
    method.inputArgs << arg("s") << arg("v");
    map.clear();
    map << method;
    QTest::newRow("two-in-one-out") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" direction=\"in\"/>"
        "<arg type=\"v\" direction=\"in\"/>"
        "<arg type=\"s\" direction=\"out\"/>"
        "</method>" << map;

    // let's try an arg with name
    method.outputArgs.clear();
    method.inputArgs.clear();
    method.inputArgs << arg("s", "foo");
    map.clear();
    map << method;
    QTest::newRow("one-in-with-name") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" name=\"foo\" direction=\"in\"/>"
        "</method>" << map;

    // two args with name
    method.inputArgs << arg("i", "bar");
    map.clear();
    map << method;
    QTest::newRow("two-in-with-name") <<
        "<method name=\"Method\">"
        "<arg type=\"s\" name=\"foo\" direction=\"in\"/>"
        "<arg type=\"i\" name=\"bar\" direction=\"in\"/>"
        "</method>" << map;

    // one complex
    map.clear();
    method = QDBusIntrospection::Method();

    // Method1(in STRING arg1, in BYTE arg2, out ARRAY of STRING)
    method.inputArgs << arg("s", "arg1") << arg("y", "arg2");
    method.outputArgs << arg("as");
    method.name = "Method1";
    map << method;

    // Method2(in ARRAY of DICT_ENTRY of (STRING,VARIANT) variantMap, in UINT32 index,
    //         out STRING key, out VARIANT value)
    // with annotation "foo.equivalent":"QVariantMap"
    method = QDBusIntrospection::Method();
    method.inputArgs << arg("a{sv}", "variantMap") << arg("u", "index");
    method.outputArgs << arg("s", "key") << arg("v", "value");
    method.annotations.insert("foo.equivalent", "QVariantMap");
    method.name = "Method2";
    map << method;

    QTest::newRow("complex") <<
        "<method name=\"Method1\">"
        "<arg name=\"arg1\" type=\"s\" direction=\"in\"/>"
        "<arg name=\"arg2\" type=\"y\" direction=\"in\"/>"
        "<arg type=\"as\" direction=\"out\"/>"
        "</method>"
        "<method name=\"Method2\">"
        "<arg name=\"variantMap\" type=\"a{sv}\" direction=\"in\"/>"
        "<arg name=\"index\" type=\"u\" direction=\"in\"/>"
        "<arg name=\"key\" type=\"s\" direction=\"out\"/>"
        "<arg name=\"value\" type=\"v\" direction=\"out\"/>"
        "<annotation name=\"foo.equivalent\" value=\"QVariantMap\"/>"
        "</method>" << map;
}

void tst_QDBusXmlParser::methods()
{
    QString xmlHeader = "<node>"
                        "<interface name=\"iface.iface1\">",
            xmlFooter = "</interface>"
                        "</node>";

    QFETCH(QString, xmlDataFragment);

    QDBusIntrospection::Interface iface =
        QDBusIntrospection::parseInterface(xmlHeader + xmlDataFragment + xmlFooter);

    QCOMPARE(iface.name, QString("iface.iface1"));

    QFETCH(MethodMap, methodMap);
    MethodMap parsedMap = iface.methods;

    QCOMPARE(parsedMap.count(), methodMap.count());
    QCOMPARE(parsedMap, methodMap);
}

void tst_QDBusXmlParser::signals__data()
{
    QTest::addColumn<QString>("xmlDataFragment");
    QTest::addColumn<SignalMap>("signalMap");

    SignalMap map;
    QTest::newRow("no-signals") << QString() << map;

    // one signal without arguments
    QDBusIntrospection::Signal signal;
    signal.name = "Foo";
    map << signal;
    QTest::newRow("one-signal") << "<signal name=\"Foo\"/>" << map;

    // add another signal without arguments
    signal.name = "Bar";
    map << signal;
    QTest::newRow("two-signals") << "<signal name=\"Foo\"/>"
                                    "<signal name=\"Bar\"/>"
                                 << map;

    // invert the order of the XML declaration
    QTest::newRow("two-signals-inverse") << "<signal name=\"Bar\"/>"
                                            "<signal name=\"Foo\"/>"
                                         << map;

    // add a third, with annotations
    signal.name = "Baz";
    signal.annotations.insert("foo.testing", "nothing to see here");
    map << signal;
    QTest::newRow("signal-with-annotation") <<
        "<signal name=\"Foo\"/>"
        "<signal name=\"Bar\"/>"
        "<signal name=\"Baz\"><annotation name=\"foo.testing\" value=\"nothing to see here\"></signal>"
                                            << map;

    // one out argument
    map.clear();
    signal.annotations.clear();
    signal.outputArgs << arg("s");
    signal.name = "Signal";
    map.clear();
    map << signal;
    QTest::newRow("one-out") <<
        "<signal name=\"Signal\">"
        "<arg type=\"s\" direction=\"out\"/>"
        "</signal>" << map;

    // without saying which direction it is
    QTest::newRow("one-out-no-direction") <<
        "<signal name=\"Signal\">"
        "<arg type=\"s\"/>"
        "</signal>" << map;    

    // two args with name
    signal.outputArgs << arg("i", "bar");
    map.clear();
    map << signal;
    QTest::newRow("two-out-with-name") <<
        "<signal name=\"Signal\">"
        "<arg type=\"s\" direction=\"out\"/>"
        "<arg type=\"i\" name=\"bar\"/>"
        "</signal>" << map;

    // one complex
    map.clear();
    signal = QDBusIntrospection::Signal();

    // Signal1(out ARRAY of STRING)
    signal.outputArgs << arg("as");
    signal.name = "Signal1";
    map << signal;

    // Signal2(out STRING key, out VARIANT value)
    // with annotation "foo.equivalent":"QVariantMap"
    signal = QDBusIntrospection::Signal();
    signal.outputArgs << arg("s", "key") << arg("v", "value");
    signal.annotations.insert("foo.equivalent", "QVariantMap");
    signal.name = "Signal2";
    map << signal;

    QTest::newRow("complex") <<
        "<signal name=\"Signal1\">"
        "<arg type=\"as\" direction=\"out\"/>"
        "</signal>"
        "<signal name=\"Signal2\">"
        "<arg name=\"key\" type=\"s\" direction=\"out\"/>"
        "<arg name=\"value\" type=\"v\" direction=\"out\"/>"
        "<annotation name=\"foo.equivalent\" value=\"QVariantMap\"/>"
        "</signal>" << map;
}

void tst_QDBusXmlParser::signals_()
{
    QString xmlHeader = "<node>"
                        "<interface name=\"iface.iface1\">",
            xmlFooter = "</interface>"
                        "</node>";

    QFETCH(QString, xmlDataFragment);

    QDBusIntrospection::Interface iface =
        QDBusIntrospection::parseInterface(xmlHeader + xmlDataFragment + xmlFooter);

    QCOMPARE(iface.name, QString("iface.iface1"));

    QFETCH(SignalMap, signalMap);
    SignalMap parsedMap = iface.signals_;

    QCOMPARE(signalMap.count(), parsedMap.count());
    QCOMPARE(signalMap, parsedMap);
}

void tst_QDBusXmlParser::properties_data()
{
    QTest::addColumn<QString>("xmlDataFragment");
    QTest::addColumn<PropertyMap>("propertyMap");

    PropertyMap map;
    QTest::newRow("no-signals") << QString() << map;

    // one readable signal
    QDBusIntrospection::Property prop;
    prop.name = "foo";
    prop.type = "s";
    prop.access = QDBusIntrospection::Property::Read;
    map << prop;
    QTest::newRow("one-readable") << "<property name=\"foo\" type=\"s\" access=\"read\"/>" << map;

    // one writable signal
    prop.access = QDBusIntrospection::Property::Write;
    map.clear();
    map << prop;
    QTest::newRow("one-writable") << "<property name=\"foo\" type=\"s\" access=\"write\"/>" << map;

    // one read- & writable signal
    prop.access = QDBusIntrospection::Property::ReadWrite;
    map.clear();
    map << prop;
    QTest::newRow("one-read-writable") << "<property name=\"foo\" type=\"s\" access=\"readwrite\"/>"
                                       << map;

    // two, mixed properties
    prop.name = "bar";
    prop.type = "i";
    prop.access = QDBusIntrospection::Property::Read;
    map << prop;
    QTest::newRow("two") <<
        "<property name=\"foo\" type=\"s\" access=\"readwrite\"/>"
        "<property name=\"bar\" type=\"i\" access=\"read\"/>" << map;

    // invert the order of the declaration
    QTest::newRow("two") <<
        "<property name=\"bar\" type=\"i\" access=\"read\"/>"
        "<property name=\"foo\" type=\"s\" access=\"readwrite\"/>" << map;

    // add a third with annotations
    prop.name = "baz";
    prop.type = "as";
    prop.access = QDBusIntrospection::Property::Write;
    prop.annotations.insert("foo.annotation", "Hello, World");
    prop.annotations.insert("foo.annotation2", "Goodbye, World");
    map << prop;
    QTest::newRow("complex") <<
        "<property name=\"bar\" type=\"i\" access=\"read\"/>"
        "<property name=\"baz\" type=\"as\" access=\"write\">"
        "<annotation name=\"foo.annotation\" value=\"Hello, World\" />"
        "<annotation name=\"foo.annotation2\" value=\"Goodbye, World\" />"
        "<property name=\"foo\" type=\"s\" access=\"readwrite\"/>" << map;

    // and now change the order
    QTest::newRow("complex2") <<
        "<property name=\"baz\" type=\"as\" access=\"write\">"
        "<annotation name=\"foo.annotation2\" value=\"Goodbye, World\" />"
        "<annotation name=\"foo.annotation\" value=\"Hello, World\" />"
        "<property name=\"bar\" type=\"i\" access=\"read\"/>"
        "<property name=\"foo\" type=\"s\" access=\"readwrite\"/>" << map;
}

void tst_QDBusXmlParser::properties()
{
    QString xmlHeader = "<node>"
                        "<interface name=\"iface.iface1\">",
            xmlFooter = "</interface>"
                        "</node>";

    QFETCH(QString, xmlDataFragment);

    QDBusIntrospection::Interface iface =
        QDBusIntrospection::parseInterface(xmlHeader + xmlDataFragment + xmlFooter);

    QCOMPARE(iface.name, QString("iface.iface1"));

    QFETCH(PropertyMap, propertyMap);
    PropertyMap parsedMap = iface.properties;

    QCOMPARE(propertyMap.count(), parsedMap.count());
    QCOMPARE(propertyMap, parsedMap);
}

QTEST_MAIN(tst_QDBusXmlParser)

#include "tst_qdbusxmlparser.moc"
