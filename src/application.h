#ifndef APPLICATION_H
#define APPLICATION_H

class Client;
class MainWindow;

class BAbstractSettingsTab;

class QWidget;

#include <BApplication>
#include <BSettingsDialog>

#include <QObject>
#include <QMap>
#include <QFont>
#include <QTextCodec>
#include <QStringList>
#include <QByteArray>
#include <QLocale>
#include <QList>

#if defined(bApp)
#undef bApp
#endif
#define bApp (static_cast<Application *>(BApplication::instance()))

/*============================================================================
================================ Application =================================
============================================================================*/

class Application : public BApplication
{
    Q_OBJECT
public:
    explicit Application();
    ~Application();
public:
    static void createInitialWindow(const QStringList &args);
    static QWidget *mostSuitableWindow();
    static bool mergeWindows();
    static void handleExternalRequest(const QStringList &args);
    static bool showLoginDialog(QWidget *parent = 0);
    static bool showRegisterDialog(QWidget *parent = 0);
    static bool showSettings(QWidget *parent = 0);
    static void checkForNewVersions(bool persistent = false);
    static void resetProxy();
public slots:
    void checkForNewVersionsSlot();
protected:
    QList<BAbstractSettingsTab *> createSettingsTabs() const;
private:
    static bool testAppInit();
private:
    void addMainWindow(const QStringList &fileNames = QStringList());
private slots:
    void mainWindowDestroyed(QObject *obj);
    void checkingForNewVersionsFinished();
private:
    bool minitialWindowCreated;
    QMap<QObject *, MainWindow *> mmainWindows;
    QList<QObject *> futureWatchers;
private:
    Q_DISABLE_COPY(Application)
};

#endif // APPLICATION_H
