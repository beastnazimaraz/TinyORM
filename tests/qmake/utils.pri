# TinyUtils library headers include path
# ---

INCLUDEPATH += $$quote($$TINYORM_SOURCE_TREE/tests/utils/src/)

# Link against tests's TinyUtils library
# ---

LIBS += $$quote(-L$$TINYORM_BUILD_TREE/tests/utils$$TINY_RELEASE_TYPE/) -lTinyUtils
