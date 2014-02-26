#include "applicationserver.h"
#include "mainwindow.h"
#include "application.h"
#include "global.h"
#include "client.h"

#include <TeXSampleGlobal>
#include <TOperationResult>

#include <BVersion>
#include <BApplication>
#include <BDirTools>
#include <BTranslator>
#include <BLogger>
#include <BAboutDialog>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QFont>
#include <QPixmap>
#include <QHash>
#include <QSettings>

#include <QDebug>

static QString resource(const QString &subpath)
{
    return BDirTools::findResource(subpath, BDirTools::GlobalOnly);
}

#include <TUserInfo>

int main(int argc, char *argv[])
{
    tInit();
    QApplication app(argc, argv);
    QApplication::setApplicationName("CloudLab Client");
    QApplication::setApplicationVersion("0.3.1-beta");
    QApplication::setOrganizationName("TeXSample Team");
    QApplication::setOrganizationDomain("https://github.com/TeXSample-Team/CloudLab-Client");
    QFont fnt = QApplication::font();
    fnt.setPointSize(10);
    QApplication::setFont(fnt);
    QStringList args = app.arguments();
    args.removeFirst();
    args.removeDuplicates();
    QString home = QDir::home().dirName();
    ApplicationServer s(9960 + qHash(home) % 10, QCoreApplication::applicationName() + "0" + home);
    int ret = 0;
    if ( !s.testServer() )
    {
        s.listen();
#if defined(BUILTIN_RESOURCES)
        Q_INIT_RESOURCE(clab_client);
        Q_INIT_RESOURCE(clab_client_translations);
#endif
        Application bapp;
        Application::resetProxy();
        Q_UNUSED(bapp)
        Application::setThemedIconsEnabled(false);
        Application::setPreferredIconFormats(QStringList() << "png");
        QApplication::setWindowIcon(Application::icon("clab-client"));
        Application::installTranslator(new BTranslator("qt"));
        Application::installTranslator(new BTranslator("beqt"));
        Application::installTranslator(new BTranslator("texsample"));
        Application::installTranslator(new BTranslator("cloudlab-client"));
        BAboutDialog::setDefaultMinimumSize(800, 400);
        Application::setApplicationCopyrightPeriod("2013-2014");
        Application::setApplicationDescriptionFile(resource("description") + "/DESCRIPTION.txt");
        Application::setApplicationChangeLogFile(resource("changelog") + "/ChangeLog.txt");
        Application::setApplicationLicenseFile(resource("copying") + "/COPYING.txt");
        Application::setApplicationAuthorsFile(resource("infos/authors.beqt-info"));
        Application::setApplicationTranslationsFile(resource("infos/translators.beqt-info"));
        Application::setApplicationThanksToFile(resource("infos/thanks-to.beqt-info"));
        Application::aboutDialogInstance()->setupWithApplicationData();
        Application::createInitialWindow(args);
        Application::loadSettings();
        if (Global::checkForNewVersions())
            Application::checkForNewVersions();
        ret = app.exec();
        Application::saveSettings();
#if defined(BUILTIN_RESOURCES)
        Q_CLEANUP_RESOURCE(clab_client);
        Q_CLEANUP_RESOURCE(clab_client_translations);
#endif
    }
    else
    {
        if (args.isEmpty())
            args << "";
        s.sendMessage(args);
    }
    tCleanup();
    return ret;
}
