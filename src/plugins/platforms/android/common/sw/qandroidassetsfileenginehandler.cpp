#include "qandroidassetsfileenginehandler.h"
#include "androidjnimain.h"

#include <QCoreApplication>
#include <QDateTime>

class AndroidAbstractFileEngine : public QAbstractFileEngine
{
public:
    AndroidAbstractFileEngine(AAsset* asset)
    {
        m_asset = asset;
    }

    ~AndroidAbstractFileEngine()
    {
        close();
    }

    virtual bool open(QIODevice::OpenMode openMode)
    {
        if (m_asset)
            return openMode&QIODevice::ReadOnly;
        return false;
    }

    virtual bool close()
    {
        if (m_asset)
        {
            AAsset_close(m_asset);
            m_asset = 0;
            return true;
        }
        return false;
    }

    virtual qint64 size() const
    {
        if (m_asset)
            return AAsset_getLength(m_asset);
        return -1;
    }

    virtual qint64 pos() const
    {
        if (m_asset)
            return AAsset_seek(m_asset, 0, SEEK_CUR);
        return -1;
    }

    virtual bool seek(qint64 pos)
    {
        if (m_asset)
            return pos==AAsset_seek(m_asset, pos, SEEK_SET);
        return false;
    }

    virtual qint64 read(char *data, qint64 maxlen)
    {
        if (m_asset)
            return AAsset_read(m_asset, data, maxlen);
        return -1;
    }

    virtual bool isSequential() const
    {
        return false;
    }

    virtual bool caseSensitive() const
    {
        return true;
    }

    virtual bool isRelativePath() const
    {
        return false;
    }

private:
    AAsset* m_asset;
};

AndroidAssetsFileEngineHandler::AndroidAssetsFileEngineHandler()
{
    m_assetManager = QtAndroid::assetManager();
}

AndroidAssetsFileEngineHandler::~AndroidAssetsFileEngineHandler()
{

}

QAbstractFileEngine * AndroidAssetsFileEngineHandler::create ( const QString & fileName ) const
{
    AAsset* asset;
    if (fileName[0]==QChar(QLatin1Char('/')))
        asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData()+1, AASSET_MODE_BUFFER);
    else
        asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData(), AASSET_MODE_BUFFER);
    if (asset)
        return new AndroidAbstractFileEngine(asset);
    return 0;
}
