/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativelistmodel_p.h"

#include "qdeclarativeopenmetaobject_p.h"

#include <qdeclarativecustomparser_p.h>
#include <qdeclarativeparser_p.h>
#include <qdeclarativeengine_p.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeinfo.h>

#include <QtCore/qdebug.h>
#include <QtCore/qstack.h>
#include <QXmlStreamReader>
#include <QtScript/qscriptvalueiterator.h>

Q_DECLARE_METATYPE(QListModelInterface *)

QT_BEGIN_NAMESPACE

#define DATA_ROLE_ID 1
#define DATA_ROLE_NAME "data"

QDeclarativeListModelParser::ListInstruction *QDeclarativeListModelParser::ListModelData::instructions() const
{
    return (QDeclarativeListModelParser::ListInstruction *)((char *)this + sizeof(ListModelData));
}

static void dump(ModelNode *node, int ind);

/*!
    \qmlclass ListModel QDeclarativeListModel
    \since 4.7
    \brief The ListModel element defines a free-form list data source.

    The ListModel is a simple hierarchy of elements containing data roles. The contents can
    be defined dynamically, or explicitly in QML:

    For example:

    \code
    ListModel {
        id: fruitModel
        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
    }
    \endcode

    Roles (properties) must begin with a lower-case letter.  The above example defines a
    ListModel containing three elements, with the roles "name" and "cost".

    The defined model can be used in views such as ListView:
    \code
    Component {
        id: fruitDelegate
        Item {
            width: 200; height: 50
            Text { text: name }
            Text { text: '$'+cost; anchors.right: parent.right }
        }
    }

    ListView {
        model: fruitModel
        delegate: fruitDelegate
        anchors.fill: parent
    }
    \endcode

    It is possible for roles to contain list data.  In the example below we create a list of fruit attributes:

    \code
    ListModel {
        id: fruitModel
        ListElement {
            name: "Apple"
            cost: 2.45
            attributes: [
                ListElement { description: "Core" },
                ListElement { description: "Deciduous" }
            ]
        }
        ListElement {
            name: "Orange"
            cost: 3.25
            attributes: [
                ListElement { description: "Citrus" }
            ]
        }
        ListElement {
            name: "Banana"
            cost: 1.95
            attributes: [
                ListElement { description: "Tropical" },
                ListElement { description: "Seedless" }
            ]
        }
    }
    \endcode

    The delegate below will list all the fruit attributes:
    \code
    Component {
        id: fruitDelegate
        Item {
            width: 200; height: 50
            Text { id: name; text: name }
            Text { text: '$'+cost; anchors.right: parent.right }
            Row {
                anchors.top: name.bottom
                spacing: 5
                Text { text: "Attributes:" }
                Repeater {
                    dataSource: attributes
                    Component { Text { text: description } }
                }
            }
        }
    }
    \endcode

    The content of a ListModel may be created and modified using the clear(),
    append(), and set() methods.  For example:

    \code
    Component {
        id: fruitDelegate
        Item {
            width: 200; height: 50
            Text { text: name }
            Text { text: '$'+cost; anchors.right: parent.right }

            // Double the price when clicked.
            MouseArea {
                anchors.fill: parent
                onClicked: fruitModel.set(index, "cost", cost*2)
            }
        }
    }
    \endcode

    When creating content dynamically, note that the set of available properties cannot be changed
    except by first clearing the model - whatever properties are first added are then the
    only permitted properties in the model.

    \sa {qmlmodels}{Data Models}
*/

class ModelObject : public QObject
{
    Q_OBJECT
public:
    ModelObject();

    void setValue(const QByteArray &name, const QVariant &val)
    {
        _mo->setValue(name, val);
    }

private:
    QDeclarativeOpenMetaObject *_mo;
};

struct ModelNode
{
    ModelNode();
    ~ModelNode();

    QList<QVariant> values;
    QHash<QString, ModelNode *> properties;

    QDeclarativeListModel *model(const QDeclarativeListModel *parent) {
        if (!modelCache) { 
            modelCache = new QDeclarativeListModel;
            QDeclarativeEngine::setContextForObject(modelCache,QDeclarativeEngine::contextForObject(parent));

            modelCache->_root = this; 
        }
        return modelCache;
    }

    ModelObject *object(const QDeclarativeListModel *parent) {
        if (!objectCache) {
            objectCache = new ModelObject();
            QHash<QString, ModelNode *>::iterator it;
            for (it = properties.begin(); it != properties.end(); ++it) {
                objectCache->setValue(it.key().toUtf8(), parent->valueForNode(*it));
            }
        }
        return objectCache;
    }

    void setObjectValue(const QScriptValue& valuemap);
    void setListValue(const QScriptValue& valuelist);

    void setProperty(const QString& prop, const QVariant& val) {
        QHash<QString, ModelNode *>::const_iterator it = properties.find(prop);
        if (it != properties.end()) {
            (*it)->values[0] = val;
        } else {
            ModelNode *n = new ModelNode;
            n->values << val;
            properties.insert(prop,n);
        }
        if (objectCache)
            objectCache->setValue(prop.toUtf8(), val);
    }

    QDeclarativeListModel *modelCache;
    ModelObject *objectCache;
    bool isArray;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(ModelNode *)

QT_BEGIN_NAMESPACE

void ModelNode::setObjectValue(const QScriptValue& valuemap) {
    QScriptValueIterator it(valuemap);
    while (it.hasNext()) {
        it.next();
        ModelNode *value = new ModelNode;
        QScriptValue v = it.value();
        if (v.isArray()) {
            value->isArray = true;
            value->setListValue(v);
        } else {
            value->values << v.toVariant();
        }
        properties.insert(it.name(),value);
    }
}

void ModelNode::setListValue(const QScriptValue& valuelist) {
    int size = valuelist.property(QString::fromLatin1("length")).toInt32();
    values.clear();
    for (int i = 0; i < size; ++i) {
        ModelNode *value = new ModelNode;
        QScriptValue v = valuelist.property(i);
        if (v.isArray()) {
            value->isArray = true;
            value->setListValue(v);
        } else if (v.isObject()) {
            value->setObjectValue(v);
        } else {
            value->values << v.toVariant();
        }
        values.append(qVariantFromValue(value));

    }
}


ModelObject::ModelObject()
: _mo(new QDeclarativeOpenMetaObject(this))
{
}

QDeclarativeListModel::QDeclarativeListModel(QObject *parent)
: QListModelInterface(parent), _rolesOk(false), _root(0)
{
}

QDeclarativeListModel::~QDeclarativeListModel()
{
    delete _root;
}

void QDeclarativeListModel::checkRoles() const
{
    if (_rolesOk || !_root)
        return;

    for (int ii = 0; ii < _root->values.count(); ++ii) {
        ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(ii));
        if (node) {
            foreach (const QString &role, node->properties.keys())
                addRole(role);
        } 
    }

    _rolesOk = true;
}

void QDeclarativeListModel::addRole(const QString &role) const
{
    if (!roleStrings.contains(role))
        roleStrings << role;
}

QList<int> QDeclarativeListModel::roles() const
{
    checkRoles();
    QList<int> rv;
    for (int ii = 0; ii < roleStrings.count(); ++ii)
        rv << ii;
    return rv;
}

QString QDeclarativeListModel::toString(int role) const
{
    checkRoles();
    if (role < roleStrings.count())
        return roleStrings.at(role);
    else
        return QString();
}

QVariant QDeclarativeListModel::valueForNode(ModelNode *node) const
{
    QObject *rv = 0;

    if (node->isArray) {
        // List
        rv = node->model(this);
    } else {
        if (!node->properties.isEmpty()) {
            // Object
            rv = node->object(this);
        } else if (node->values.count() == 0) {
            // Invalid
            return QVariant();
        } else if (node->values.count() == 1) {
            // Value
            QVariant &var = node->values[0];
            ModelNode *valueNode = qvariant_cast<ModelNode *>(var);
            if (valueNode) {
                if (!valueNode->properties.isEmpty())
                    rv = valueNode->object(this);
                else
                    rv = valueNode->model(this);
            } else {
                return var;
            }
        }
    }

    if (rv)
        return QVariant::fromValue(rv);
    else
        return QVariant();
}

QHash<int,QVariant> QDeclarativeListModel::data(int index, const QList<int> &roles) const
{
    checkRoles();
    QHash<int, QVariant> rv;
    if (index >= count() || index < 0)
        return rv;

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    if (!node) 
        return rv;

    for (int ii = 0; ii < roles.count(); ++ii) {
        const QString &roleString = roleStrings.at(roles.at(ii));

        QHash<QString, ModelNode *>::ConstIterator iter = 
            node->properties.find(roleString);
        if (iter != node->properties.end()) {
            ModelNode *row = *iter;
            rv.insert(roles.at(ii), valueForNode(row));
        }
    }

    return rv;
}

QVariant QDeclarativeListModel::data(int index, int role) const
{
    checkRoles();
    QVariant rv;
    if (index >= count() || index < 0)
        return rv;

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    if (!node)
        return rv;

    const QString &roleString = roleStrings.at(role);

    QHash<QString, ModelNode *>::ConstIterator iter =
        node->properties.find(roleString);
    if (iter != node->properties.end()) {
        ModelNode *row = *iter;
        rv = valueForNode(row);
    }

    return rv;
}

/*!
    \qmlproperty int ListModel::count
    The number of data entries in the model.
*/
int QDeclarativeListModel::count() const
{
    if (!_root) return 0;
    return _root->values.count();
}

/*!
    \qmlmethod ListModel::clear()

    Deletes all content from the model. The properties are cleared such that
    different properties may be set on subsequent additions.

    \sa append() remove()
*/
void QDeclarativeListModel::clear()
{
    int cleared = count();
    _rolesOk = false;
    delete _root;
    _root = 0;
    roleStrings.clear();
    emit itemsRemoved(0,cleared);
    emit countChanged(0);
}

/*!
    \qmlmethod ListModel::remove(int index)

    Deletes the content at \a index from the model.

    \sa clear()
*/
void QDeclarativeListModel::remove(int index)
{
    if (!_root || index < 0 || index >= _root->values.count()) {
        qmlInfo(this) << tr("remove: index %1 out of range").arg(index);
        return;
    }

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    _root->values.removeAt(index);
    if (node)
        delete node;
    emit itemsRemoved(index,1);
    emit countChanged(_root->values.count());
}

/*!
    \qmlmethod ListModel::insert(int index, jsobject dict)

    Adds a new item to the list model at position \a index, with the
    values in \a dict.

    \code
        FruitModel.insert(2, {"cost": 5.95, "name":"Pizza"})
    \endcode

    The \a index must be to an existing item in the list, or one past
    the end of the list (equivalent to append).

    \sa set() append()
*/
void QDeclarativeListModel::insert(int index, const QScriptValue& valuemap)
{
    if (!valuemap.isObject() || valuemap.isArray()) {
        qmlInfo(this) << tr("insert: value is not an object");
        return;
    }
    if (!_root)
        _root = new ModelNode;
    if (index >= _root->values.count() || index<0) {
        if (index == _root->values.count())
            append(valuemap);
        else
            qmlInfo(this) << tr("insert: index %1 out of range").arg(index);
        return;
    }
    ModelNode *mn = new ModelNode;
    mn->setObjectValue(valuemap);
    _root->values.insert(index,qVariantFromValue(mn));
    emit itemsInserted(index,1);
    emit countChanged(_root->values.count());
}

/*!
    \qmlmethod ListModel::move(int from, int to, int n)

    Moves \a n items \a from one position \a to another.

    The from and to ranges must exist; for example, to move the first 3 items
    to the end of the list:

    \code
        FruitModel.move(0,FruitModel.count-3,3)
    \endcode

    \sa append()
*/
void QDeclarativeListModel::move(int from, int to, int n)
{
    if (n==0 || from==to)
        return;
    if (from+n > count() || to+n > count() || from < 0 || to < 0 || n < 0) {
        qmlInfo(this) << tr("move: out of range");
        return;
    }
    int origfrom=from; // preserve actual move, so any animations are correct
    int origto=to;
    int orign=n;
    if (from > to) {
        // Only move forwards - flip if backwards moving
        int tfrom = from;
        int tto = to;
        from = tto;
        to = tto+n;
        n = tfrom-tto;
    }
    if (n==1) {
        _root->values.move(from,to);
    } else {
        QList<QVariant> replaced;
        int i=0;
        QVariantList::const_iterator it=_root->values.begin(); it += from+n;
        for (; i<to-from; ++i,++it)
            replaced.append(*it);
        i=0;
        it=_root->values.begin(); it += from;
        for (; i<n; ++i,++it)
            replaced.append(*it);
        QVariantList::const_iterator f=replaced.begin();
        QVariantList::iterator t=_root->values.begin(); t += from;
        for (; f != replaced.end(); ++f, ++t)
            *t = *f;
    }
    emit itemsMoved(origfrom,origto,orign);
}

/*!
    \qmlmethod ListModel::append(jsobject dict)

    Adds a new item to the end of the list model, with the
    values in \a dict.

    \code
        FruitModel.append({"cost": 5.95, "name":"Pizza"})
    \endcode

    \sa set() remove()
*/
void QDeclarativeListModel::append(const QScriptValue& valuemap)
{
    if (!valuemap.isObject() || valuemap.isArray()) {
        qmlInfo(this) << tr("append: value is not an object");
        return;
    }
    if (!_root)
        _root = new ModelNode;
    ModelNode *mn = new ModelNode;
    mn->setObjectValue(valuemap);
    _root->values << qVariantFromValue(mn);
    emit itemsInserted(count()-1,1);
    emit countChanged(_root->values.count());
}

/*!
    \qmlmethod object ListModel::get(int index)

    Returns the item at \a index in the list model.

    \code
        FruitModel.append({"cost": 5.95, "name":"Jackfruit"})
        FruitModel.get(0).cost
    \endcode

    The \a index must be an element in the list.

    Note that properties of the returned object that are themselves objects
    will also be models, and this get() method is used to access elements:

    \code
        FruitModel.append(..., "attributes":
            [{"name":"spikes","value":"7mm"},
             {"name":"color","value":"green"}]);
        FruitModel.get(0).attributes.get(1).value; // == "green"
    \endcode

    \sa append()
*/
QScriptValue QDeclarativeListModel::get(int index) const
{
    if (index >= count() || index < 0) {
        qmlInfo(this) << tr("get: index %1 out of range").arg(index);
        return 0;
    }

    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    if (!node) 
        return 0;
    QDeclarativeEngine *eng = qmlEngine(this);
    if (!eng) {
        qWarning("Cannot call QDeclarativeListModel::get() without a QDeclarativeEngine");
        return 0;
    }
    return QDeclarativeEnginePrivate::qmlScriptObject(node->object(this), eng);
}

/*!
    \qmlmethod ListModel::set(int index, jsobject dict)

    Changes the item at \a index in the list model with the
    values in \a dict. Properties not appearing in \a valuemap
    are left unchanged.

    \code
        FruitModel.set(3, {"cost": 5.95, "name":"Pizza"})
    \endcode

    The \a index must be an element in the list.

    \sa append()
*/
void QDeclarativeListModel::set(int index, const QScriptValue& valuemap)
{
    if (!valuemap.isObject() || valuemap.isArray()) {
        qmlInfo(this) << tr("set: value is not an object");
        return;
    }
    if ( !_root || index > _root->values.count() || index < 0) {
        qmlInfo(this) << tr("set: index %1 out of range").arg(index);
        return;
    }
    if (index == _root->values.count())
        append(valuemap);
    else {
        ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
        QList<int> roles;
        node->setObjectValue(valuemap);
        QScriptValueIterator it(valuemap);
        while (it.hasNext()) {
            it.next();
            int r = roleStrings.indexOf(it.name());
            if (r<0) {
                r = roleStrings.count();
                roleStrings << it.name();
            }
            roles.append(r);
        }
        emit itemsChanged(index,1,roles);
    }
}

/*!
    \qmlmethod ListModel::setProperty(int index, string property, variant value)

    Changes the \a property of the item at \a index in the list model to \a value.

    \code
        FruitModel.set(3, "cost", 5.95)
    \endcode

    The \a index must be an element in the list.

    \sa append()
*/
void QDeclarativeListModel::setProperty(int index, const QString& property, const QVariant& value)
{
    if ( !_root || index >= _root->values.count() || index < 0) {
        qmlInfo(this) << tr("set: index %1 out of range").arg(index);
        return;
    }
    ModelNode *node = qvariant_cast<ModelNode *>(_root->values.at(index));
    int r = roleStrings.indexOf(property);
    if (r<0) {
        r = roleStrings.count();
        roleStrings << property;
    }
    QList<int> roles;
    roles.append(r);

    if (node)
        node->setProperty(property,value);
    emit itemsChanged(index,1,roles);
}

bool QDeclarativeListModelParser::compileProperty(const QDeclarativeCustomParserProperty &prop, QList<ListInstruction> &instr, QByteArray &data)
{
    QList<QVariant> values = prop.assignedValues();
    for(int ii = 0; ii < values.count(); ++ii) {
        const QVariant &value = values.at(ii);

        if(value.userType() == qMetaTypeId<QDeclarativeCustomParserNode>()) {
            QDeclarativeCustomParserNode node = 
                qvariant_cast<QDeclarativeCustomParserNode>(value);

            {
            ListInstruction li;
            li.type = ListInstruction::Push;
            li.dataIdx = -1;
            instr << li;
            }

            QList<QDeclarativeCustomParserProperty> props = node.properties();
            for(int jj = 0; jj < props.count(); ++jj) {
                const QDeclarativeCustomParserProperty &nodeProp = props.at(jj);
                if (nodeProp.name() == "") {
                    error(nodeProp, QDeclarativeListModel::tr("ListElement: cannot use default property"));
                    return false;
                }
                if (nodeProp.name() == "id") {
                    error(nodeProp, QDeclarativeListModel::tr("ListElement: cannot use reserved \"id\" property"));
                    return false;
                }

                ListInstruction li;
                int ref = data.count();
                data.append(nodeProp.name());
                data.append('\0');
                li.type = ListInstruction::Set;
                li.dataIdx = ref;
                instr << li;

                if(!compileProperty(nodeProp, instr, data))
                    return false;

                li.type = ListInstruction::Pop;
                li.dataIdx = -1;
                instr << li;
            }

            {
            ListInstruction li;
            li.type = ListInstruction::Pop;
            li.dataIdx = -1;
            instr << li;
            }

        } else {

            QDeclarativeParser::Variant variant = 
                qvariant_cast<QDeclarativeParser::Variant>(value);

            int ref = data.count();

            QByteArray d;
            d += char(variant.type()); // type tag
            if (variant.isString()) {
                d += variant.asString().toUtf8();
            } else if (variant.isNumber()) {
                d += QByteArray::number(variant.asNumber(),'g',20);
            } else if (variant.isBoolean()) {
                d += char(variant.asBoolean());
            } else if (variant.isScript()) {
                if (definesEmptyList(variant.asScript())) {
                    d[0] = 0; // QDeclarativeParser::Variant::Invalid - marks empty list
                } else {
                    error(prop, QDeclarativeListModel::tr("ListElement: cannot use script for property value"));
                    return false;
                }
            }
            d.append('\0');
            data.append(d);

            ListInstruction li;
            li.type = ListInstruction::Value;
            li.dataIdx = ref;
            instr << li;
        }
    }

    return true;
}

QByteArray QDeclarativeListModelParser::compile(const QList<QDeclarativeCustomParserProperty> &customProps)
{
    QList<ListInstruction> instr;
    QByteArray data;

    for(int ii = 0; ii < customProps.count(); ++ii) {
        const QDeclarativeCustomParserProperty &prop = customProps.at(ii);
        if(prop.name() != "") { // isn't default property
            error(prop, QDeclarativeListModel::tr("ListModel: undefined property '%1'").arg(QString::fromUtf8(prop.name())));
            return QByteArray();
        }

        if(!compileProperty(prop, instr, data)) {
            return QByteArray();
        }
    }

    int size = sizeof(ListModelData) + 
               instr.count() * sizeof(ListInstruction) + 
               data.count();

    QByteArray rv;
    rv.resize(size);

    ListModelData *lmd = (ListModelData *)rv.data();
    lmd->dataOffset = sizeof(ListModelData) + 
                     instr.count() * sizeof(ListInstruction);
    lmd->instrCount = instr.count();
    for (int ii = 0; ii < instr.count(); ++ii)
        lmd->instructions()[ii] = instr.at(ii);
    ::memcpy(rv.data() + lmd->dataOffset, data.constData(), data.count());

    return rv;
}

void QDeclarativeListModelParser::setCustomData(QObject *obj, const QByteArray &d)
{
    QDeclarativeListModel *rv = static_cast<QDeclarativeListModel *>(obj);

    ModelNode *root = new ModelNode;
    rv->_root = root;
    QStack<ModelNode *> nodes;
    nodes << root;

    bool processingSet = false;

    const ListModelData *lmd = (const ListModelData *)d.constData();
    const char *data = ((const char *)lmd) + lmd->dataOffset;

    for (int ii = 0; ii < lmd->instrCount; ++ii) {
        const ListInstruction &instr = lmd->instructions()[ii];

        switch(instr.type) {
        case ListInstruction::Push:
            {
                ModelNode *n = nodes.top();
                ModelNode *n2 = new ModelNode;
                n->values << qVariantFromValue(n2);
                nodes.push(n2);
                if (processingSet)
                    n->isArray = true;
            }
            break;

        case ListInstruction::Pop:
            nodes.pop();
            break;

        case ListInstruction::Value:
            {
                ModelNode *n = nodes.top();
                switch (QDeclarativeParser::Variant::Type(data[instr.dataIdx])) {
                 case QDeclarativeParser::Variant::Invalid:
                    n->isArray = true;
                    break;
                 case QDeclarativeParser::Variant::Boolean:
                    n->values.append(bool(data[1 + instr.dataIdx]));
                    break;
                 case QDeclarativeParser::Variant::Number:
                    n->values.append(QByteArray(data + 1 + instr.dataIdx).toDouble());
                    break;
                 case QDeclarativeParser::Variant::String:
                    n->values.append(QString::fromUtf8(data + 1 + instr.dataIdx));
                    break;
                 default:
                    Q_ASSERT("Format error in ListInstruction");
                }

                processingSet = false;
            }
            break;

        case ListInstruction::Set:
            {
                ModelNode *n = nodes.top();
                ModelNode *n2 = new ModelNode;
                n->properties.insert(QString::fromUtf8(data + instr.dataIdx), n2);
                nodes.push(n2);
                processingSet = true;
            }
            break;
        }
    }
}

bool QDeclarativeListModelParser::definesEmptyList(const QString &s)
{
    if (s.startsWith(QLatin1Char('[')) && s.endsWith(QLatin1Char(']'))) {
        for (int i=1; i<s.length()-1; i++) {
            if (!s[i].isSpace())
                return false;
        }
        return true;
    }
    return false;
}

/*!
    \qmlclass ListElement
    \since 4.7
    \brief The ListElement element defines a data item in a ListModel.

    \sa ListModel
*/

static void dump(ModelNode *node, int ind)
{
    QByteArray indentBa(ind * 4, ' ');
    const char *indent = indentBa.constData();

    for (int ii = 0; ii < node->values.count(); ++ii) {
        ModelNode *subNode = qvariant_cast<ModelNode *>(node->values.at(ii));
        if (subNode) {
            qWarning().nospace() << indent << "Sub-node " << ii;
            dump(subNode, ind + 1);
        } else {
            qWarning().nospace() << indent << "Sub-node " << ii << ": " << node->values.at(ii).toString();
        }
    }

    for (QHash<QString, ModelNode *>::ConstIterator iter = node->properties.begin(); iter != node->properties.end(); ++iter) {
        qWarning().nospace() << indent << "Property " << iter.key() << ':';
        dump(iter.value(), ind + 1);
    }
}

ModelNode::ModelNode()
: modelCache(0), objectCache(0), isArray(false)
{
}

ModelNode::~ModelNode()
{
    qDeleteAll(properties);
    for (int ii = 0; ii < values.count(); ++ii) {
        ModelNode *node = qvariant_cast<ModelNode *>(values.at(ii));
        if (node) { delete node; node = 0; }
    }
    if (modelCache) { modelCache->_root = 0/* ==this */; delete modelCache; modelCache = 0; }
    if (objectCache) { delete objectCache; }
}

QT_END_NAMESPACE

#include <qdeclarativelistmodel.moc>
