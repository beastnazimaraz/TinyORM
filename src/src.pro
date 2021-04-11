QT *= core sql
QT -= gui

TEMPLATE = lib
TARGET = TinyOrm

# Common Configuration
# ---

include(../qmake/common.pri)

# TinyORM library specific configuration
# ---

CONFIG *= create_prl
#CONFIG *= create_libtool create_pc

# Some info output
# ---

CONFIG(debug, debug|release): message( "Project is built in DEBUG mode." )
CONFIG(release, debug|release): message( "Project is built in RELEASE mode." )

# Disable debug output in release mode
CONFIG(release, debug|release) {
    message( "Disabling debug output." )
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# TinyORM defines
# ---

DEFINES += PROJECT_TINYORM
# Log queries with time measurement
DEFINES += TINYORM_DEBUG_SQL

# Build as shared library
DEFINES += TINYORM_BUILDING_SHARED

# Enable code needed by tests, eg connection overriding in the BaseModel
build_tests {
    DEFINES += TINYORM_TESTS_CODE
}

# File version and windows manifest
# ---

win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32-msvc* {
    QMAKE_TARGET_PRODUCT = TinyORM
    QMAKE_TARGET_DESCRIPTION = TinyORM user-friendly ORM
    QMAKE_TARGET_COMPANY = Crystal Studio
    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2020 Crystal Studio
#    RC_ICONS = images/TinyOrm.ico
    RC_LANG = 1033
}

# User Configuration
# ---

exists(../conf.pri) {
    include(../conf.pri)
}
else {
    error( "'conf.pri' for 'src' project does not exist. See an example configuration in 'conf.pri.example'." )
}

# Use Precompiled headers (PCH)
# ---

# TODO add possibility to control PCH by qmake CONFIG variable silverqx
include(../include/pch.pri)

# TinyORM library header and source files
# ---

include(src.pri)

# Default rules for deployment
# ---

release {
    win32-msvc*: target.path = C:/optx64/$${TARGET}
    !isEmpty(target.path): INSTALLS += target
}
