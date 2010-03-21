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

#include "symmake.h"

#include <qstring.h>
#include <qhash.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qdatetime.h>
#include <stdlib.h>
#include <qdebug.h>

// Included from tools/shared
#include <symbian/epocroot.h>

#define RESOURCE_DIRECTORY_MMP "/resource/apps"
#define RESOURCE_DIRECTORY_RESOURCE "\\\\resource\\\\apps\\\\"
#define REGISTRATION_RESOURCE_DIRECTORY_HW "/private/10003a3f/import/apps"
#define PLUGIN_COMMON_DEF_FILE_FOR_MMP "./plugin_common.def"
#define PLUGIN_COMMON_DEF_FILE_ACTUAL "plugin_commonu.def"
#define BLD_INF_FILENAME_LEN (sizeof(BLD_INF_FILENAME) - 1)

#define BLD_INF_RULES_BASE "BLD_INF_RULES."
#define BLD_INF_TAG_PLATFORMS "prj_platforms"
#define BLD_INF_TAG_MMPFILES "prj_mmpfiles"
#define BLD_INF_TAG_TESTMMPFILES "prj_testmmpfiles"
#define BLD_INF_TAG_EXTENSIONS "prj_extensions"
#define BLD_INF_TAG_TESTEXTENSIONS "prj_testextensions"

#define RSS_RULES "RSS_RULES"
#define RSS_RULES_BASE "RSS_RULES."
#define RSS_TAG_NBROFICONS "number_of_icons"
#define RSS_TAG_ICONFILE "icon_file"
#define RSS_TAG_HEADER "header"
#define RSS_TAG_SERVICE_LIST "service_list"
#define RSS_TAG_FILE_OWNERSHIP_LIST "file_ownership_list"
#define RSS_TAG_DATATYPE_LIST "datatype_list"
#define RSS_TAG_FOOTER "footer"
#define RSS_TAG_DEFAULT "default_rules" // Same as just giving rules without tag

#define MMP_TARGET "TARGET"
#define MMP_TARGETTYPE "TARGETTYPE"
#define MMP_SECUREID "SECUREID"
#define MMP_OPTION_CW "OPTION CW"
#define MMP_OPTION_ARMCC "OPTION ARMCC"
#define MMP_OPTION_GCCE "OPTION GCCE"
#define MMP_LINKEROPTION_CW "LINKEROPTION CW"
#define MMP_LINKEROPTION_ARMCC "LINKEROPTION ARMCC"
#define MMP_LINKEROPTION_GCCE "LINKEROPTION GCCE"
#define MMP_CAPABILITY "CAPABILITY"
#define MMP_EPOCALLOWDLLDATA "EPOCALLOWDLLDATA"
#define MMP_EPOCHEAPSIZE "EPOCHEAPSIZE"
#define MMP_EPOCSTACKSIZE "EPOCSTACKSIZE"
#define MMP_UID "UID"
#define MMP_VENDORID "VENDORID"
#define MMP_VERSION "VERSION"
#define MMP_START_RESOURCE "START RESOURCE"
#define MMP_END_RESOURCE "END"

#define PRINT_FILE_CREATE_ERROR(filename) fprintf(stderr, "Error: Could not create '%s'\n", qPrintable(filename));

#define MANUFACTURER_NOTE_FILE "manufacturer_note.txt"
#define DEFAULT_MANUFACTURER_NOTE \
    "The package is not supported for devices from this manufacturer. Please try the selfsigned " \
    "version of the package instead."

QString SymbianMakefileGenerator::fixPathForMmp(const QString& origPath, const QDir& parentDir)
{
    static QString epocRootStr;
    if (epocRootStr.isEmpty()) {
        epocRootStr = epocRoot();
        QFileInfo efi(epocRootStr);
        if (!efi.exists() || epocRootStr.isEmpty()) {
            fprintf(stderr, "Unable to resolve epocRoot '%s' to real dir on current drive, defaulting to '/' for mmp paths\n", qPrintable(epocRoot()));
            epocRootStr = "/";
        } else {
            epocRootStr = efi.absoluteFilePath();
        }
        if (!epocRootStr.endsWith("/"))
            epocRootStr += "/";

        epocRootStr += "epoc32/";
    }

    QString resultPath = origPath;

    // Make it relative, unless it starts with "%epocroot%/epoc32/"
    if (resultPath.startsWith(epocRootStr, Qt::CaseInsensitive)) {
        resultPath.replace(epocRootStr, "/epoc32/", Qt::CaseInsensitive);
    } else {
        resultPath = parentDir.relativeFilePath(resultPath);
    }
    resultPath = QDir::cleanPath(resultPath);

    if (resultPath.isEmpty())
        resultPath = ".";

    return resultPath;
}

QString SymbianMakefileGenerator::absolutizePath(const QString& origPath)
{
    // Prepend epocroot to any paths beginning with "/epoc32/"
    QString resultPath = QDir::fromNativeSeparators(origPath);
    if (resultPath.startsWith("/epoc32/", Qt::CaseInsensitive))
        resultPath = QDir::fromNativeSeparators(epocRoot()) + resultPath.mid(1);

    QFileInfo fi(fileInfo(resultPath));

    // Since origPath can be something given in HEADERS, we need to check if we are dealing
    // with a file or a directory. In case the origPath doesn't yet exist, isFile() returns
    // false and we default to assuming it is a dir.
    if (fi.isFile()) {
        resultPath = fi.absolutePath();
    } else {
        resultPath = fi.absoluteFilePath();
    }

    resultPath = QDir::cleanPath(resultPath);

    return resultPath;
}

SymbianMakefileGenerator::SymbianMakefileGenerator() : MakefileGenerator() { }
SymbianMakefileGenerator::~SymbianMakefileGenerator() { }

void SymbianMakefileGenerator::writeHeader(QTextStream &t)
{
    t << "// ============================================================================" << endl;
    t << "// * Makefile for building: " << escapeFilePath(var("TARGET")) << endl;
    t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
    t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
    t << "// * This file is generated by qmake and should not be modified by the" << endl;
    t << "// * user." << endl;
    t << "// * Project:  " << fileFixify(project->projectFile()) << endl;
    t << "// * Template: " << var("TEMPLATE") << endl;
    t << "// ============================================================================" << endl;
    t << endl;

    // Defining define for bld.inf

    QString shortProFilename = project->projectFile();
    shortProFilename.replace(0, shortProFilename.lastIndexOf("/") + 1, QString(""));
    shortProFilename.replace(Option::pro_ext, QString(""));

    QString bldinfDefine = shortProFilename;
    bldinfDefine.append("_");
    bldinfDefine.append(generate_uid(project->projectFile()));

    bldinfDefine.prepend("BLD_INF_");
    removeSpecialCharacters(bldinfDefine);

    t << "#define " << bldinfDefine.toUpper() << endl << endl;
}

bool SymbianMakefileGenerator::writeMakefile(QTextStream &t)
{
    writeHeader(t);

    QString numberOfIcons;
    QString iconFile;
    QMap<QString, QStringList> userRssRules;
    readRssRules(numberOfIcons, iconFile, userRssRules);

    // Get the application translations and convert to symbian OS lang code, i.e. decical number
    QStringList symbianLangCodes = symbianLangCodesFromTsFiles();

    // Generate pkg files if there are any actual files to deploy
    bool generatePkg = false;
    DeploymentList depList;

    if (targetType == TypeExe) {
        generatePkg = true;
    } else {
        foreach(QString item, project->values("DEPLOYMENT")) {
            if (!project->values(item + ".sources").isEmpty()) {
                generatePkg = true;
                break;
            }
        }
    }

    if (generatePkg) {
        generatePkgFile(iconFile, depList);
    }

    writeBldInfContent(t, generatePkg, iconFile, depList);

    // Generate empty wrapper makefile here, because wrapper makefile must exist before writeMkFile,
    // but all required data is not yet available.
    bool isPrimaryMakefile = true;
    QString wrapperFileName("Makefile");
    QString outputFileName = fileInfo(Option::output.fileName()).fileName();
    if (outputFileName != BLD_INF_FILENAME) {
        wrapperFileName.append(".").append(outputFileName.startsWith(BLD_INF_FILENAME)
                                           ? outputFileName.mid(sizeof(BLD_INF_FILENAME))
                                           : outputFileName);
        isPrimaryMakefile = false;
    }

    QFile wrapperMakefile(wrapperFileName);
    if (wrapperMakefile.open(QIODevice::WriteOnly)) {
        generatedFiles << wrapperFileName;
    } else {
        PRINT_FILE_CREATE_ERROR(wrapperFileName);
        return false;
    }

    if (targetType == TypeSubdirs) {
        // If we have something to deploy, generate extension makefile for just that, since
        // normal extension makefile is not getting generated and we need emulator deployment to be done.
        if (generatePkg)
            writeMkFile(wrapperFileName, true);
        writeWrapperMakefile(wrapperMakefile, isPrimaryMakefile);
        return true;
    }

    writeMkFile(wrapperFileName, false);

    QString shortProFilename = project->projectFile();
    shortProFilename.replace(0, shortProFilename.lastIndexOf("/") + 1, QString(""));
    shortProFilename.replace(Option::pro_ext, QString(""));

    QString mmpFilename = shortProFilename;
    mmpFilename.append("_");
    mmpFilename.append(uid3);
    mmpFilename.append(Option::mmp_ext);
    writeMmpFile(mmpFilename, symbianLangCodes);

    if (targetType == TypeExe) {
        if (!project->isActiveConfig("no_icon")) {
            writeRegRssFile(userRssRules);
            writeRssFile(numberOfIcons, iconFile);
            writeLocFile(symbianLangCodes);
        }
    }

    writeCustomDefFile();
    writeWrapperMakefile(wrapperMakefile, isPrimaryMakefile);

    return true;
}

void SymbianMakefileGenerator::generatePkgFile(const QString &iconFile, DeploymentList &depList)
{
    QString pkgFilename = QString("%1_template.%2")
                          .arg(fixedTarget)
                          .arg("pkg");
    QFile pkgFile(pkgFilename);
    if (!pkgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        PRINT_FILE_CREATE_ERROR(pkgFilename);
        return;
    }

    generatedFiles << pkgFile.fileName();
    QTextStream t(&pkgFile);

    QString installerSisHeader = project->values("DEPLOYMENT.installer_header").join("\n");
    if (installerSisHeader.isEmpty())
        installerSisHeader = "0xA000D7CE"; // Use default self-signable UID if not defined

    QString wrapperStreamBuffer;
    QTextStream tw(&wrapperStreamBuffer);

    QString dateStr = QDateTime::currentDateTime().toString(Qt::ISODate);

    // Header info
    QString wrapperPkgFilename = QString("%1_installer.%2")
                                .arg(fixedTarget)
                                .arg("pkg");
    QString headerComment = "; %1 generated by qmake at %2\n"
        "; This file is generated by qmake and should not be modified by the user\n"
        ";\n\n";
    t << headerComment.arg(pkgFilename).arg(dateStr);
    tw << headerComment.arg(wrapperPkgFilename).arg(dateStr);

    // Construct QStringList from pkg_prerules since we need search it before printed to file
    QStringList rawPkgPreRules;
    foreach(QString deploymentItem, project->values("DEPLOYMENT")) {
        foreach(QString pkgrulesItem, project->values(deploymentItem + ".pkg_prerules")) {
            QStringList pkgrulesValue = project->values(pkgrulesItem);
            // If there is no stringlist defined for a rule, use rule name directly
            // This is convenience for defining single line mmp statements
            if (pkgrulesValue.isEmpty()) {
                rawPkgPreRules << pkgrulesItem;
            } else {
                foreach(QString pkgrule, pkgrulesValue) {
                    rawPkgPreRules << pkgrule;
                }
            }
        }
    }

    // Apply some defaults if specific data does not exist in PKG pre-rules

    if (!containsStartWithItem('&', rawPkgPreRules)) {
        // language, (*** hardcoded to english atm, should be parsed from TRANSLATIONS)
        QString languageCode = "; Language\n&EN\n\n";
        t << languageCode;
        tw << languageCode;
    } else {
        // In case user defines langs, he must take care also about SIS header
        if (!containsStartWithItem('#', rawPkgPreRules))
            fprintf(stderr, "Warning: If language is defined with DEPLOYMENT pkg_prerules, also the SIS header must be defined\n");
    }

    // name of application, UID and version
    QString applicationVersion = project->first("VERSION").isEmpty() ? "1,0,0" : project->first("VERSION").replace('.', ',');
    QString sisHeader = "; SIS header: name, uid, version\n#{\"%1\"},(%2),%3\n\n";
    QString visualTarget = escapeFilePath(fileFixify(project->first("TARGET")));
    visualTarget = removePathSeparators(visualTarget);
    QString wrapperTarget = visualTarget + " installer";

    if (installerSisHeader.startsWith("0x", Qt::CaseInsensitive)) {
        tw << sisHeader.arg(wrapperTarget).arg(installerSisHeader).arg(applicationVersion);
    } else {
        tw << installerSisHeader << endl;
    }

    if (!containsStartWithItem('#', rawPkgPreRules)) {
        t << sisHeader.arg(visualTarget).arg(uid3).arg(applicationVersion);
    }

    // Localized vendor name
    QString vendorName;
    if (!containsStartWithItem('%', rawPkgPreRules)) {
        vendorName += "; Localised Vendor name\n%{\"Vendor\"}\n\n";
    }

    // Unique vendor name
    if (!containsStartWithItem(':', rawPkgPreRules)) {
        vendorName += "; Unique Vendor name\n:\"Vendor\"\n\n";
    }

    t << vendorName;
    tw << vendorName;

    // PKG pre-rules - these are added before actual file installations i.e. SIS package body
    if (rawPkgPreRules.size()) {
        QString comment = "\n; Manual PKG pre-rules from PRO files\n";
        t << comment;
        tw << comment;

        foreach(QString item, rawPkgPreRules) {
            // Only regular pkg file should have package dependencies or pkg header if that is
            // defined using prerules.
            if (!item.startsWith("(") && !item.startsWith("#")) {
                tw << item << endl;
            }
            t << item << endl;
        }
        t << endl;
        tw << endl;
    }

    // Begin Manufacturer block
    if (!project->values("DEPLOYMENT.manufacturers").isEmpty()) {
        QString manufacturerStr("IF ");
        foreach(QString manufacturer, project->values("DEPLOYMENT.manufacturers")) {
            manufacturerStr.append(QString("(MANUFACTURER)=(%1) OR \n   ").arg(manufacturer));
        }
        // Remove the final OR
        manufacturerStr.chop(8);
        t << manufacturerStr << endl;
    }

    // Install paths on the phone *** should be dynamic at some point
    QString installPathBin = "!:\\sys\\bin";
    QString installPathResource = "!:\\resource\\apps";
    QString installPathRegResource = "!:\\private\\10003a3f\\import\\apps";

    // Find location of builds
    QString epocReleasePath = QString("%1epoc32/release/$(PLATFORM)/$(TARGET)")
                              .arg(epocRoot());

    if (targetType == TypeExe) {
        // deploy .exe file
        t << "; Executable and default resource files" << endl;
        QString exeFile = fixedTarget + ".exe";
        t << QString("\"%1/%2\"    - \"%3\\%4\"")
             .arg(epocReleasePath)
             .arg(exeFile)
             .arg(installPathBin)
             .arg(exeFile) << endl;

        // deploy rsc & reg_rsc file
        if (!project->isActiveConfig("no_icon")) {
            t << QString("\"%1epoc32/data/z/resource/apps/%2\"    - \"%3\\%4\"")
                 .arg(epocRoot())
                 .arg(fixedTarget + ".rsc")
                 .arg(installPathResource)
                 .arg(fixedTarget + ".rsc") << endl;

            t << QString("\"%1epoc32/data/z/private/10003a3f/import/apps/%2\"    - \"%3\\%4\"")
                 .arg(epocRoot())
                 .arg(fixedTarget + "_reg.rsc")
                 .arg(installPathRegResource)
                 .arg(fixedTarget + "_reg.rsc") << endl;

            if (!iconFile.isEmpty())  {
                t << QString("\"%1epoc32/data/z%2\"    - \"!:%3\"")
                     .arg(epocRoot())
                     .arg(iconFile)
                     .arg(QDir::toNativeSeparators(iconFile)) << endl << endl;
            }
        }
    }

    // deploy any additional DEPLOYMENT  files
    QString remoteTestPath;
    remoteTestPath = QString("!:\\private\\%1").arg(privateDirUid);

    initProjectDeploySymbian(project, depList, remoteTestPath, true, "$(PLATFORM)", "$(TARGET)", generatedDirs, generatedFiles);
    if (depList.size())
        t << "; DEPLOYMENT" << endl;
    for (int i = 0; i < depList.size(); ++i)  {
        t << QString("\"%1\"    - \"%2\"")
             .arg(QString(depList.at(i).from).replace('\\','/'))
             .arg(QString(depList.at(i).to).replace('/','\\')) << endl;
    }
    t << endl;

    // PKG post-rules - these are added after actual file installations i.e. SIS package body
    t << "; Manual PKG post-rules from PRO files" << endl;
    foreach(QString deploymentItem, project->values("DEPLOYMENT")) {
        foreach(QString pkgrulesItem, project->values(deploymentItem + ".pkg_postrules")) {
            QStringList pkgrulesValue = project->values(pkgrulesItem);
            // If there is no stringlist defined for a rule, use rule name directly
            // This is convenience for defining single line statements
            if (pkgrulesValue.isEmpty()) {
                t << pkgrulesItem << endl;
            } else {
                foreach(QString pkgrule, pkgrulesValue) {
                    t << pkgrule << endl;
                }
            }
            t << endl;
        }
    }

    // Close Manufacturer block
    if (!project->values("DEPLOYMENT.manufacturers").isEmpty()) {
        QString manufacturerFailNoteFile;
        if (project->values("DEPLOYMENT.manufacturers.fail_note").isEmpty()) {
            manufacturerFailNoteFile = QString("%1_" MANUFACTURER_NOTE_FILE).arg(uid3);
            QFile ft(manufacturerFailNoteFile);
            if (ft.open(QIODevice::WriteOnly)) {
                generatedFiles << ft.fileName();
                QTextStream t2(&ft);

                t2 << QString(DEFAULT_MANUFACTURER_NOTE) << endl;
            } else {
                PRINT_FILE_CREATE_ERROR(manufacturerFailNoteFile)
            }
        } else {
            manufacturerFailNoteFile = project->values("DEPLOYMENT.manufacturers.fail_note").join("");
        }

        t << "ELSEIF NOT(0) ; MANUFACTURER" << endl
          << "\"" << fileInfo(manufacturerFailNoteFile).absoluteFilePath() << "\""
          << " - \"\", FILETEXT, TEXTEXIT" << endl
          << "ENDIF ; MANUFACTURER" << endl;
    }

    // Write wrapper pkg
    if (!installerSisHeader.isEmpty()) {
        QFile wrapperPkgFile(wrapperPkgFilename);
        if (!wrapperPkgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            PRINT_FILE_CREATE_ERROR(wrapperPkgFilename);
            return;
        }

        generatedFiles << wrapperPkgFile.fileName();
        QTextStream twf(&wrapperPkgFile);

        twf << wrapperStreamBuffer << endl;

        // Wrapped files deployment
        QString currentPath = qmake_getpwd();
        QString sisName = QString("%1.sis").arg(fixedTarget);
        twf << "\"" << currentPath << "/" << sisName << "\" - \"c:\\adm\\" << sisName << "\"" << endl;

        QString bootStrapPath = QLibraryInfo::location(QLibraryInfo::PrefixPath);
        bootStrapPath.append("/smartinstaller.sis");
        QFileInfo fi(fileInfo(bootStrapPath));
        twf << "@\"" << fi.absoluteFilePath() << "\",(0x2002CCCD)" << endl;
    }
}

bool SymbianMakefileGenerator::containsStartWithItem(const QChar &c, const QStringList& src)
{
    bool result = false;
    foreach(QString str, src) {
        if (str.startsWith(c)) {
            result =  true;
            break;
        }
    }
    return result;
}

void SymbianMakefileGenerator::writeCustomDefFile()
{
    if (targetType == TypePlugin && !project->isActiveConfig("stdbinary")) {
        // Create custom def file for plugin
        QFile ft(QLatin1String(PLUGIN_COMMON_DEF_FILE_ACTUAL));

        if (ft.open(QIODevice::WriteOnly)) {
            generatedFiles << ft.fileName();
            QTextStream t(&ft);

            t << "; ==============================================================================" << endl;
            t << "; Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
            t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
            t << "; This file is generated by qmake and should not be modified by the" << endl;
            t << "; user." << endl;
            t << ";  Name        : " PLUGIN_COMMON_DEF_FILE_ACTUAL << endl;
            t << ";  Part of     : " << project->values("TARGET").join(" ") << endl;
            t << ";  Description : Fixes common plugin symbols to known ordinals" << endl;
            t << ";  Version     : " << endl;
            t << ";" << endl;
            t << "; ==============================================================================" << "\n" << endl;

            t << endl;

            t << "EXPORTS" << endl;
            t << "\tqt_plugin_query_verification_data @ 1 NONAME" << endl;
            t << "\tqt_plugin_instance @ 2 NONAME" << endl;
            t << endl;
        } else {
            PRINT_FILE_CREATE_ERROR(QString(PLUGIN_COMMON_DEF_FILE_ACTUAL))
        }
    }
}

void SymbianMakefileGenerator::init()
{
    MakefileGenerator::init();
    fixedTarget = escapeFilePath(fileFixify(project->first("TARGET")));
    fixedTarget = removePathSeparators(fixedTarget);
    removeSpecialCharacters(fixedTarget);

    if (0 != project->values("QMAKE_PLATFORM").size())
        platform = varGlue("QMAKE_PLATFORM", "", " ", "");

    if (0 == project->values("QMAKESPEC").size())
        project->values("QMAKESPEC").append(qgetenv("QMAKESPEC"));

    project->values("QMAKE_LIBS") += escapeFilePaths(project->values("LIBS"));
    project->values("QMAKE_LIBS_PRIVATE") += escapeFilePaths(project->values("LIBS_PRIVATE"));

    // bld.inf
    project->values("MAKEFILE") += BLD_INF_FILENAME;

    // .mmp
    initMmpVariables();

    // Check TARGET.UID3 presence
    if (0 != project->values("TARGET.UID3").size()) {
        uid3 = project->first("TARGET.UID3");
    } else {
        uid3 = generateUID3();
    }

    if ((project->values("TEMPLATE")).contains("app"))
        targetType = TypeExe;
    else if ((project->values("TEMPLATE")).contains("lib")) {
        // Check CONFIG to see if we are to build staticlib or dll
        if (project->isActiveConfig("staticlib") || project->isActiveConfig("static"))
            targetType = TypeLib;
        else if (project->isActiveConfig("plugin"))
            targetType = TypePlugin;
        else
            targetType = TypeDll;
    } else {
        targetType = TypeSubdirs;
    }

    if (0 != project->values("TARGET.UID2").size()) {
        uid2 = project->first("TARGET.UID2");
    } else if (project->isActiveConfig("stdbinary")) {
        uid2 = "0x20004C45";
    } else {
        if (targetType == TypeExe) {
            if (project->values("QT").contains("gui", Qt::CaseInsensitive)) {
                // exe and gui -> uid2 needed
                uid2 = "0x100039CE";
            } else {
                // exe but not gui: uid2 is ignored anyway -> set it to 0
                uid2 = "0";
            }
        } else if (targetType == TypeDll || targetType == TypeLib || targetType == TypePlugin) {
            uid2 = "0x1000008d";
        }
    }

    uid2 = uid2.trimmed();
    uid3 = uid3.trimmed();

    // UID is valid as either hex or decimal, so just convert it to number and back to hex
    // to get proper string for private dir
    bool conversionOk = false;
    uint uidNum = uid3.toUInt(&conversionOk, 0);

    if (!conversionOk) {
        fprintf(stderr, "Error: Invalid UID \"%s\".\n", uid3.toUtf8().constData());
    } else {
        privateDirUid.setNum(uidNum, 16);
        while (privateDirUid.length() < 8)
            privateDirUid.insert(0, QLatin1Char('0'));
    }
}

QString SymbianMakefileGenerator::getTargetExtension()
{
    QString ret;
    if (targetType == TypeExe) {
        ret.append("exe");
    } else if (targetType == TypeLib) {
        ret.append("lib");
    } else if (targetType == TypeDll || targetType == TypePlugin) {
        ret.append("dll");
    } else if (targetType == TypeSubdirs) {
        // Not actually usable, so return empty
    } else {
        // If nothing else set, default to exe
        ret.append("exe");
    }

    return ret;
}

QString SymbianMakefileGenerator::generateUID3()
{
    QString target = project->first("TARGET");
    QString currPath = qmake_getpwd();
    target.prepend("/").prepend(currPath);
    return generate_test_uid(target);
}

void SymbianMakefileGenerator::initMmpVariables()
{
    QStringList sysincspaths;
    QStringList srcincpaths;
    QStringList srcpaths;

    srcpaths << project->values("SOURCES") << project->values("GENERATED_SOURCES");
    srcpaths << project->values("UNUSED_SOURCES") << project->values("UI_SOURCES_DIR");
    srcpaths << project->values("UI_DIR");

    QDir current = QDir::current();
    QString absolutizedCurrent = absolutizePath(".");

    for (int j = 0; j < srcpaths.size(); ++j) {
        QFileInfo fi(fileInfo(srcpaths.at(j)));
        // Sometimes sources have other than *.c* files (e.g. *.moc); prune them.
        if (fi.suffix().startsWith("c")) {
            if (fi.filePath().length() > fi.fileName().length()) {
                appendIfnotExist(srcincpaths, fi.path());
                sources[absolutizePath(fi.path())] += fi.fileName();
            } else {
                sources[absolutizedCurrent] += fi.fileName();
                appendIfnotExist(srcincpaths, absolutizedCurrent);
            }
        }
    }

    QStringList incpaths;
    incpaths << project->values("INCLUDEPATH");
    incpaths << QLibraryInfo::location(QLibraryInfo::HeadersPath);
    incpaths << project->values("HEADERS");
    incpaths << srcincpaths;
    incpaths << project->values("UI_HEADERS_DIR");
    incpaths << project->values("UI_DIR");

    for (int j = 0; j < incpaths.size(); ++j) {
        QString includepath = absolutizePath(incpaths.at(j));
        appendIfnotExist(sysincspaths, includepath);
        appendAbldTempDirs(sysincspaths, includepath);
    }

    // Remove duplicate include path entries
    QStringList temporary;
    for (int i = 0; i < sysincspaths.size(); ++i) {
        QString origPath = sysincspaths.at(i);
        QFileInfo origPathInfo(fileInfo(origPath));
        bool bFound = false;

        for (int j = 0; j < temporary.size(); ++j) {
            QString tmpPath = temporary.at(j);
            QFileInfo tmpPathInfo(fileInfo(tmpPath));

            if (origPathInfo.absoluteFilePath() == tmpPathInfo.absoluteFilePath()) {
                bFound = true;
                if (!tmpPathInfo.isRelative() && origPathInfo.isRelative()) {
                    // We keep the relative notation
                    temporary.removeOne(tmpPath);
                    temporary << origPath;
                }
            }
        }

        if (!bFound)
            temporary << origPath;

    }

    sysincspaths.clear();
    sysincspaths << temporary;

    systeminclude.insert("SYSTEMINCLUDE", sysincspaths);

    // Check MMP_RULES for singleton keywords that are overridden
    QStringList overridableMmpKeywords;
    QStringList restrictableMmpKeywords;
    QStringList restrictedMmpKeywords;
    bool inResourceBlock = false;

    overridableMmpKeywords << QLatin1String(MMP_TARGETTYPE) << QLatin1String(MMP_EPOCHEAPSIZE);
    restrictableMmpKeywords << QLatin1String(MMP_TARGET) << QLatin1String(MMP_SECUREID)
       << QLatin1String(MMP_OPTION_CW) << QLatin1String(MMP_OPTION_ARMCC)
       << QLatin1String(MMP_OPTION_GCCE) << QLatin1String(MMP_LINKEROPTION_CW)
       << QLatin1String(MMP_LINKEROPTION_ARMCC) << QLatin1String(MMP_LINKEROPTION_GCCE)
       << QLatin1String(MMP_CAPABILITY) << QLatin1String(MMP_EPOCALLOWDLLDATA)
       << QLatin1String(MMP_EPOCSTACKSIZE) << QLatin1String(MMP_UID)
       << QLatin1String(MMP_VENDORID) << QLatin1String(MMP_VERSION);

    foreach (QString item, project->values("MMP_RULES")) {
        if (project->values(item).isEmpty()) {
            handleMmpRulesOverrides(item, inResourceBlock, restrictedMmpKeywords,
                                    restrictableMmpKeywords, overridableMmpKeywords);
        } else {
            foreach (QString itemRow, project->values(item)) {
                handleMmpRulesOverrides(itemRow, inResourceBlock, restrictedMmpKeywords,
                                        restrictableMmpKeywords, overridableMmpKeywords);
            }
        }
    }

    if (restrictedMmpKeywords.size()) {
        fprintf(stderr, "Warning: Restricted statements detected in MMP_RULES:\n"
                "         (%s)\n"
                "         Use corresponding qmake variable(s) instead.\n",
                qPrintable(restrictedMmpKeywords.join(", ")));
        }
}

void SymbianMakefileGenerator::handleMmpRulesOverrides(QString &checkString,
                                                       bool &inResourceBlock,
                                                       QStringList &restrictedMmpKeywords,
                                                       const QStringList &restrictableMmpKeywords,
                                                       const QStringList &overridableMmpKeywords)
{
    QString simplifiedString = checkString.simplified();

    if (!inResourceBlock && simplifiedString.startsWith(MMP_START_RESOURCE, Qt::CaseInsensitive))
        inResourceBlock = true;
    else if (inResourceBlock && simplifiedString.startsWith(MMP_END_RESOURCE, Qt::CaseInsensitive))
        inResourceBlock = false;

    // Allow restricted and overridable items in RESOURCE blocks as those do not actually
    // override anything.
    if (!inResourceBlock) {
        appendKeywordIfMatchFound(overriddenMmpKeywords, overridableMmpKeywords, simplifiedString);
        appendKeywordIfMatchFound(restrictedMmpKeywords, restrictableMmpKeywords, simplifiedString);
    }
}

void SymbianMakefileGenerator::appendKeywordIfMatchFound(QStringList &list,
                                                         const QStringList &keywordList,
                                                         QString &checkString)
{
    // Check if checkString starts with any supplied keyword and
    // add the found keyword to list if it does.
    foreach (QString item, keywordList) {
        if (checkString.startsWith(QString(item).append(" "), Qt::CaseInsensitive)
            || checkString.compare(item, Qt::CaseInsensitive) == 0) {
            appendIfnotExist(list, item);
        }
    }
}


bool SymbianMakefileGenerator::removeDuplicatedStrings(QStringList &stringList)
{
    QStringList tmpStringList;

    for (int i = 0; i < stringList.size(); ++i) {
        QString string = stringList.at(i);
        if (tmpStringList.contains(string))
            continue;
        else
            tmpStringList.append(string);
    }

    stringList.clear();
    stringList = tmpStringList;
    return true;
}

void SymbianMakefileGenerator::writeMmpFileHeader(QTextStream &t)
{
    t << "// ==============================================================================" << endl;
    t << "// Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
    t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
    t << "// This file is generated by qmake and should not be modified by the" << endl;
    t << "// user." << endl;
    t << "//  Name        : " << escapeFilePath(fileFixify(project->projectFile().remove(project->projectFile().length() - 4, 4))) << Option::mmp_ext << endl;
    t << "// ==============================================================================" << endl << endl;
}

void SymbianMakefileGenerator::writeMmpFile(QString &filename, QStringList &symbianLangCodes)
{
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();

        QTextStream t(&ft);

        writeMmpFileHeader(t);

        writeMmpFileTargetPart(t);

        writeMmpFileResourcePart(t, symbianLangCodes);

        writeMmpFileMacrosPart(t);

        writeMmpFileIncludePart(t);

        QDir current = QDir::current();

        for (QMap<QString, QStringList>::iterator it = sources.begin(); it != sources.end(); ++it) {
            QStringList values = it.value();
            QString currentSourcePath = it.key();

            if (values.size())
                t << "SOURCEPATH \t" <<  fixPathForMmp(currentSourcePath, current) << endl;

            for (int i = 0; i < values.size(); ++i) {
                QString sourceFileName = values.at(i);
                t << "SOURCE\t\t" << sourceFileName << endl;
            }
            t << endl;
        }
        t << endl;

        if (!project->isActiveConfig("static") && !project->isActiveConfig("staticlib")) {
            writeMmpFileLibraryPart(t);
        }

        writeMmpFileCapabilityPart(t);

        writeMmpFileCompilerOptionPart(t);

        writeMmpFileBinaryVersionPart(t);

        writeMmpFileRulesPart(t);
    } else {
        PRINT_FILE_CREATE_ERROR(filename)
    }
}

void SymbianMakefileGenerator::writeMmpFileMacrosPart(QTextStream& t)
{
    t << endl;

    QStringList &defines = project->values("DEFINES");
    if (defines.size())
        t << "// Qt Macros" << endl;
    for (int i = 0; i < defines.size(); ++i) {
        QString def = defines.at(i);
        addMacro(t, def);
    }

    // These are required in order that all methods will be correctly exported e.g from qtestlib
    QStringList &exp_defines = project->values("PRL_EXPORT_DEFINES");
    if (exp_defines.size())
        t << endl << "// Qt Export Defines" << endl;
    for (int i = 0; i < exp_defines.size(); ++i) {
        QString def = exp_defines.at(i);
        addMacro(t, def);
    }

    t << endl;
}

void SymbianMakefileGenerator::addMacro(QTextStream& t, const QString& value)
{
    t << "MACRO\t\t" <<  value << endl;
}


void SymbianMakefileGenerator::writeMmpFileTargetPart(QTextStream& t)
{
    bool skipTargetType = overriddenMmpKeywords.contains(MMP_TARGETTYPE);
    bool skipEpocHeapSize = overriddenMmpKeywords.contains(MMP_EPOCHEAPSIZE);

    if (targetType == TypeExe) {
        t << MMP_TARGET "\t\t" << fixedTarget << ".exe" << endl;
        if (!skipTargetType) {
            if (project->isActiveConfig("stdbinary"))
                t << MMP_TARGETTYPE "\t\tSTDEXE" << endl;
            else
                t << MMP_TARGETTYPE "\t\tEXE" << endl;
        }
    } else if (targetType == TypeDll || targetType == TypePlugin) {
        t << MMP_TARGET "\t\t" << fixedTarget << ".dll" << endl;
        if (!skipTargetType) {
            if (project->isActiveConfig("stdbinary"))
                t << MMP_TARGETTYPE "\t\tSTDDLL" << endl;
            else
                t << MMP_TARGETTYPE "\t\tDLL" << endl;
        }
    } else if (targetType == TypeLib) {
        t << MMP_TARGET "\t\t" << fixedTarget << ".lib" << endl;
        if (!skipTargetType) {
            if (project->isActiveConfig("stdbinary"))
                t << MMP_TARGETTYPE "\t\tSTDLIB" << endl;
            else
                t << MMP_TARGETTYPE "\t\tLIB" << endl;
        }
    } else {
        fprintf(stderr, "Error: Unexpected targettype (%d) in SymbianMakefileGenerator::writeMmpFileTargetPart\n", targetType);
    }

    t << endl;

    t << MMP_UID "\t\t" << uid2 << " " << uid3 << endl;

    if (0 != project->values("TARGET.SID").size()) {
        t << MMP_SECUREID "\t\t" << project->values("TARGET.SID").join(" ") << endl;
    } else {
        if (0 == uid3.size())
            t << MMP_SECUREID "\t\t0" << endl;
        else
            t << MMP_SECUREID "\t\t" << uid3 << endl;
    }

    // default value used from mkspecs is 0
    if (0 != project->values("TARGET.VID").size()) {
        t << MMP_VENDORID "\t\t" << project->values("TARGET.VID").join(" ") << endl;
    }

    t << endl;

    if (0 != project->first("TARGET.EPOCSTACKSIZE").size())
        t << MMP_EPOCSTACKSIZE "\t\t" << project->first("TARGET.EPOCSTACKSIZE") << endl;
    if (!skipEpocHeapSize && 0 != project->values("TARGET.EPOCHEAPSIZE").size())
        t << MMP_EPOCHEAPSIZE "\t\t" << project->values("TARGET.EPOCHEAPSIZE").join(" ") << endl;
    if (0 != project->values("TARGET.EPOCALLOWDLLDATA").size())
        t << MMP_EPOCALLOWDLLDATA << endl;

    if (targetType == TypePlugin && !project->isActiveConfig("stdbinary")) {
        // Use custom def file for Qt plugins
        t << "DEFFILE " PLUGIN_COMMON_DEF_FILE_FOR_MMP << endl;
    }

    t << endl;
}


/*
    Application registration resource files should be installed to the
    \private\10003a3f\import\apps directory.
*/
void SymbianMakefileGenerator::writeMmpFileResourcePart(QTextStream& t, QStringList &symbianLangCodes)
{
    if ((targetType == TypeExe) &&
            !project->isActiveConfig("no_icon")) {

        QString locTarget = fixedTarget;
        locTarget.append(".rss");

        t << "SOURCEPATH\t\t\t. " << endl;
        t << "LANG SC ";    // no endl
        foreach(QString lang, symbianLangCodes) {
            t << lang << " "; // no endl
        }
        t << endl;
        t << MMP_START_RESOURCE "\t\t" << locTarget << endl;
        t << "HEADER" << endl;
        t << "TARGETPATH\t\t\t" RESOURCE_DIRECTORY_MMP << endl;
        t << MMP_END_RESOURCE << endl << endl;

        QString regTarget = fixedTarget;
        regTarget.append("_reg.rss");

        t << "SOURCEPATH\t\t\t." << endl;
        t << MMP_START_RESOURCE "\t\t" << regTarget << endl;
        if (isForSymbianSbsv2())
            t << "DEPENDS " << fixedTarget << ".rsg" << endl;
        t << "TARGETPATH\t\t" REGISTRATION_RESOURCE_DIRECTORY_HW << endl;
        t << MMP_END_RESOURCE << endl << endl;
    }
}

void SymbianMakefileGenerator::writeMmpFileSystemIncludePart(QTextStream& t)
{
    QDir current = QDir::current();

    for (QMap<QString, QStringList>::iterator it = systeminclude.begin(); it != systeminclude.end(); ++it) {
        QStringList values = it.value();
        for (int i = 0; i < values.size(); ++i) {
            QString handledPath = values.at(i);
            t << "SYSTEMINCLUDE\t\t" << fixPathForMmp(handledPath, current) << endl;
        }
    }

    t << endl;
}

void SymbianMakefileGenerator::writeMmpFileIncludePart(QTextStream& t)
{
    writeMmpFileSystemIncludePart(t);
}

void SymbianMakefileGenerator::writeMmpFileLibraryPart(QTextStream& t)
{
    QStringList &libs = project->values("LIBS");
    libs << project->values("QMAKE_LIBS") << project->values("QMAKE_LIBS_PRIVATE");

    removeDuplicatedStrings(libs);

    for (int i = 0; i < libs.size(); ++i) {
        QString lib = libs.at(i);
        // The -L flag is uninteresting, since all symbian libraries exist in the same directory.
        if (lib.startsWith("-l")) {
            lib.remove(0, 2);
            QString mmpStatement;
            if (lib.endsWith(".dll")) {
                lib.chop(4);
                mmpStatement = "LIBRARY\t\t";
            } else if (lib.endsWith(".lib")) {
                lib.chop(4);
                mmpStatement = "STATICLIBRARY\t";
            } else {
                // Hacky way to find out what kind of library it is. Check the
                // ARMV5 build directory for library type. We default to shared
                // library, since that is more common.
                QString udebStaticLibLocation(epocRoot());
                QString urelStaticLibLocation(udebStaticLibLocation);
                udebStaticLibLocation += QString("epoc32/release/armv5/udeb/%1.lib").arg(lib);
                urelStaticLibLocation += QString("epoc32/release/armv5/urel/%1.lib").arg(lib);
                if (QFile::exists(udebStaticLibLocation) || QFile::exists(urelStaticLibLocation)) {
                    mmpStatement = "STATICLIBRARY\t";
                } else {
                    mmpStatement = "LIBRARY\t\t";
                }
            }
            t << mmpStatement <<  lib << ".lib" << endl;
        }
    }

    t << endl;
}

void SymbianMakefileGenerator::writeMmpFileCapabilityPart(QTextStream& t)
{
    if (0 != project->first("TARGET.CAPABILITY").size()) {
        QStringList &capabilities = project->values("TARGET.CAPABILITY");
        t << MMP_CAPABILITY "\t\t";

        for (int i = 0; i < capabilities.size(); ++i) {
            QString cap = capabilities.at(i);
            t << cap << " ";
        }
    } else {
        t << MMP_CAPABILITY "\t\tNone";
    }
    t << endl << endl;
}

void SymbianMakefileGenerator::writeMmpFileCompilerOptionPart(QTextStream& t)
{
    QString cw, armcc, gcce;
    QString cwlink, armlink, gccelink;

    if (0 != project->values("QMAKE_CXXFLAGS.CW").size()) {
        cw.append(project->values("QMAKE_CXXFLAGS.CW").join(" "));
        cw.append(" ");
    }

    if (0 != project->values("QMAKE_CXXFLAGS.ARMCC").size()) {
        armcc.append(project->values("QMAKE_CXXFLAGS.ARMCC").join(" "));
        armcc.append(" ");
    }

    if (0 != project->values("QMAKE_CXXFLAGS.GCCE").size()) {
        gcce.append(project->values("QMAKE_CXXFLAGS.GCCE").join(" "));
        gcce.append(" ");
    }

    if (0 != project->values("QMAKE_CFLAGS.CW").size()) {
        cw.append(project->values("QMAKE_CFLAGS.CW").join(" "));
        cw.append(" ");
    }

    if (0 != project->values("QMAKE_CFLAGS.ARMCC").size()) {
        armcc.append(project->values("QMAKE_CFLAGS.ARMCC").join(" "));
        armcc.append(" ");
    }

    if (0 != project->values("QMAKE_CFLAGS.GCCE").size()) {
        gcce.append(project->values("QMAKE_CXXFLAGS.GCCE").join(" "));
        gcce.append(" ");
    }

    if (0 != project->values("QMAKE_CXXFLAGS").size()) {
        cw.append(project->values("QMAKE_CXXFLAGS").join(" "));
        cw.append(" ");
        armcc.append(project->values("QMAKE_CXXFLAGS").join(" "));
        armcc.append(" ");
        gcce.append(project->values("QMAKE_CXXFLAGS").join(" "));
        gcce.append(" ");
    }

    if (0 != project->values("QMAKE_CFLAGS").size()) {
        cw.append(project->values("QMAKE_CFLAGS").join(" "));
        cw.append(" ");
        armcc.append(project->values("QMAKE_CFLAGS").join(" "));
        armcc.append(" ");
        gcce.append(project->values("QMAKE_CFLAGS").join(" "));
        gcce.append(" ");
    }

    if (0 != project->values("QMAKE_LFLAGS.CW").size()) {
        cwlink.append(project->values("QMAKE_LFLAGS.CW").join(" "));
        cwlink.append(" ");
    }

    if (0 != project->values("QMAKE_LFLAGS.ARMCC").size()) {
        armlink.append(project->values("QMAKE_LFLAGS.ARMCC").join(" "));
        armlink.append(" ");
    }

    if (0 != project->values("QMAKE_LFLAGS.GCCE").size()) {
        gccelink.append(project->values("QMAKE_LFLAGS.GCCE").join(" "));
        gccelink.append(" ");
    }

    if (0 != project->values("QMAKE_LFLAGS").size()) {
        cwlink.append(project->values("QMAKE_LFLAGS").join(" "));
        cwlink.append(" ");
        armlink.append(project->values("QMAKE_LFLAGS").join(" "));
        armlink.append(" ");
        gccelink.append(project->values("QMAKE_LFLAGS").join(" "));
        gccelink.append(" ");
    }

    if (!cw.isEmpty() && cw[cw.size()-1] == ' ')
        cw.chop(1);
    if (!armcc.isEmpty() && armcc[armcc.size()-1] == ' ')
        armcc.chop(1);
    if (!gcce.isEmpty() && gcce[gcce.size()-1] == ' ')
        gcce.chop(1);
    if (!cwlink.isEmpty() && cwlink[cwlink.size()-1] == ' ')
        cwlink.chop(1);
    if (!armlink.isEmpty() && armlink[armlink.size()-1] == ' ')
        armlink.chop(1);
    if (!gccelink.isEmpty() && gccelink[gccelink.size()-1] == ' ')
        gccelink.chop(1);

    if (!cw.isEmpty())
        t << MMP_OPTION_CW " " << cw <<  endl;
    if (!armcc.isEmpty())
        t << MMP_OPTION_ARMCC " " << armcc <<  endl;
    if (!gcce.isEmpty())
        t << MMP_OPTION_GCCE " " << gcce <<  endl;

    if (!cwlink.isEmpty())
        t << MMP_LINKEROPTION_CW " " << cwlink <<  endl;
    if (!armlink.isEmpty())
        t << MMP_LINKEROPTION_ARMCC " " << armlink <<  endl;
    if (!gccelink.isEmpty())
        t << MMP_LINKEROPTION_GCCE " " << gccelink <<  endl;

    t << endl;
}

void SymbianMakefileGenerator::writeMmpFileBinaryVersionPart(QTextStream& t)
{
    QString applicationVersion = project->first("VERSION");
    QStringList verNumList = applicationVersion.split('.');
    uint major = 0;
    uint minor = 0;
    uint patch = 0;
    bool success = false;

    if (verNumList.size() > 0) {
        major = verNumList[0].toUInt(&success);
        if (success && verNumList.size() > 1) {
            minor = verNumList[1].toUInt(&success);
            if (success && verNumList.size() > 2) {
                patch = verNumList[2].toUInt(&success);
            }
        }
    }

    QString mmpVersion;
    if (success && major <= 0xFFFF && minor <= 0xFF && patch <= 0xFF) {
        // Symbian binary version only has major and minor components, so compress
        // Qt's minor and patch values into the minor component. Since Symbian's minor
        // component is a 16 bit value, only allow 8 bits for each to avoid overflow.
        mmpVersion.append(QString::number(major))
            .append('.')
            .append(QString::number((minor << 8) + patch));
    } else {
        if (!applicationVersion.isEmpty())
            fprintf(stderr, "Invalid VERSION string: %s\n", qPrintable(applicationVersion));
        mmpVersion = "10.0"; // Default binary version for symbian is 10.0
    }

    t << MMP_VERSION " " << mmpVersion  << endl;
}

void SymbianMakefileGenerator::writeMmpFileRulesPart(QTextStream& t)
{
    foreach(QString item, project->values("MMP_RULES")) {
        t << endl;
        // If there is no stringlist defined for a rule, use rule name directly
        // This is convenience for defining single line mmp statements
        if (project->values(item).isEmpty()) {
            t << item << endl;
        } else {
            foreach(QString itemRow, project->values(item)) {
                t << itemRow << endl;
            }
        }
    }
}

void SymbianMakefileGenerator::writeBldInfContent(QTextStream &t, bool addDeploymentExtension, const QString &iconFile, DeploymentList &depList)
{
    // Read user defined bld inf rules

    QMap<QString, QStringList> userBldInfRules;
    for (QMap<QString, QStringList>::iterator it = project->variables().begin(); it != project->variables().end(); ++it) {
        if (it.key().startsWith(BLD_INF_RULES_BASE)) {
            QString newKey = it.key().mid(sizeof(BLD_INF_RULES_BASE) - 1);
            if (newKey.isEmpty()) {
                fprintf(stderr, "Warning: Empty BLD_INF_RULES key encountered\n");
                continue;
            }
            QStringList newValues;
            QStringList values = it.value();
            foreach(QString item, values) {
                // If there is no stringlist defined for a rule, use rule name directly
                // This is convenience for defining single line statements
                if (project->values(item).isEmpty()) {
                    newValues << item;
                } else {
                    foreach(QString itemRow, project->values(item)) {
                        newValues << itemRow;
                    }
                }
            }
            userBldInfRules.insert(newKey, newValues);
        }
    }

    // Add includes of subdirs bld.inf files

    QString mmpfilename = escapeFilePath(fileFixify(project->projectFile()));
    mmpfilename = mmpfilename.replace(mmpfilename.lastIndexOf("."), 4, Option::mmp_ext);
    QString currentPath = qmake_getpwd();
    QDir directory(currentPath);

    const QStringList &subdirs = project->values("SUBDIRS");
    foreach(QString item, subdirs) {
        QString fixedItem;
        if (!project->isEmpty(item + ".file")) {
            fixedItem = project->first(item + ".file");
        } else if (!project->isEmpty(item + ".subdir")) {
            fixedItem = project->first(item + ".subdir");
        } else {
            fixedItem = item;
        }

        QString condition;
        if (!project->isEmpty(item + ".condition"))
            condition = project->first(item + ".condition");

        QFileInfo subdir(fileInfo(fixedItem));
        QString relativePath = directory.relativeFilePath(fixedItem);
        QString subdirFileName = subdir.completeBaseName();
        QString fullProName = subdir.absoluteFilePath();;
        QString bldinfFilename;

        if (subdir.isDir()) {
            // Subdir is a regular project
            bldinfFilename = relativePath + QString("/") + QString(BLD_INF_FILENAME);
            fullProName += QString("/") + subdirFileName + Option::pro_ext;
        } else {
            // Subdir is actually a .pro file
            if (relativePath.contains("/")) {
                // .pro not in same directory as parent .pro
                relativePath.remove(relativePath.lastIndexOf("/") + 1, relativePath.length());
                bldinfFilename = relativePath;
            } else {
                // .pro and parent .pro in same directory
                bldinfFilename = QString("./");
            }
            bldinfFilename += QString(BLD_INF_FILENAME ".") + subdirFileName;
        }

        QString uid = generate_uid(fullProName);
        QString bldinfDefine = QString("BLD_INF_") + subdirFileName + QString("_") + uid;
        bldinfDefine = bldinfDefine.toUpper();
        removeSpecialCharacters(bldinfDefine);

        if (!condition.isEmpty())
            t << "#if defined(" << condition << ")" << endl;

        t << "#ifndef " << bldinfDefine << endl;
        t << "\t#include \"" << bldinfFilename << "\"" << endl;
        t << "#endif" << endl;

        if (!condition.isEmpty())
            t << "#endif" << endl;

    }

    // Add supported project platforms

    t << endl << BLD_INF_TAG_PLATFORMS << endl << endl;
    if (0 != project->values("SYMBIAN_PLATFORMS").size())
        t << project->values("SYMBIAN_PLATFORMS").join(" ") << endl;

    QStringList userItems = userBldInfRules.value(BLD_INF_TAG_PLATFORMS);
    foreach(QString item, userItems)
        t << item << endl;
    userBldInfRules.remove(BLD_INF_TAG_PLATFORMS);
    t << endl;

    // Add project mmps and old style extension makefiles

    QString mmpTag;
    if (project->isActiveConfig(SYMBIAN_TEST_CONFIG))
        mmpTag = QLatin1String(BLD_INF_TAG_TESTMMPFILES);
    else
        mmpTag = QLatin1String(BLD_INF_TAG_MMPFILES);

    t << endl << mmpTag << endl << endl;

    writeBldInfMkFilePart(t, addDeploymentExtension);
    if (targetType != TypeSubdirs) {
        QString shortProFilename = project->projectFile();
        shortProFilename.replace(0, shortProFilename.lastIndexOf("/") + 1, QString(""));
        shortProFilename.replace(Option::pro_ext, QString(""));

        QString mmpFilename = shortProFilename + QString("_") + uid3 + Option::mmp_ext;

        t << mmpFilename << endl;
    }

    userItems = userBldInfRules.value(mmpTag);
    foreach(QString item, userItems)
        t << item << endl;
    userBldInfRules.remove(mmpTag);

    QString extensionTag;
    if (project->isActiveConfig(SYMBIAN_TEST_CONFIG))
        extensionTag = QLatin1String(BLD_INF_TAG_TESTEXTENSIONS);
    else
        extensionTag = QLatin1String(BLD_INF_TAG_EXTENSIONS);

    t << endl << extensionTag << endl << endl;

    // Generate extension rules

    writeBldInfExtensionRulesPart(t, iconFile);

    userItems = userBldInfRules.value(extensionTag);
    foreach(QString item, userItems)
        t << item << endl;
    userBldInfRules.remove(extensionTag);

    // Add rest of the user defined content

    for (QMap<QString, QStringList>::iterator it = userBldInfRules.begin(); it != userBldInfRules.end(); ++it) {
        t << endl << endl << it.key() << endl << endl;
        userItems = it.value();
        foreach(QString item, userItems)
            t << item << endl;
    }
}

void SymbianMakefileGenerator::writeRegRssFile(QMap<QString, QStringList> &userItems)
{
    QString filename(fixedTarget);
    filename.append("_reg.rss");
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();
        QTextStream t(&ft);
        t << "// ============================================================================" << endl;
        t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
        t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
        t << "// * This file is generated by qmake and should not be modified by the" << endl;
        t << "// * user." << endl;
        t << "// ============================================================================" << endl;
        t << endl;
        t << "#include <" << fixedTarget << ".rsg>" << endl;
        t << "#include <appinfo.rh>" << endl;
        foreach(QString item, userItems[RSS_TAG_HEADER])
            t << item << endl;
        t << endl;
        t << "UID2 KUidAppRegistrationResourceFile" << endl;
        t << "UID3 " << uid3 << endl << endl;
        t << "RESOURCE APP_REGISTRATION_INFO" << endl;
        t << "\t{" << endl;
        t << "\tapp_file=\"" << fixedTarget << "\";" << endl;
        t << "\tlocalisable_resource_file=\"" RESOURCE_DIRECTORY_RESOURCE << fixedTarget << "\";" << endl;

        writeRegRssList(t, userItems[RSS_TAG_SERVICE_LIST],
                        QLatin1String(RSS_TAG_SERVICE_LIST),
                        QLatin1String("SERVICE_INFO"));
        writeRegRssList(t, userItems[RSS_TAG_FILE_OWNERSHIP_LIST],
                        QLatin1String(RSS_TAG_FILE_OWNERSHIP_LIST),
                        QLatin1String("FILE_OWNERSHIP_INFO"));
        writeRegRssList(t, userItems[RSS_TAG_DATATYPE_LIST],
                        QLatin1String(RSS_TAG_DATATYPE_LIST),
                        QLatin1String("DATATYPE"));
        t << endl;

        foreach(QString item, userItems[RSS_TAG_DEFAULT])
            t << "\t" << item.replace("\n","\n\t") << endl;
        t << "\t}" << endl;

        foreach(QString item, userItems[RSS_TAG_FOOTER])
            t << item << endl;
    } else {
        PRINT_FILE_CREATE_ERROR(filename)
    }
}

void SymbianMakefileGenerator::writeRegRssList(QTextStream &t,
                                               QStringList &userList,
                                               const QString &listTag,
                                               const QString &listItem)
{
    int itemCount = userList.count();
    if (itemCount) {
        t << "\t" << listTag << " ="<< endl;
        t << "\t\t{" << endl;
        foreach(QString item, userList) {
            t << "\t\t" << listItem << endl;
            t << "\t\t\t{" << endl;
            t << "\t\t\t" << item.replace("\n","\n\t\t\t") << endl;
            t << "\t\t\t}";
            if (--itemCount)
                t << ",";
            t << endl;
        }
        t << "\t\t}; "<< endl;
    }
}

void SymbianMakefileGenerator::writeRssFile(QString &numberOfIcons, QString &iconFile)
{
    QString filename(fixedTarget);
    filename.append(".rss");
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();
        QTextStream t(&ft);
        t << "// ============================================================================" << endl;
        t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
        t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
        t << "// * This file is generated by qmake and should not be modified by the" << endl;
        t << "// * user." << endl;
        t << "// ============================================================================" << endl;
        t << endl;
        t << "#include <appinfo.rh>" << endl;
        t << "#include \"" << fixedTarget << ".loc\"" << endl;
        t << endl;
        t << "RESOURCE LOCALISABLE_APP_INFO r_localisable_app_info" << endl;
        t << "\t{" << endl;
        t << "\tshort_caption = STRING_r_short_caption;" << endl;
        t << "\tcaption_and_icon =" << endl;
        t << "\tCAPTION_AND_ICON_INFO" << endl;
        t << "\t\t{" << endl;
        t << "\t\tcaption = STRING_r_caption;" << endl;

        QString rssIconFile = iconFile;
        rssIconFile = rssIconFile.replace("/", "\\\\");

        if (numberOfIcons.isEmpty() || rssIconFile.isEmpty()) {
            // There can be maximum one item in this tag, validated when parsed
            t << "\t\tnumber_of_icons = 0;" << endl;
            t << "\t\ticon_file = \"\";" << endl;
        } else {
            // There can be maximum one item in this tag, validated when parsed
            t << "\t\tnumber_of_icons = " << numberOfIcons << ";" << endl;
            t << "\t\ticon_file = \"" << rssIconFile << "\";" << endl;
        }
        t << "\t\t};" << endl;
        t << "\t}" << endl;
        t << endl;
    } else {
        PRINT_FILE_CREATE_ERROR(filename);
    }
}

void SymbianMakefileGenerator::writeLocFile(QStringList &symbianLangCodes)
{
    QString filename(fixedTarget);
    filename.append(".loc");
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();
        QTextStream t(&ft);
        t << "// ============================================================================" << endl;
        t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
        t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
        t << "// * This file is generated by qmake and should not be modified by the" << endl;
        t << "// * user." << endl;
        t << "// ============================================================================" << endl;
        t << endl;
        t << "#ifdef LANGUAGE_SC" << endl;
        t << "#define STRING_r_short_caption \"" << fixedTarget  << "\"" << endl;
        t << "#define STRING_r_caption \"" << fixedTarget  << "\"" << endl;
        foreach(QString lang, symbianLangCodes) {
            t << "#elif defined LANGUAGE_" << lang << endl;
            t << "#define STRING_r_short_caption \"" << fixedTarget  << "\"" << endl;
            t << "#define STRING_r_caption \"" << fixedTarget  << "\"" << endl;
        }
        t << "#else" << endl;
        t << "#define STRING_r_short_caption \"" << fixedTarget  << "\"" << endl;
        t << "#define STRING_r_caption \"" << fixedTarget  << "\"" << endl;
        t << "#endif" << endl;
    } else {
        PRINT_FILE_CREATE_ERROR(filename);
    }
}

void SymbianMakefileGenerator::readRssRules(QString &numberOfIcons,
                                            QString &iconFile, QMap<QString,
                                            QStringList> &userRssRules)
{
    for (QMap<QString, QStringList>::iterator it = project->variables().begin(); it != project->variables().end(); ++it) {
        if (it.key().startsWith(RSS_RULES_BASE)) {
            QString newKey = it.key().mid(sizeof(RSS_RULES_BASE) - 1);
            if (newKey.isEmpty()) {
                fprintf(stderr, "Warning: Empty RSS_RULES_BASE key encountered\n");
                continue;
            }
            QStringList newValues;
            QStringList values = it.value();
            foreach(QString item, values) {
                // If there is no stringlist defined for a rule, use rule value directly
                // This is convenience for defining single line statements
                if (project->values(item).isEmpty()) {
                    newValues << item;
                } else {
                    QStringList itemList;
                    foreach(QString itemRow, project->values(item)) {
                        itemList << itemRow;
                    }
                    newValues << itemList.join("\n");
                }
            }
            // Verify thet there is exactly one value in RSS_TAG_NBROFICONS
            if (newKey == RSS_TAG_NBROFICONS) {
                if (newValues.count() == 1) {
                    numberOfIcons = newValues[0];
                } else {
                    fprintf(stderr, "Warning: There must be exactly one value in '%s%s'\n",
                            RSS_RULES_BASE, RSS_TAG_NBROFICONS);
                    continue;
                }
            // Verify thet there is exactly one value in RSS_TAG_ICONFILE
            } else if (newKey == RSS_TAG_ICONFILE) {
                if (newValues.count() == 1) {
                    iconFile = newValues[0];
                } else {
                    fprintf(stderr, "Warning: There must be exactly one value in '%s%s'\n",
                            RSS_RULES_BASE, RSS_TAG_ICONFILE);
                    continue;
                }
            } else if (newKey == RSS_TAG_HEADER
                       || newKey == RSS_TAG_SERVICE_LIST
                       || newKey == RSS_TAG_FILE_OWNERSHIP_LIST
                       || newKey == RSS_TAG_DATATYPE_LIST
                       || newKey == RSS_TAG_FOOTER
                       || newKey == RSS_TAG_DEFAULT) {
                userRssRules[newKey] = newValues;
                continue;
            } else {
                fprintf(stderr, "Warning: Unsupported key:'%s%s'\n",
                        RSS_RULES_BASE, newKey.toLatin1().constData());
                continue;
            }
        }
    }

    QStringList newValues;
    foreach(QString item, project->values(RSS_RULES)) {
        // If there is no stringlist defined for a rule, use rule value directly
        // This is convenience for defining single line statements
        if (project->values(item).isEmpty()) {
            newValues << item;
        } else {
            newValues << project->values(item);
        }
    }
    userRssRules[RSS_TAG_DEFAULT] << newValues;

    // Validate that either both RSS_TAG_NBROFICONS and RSS_TAG_ICONFILE keys exist
    // or neither of them exist
    if (!((numberOfIcons.isEmpty() && iconFile.isEmpty()) ||
            (!numberOfIcons.isEmpty() && !iconFile.isEmpty()))) {
        numberOfIcons.clear();
        iconFile.clear();
        fprintf(stderr, "Warning: Both or neither of '%s%s' and '%s%s' keys must exist.\n",
                RSS_RULES_BASE, RSS_TAG_NBROFICONS, RSS_RULES_BASE, RSS_TAG_ICONFILE);
    }

    // Validate that RSS_TAG_NBROFICONS contains only numbers
    if (!numberOfIcons.isEmpty()) {
        bool ok;
        numberOfIcons = numberOfIcons.simplified();
        numberOfIcons.toInt(&ok);
        if (!ok) {
            numberOfIcons.clear();
            iconFile.clear();
            fprintf(stderr, "Warning: '%s%s' must be integer in decimal format.\n",
                    RSS_RULES_BASE, RSS_TAG_NBROFICONS);
        }
    }
}

QStringList SymbianMakefileGenerator::symbianLangCodesFromTsFiles()
{
    QStringList tsfiles;
    QStringList symbianLangCodes;
    tsfiles << project->values("TRANSLATIONS");

    fillQt2S60LangMapTable();

    foreach(QString file, tsfiles) {
        int extIndex = file.lastIndexOf(".");
        int langIndex = file.lastIndexOf("_", (extIndex - file.length()));
        langIndex += 1;
        QString qtlang = file.mid(langIndex, extIndex - langIndex);
        QString s60lang = qt2S60LangMapTable.value(qtlang, QString("SC"));

        if (!symbianLangCodes.contains(s60lang) && s60lang != "SC")
            symbianLangCodes += s60lang;
    }

    return symbianLangCodes;
}

void SymbianMakefileGenerator::fillQt2S60LangMapTable()
{
    qt2S60LangMapTable.reserve(170); // 165 items at time of writing.
    qt2S60LangMapTable.insert("ab", "SC");            //Abkhazian                     //
    qt2S60LangMapTable.insert("om", "SC");            //Afan                          //
    qt2S60LangMapTable.insert("aa", "SC");            //Afar                          //
    qt2S60LangMapTable.insert("af", "34");            //Afrikaans                     //Afrikaans
    qt2S60LangMapTable.insert("sq", "35");            //Albanian                      //Albanian
    qt2S60LangMapTable.insert("am", "36");            //Amharic                       //Amharic
    qt2S60LangMapTable.insert("ar", "37");            //Arabic                        //Arabic
    qt2S60LangMapTable.insert("hy", "38");            //Armenian                      //Armenian
    qt2S60LangMapTable.insert("as", "SC");            //Assamese                      //
    qt2S60LangMapTable.insert("ay", "SC");            //Aymara                        //
    qt2S60LangMapTable.insert("az", "SC");            //Azerbaijani                   //
    qt2S60LangMapTable.insert("ba", "SC");            //Bashkir                       //
    qt2S60LangMapTable.insert("eu", "SC");            //Basque                        //
    qt2S60LangMapTable.insert("bn", "41");            //Bengali                       //Bengali
    qt2S60LangMapTable.insert("dz", "SC");            //Bhutani                       //
    qt2S60LangMapTable.insert("bh", "SC");            //Bihari                        //
    qt2S60LangMapTable.insert("bi", "SC");            //Bislama                       //
    qt2S60LangMapTable.insert("br", "SC");            //Breton                        //
    qt2S60LangMapTable.insert("bg", "42");            //Bulgarian                     //Bulgarian
    qt2S60LangMapTable.insert("my", "43");            //Burmese                       //Burmese
    qt2S60LangMapTable.insert("be", "40");            //Byelorussian                  //Belarussian
    qt2S60LangMapTable.insert("km", "SC");            //Cambodian                     //
    qt2S60LangMapTable.insert("ca", "44");            //Catalan                       //Catalan
    qt2S60LangMapTable.insert("zh", "SC");            //Chinese                       //
    qt2S60LangMapTable.insert("co", "SC");            //Corsican                      //
    qt2S60LangMapTable.insert("hr", "45");            //Croatian                      //Croatian
    qt2S60LangMapTable.insert("cs", "25");            //Czech                         //Czech
    qt2S60LangMapTable.insert("da", "07");            //Danish                        //Danish
    qt2S60LangMapTable.insert("nl", "18");            //Dutch                         //Dutch
    qt2S60LangMapTable.insert("en", "01");            //English                       //English(UK)
    qt2S60LangMapTable.insert("eo", "SC");            //Esperanto                     //
    qt2S60LangMapTable.insert("et", "49");            //Estonian                      //Estonian
    qt2S60LangMapTable.insert("fo", "SC");            //Faroese                       //
    qt2S60LangMapTable.insert("fj", "SC");            //Fiji                          //
    qt2S60LangMapTable.insert("fi", "09");            //Finnish                       //Finnish
    qt2S60LangMapTable.insert("fr", "02");            //French                        //French
    qt2S60LangMapTable.insert("fy", "SC");            //Frisian                       //
    qt2S60LangMapTable.insert("gd", "52");            //Gaelic                        //Gaelic
    qt2S60LangMapTable.insert("gl", "SC");            //Galician                      //
    qt2S60LangMapTable.insert("ka", "53");            //Georgian                      //Georgian
    qt2S60LangMapTable.insert("de", "03");            //German                        //German
    qt2S60LangMapTable.insert("el", "54");            //Greek                         //Greek
    qt2S60LangMapTable.insert("kl", "SC");            //Greenlandic                   //
    qt2S60LangMapTable.insert("gn", "SC");            //Guarani                       //
    qt2S60LangMapTable.insert("gu", "56");            //Gujarati                      //Gujarati
    qt2S60LangMapTable.insert("ha", "SC");            //Hausa                         //
    qt2S60LangMapTable.insert("he", "57");            //Hebrew                        //Hebrew
    qt2S60LangMapTable.insert("hi", "58");            //Hindi                         //Hindi
    qt2S60LangMapTable.insert("hu", "17");            //Hungarian                     //Hungarian
    qt2S60LangMapTable.insert("is", "15");            //Icelandic                     //Icelandic
    qt2S60LangMapTable.insert("id", "59");            //Indonesian                    //Indonesian
    qt2S60LangMapTable.insert("ia", "SC");            //Interlingua                   //
    qt2S60LangMapTable.insert("ie", "SC");            //Interlingue                   //
    qt2S60LangMapTable.insert("iu", "SC");            //Inuktitut                     //
    qt2S60LangMapTable.insert("ik", "SC");            //Inupiak                       //
    qt2S60LangMapTable.insert("ga", "60");            //Irish                         //Irish
    qt2S60LangMapTable.insert("it", "05");            //Italian                       //Italian
    qt2S60LangMapTable.insert("ja", "32");            //Japanese                      //Japanese
    qt2S60LangMapTable.insert("jv", "SC");            //Javanese                      //
    qt2S60LangMapTable.insert("kn", "62");            //Kannada                       //Kannada
    qt2S60LangMapTable.insert("ks", "SC");            //Kashmiri                      //
    qt2S60LangMapTable.insert("kk", "63");            //Kazakh                        //Kazakh
    qt2S60LangMapTable.insert("rw", "SC");            //Kinyarwanda                   //
    qt2S60LangMapTable.insert("ky", "SC");            //Kirghiz                       //
    qt2S60LangMapTable.insert("ko", "65");            //Korean                        //Korean
    qt2S60LangMapTable.insert("ku", "SC");            //Kurdish                       //
    qt2S60LangMapTable.insert("rn", "SC");            //Kurundi                       //
    qt2S60LangMapTable.insert("lo", "66");            //Laothian                      //Laothian
    qt2S60LangMapTable.insert("la", "SC");            //Latin                         //
    qt2S60LangMapTable.insert("lv", "67");            //Latvian                       //Latvian
    qt2S60LangMapTable.insert("ln", "SC");            //Lingala                       //
    qt2S60LangMapTable.insert("lt", "68");            //Lithuanian                    //Lithuanian
    qt2S60LangMapTable.insert("mk", "69");            //Macedonian                    //Macedonian
    qt2S60LangMapTable.insert("mg", "SC");            //Malagasy                      //
    qt2S60LangMapTable.insert("ms", "70");            //Malay                         //Malay
    qt2S60LangMapTable.insert("ml", "71");            //Malayalam                     //Malayalam
    qt2S60LangMapTable.insert("mt", "SC");            //Maltese                       //
    qt2S60LangMapTable.insert("mi", "SC");            //Maori                         //
    qt2S60LangMapTable.insert("mr", "72");            //Marathi                       //Marathi
    qt2S60LangMapTable.insert("mo", "73");            //Moldavian                     //Moldovian
    qt2S60LangMapTable.insert("mn", "74");            //Mongolian                     //Mongolian
    qt2S60LangMapTable.insert("na", "SC");            //Nauru                         //
    qt2S60LangMapTable.insert("ne", "SC");            //Nepali                        //
    qt2S60LangMapTable.insert("nb", "08");            //Norwegian                     //Norwegian
    qt2S60LangMapTable.insert("oc", "SC");            //Occitan                       //
    qt2S60LangMapTable.insert("or", "SC");            //Oriya                         //
    qt2S60LangMapTable.insert("ps", "SC");            //Pashto                        //
    qt2S60LangMapTable.insert("fa", "SC");            //Persian                       //
    qt2S60LangMapTable.insert("pl", "27");            //Polish                        //Polish
    qt2S60LangMapTable.insert("pt", "13");            //Portuguese                    //Portuguese
    qt2S60LangMapTable.insert("pa", "77");            //Punjabi                       //Punjabi
    qt2S60LangMapTable.insert("qu", "SC");            //Quechua                       //
    qt2S60LangMapTable.insert("rm", "SC");            //RhaetoRomance                 //
    qt2S60LangMapTable.insert("ro", "78");            //Romanian                      //Romanian
    qt2S60LangMapTable.insert("ru", "16");            //Russian                       //Russian
    qt2S60LangMapTable.insert("sm", "SC");            //Samoan                        //
    qt2S60LangMapTable.insert("sg", "SC");            //Sangho                        //
    qt2S60LangMapTable.insert("sa", "SC");            //Sanskrit                      //
    qt2S60LangMapTable.insert("sr", "79");            //Serbian                       //Serbian
    qt2S60LangMapTable.insert("sh", "SC");            //SerboCroatian                 //
    qt2S60LangMapTable.insert("st", "SC");            //Sesotho                       //
    qt2S60LangMapTable.insert("tn", "SC");            //Setswana                      //
    qt2S60LangMapTable.insert("sn", "SC");            //Shona                         //
    qt2S60LangMapTable.insert("sd", "SC");            //Sindhi                        //
    qt2S60LangMapTable.insert("si", "80");            //Singhalese                    //Sinhalese
    qt2S60LangMapTable.insert("ss", "SC");            //Siswati                       //
    qt2S60LangMapTable.insert("sk", "26");            //Slovak                        //Slovak
    qt2S60LangMapTable.insert("sl", "28");            //Slovenian                     //Slovenian
    qt2S60LangMapTable.insert("so", "81");            //Somali                        //Somali
    qt2S60LangMapTable.insert("es", "04");            //Spanish                       //Spanish
    qt2S60LangMapTable.insert("su", "SC");            //Sundanese                     //
    qt2S60LangMapTable.insert("sw", "84");            //Swahili                       //Swahili
    qt2S60LangMapTable.insert("sv", "06");            //Swedish                       //Swedish
    qt2S60LangMapTable.insert("tl", "39");            //Tagalog                       //Tagalog
    qt2S60LangMapTable.insert("tg", "SC");            //Tajik                         //
    qt2S60LangMapTable.insert("ta", "87");            //Tamil                         //Tamil
    qt2S60LangMapTable.insert("tt", "SC");            //Tatar                         //
    qt2S60LangMapTable.insert("te", "88");            //Telugu                        //Telugu
    qt2S60LangMapTable.insert("th", "33");            //Thai                          //Thai
    qt2S60LangMapTable.insert("bo", "89");            //Tibetan                       //Tibetan
    qt2S60LangMapTable.insert("ti", "90");            //Tigrinya                      //Tigrinya
    qt2S60LangMapTable.insert("to", "SC");            //Tonga                         //
    qt2S60LangMapTable.insert("ts", "SC");            //Tsonga                        //
    qt2S60LangMapTable.insert("tr", "14");            //Turkish                       //Turkish
    qt2S60LangMapTable.insert("tk", "92");            //Turkmen                       //Turkmen
    qt2S60LangMapTable.insert("tw", "SC");            //Twi                           //
    qt2S60LangMapTable.insert("ug", "SC");            //Uigur                         //
    qt2S60LangMapTable.insert("uk", "93");            //Ukrainian                     //Ukrainian
    qt2S60LangMapTable.insert("ur", "94");            //Urdu                          //Urdu
    qt2S60LangMapTable.insert("uz", "SC");            //Uzbek                         //
    qt2S60LangMapTable.insert("vi", "96");            //Vietnamese                    //Vietnamese
    qt2S60LangMapTable.insert("vo", "SC");            //Volapuk                       //
    qt2S60LangMapTable.insert("cy", "97");            //Welsh                         //Welsh
    qt2S60LangMapTable.insert("wo", "SC");            //Wolof                         //
    qt2S60LangMapTable.insert("xh", "SC");            //Xhosa                         //
    qt2S60LangMapTable.insert("yi", "SC");            //Yiddish                       //
    qt2S60LangMapTable.insert("yo", "SC");            //Yoruba                        //
    qt2S60LangMapTable.insert("za", "SC");            //Zhuang                        //
    qt2S60LangMapTable.insert("zu", "98");            //Zulu                          //Zulu
    qt2S60LangMapTable.insert("nn", "75");            //Nynorsk                       //NorwegianNynorsk
    qt2S60LangMapTable.insert("bs", "SC");            //Bosnian                       //
    qt2S60LangMapTable.insert("dv", "SC");            //Divehi                        //
    qt2S60LangMapTable.insert("gv", "SC");            //Manx                          //
    qt2S60LangMapTable.insert("kw", "SC");            //Cornish                       //
    qt2S60LangMapTable.insert("ak", "SC");            //Akan                          //
    qt2S60LangMapTable.insert("kok", "SC");           //Konkani                       //
    qt2S60LangMapTable.insert("gaa", "SC");           //Ga                            //
    qt2S60LangMapTable.insert("ig", "SC");            //Igbo                          //
    qt2S60LangMapTable.insert("kam", "SC");           //Kamba                         //
    qt2S60LangMapTable.insert("syr", "SC");           //Syriac                        //
    qt2S60LangMapTable.insert("byn", "SC");           //Blin                          //
    qt2S60LangMapTable.insert("gez", "SC");           //Geez                          //
    qt2S60LangMapTable.insert("kfo", "SC");           //Koro                          //
    qt2S60LangMapTable.insert("sid", "SC");           //Sidamo                        //
    qt2S60LangMapTable.insert("cch", "SC");           //Atsam                         //
    qt2S60LangMapTable.insert("tig", "SC");           //Tigre                         //
    qt2S60LangMapTable.insert("kaj", "SC");           //Jju                           //
    qt2S60LangMapTable.insert("fur", "SC");           //Friulian                      //
    qt2S60LangMapTable.insert("ve", "SC");            //Venda                         //
    qt2S60LangMapTable.insert("ee", "SC");            //Ewe                           //
    qt2S60LangMapTable.insert("wa", "SC");            //Walamo                        //
    qt2S60LangMapTable.insert("haw", "SC");           //Hawaiian                      //
    qt2S60LangMapTable.insert("kcg", "SC");           //Tyap                          //
    qt2S60LangMapTable.insert("ny", "SC");            //Chewa                         //
}

void SymbianMakefileGenerator::appendIfnotExist(QStringList &list, QString value)
{
    if (!list.contains(value))
        list += value;
}

void SymbianMakefileGenerator::appendIfnotExist(QStringList &list, QStringList values)
{
    foreach(QString item, values)
        appendIfnotExist(list, item);
}

QString SymbianMakefileGenerator::removePathSeparators(QString &file)
{
    QString ret = file;
    while (ret.indexOf(QDir::separator()) > 0) {
        ret.remove(0, ret.indexOf(QDir::separator()) + 1);
    }

    return ret;
}


QString SymbianMakefileGenerator::removeTrailingPathSeparators(QString &file)
{
    QString ret = file;
    if (ret.endsWith(QDir::separator())) {
        ret.remove(ret.length() - 1, 1);
    }

    return ret;
}

void SymbianMakefileGenerator::generateCleanCommands(QTextStream& t,
        const QStringList& toClean,
        const QString& cmd,
        const QString& cmdOptions,
        const QString& itemPrefix,
        const QString& itemSuffix)
{
    for (int i = 0; i < toClean.size(); ++i) {
        QString item = toClean.at(i);
        item.prepend(itemPrefix).append(itemSuffix);
#if defined(Q_OS_WIN)
        t << "\t-@ if EXIST \"" << QDir::toNativeSeparators(item) << "\" ";
        t << cmd << " " << cmdOptions << " \"" << QDir::toNativeSeparators(item) << "\"" << endl;
#else
        t << "\t-if test -e " << QDir::toNativeSeparators(item) << "; then ";
        t << cmd << " " << cmdOptions << " " << QDir::toNativeSeparators(item) << "; fi" << endl;
#endif
    }
}

void SymbianMakefileGenerator::removeSpecialCharacters(QString& str)
{
    // When modifying this method check also application_icon.prf
    str.replace(QString("/"), QString("_"));
    str.replace(QString("\\"), QString("_"));
    str.replace(QString("-"), QString("_"));
    str.replace(QString(":"), QString("_"));
    str.replace(QString("."), QString("_"));
    str.replace(QString(" "), QString("_"));
}

void SymbianMakefileGenerator::generateDistcleanTargets(QTextStream& t)
{
    t << "dodistclean:" << endl;
    const QStringList &subdirs = project->values("SUBDIRS");
    foreach(QString item, subdirs) {
        bool fromFile = false;
        QString fixedItem;
        if (!project->isEmpty(item + ".file")) {
            fixedItem = project->first(item + ".file");
            fromFile = true;
        } else if (!project->isEmpty(item + ".subdir")) {
            fixedItem = project->first(item + ".subdir");
            fromFile = false;
        } else {
            fromFile = item.endsWith(Option::pro_ext);
            fixedItem = item;
        }
        QFileInfo fi(fileInfo(fixedItem));
        if (!fromFile) {
            t << "\t-$(MAKE) -f \"" << Option::fixPathToTargetOS(fi.absoluteFilePath() + "/Makefile") << "\" dodistclean" << endl;
        } else {
            QString itemName = fi.fileName();
            int extIndex = itemName.lastIndexOf(Option::pro_ext);
            if (extIndex)
                fixedItem = fi.absolutePath() + "/" + QString("Makefile.") + itemName.mid(0, extIndex);
            t << "\t-$(MAKE) -f \"" << Option::fixPathToTargetOS(fixedItem) << "\" dodistclean" << endl;
        }

    }

    generatedFiles << Option::fixPathToTargetOS(fileInfo(Option::output.fileName()).absoluteFilePath()); // bld.inf
    generatedFiles << project->values("QMAKE_INTERNAL_PRL_FILE"); // Add generated prl files for cleanup
    generatedFiles << project->values("QMAKE_DISTCLEAN"); // Add any additional files marked for distclean
    QStringList fixedFiles;
    QStringList fixedDirs;
    foreach(QString item, generatedFiles) {
        QString fixedItem = Option::fixPathToTargetOS(fileInfo(item).absoluteFilePath());
        if (!fixedFiles.contains(fixedItem)) {
            fixedFiles << fixedItem;
        }
    }
    foreach(QString item, generatedDirs) {
        QString fixedItem = Option::fixPathToTargetOS(fileInfo(item).absoluteFilePath());
        if (!fixedDirs.contains(fixedItem)) {
            fixedDirs << fixedItem;
        }
    }
    generateCleanCommands(t, fixedFiles, "$(DEL_FILE)", "", "", "");
    generateCleanCommands(t, fixedDirs, "$(DEL_DIR)", "", "", "");
    t << endl;

    t << "distclean: clean dodistclean" << endl;
    t << endl;
}

void SymbianMakefileGenerator::generateExecutionTargets(QTextStream& t, const QStringList& platforms)
{
    // create execution targets
    if (targetType == TypeExe) {
        if (platforms.contains("winscw")) {
            t << "run:" << endl;
            t << "\t-call " << epocRoot() << "epoc32/release/winscw/udeb/" << fixedTarget << ".exe " << "$(QT_RUN_OPTIONS)" << endl;
        }
        t << "runonphone: sis" << endl;
        t << "\trunonphone $(QT_RUN_ON_PHONE_OPTIONS) --sis " << fixedTarget << ".sis " << fixedTarget << ".exe " << "$(QT_RUN_OPTIONS)" << endl;
        t << endl;
    }
}
