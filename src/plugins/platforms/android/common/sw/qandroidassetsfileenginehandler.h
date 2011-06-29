#ifndef QANDROIDASSETSFILEENGINEHANDLER_H
#define QANDROIDASSETSFILEENGINEHANDLER_H

#include <QAbstractFileEngineHandler>
#include <android/asset_manager.h>

class AndroidAssetsFileEngineHandler: public QAbstractFileEngineHandler
{
public:
    AndroidAssetsFileEngineHandler();
    virtual ~AndroidAssetsFileEngineHandler();
    QAbstractFileEngine * create ( const QString & fileName ) const;

private:
    AAssetManager * m_assetManager;
};

#endif // QANDROIDASSETSFILEENGINEHANDLER_H
