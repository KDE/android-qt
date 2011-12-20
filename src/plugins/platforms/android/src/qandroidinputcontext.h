/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL BogDan Vatra <bog_dan_ro@yahoo.com> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ANDROIDINPUTCONTEXT_H
#define ANDROIDINPUTCONTEXT_H

#include <QInputContext>
#include <jni.h>

class QAndroidInputContext : public QInputContext
{
    Q_OBJECT
    enum CapsMode
    {
        CAP_MODE_CHARACTERS = 0x00001000,
        CAP_MODE_SENTENCES  = 0x00004000,
        CAP_MODE_WORDS      = 0x00002000
    };

public:
    struct ExtractedText
    {
        ExtractedText() {clear();}
        void clear()
        {
            partialEndOffset=partialStartOffset=selectionEnd=selectionStart=startOffset = -1;
            text.clear();
        }
        int partialEndOffset;
        int partialStartOffset;
        int selectionEnd;
        int selectionStart;
        int startOffset;
        QString text;
    };

public:
    explicit QAndroidInputContext(QObject *parent = 0);
    ~QAndroidInputContext();
    virtual QString identifierName ();
    virtual bool isComposing () const;
    virtual QString language ();
    void clear();
    virtual void reset ();
    virtual void update ();
    virtual void setFocusWidget( QWidget *w );
    virtual bool filterEvent ( const QEvent * event );

    //---------------//
    jboolean commitText(const QString & text, jint newCursorPosition);
    jboolean deleteSurroundingText(jint leftLength, jint rightLength);
    jboolean finishComposingText();
    jint getCursorCapsMode(jint reqModes);
    const ExtractedText & getExtractedText(jint hintMaxChars, jint hintMaxLines, jint flags);
    QString getSelectedText(jint flags);
    QString getTextAfterCursor(jint length, jint flags);
    QString getTextBeforeCursor(jint length, jint flags);
    jboolean setComposingText(const QString & text, jint newCursorPosition);
    jboolean setSelection(jint start, jint end);
    jboolean selectAll();
    jboolean cut();
    jboolean copy();
    jboolean copyURL();
    jboolean paste();

signals:

private slots:
    virtual void sendEvent(const QInputMethodEvent &event);

private:
     ExtractedText m_extractedText;
     QString m_composingText;
};

#endif // ANDROIDINPUTCONTEXT_H
