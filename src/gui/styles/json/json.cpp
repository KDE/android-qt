/****************************************************************************
**
** Copyright (c) 2010 Girish Ramakrishnan <girish@forwardbias.in>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "json.h"
#include "jsonparser.cpp"

#include <QTextCodec>
#include <qnumeric.h>

/*!
  \class JsonReader
  \internal
  \reentrant

  \brief The JsonReader class provides a fast parser for reading
  well-formed JSON into a QVariant.

  The parser converts JSON types into QVariant types. For example, JSON
  arrays are translated into QVariantList and JSON objects are translated 
  into QVariantMap. For example,
  \code
  JsonReader reader;
  if (reader.parse("{ \"id\": 123, \"company\": \"Nokia\", \"authors\": [\"Denis\",\"Ettrich\",\"Girish\"])) {
    QVariant result = reader.result();
    QVariantMap map = result.toMap(); // the JSON object
    qDebug() << map.count(); // 3
    qDebug() << map["id"].toInt(); // 123
    qDebug() << map["company"].toString(); // "Nokia"
    QVariantList list = map["authors"].toList();
    qDebug() << list[1].toString(); // "Girish"
  } else {
    qDebug() << reader.errorString();
  }
  \endcode

  As seen above, the reader converts the JSON into a QVariant with arbitrary nesting.
  A complete listing of JSON to QVariant conversion is documented at parse().

  JsonWriter can be used to convert a QVariant into JSON string.
*/

/*!
  Constructs a JSON reader.
 */
JsonReader::JsonReader()
{
}

/*!
  Destructor
 */
JsonReader::~JsonReader()
{
}

/*!
  Parses the JSON \a ba as a QVariant.

  If the parse succeeds, this function returns true and the QVariant can
  be accessed using result(). If the parse fails, this function returns
  false and the error message can be accessed using errorMessage().

  The encoding of \ba is auto-detected based on the pattern of nulls in the
  initial 4 octets as described in "Section 3. Encoding" of RFC 2647. If an 
  encoding could not be auto-detected, this function assumes UTF-8.

  The conversion from JSON type into QVariant type is as listed below:
  \table
  \row
  \li false
  \li QVariant::Bool with the value false.
  \row
  \li true
  \li QVariant::Bool with the value true.
  \row
  \li null
  \li QVariant::Invalid i.e QVariant()
  \row
  \li object
  \li QVariant::Map i.e QVariantMap
  \row
  \li array
  \li QVariant::List i.e QVariantList
  \row
  \li string
  \li QVariant::String i.e QString
  \row
  \li number
  \li QVariant::Double or QVariant::LongLong. If the JSON number contains a '.' or 'e' 
     or 'E', QVariant::Double is used.
  \endtable

  The byte array \ba may or may not contain a BOM.
 */
bool JsonReader::parse(const QByteArray &ba)
{
    int mib = 106; // utf-8

    QTextCodec *codec = QTextCodec::codecForUtfText(ba, 0); // try BOM detection
    if (!codec) {
        if (ba.length() > 3) { // auto-detect
            const char *data = ba.constData();
            if (data[0] != 0) {
                if (data[1] != 0)
                    mib = 106; // utf-8
                else if (data[2] != 0)
                    mib = 1014; // utf16 le
                else
                    mib = 1019; // utf32 le
            } else if (data[1] != 0)
                mib = 1013; // utf16 be
            else
                mib = 1018; // utf32 be
        }
        codec = QTextCodec::codecForMib(mib);
    }
    QString str = codec->toUnicode(ba);
    return parse(str);
}

/*!
  Parses the JSON string \a str as a QVariant.

  If the parse succeeds, this function returns true and the QVariant can
  be accessed using result(). If the parse fails, this function returns
  false and the error message can be accessed using errorMessage().
 */
bool JsonReader::parse(const QString &str)
{
    JsonLexer lexer(str);
    JsonParser parser;
    if (!parser.parse(&lexer)) {
        m_errorString = parser.errorMessage();
        m_result = QVariant();
        return false;
    }
    m_errorString.clear();
    m_result = parser.result();
    return true;
}

/*!
  Returns the result of the last parse() call.

  If parse() failed, this function returns an invalid QVariant.
 */
QVariant JsonReader::result() const
{
    return m_result;
}

/*!
  Returns the error message for the last parse() call.

  If parse() succeeded, this functions return an empty string. The error message
  should be used for debugging purposes only.
 */
QString JsonReader::errorString() const
{
    return m_errorString;
}

/*!
  \class JsonWriter
  \internal
  \reentrant

  \brief The JsonWriter class converts a QVariant into a JSON string.

  The writer converts specific supported types stored in a QVariant into JSON.
  For example,
  \code
    QVariant v;
    QVariantMap map;
    map["id"] = 123;
    map["company"] = "Nokia";
    QVariantList list;
    list << "Denis" << "Ettrich" << "Girish";
    map["authors"] = list;

    JsonWriter writer;
    QString json = writer.toString(v);
    qDebug() << json; // {"authors": ["Denis", "Ettrich", "Girish"], "company": "Nokia", "id": 123 }
  \endcode

  The list of QVariant types that the writer supports is listed in toString(). Note that
  custom C++ types registered using Q_DECLARE_METATYPE are not supported.
*/

/*!
  Creates a JsonWriter.
 */
JsonWriter::JsonWriter()
    : m_autoFormatting(false), m_autoFormattingIndent(4, QLatin1Char(' '))
{
}

/*!
  Destructor.
 */
JsonWriter::~JsonWriter()
{
}

/*!
  Enables auto formatting if \a enable is \c true, otherwise
  disables it.
 
  When auto formatting is enabled, the writer automatically inserts
  spaces and new lines to make the output more human readable.
 
  The default value is \c false.
 */
void JsonWriter::setAutoFormatting(bool enable)
{
    m_autoFormatting = enable;
}

/*!
  Returns \c true if auto formattting is enabled, otherwise \c false.
 */
bool JsonWriter::autoFormatting() const
{
    return m_autoFormatting;
}

/*!
  Sets the number of spaces or tabs used for indentation when
  auto-formatting is enabled. Positive numbers indicate spaces,
  negative numbers tabs.

  The default indentation is 4.

  \sa setAutoFormatting()
*/
void JsonWriter::setAutoFormattingIndent(int spacesOrTabs)
{
    m_autoFormattingIndent = QString(qAbs(spacesOrTabs), QLatin1Char(spacesOrTabs >= 0 ? ' ' : '\t'));
}

/*!
  Retuns the numbers of spaces or tabs used for indentation when
  auto-formatting is enabled. Positive numbers indicate spaces,
  negative numbers tabs.

  The default indentation is 4.

  \sa setAutoFormatting()
*/
int JsonWriter::autoFormattingIndent() const
{
    return m_autoFormattingIndent.count(QLatin1Char(' ')) - m_autoFormattingIndent.count(QLatin1Char('\t'));
}

/*! \internal
  Inserts escape character \ for characters in string as described in JSON specification.
 */
static QString escape(const QVariant &variant)
{
    QString str = variant.toString();
    QString res;
    res.reserve(str.length());
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == QLatin1Char('\b')) {
            res += QLatin1String("\\b");
        } else if (str[i] == QLatin1Char('\f')) {
            res += QLatin1String("\\f");
        } else if (str[i] == QLatin1Char('\n')) {
            res += QLatin1String("\\n");
        } else if (str[i] == QLatin1Char('\r')) {
            res += QLatin1String("\\r");
        } else if (str[i] == QLatin1Char('\t')) {
            res += QLatin1String("\\t");
        } else if (str[i] == QLatin1Char('\"')) {
            res += QLatin1String("\\\"");
        } else if (str[i] == QLatin1Char('\\')) {
            res += QLatin1String("\\\\");
        } else if (str[i] == QLatin1Char('/')) {
            res += QLatin1String("\\/");
        } else if (str[i].unicode() > 127) {
            res += QLatin1String("\\u") + QString::number(str[i].unicode(), 16).rightJustified(4, QLatin1Char('0'));
        } else {
            res += str[i];
        }
    }
    return res;
}

/*! \internal
  Stringifies \a variant.
 */
QString JsonWriter::stringify(const QVariant &variant, int depth)
{
    QString result;
    if (variant.type() == QVariant::List || variant.type() == QVariant::StringList) {
        result += QLatin1Char('[');
        QVariantList list = variant.toList();
        for (int i = 0; i < list.count(); i++) {
            if (i != 0) {
                result += QLatin1Char(',');
                if (m_autoFormatting)
                    result += QLatin1Char(' ');
            }
            result += stringify(list[i], depth+1);
        }
        result += QLatin1Char(']');
    } else if (variant.type() == QVariant::Map) {
        QString indent = m_autoFormattingIndent.repeated(depth);
        QVariantMap map = variant.toMap();
        if (m_autoFormatting && depth != 0) {
            result += QLatin1Char('\n');
            result += indent;
            result += QLatin1String("{\n");
        } else {
            result += QLatin1Char('{');
        }
        for (QVariantMap::const_iterator it = map.constBegin(); it != map.constEnd(); ++it) {
            if (it != map.constBegin()) {
                result += QLatin1Char(',');
                if (m_autoFormatting)
                    result += QLatin1Char('\n');
            }
            if (m_autoFormatting)
                result += indent + QLatin1Char(' ');
            result += QLatin1Char('\"') + escape(it.key()) + QLatin1String("\":");
            result += stringify(it.value(), depth+1);
        }
        if (m_autoFormatting) {
            result += QLatin1Char('\n');
            result += indent;
        }
        result += QLatin1Char('}');
    } else if (variant.type() == QVariant::String || variant.type() == QVariant::ByteArray) {
        result = QLatin1Char('\"') + escape(variant) + QLatin1Char('\"');
    } else if (variant.type() == QVariant::Double || (int)variant.type() == (int)QMetaType::Float) {
        double d = variant.toDouble();
        if (qIsFinite(d))
            result.setNum(variant.toDouble(), 'g', 15);
        else
            result = QLatin1String("null");
    } else if (variant.type() == QVariant::Bool) {
        result = variant.toBool() ? QLatin1String("true") : QLatin1String("false");
    } else if (variant.type() == QVariant::Invalid) {
        result = QLatin1String("null");
    } else if (variant.type() == QVariant::ULongLong) {
        result = QString::number(variant.toULongLong());
    } else if (variant.type() == QVariant::LongLong) {
        result = QString::number(variant.toLongLong());
    } else if (variant.type() == QVariant::Int) {
        result = QString::number(variant.toInt());
    } else if (variant.type() == QVariant::UInt) {
        result = QString::number(variant.toUInt());
    } else if (variant.type() == QVariant::Char) {
        QChar c = variant.toChar();
        if (c.unicode() > 127)
            result = QLatin1String("\"\\u") + QString::number(c.unicode(), 16).rightJustified(4, QLatin1Char('0')) + QLatin1Char('\"');
        else
            result = QLatin1Char('\"') + c + QLatin1Char('\"');
    } else if (variant.canConvert<qlonglong>()) {
        result = QString::number(variant.toLongLong());
    } else if (variant.canConvert<QString>()) {
        result = QLatin1Char('\"') + escape(variant) + QLatin1Char('\"');
    } else {
        result = QLatin1String("null");
    }

    return result;
}

/*!
  Converts the variant \a var into a JSON string.

  The stringizer converts \a var into JSON based on the type of it's contents. The supported
  types and their conversion into JSON is as listed below:

  \table
  \row
  \li QVariant::List, QVariant::StringList
  \li JSON array []
  \row
  \li QVariant::Map
  \li JSON object {}
  \row
  \li QVariant::String, QVariant::ByteArray
  \li JSON string encapsulated in double quotes. String contents are escaped using '\' if necessary.
  \row
  \li QVariant::Double, QMetaType::Float
  \li JSON number with a precision 15. Infinity and NaN are converted into null.
  \row
  \li QVariant::Bool
  \li JSON boolean true and false
  \row
  \li QVariant::Invalid
  \li JSON null
  \row
  \li QVariant::ULongLong, QVariant::LongLong, QVariant::Int, QVariant::UInt, 
  \li JSON number
  \row
  \li QVariant::Char
  \li JSON string. Non-ASCII characters are converted into the \uXXXX notation.
  \endtable

  As a fallback, the writer attempts to convert a type not listed above into a long long or a
  QString using QVariant::canConvert. See the QVariant documentation for possible conversions.

  JsonWriter does not support stringizing custom user types stored in the QVariant. Any such
  value would be converted into JSON null.
 */
QString JsonWriter::toString(const QVariant &var)
{
    return stringify(var);
}

/*!
  Converts the variant \a var into a JSON string.

  The returned QByteArray is UTF-8 encoded and does not contain a BOM. The list of supported
  C++ types is documented in toString().

  \sa toString()
 */
QByteArray JsonWriter::toByteArray(const QVariant &var)
{
    return stringify(var).toUtf8();
}

