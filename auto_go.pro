QT += quick gui qml

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += qmltypes
QML_IMPORT_NAME = com.LFWQSP2641.qmlcomponents
QML_IMPORT_MAJOR_VERSION = 1

INCLUDEPATH += \
    src \
    D:/source/opencv/build/include/opencv2 \
    D:/source/opencv/build/include

LIBS += D:/source/opencv/build/x64/vc16/lib/opencv_world4100.lib

RESOURCES += \
    resources/resources.qrc

HEADERS += \
    src/BoardAnalyzer.h \
    src/BoardData.h \
    src/BoardInteractor.h \
    src/GameBoardHandler.h \
    src/Global.h \
    src/GoBoardItem.h \
    src/InteractiveInterface/Controller.h \
    src/InteractiveInterface/MaaController.h \
    src/InteractiveInterface/MumuScreencaptor.h \
    src/InteractiveInterface/Screencaptor.h \
    src/KatagoInteractor.h \
    src/Settings.h \
    src/StoneData.h \
    src/Util.h

SOURCES += \
        src/BoardAnalyzer.cpp \
        src/BoardData.cpp \
        src/BoardInteractor.cpp \
        src/GameBoardHandler.cpp \
        src/Global.cpp \
        src/GoBoardItem.cpp \
        src/InteractiveInterface/Controller.cpp \
        src/InteractiveInterface/MaaController.cpp \
        src/InteractiveInterface/MumuScreencaptor.cpp \
        src/InteractiveInterface/Screencaptor.cpp \
        src/KatagoInteractor.cpp \
        src/Settings.cpp \
        src/StoneData.cpp \
        src/Util.cpp \
        src/main.cpp
