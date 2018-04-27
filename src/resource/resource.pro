TARGET = resources
TEMPLATE = lib

CONFIG(debug, debug|release){
    BIN_ROOT = $$PWD/../../bin/debug
    BUILD_ROOT = $$PWD/../../tmp/resource/debug
}else{
    BIN_ROOT = $$PWD/../../bin/release
    BUILD_ROOT = $$PWD/../../tmp/resource/release
}

DESTDIR = $$BIN_ROOT
OBJECTS_DIR = $$BUILD_ROOT/.obj
MOC_DIR = $$BUILD_ROOT/.moc
RCC_DIR = $$BUILD_ROOT/.rcc
UI_DIR =  $$BUILD_ROOT/.ui

RESOURCES += \
    resource.qrc

OTHER_FILES +=
