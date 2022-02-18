#!/bin/bash
DYLD_FRAMEWORK_PATH=/Users/hidog/code/qt5-build/qtbase/lib${DYLD_FRAMEWORK_PATH:+:$DYLD_FRAMEWORK_PATH}
export DYLD_FRAMEWORK_PATH
QT_PLUGIN_PATH=/Users/hidog/code/qt5-build/qtbase/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec /Users/hidog/code/qt5-build/qtbase/bin/uic "$@"
