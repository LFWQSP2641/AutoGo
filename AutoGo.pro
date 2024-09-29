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
    src

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
    src/KatagoAnalysisInteractor.h \
    src/KatagoGTPInteractor.h \
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
        src/KatagoAnalysisInteractor.cpp \
        src/KatagoGTPInteractor.cpp \
        src/KatagoInteractor.cpp \
        src/Settings.cpp \
        src/StoneData.cpp \
        src/Util.cpp \
        src/main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x64/vc16/lib/ -lopencv_world4100
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x64/vc16/lib/ -lopencv_world4100d

INCLUDEPATH += $$PWD/../../opencv/build/include
DEPENDPATH += $$PWD/../../opencv/build/include
