# Unsupported build types
# ---
win32-clang-g++: {
    CONFIG(static, dll|shared|static|staticlib) | \
    CONFIG(staticlib, dll|shared|static|staticlib): \
        error( "MinGW clang static build is not supported, contains a problem with\
                duplicit symbols, you can try to fix it :)." )
}

# Common Configuration ( also for tests )
# ---

CONFIG *= c++2a strict_c++ warn_on utf8_source hide_symbols silent
CONFIG -= c++11 app_bundle

# Qt defines
# ---

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# Disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

#DEFINES += QT_ASCII_CAST_WARNINGS
#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_RESTRICTED_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_USE_QSTRINGBUILDER
DEFINES += QT_STRICT_ITERATORS

# TinyORM defines
# ---

# Enable MySQL ping on Orm::MySqlConnection
mysql_ping: DEFINES += TINYORM_MYSQL_PING

# Platform specific configuration
# ---
win32: include(winconf.pri)
macx: include(macxconf.pri)
mingw|if(unix:!macx): include(unixconf.pri)

# Common Variables
# ---

# Folder by release type
debug_and_release: {
    CONFIG(release, debug|release): \
        TINY_RELEASE_TYPE = $$quote(/release)
    else:CONFIG(debug, debug|release): \
        TINY_RELEASE_TYPE = $$quote(/debug)
}
else: TINY_RELEASE_TYPE =

# Other
# ---

# Use extern constants for shared build
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    # Support override because inline_constants can be used in the shared build too
    !inline_constants: \
        CONFIG += extern_constants

# Archive library build
else: \
    CONFIG += inline_constants
