#-----------------------------------------------------------------------------------------------#
# OpenRGB SteelSeries GameSense Plugin                                                          #
#   Layout follows gitlab.com/OpenRGBDevelopers/OpenRGBSamplePlugin                             #
#-----------------------------------------------------------------------------------------------#
QT       += core gui widgets
TEMPLATE  = lib
CONFIG   += plugin silent c++17
TARGET    = OpenRGBGameSensePlugin

HEADERS +=                                                                                      \
    GameSensePlugin.h                                                                           \
    SteelSeriesGameSense.h                                                                      \
    OpenRGB/OpenRGBPluginInterface.h                                                            \

SOURCES +=                                                                                      \
    GameSensePlugin.cpp                                                                         \
    SteelSeriesGameSense.cpp                                                                    \

DISTFILES += GameSensePlugin.json

INCLUDEPATH +=                                                                                  \
    OpenRGB/                                                                                    \
    OpenRGB/RGBController                                                                       \
    OpenRGB/dependencies/json                                                                   \
    OpenRGB/dependencies/httplib                                                                \

win32 {
    CONFIG(debug, debug|release):   DESTDIR = debug
    CONFIG(release, debug|release): DESTDIR = release

    OBJECTS_DIR = _intermediate_$$DESTDIR/.obj
    MOC_DIR     = _intermediate_$$DESTDIR/.moc

    LIBS    += -lws2_32
    DEFINES += WIN32 _WIN32_WINNT=0x0A00 _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX
}

unix:!macx {
    target.path = $$PREFIX/lib/openrgb/plugins/
    INSTALLS   += target
}
