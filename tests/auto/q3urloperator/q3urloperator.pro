load(qttest_p4)
SOURCES  += tst_q3urloperator.cpp


QT += qt3support
requires(contains(QT_CONFIG,qt3support))
QT += network


CONFIG+=insignificant_test # QTQAINFRA-428
