#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>




int main(int argc, char *argv[])
{
#ifdef _CRT_SECURE_NO_WARNINGS
    printf("define _CRT_SECURE_NO_WARNINGS\n");
#endif

#ifdef DEBUG
    printf("defined debug.\n");
#endif

#ifdef NDEBUG
    printf("define NDEBUG\n");
#endif
    
#ifdef MACOS
    printf("defined MACOS\n");
#endif

    //assert(0);
    printf("test\n");

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "qt_test_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
