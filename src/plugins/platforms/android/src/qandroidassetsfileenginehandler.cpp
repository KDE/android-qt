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

#include "qandroidassetsfileenginehandler.h"
#include "androidjnimain.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QMutex>
#include <QHash>
#include <QSharedPointer>

struct AndroidAssetDir
{
    AndroidAssetDir(AAssetDir* ad = 0)
    {
        count = 0;
        assetDir = ad;
        mutext = new QMutex();
    }
    ~AndroidAssetDir()
    {
        if (assetDir)
            AAssetDir_close(assetDir);
        delete mutext;
    }
    AAssetDir* assetDir;
    QMutex * mutext;
    volatile int count;
};

static const int MaxCachedPaths(10);
class AndroidAssetsDirCache
{
public:
    QSharedPointer<AndroidAssetDir> insert(const QString &fileName, AAssetDir* assetDir)
    {
        QSharedPointer<AndroidAssetDir> ad(new AndroidAssetDir(assetDir));
        if (m_assetsCache.size() == MaxCachedPaths)
        {
            AndroidAssetDirHash::iterator bestMatch = m_assetsCache.begin();
            for (AndroidAssetDirHash::iterator it = m_assetsCache.begin(); it!=m_assetsCache.end();++it)
            {
                if (it.value()->count<bestMatch.value()->count)
                    bestMatch = it;
            }
            m_assetsCache.erase(bestMatch);
        }
        m_assetsCache.insert(fileName, ad);
        return ad;
    }

    QSharedPointer<AndroidAssetDir> get(const QString &fileName) const
    {
        AndroidAssetDirHash::const_iterator it = m_assetsCache.find(fileName);
        if (it != m_assetsCache.end())
        {
            ++it.value()->count;
            return it.value();
        }
        return QSharedPointer<AndroidAssetDir>(0);
    }

private:
    typedef QHash<QString, QSharedPointer<AndroidAssetDir> > AndroidAssetDirHash;
    AndroidAssetDirHash m_assetsCache;
};

class AndroidAbstractFileEngineIterator: public QAbstractFileEngineIterator
{
public:
    AndroidAbstractFileEngineIterator( QDir::Filters filters, const QStringList & nameFilters, QSharedPointer<AndroidAssetDir> asset, const QString & path ):
        QAbstractFileEngineIterator(filters, nameFilters )
    {
        asset->mutext->lock();
        AAssetDir_rewind(asset->assetDir);
        const char * fileName;
        while((fileName=AAssetDir_getNextFileName(asset->assetDir)))
            m_items<<fileName;
        asset->mutext->unlock();
        m_index = -1;
        m_path = path;
    }

    virtual QFileInfo currentFileInfo() const
    {
        return QFileInfo(currentFilePath());
    }

    virtual QString currentFileName() const
    {
        if (m_index < 0 || m_index >= m_items.size())
            return QString();
        return m_items[m_index];
    }

    virtual QString currentFilePath() const
    {
        return m_path+currentFileName();
    }

    virtual bool hasNext() const
    {
        return m_items.size()&&(m_index<m_items.size()-1);
    }

    virtual QString next()
    {
        if (!hasNext())
            return QString();
        m_index++;
        return currentFileName();
    }

private:
    QString     m_path;
    QStringList m_items;
    int         m_index;
};

class AndroidAbstractFileEngine : public QAbstractFileEngine
{
public:
    explicit AndroidAbstractFileEngine(AAsset* asset, const QString & fileName)
    {
        m_assetFile = asset;
        m_fileName = fileName;
    }

    explicit AndroidAbstractFileEngine(QSharedPointer<AndroidAssetDir> asset, const QString & fileName)
    {
        m_assetFile = 0;
        m_assetDir = asset;
        m_fileName = fileName;
        if (!m_fileName.endsWith(QChar(QLatin1Char('/'))))
            m_fileName+="/";
    }

    ~AndroidAbstractFileEngine()
    {
        close();
    }

    virtual bool open(QIODevice::OpenMode openMode)
    {
        if (m_assetFile)
            return openMode&QIODevice::ReadOnly;
        return false;
    }

    virtual bool close()
    {
        if (m_assetFile)
        {
            AAsset_close(m_assetFile);
            m_assetFile = 0;
            return true;
        }
        return false;
    }

    virtual qint64 size() const
    {
        if (m_assetFile)
            return AAsset_getLength(m_assetFile);
        return -1;
    }

    virtual qint64 pos() const
    {
        if (m_assetFile)
            return AAsset_seek(m_assetFile, 0, SEEK_CUR);
        return -1;
    }

    virtual bool seek(qint64 pos)
    {
        if (m_assetFile)
            return pos==AAsset_seek(m_assetFile, pos, SEEK_SET);
        return false;
    }

    virtual qint64 read(char *data, qint64 maxlen)
    {
        if (m_assetFile)
            return AAsset_read(m_assetFile, data, maxlen);
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

    virtual FileFlags fileFlags(FileFlags type=FileInfoAll) const
    {
        FileFlags flags(ReadOwnerPerm|ReadUserPerm|ReadGroupPerm|ReadOtherPerm|ExistsFlag);
        if (m_assetFile)
            flags|=FileType;
        if(!m_assetDir.isNull())
            flags|=DirectoryType;
        return type&flags;
    }

    virtual QString fileName(FileName file=DefaultName) const
    {
        int pos;
        switch(file)
        {
        case DefaultName:
        case AbsoluteName:
        case CanonicalName:
                return m_fileName;
        case BaseName:
            if((pos=m_fileName.lastIndexOf(QChar(QLatin1Char('/'))))!=-1)
                return m_fileName.mid(pos);
            else
                return m_fileName;
        case PathName:
        case AbsolutePathName:
        case CanonicalPathName:
            if((pos=m_fileName.lastIndexOf(QChar(QLatin1Char('/'))))!=-1)
                return m_fileName.left(pos);
            else
                return m_fileName;
        default:
            return QString();
        }
    }

    virtual void setFileName(const QString &file)
    {
        if ( file == m_fileName )
            return;
        m_fileName = file;
        if (!m_fileName.endsWith(QChar(QLatin1Char('/'))))
            m_fileName+="/";
        close();
    }

    virtual Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames)
    {
        if (!m_assetDir.isNull())
            return new AndroidAbstractFileEngineIterator(filters, filterNames, m_assetDir, m_fileName);
        return 0;
    }

private:
    AAsset* m_assetFile;
    QSharedPointer<AndroidAssetDir> m_assetDir;
    QString m_fileName;
};


AndroidAssetsFileEngineHandler::AndroidAssetsFileEngineHandler()
{
    m_assetManager = QtAndroid::assetManager();
    m_necessitasApiLevel = -1;
    m_androidAssetDir = new AndroidAssetsDirCache();
}

AndroidAssetsFileEngineHandler::~AndroidAssetsFileEngineHandler()
{
    delete m_androidAssetDir;
}

QAbstractFileEngine * AndroidAssetsFileEngineHandler::create ( const QString & fileName ) const
{
    if(fileName.isEmpty())
        return 0;
    if (-1 == m_necessitasApiLevel)
        m_necessitasApiLevel = qgetenv("NECESSITAS_API_LEVEL").toInt();

    if (m_necessitasApiLevel>1)
    {
        if (!fileName.startsWith(QLatin1String("assets:/")))
            return 0;

        int prefixSize=8;

        path.clear();
        if (!fileName.endsWith(QLatin1Char('/')))
        {
            path = fileName.toUtf8();
            AAsset* asset=AAssetManager_open(m_assetManager, path.constData()+prefixSize, AASSET_MODE_BUFFER);
            if (asset)
                return new AndroidAbstractFileEngine(asset, fileName);
        }

        if (!path.size())
             path = fileName.left(fileName.length()-1).toUtf8();

        QSharedPointer<AndroidAssetDir> ad=m_androidAssetDir->get(fileName);
        if (ad.isNull())
        {
            AAssetDir* assetDir=AAssetManager_openDir(m_assetManager, path.constData()+prefixSize);
            if (assetDir)
            {
                if (AAssetDir_getNextFileName(assetDir))
                    return new AndroidAbstractFileEngine(m_androidAssetDir->insert(fileName, assetDir), fileName);
                else
                    AAssetDir_close(assetDir);
            }
        }
        else
            return new AndroidAbstractFileEngine(ad, fileName);
    }
    else
    {
        AAsset* asset;
        if (fileName[0]==QChar(QLatin1Char('/')))
            asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData()+1, AASSET_MODE_BUFFER);
        else
        {
            if (fileName.startsWith(QLatin1String("file://")))
                asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData()+7, AASSET_MODE_BUFFER);
            else
                asset=AAssetManager_open(m_assetManager, fileName.toUtf8().constData(), AASSET_MODE_BUFFER);
        }
        if (asset)
            return new AndroidAbstractFileEngine(asset, fileName);

        if (!fileName.endsWith(QChar(QLatin1Char('/'))))
            return 0;
        QString dirName;
        if (fileName[0]==QChar(QLatin1Char('/')))
            dirName = fileName.mid(1, fileName.size()-2);
        else
        {
            if (fileName.startsWith(QLatin1String("file://")))
                dirName = fileName.mid(7,fileName.size()-8);
            else
                dirName = fileName.left(fileName.size()-1);
        }

        QSharedPointer<AndroidAssetDir> ad=m_androidAssetDir->get(fileName);
        if (ad.isNull())
        {
            AAssetDir* assetDir=AAssetManager_openDir(m_assetManager, dirName.toUtf8().constData());
            if (assetDir)
            {
                if (AAssetDir_getNextFileName(assetDir))
                    return new AndroidAbstractFileEngine(m_androidAssetDir->insert(fileName, assetDir), fileName);
                else
                    AAssetDir_close(assetDir);
            }
        }
        else
            return new AndroidAbstractFileEngine(ad, fileName);
    }
    return 0;
}
