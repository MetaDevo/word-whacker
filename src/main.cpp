#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QScreen>
#include <QTranslator>
#include <QWindow>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationVersion("0.1.0");
    QCoreApplication::setApplicationName("WordWhacker");
    QCoreApplication::setOrganizationName("MetaDevo");
    QCoreApplication::setOrganizationDomain("metadevo.com");

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "wordwhacker_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;

    w.show();
    return a.exec();
}
