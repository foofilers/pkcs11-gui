#-------------------------------------------------
#
# Project created by QtCreator 2017-10-06T11:25:16
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pkcs11-gui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH = inc

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/p11core.cpp \
    src/smartcard.cpp \
    src/smartcardreader.cpp \
    src/key.cpp \
    src/certificate.cpp \
    src/askpindialog.cpp \
    src/importp12dialog.cpp \
    src/p11errors.cpp \
    src/util.cpp \
    src/changepindialog.cpp \
    src/keypairgeneratordialog.cpp \
    src/settingsdialog.cpp \
    src/licensedialog.cpp

HEADERS += \
    inc/mainwindow.h \
    inc/p11core.h \
    inc/smartcard.h \
    inc/smartcardreader.h \
    inc/key.h \
    inc/certificate.h \
    inc/askpindialog.h \
    inc/neither/try.hpp \
    inc/neither/traits.hpp \
    inc/neither/neither.hpp \
    inc/neither/maybe.hpp \
    inc/neither/lift.hpp \
    inc/neither/either.hpp \
    inc/importp12dialog.h \
    inc/p11errors.h \
    inc/util.h \
    inc/changepindialog.h \
    inc/keypairgeneratordialog.h \
    inc/settingsdialog.h \
    inc/licensedialog.h

FORMS += \
        ui/mainwindow.ui \
    ui/askpindialog.ui \
    ui/importp12dialog.ui \
    ui/changepindialog.ui \
    ui/keypairgeneratordialog.ui \
    ui/settingsdialog.ui \
    ui/licensedialog.ui



unix|win32: LIBS +=  -lcrypto -ldl

RESOURCES += \
    assets/icons.qrc


unix:!macx: LIBS += -L$$PWD/../libp11/lib/ -lp11

INCLUDEPATH += $$PWD/../libp11/include
DEPENDPATH += $$PWD/../libp11/include
