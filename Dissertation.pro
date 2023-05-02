QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ClientConnection.cpp \
    Server.cpp \
    clientpreviewwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    my_grid_layout.cpp

HEADERS += \
    AckHeader.h \
    ClientConnection.h \
    Header.h \
    Server.h \
    clientpreviewwidget.h \
    mainwindow.h \
    my_grid_layout.h

FORMS += \
    clientpreviewwidget.ui \
    mainwindow.ui

TRANSLATIONS += \
    Dissertation_en_GB.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../../../../usr/lib64/ -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/lib64

