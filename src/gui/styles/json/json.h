/****************************************************************************
**
** Copyright (c) 2010 Girish Ramakrishnan <girish@forwardbias.in>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef JSON_H
#define JSON_H

#include <QByteArray>
#include <QVariant>

class JsonReader
{
public:
    JsonReader();
    ~JsonReader();

    bool parse(const QByteArray &ba);
    bool parse(const QString &str);

    QVariant result() const;

    QString errorString() const;

private:
    QVariant m_result;
    QString m_errorString;
    void *reserved;
};

class JsonWriter
{
public:
    JsonWriter();
    ~JsonWriter();

    QString toString(const QVariant &variant);
    QByteArray toByteArray(const QVariant &variant);

    void setAutoFormatting(bool autoFormat);
    bool autoFormatting() const;

    void setAutoFormattingIndent(int spaceOrTabs);
    int autoFormattingIndent() const;

private:
    QString stringify(const QVariant &variant, int depth = 0);
    bool m_autoFormatting;
    QString m_autoFormattingIndent;
    void *reserved;
};

#endif // JSON_H

