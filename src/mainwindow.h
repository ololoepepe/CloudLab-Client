#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class ClabWidget;

class QString;
class QAction;
class QMenu;
class QSignalMapper;
class QCloseEvent;
class QToolBar;
class QByteArray;
class QLabel;

#include <BApplication>

#include <QMainWindow>
#include <QTextCodec>

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow();
    ~MainWindow();
public:
    static QByteArray getWindowGeometry();
    static QByteArray getWindowState();
    static void setWindowGeometry(const QByteArray &geometry);
    static void setWindowState(const QByteArray &state);
protected:
    void closeEvent(QCloseEvent *e);
private:
    void initCentralWidget();
    void initMenus();
private slots:
    void retranslateUi();
private:
    QSignalMapper *mmprAutotext;
    QSignalMapper *mmprOpenFile;
    //
    ClabWidget *mwgt;
    //
    QMenu *mmnuFile;
      QAction *mactQuit;
    QMenu *mmnuEdit;
      QMenu *mmnuAutotext;
    QMenu *mmnuClab;
    QMenu *mmnuHelp;
private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
