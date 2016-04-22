#-------------------------------------------------
#
# Project MUR RoboIDE
#
#-------------------------------------------------

QT       += core gui network help widgets

TARGET = RoboIDE
TEMPLATE = app
RC_FILE = icon.rc
CONFIG += c++11

defineTest(copyToDestdir) {
    files = $$1
    for(FILE, files) {
        DDIR = $$OUT_PWD
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

HELP_COMMAND = $$[QT_INSTALL_BINS]\qcollectiongenerator
HELP_SOURCE = $$PWD\help\robohelp.qhcp
HELP_INDEX = $$OUT_PWD\robohelp.qhc
HELP_CONTENT = $$PWD\help\help.qch
BLUETOOTH_BIN_DIR = $$PWD\connections\bluetooth\build\exe.win32-2.7

win32 {
    INCLUDEPATH += "$$PWD\extlibs\qsci\include"
    DEPENDPATH += "$$PWD\extlibs\qsci\include"

    INCLUDEPATH += "$$PWD\extlibs\zmq\include"

    CONFIG(release, debug | release) : {
        message(Building for Windows release mode)
        LIBS += -L"$$PWD\extlibs\qsci\mingw32\lib\release"
        LIBS += -L"$$PWD\extlibs\zmq\mingw32\lib"
        LIBS += -lqscintilla2 -llibzmq
        EXTRA_DLL += \
            $$PWD\extlibs\qsci\mingw32\bin\release\qscintilla2.dll \
            $$PWD\extlibs\zmq\mingw32\bin\libzmq.dll
    }

    CONFIG(debug, debug | release) : {
        message(Building for Windows debug mode)
        LIBS += -L"$$PWD\extlibs\qsci\mingw32\lib\debug"
        LIBS += -L"$$PWD\extlibs\zmq\mingw32\lib"
        LIBS +=  -lqscintilla2 -llibzmq

        EXTRA_DLL += \
            $$PWD\extlibs\qsci\mingw32\bin\debug\qscintilla2.dll \
            $$PWD\extlibs\zmq\mingw32\bin\libzmq.dll
    }

    copyToDestdir($$EXTRA_DLL)

    AUTOCOMPLETE_API = $$PWD\autocomplete_api\cpp.api \
                       $$PWD\autocomplete_api\roboIDE.api

    copyToDestdir($$AUTOCOMPLETE_API)

    build_help.commands = $$quote(cmd /c $${HELP_COMMAND} /S /I /Y $${HELP_SOURCE} -o $${HELP_INDEX})
}

Linux {
    LIBS += -lqscintilla2 -lzmq

    build_help.commands = $$quote($${HELP_COMMAND} $${HELP_SOURCE} -o $${HELP_INDEX})
}
QMAKE_EXTRA_TARGETS += build_help
POST_TARGETDEPS += build_help

copyToDestdir($$HELP_CONTENT)

BLUETOOTH_SOURCE = $$PWD\connections\bluetooth\build\exe.win32-2.7
BLUETOOTH_SOURCE ~= s,/,\\,g
BLUETOOTH_DEST = $$OUT_PWD\bluetooth
BLUETOOTH_DEST ~= s,/,\\,g
QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$BLUETOOTH_SOURCE) $$quote($$BLUETOOTH_DEST)
#QMAKE_POST_LINK += $$QMAKE_COPY_DIR "\"{$$BLUETOOTH_SOURCE}\"" "\"{$$BLUETOOTH_DEST}\""

include(widgets/widgets.pri)
include(logic/logic.pri)
include(connections/connections.pri)

SOURCES += main.cpp

HEADERS  +=

RESOURCES += \
    icons.qrc \
    styles.qrc

DISTFILES +=
