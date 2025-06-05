QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 禁用旧API（可选）
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# 源文件
SOURCES += \
    createquat.cpp \
    main.cpp \
    mainwindow.cpp \
    symboltable.cpp \
    synana.cpp \
    wordana.cpp \
    optimize.cpp

# 头文件
HEADERS += \
    createquat.h \
    mainwindow.h \
    symboltable.h \
    synana.h \
    wordana.h \
    optimize.h \
    global.h

# UI文件
FORMS += \
    mainwindow.ui \
    tables.ui

# 资源文件（确保路径正确）
RESOURCES += resources.qrc

# 部署路径（可选）
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
