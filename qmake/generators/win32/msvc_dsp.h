/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef MSVC_DSP_H
#define MSVC_DSP_H

#include "winmakefile.h"

QT_BEGIN_NAMESPACE

class FolderGroup;

class DspMakefileGenerator : public Win32MakefileGenerator
{
    bool init_flag;
    bool writeDspHeader(QTextStream &);
    bool writeDspParts(QTextStream &);
    bool writeFileGroup(QTextStream &t, const QStringList &listNames, const QString &group, const QString &filter);
    void writeSubFileGroup(QTextStream &t, FolderGroup *folder);
    bool writeBuildstepForFile(QTextStream &t, const QString &file, const QString &listName);
    static bool writeDspConfig(QTextStream &t, DspMakefileGenerator *config);
    static QString writeBuildstepForFileForConfig(const QString &file, const QString &listName, DspMakefileGenerator *config);
    QString configName(DspMakefileGenerator * config);

    bool writeMakefile(QTextStream &);
    bool writeProjectMakefile();
    void writeSubDirs(QTextStream &t);
    void init();

public:
    DspMakefileGenerator();
    ~DspMakefileGenerator();

    bool openOutput(QFile &file, const QString &build) const;
    bool hasBuiltinCompiler(const QString &filename) const;

protected:
    virtual bool doDepends() const { return false; } //never necesary
    virtual void processSources() { filterIncludedFiles("SOURCES"); filterIncludedFiles("GENERATED_SOURCES"); }
    virtual QString replaceExtraCompilerVariables(const QString &, const QStringList &, const QStringList &);
    inline QString replaceExtraCompilerVariables(const QString &val, const QString &in, const QString &out)
    { return MakefileGenerator::replaceExtraCompilerVariables(val, in, out); }
    virtual bool supportsMetaBuild() { return true; }
    virtual bool supportsMergedBuilds() { return true; }
    virtual bool mergeBuildProject(MakefileGenerator *other);
    virtual void processPrlVariable(const QString &, const QStringList &);
    virtual bool findLibraries();

    bool usePCH;
    QString precompH, namePCH,
            precompObj, precompPch;

    QString platform;

    struct BuildStep {
        BuildStep() {}
        BuildStep &operator<<(const BuildStep &other) {
            deps << other.deps;
            buildStep += other.buildStep;
            buildName += other.buildName;
            buildOutputs += other.buildOutputs;
            return *this;
        }

        QStringList deps;
        QString buildStep;
        QString buildName;
        QStringList buildOutputs;
    };
    QMap<QString, BuildStep> swappedBuildSteps;

    // Holds all configurations for glue (merged) project
    QList<DspMakefileGenerator*> mergedProjects;
};

inline DspMakefileGenerator::~DspMakefileGenerator()
{ }

inline bool DspMakefileGenerator::findLibraries()
{ return Win32MakefileGenerator::findLibraries("MSVCDSP_LIBS"); }

QT_END_NAMESPACE

#endif // MSVC_DSP_H
