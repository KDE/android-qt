TEMPLATE = subdirs

SUBDIRS = subtest test warnings maxwarnings cmptest globaldata skipglobal skip \
          strcmp expectfail sleep fetchbogus crashes multiexec failinit failinitdata \
          skipinit skipinitdata datetime singleskip assert waitwithoutgui differentexec \
          exceptionthrow qexecstringlist datatable commandlinedata\
          benchlibwalltime benchlibcallgrind benchlibeventcounter benchlibtickcounter \
          benchliboptions xunit badxml longstring printdatatags \
          printdatatagswithglobaltags

INSTALLS =

QT = core

integrity: SUBDIRS -= test

CONFIG += parallel_test
