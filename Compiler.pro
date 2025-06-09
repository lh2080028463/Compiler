QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 禁用旧API（可选）
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# 源文件
SOURCES += \
    createquat.cpp \
    graana_result.cpp \
    main.cpp \
    mainwindow.cpp \
    objectcode.cpp \
    quat_result.cpp \
    switch.cpp \
    symboltable.cpp \
    symtable.cpp \
    synana.cpp \
    wordana.cpp \
    optimize.cpp

# 头文件
HEADERS += \
    createquat.h \
    graana_result.h \
    mainwindow.h \
    objectcode.h \
    quat_result.h \
    switch.h \
    symboltable.h \
    symtable.h \
    synana.h \
    wordana.h \
    optimize.h \
    global.h

# UI文件
FORMS += \
    graana_result.ui \
    mainwindow.ui \
    quat_result.ui \
    switch.ui \
    symtable.ui

# 资源文件（确保路径正确）
RESOURCES += resources.qrc

# 部署路径（可选）
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
