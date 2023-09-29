QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/authorization.cpp \
    src/changepassworddialog.cpp \
    src/clientdialog.cpp \
    src/clientselectdialog.cpp \
    src/connectiondialog.cpp \
    src/filtertable.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/newpapertypedialog.cpp \
    src/newuserdialog.cpp \
    src/paperdialog.cpp \
    src/paperstablewidget.cpp \
    src/pasportbaseviewwidget.cpp \
    src/pasportdistributionwidget.cpp \
    src/postpresstablemodel.cpp \
    src/printreport.cpp \
    src/tlib.cpp \
    src/tuser.cpp \
    src/typography.cpp \
    src/zofsetdialog.cpp

HEADERS += \
    include/authorization.h \
    include/changepassworddialog.h \
    include/clientdialog.h \
    include/clientselectdialog.h \
    include/connectiondialog.h \
    include/filtertable.h \
    include/mainwindow.h \
    include/newpapertypedialog.h \
    include/newuserdialog.h \
    include/paperdialog.h \
    include/paperstablewidget.h \
    include/pasportbaseviewwidget.h \
    include/pasportdistributionwidget.h \
    include/postpresstablemodel.h \
    include/printreport.h \
    include/tlib.h \
    include/tuser.h \
    include/typography.h \
    include/zofsetdialog.h

FORMS += \
    src/authorization.ui \
    src/changepassworddialog.ui \
    src/clientdialog.ui \
    src/clientselectdialog.ui \
    src/connectiondialog.ui \
    src/filtertable.ui \
    src/mainwindow.ui \
    src/newpapertypedialog.ui \
    src/newuserdialog.ui \
    src/paperdialog.ui \
    src/paperstablewidget.ui \
    src/pasportbaseviewwidget.ui \
    src/pasportdistributionwidget.ui \
    src/zofsetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Icon.png

RESOURCES += \
    images/resources.qrc
