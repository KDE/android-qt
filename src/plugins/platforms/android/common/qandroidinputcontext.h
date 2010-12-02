#ifndef ANDROIDINPUTCONTEXT_H
#define ANDROIDINPUTCONTEXT_H

#include <QInputContext>

class QAndroidInputContext : public QInputContext
{
    Q_OBJECT
public:
    explicit QAndroidInputContext(QObject *parent = 0);
    ~QAndroidInputContext();
    virtual QString identifierName ();
    virtual bool isComposing () const;
    virtual QString language ();
    virtual void reset ();
    virtual void update ();
    virtual bool filterEvent ( const QEvent * event );
signals:

public slots:

};

#endif // ANDROIDINPUTCONTEXT_H
