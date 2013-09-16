class QWidget;

#include "mainwindow.h"
#include "clabwidget.h"
#include "application.h"
#include "global.h"

#include <BDirTools>

#include <QString>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QMenuBar>
#include <QSizePolicy>
#include <QDockWidget>
#include <QStatusBar>
#include <QLabel>
#include <QStringList>
#include <QLayout>
#include <QSettings>
#include <QByteArray>
#include <QRect>
#include <QFile>
#include <QEvent>
#include <QMainWindow>
#include <QWindowStateChangeEvent>
#include <QApplication>
#include <QFileInfo>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QPushButton>
#include <QSignalMapper>
#include <QToolBar>
#include <QIcon>
#include <QPoint>
#include <QScopedPointer>
#include <QProcess>
#include <QSize>
#include <QScopedPointer>
#include <QLocale>
#include <QLayout>
#include <QToolButton>
#include <QLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QTextStream>
#include <QTimer>
#include <QCloseEvent>
#include <QTextBlock>
#include <QRegExp>
#include <QDesktopWidget>
#include <QPointer>

#include <QDebug>

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

/*============================== Public constructors =======================*/

MainWindow::MainWindow() :
    QMainWindow(0)
{
    setAcceptDrops(true);
    setDockOptions(dockOptions() | QMainWindow::ForceTabbedDocks);
    setGeometry(200, 200, 400, 600); //The default
    restoreGeometry( getWindowGeometry() );
    //
    mmprAutotext = new QSignalMapper(this);
    mmprOpenFile = new QSignalMapper(this);
    connect( mmprOpenFile, SIGNAL( mapped(QString) ), bApp, SLOT( openLocalFile(QString) ) );
    //
    initCentralWidget();
    initMenus();
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
    restoreState( getWindowState() );
}

MainWindow::~MainWindow()
{
    //
}

/*============================== Static public methods =====================*/

QByteArray MainWindow::getWindowGeometry()
{
    return bSettings->value("MainWindow/geomery").toByteArray();
}

QByteArray MainWindow::getWindowState()
{
    return bSettings->value("MainWindow/state").toByteArray();
}

void MainWindow::setWindowGeometry(const QByteArray &geometry)
{
    bSettings->setValue("MainWindow/geomery", geometry);
}

void MainWindow::setWindowState(const QByteArray &state)
{
    bSettings->setValue("MainWindow/state", state);
}

/*============================== Purotected methods ========================*/

void MainWindow::closeEvent(QCloseEvent *e)
{
    setWindowGeometry( saveGeometry() );
    setWindowState( saveState() );
    return QMainWindow::closeEvent(e);
}

/*============================== Private methods ===========================*/

void MainWindow::initCentralWidget()
{
    //Samples
    mwgt = new ClabWidget(this);
    connect(mwgt, SIGNAL(message(QString)), statusBar(), SLOT(showMessage(QString)));
    setCentralWidget(mwgt);
}

void MainWindow::initMenus()
{
    //File
    mmnuFile = menuBar()->addMenu("");
    mactQuit = mmnuFile->addAction("");
    mactQuit->setMenuRole(QAction::QuitRole);
    mactQuit->setIcon(BApplication::icon("exit"));
    mactQuit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(mactQuit, SIGNAL(triggered()), this, SLOT(close()));
    //Edit
    mmnuEdit = menuBar()->addMenu("");
    QAction *act = BApplication::createStandardAction(BApplication::SettingsAction);
    act->setShortcut(QKeySequence("Ctrl+P"));
    mmnuEdit->addAction(act);
    //Clab
    mmnuClab = menuBar()->addMenu("");
    mmnuClab->addActions(mwgt->toolBarActions());
    //Help
    mmnuHelp = menuBar()->addMenu("");
    mmnuHelp->addAction( BApplication::createStandardAction(BApplication::HomepageAction) );
    mmnuHelp->addSeparator();
    act = BApplication::createStandardAction(BApplication::HelpContentsAction);
    act->setShortcut(QKeySequence("F1"));
    mmnuHelp->addAction(act);
    mmnuHelp->addAction(BApplication::createStandardAction(BApplication::WhatsThisAction));
    mmnuHelp->addSeparator();
    mmnuHelp->addAction(BApplication::createStandardAction(BApplication::AboutAction));
}

/*============================== Private slots =============================*/

void MainWindow::retranslateUi()
{
    setWindowTitle("CloudLab Client");
    //menus
    mmnuFile->setTitle(tr("File", "mnu title"));
    mactQuit->setText(tr("Quit", "act text"));
    mmnuEdit->setTitle(tr("Edit", "mnu title"));
    mmnuClab->setTitle(tr("CloudLab", "mnuTitle"));
    mmnuHelp->setTitle(tr("Help", "mnuTitle"));
}
