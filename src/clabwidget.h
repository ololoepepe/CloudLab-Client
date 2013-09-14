#ifndef CLABWIDGET_H
#define CLABWIDGET_H

class MainWindow;
class ConnectionAction;
class LabWidget;
class LabsProxyModel;

class BInputField;

class QToolBar;
class QGroupBox;
class QLineEdit;
class QTableView;
class QLabel;
class QAction;
class QPoint;
class QString;
class QModelIndex;
class QDialog;
class QByteArray;

#include "client.h"

#include <BDialog>

#include <QWidget>
#include <QList>
#include <QMap>
#include <QPointer>

/*============================================================================
================================ AddLabDialog ================================
============================================================================*/

class AddLabDialog : public BDialog
{
    Q_OBJECT
public:
    explicit AddLabDialog(QWidget *parent = 0);
public:
    LabWidget *labWidget() const;
protected:
    void closeEvent(QCloseEvent *e);
private:
    LabWidget *mlabwgt;
private:
    Q_DISABLE_COPY(AddLabDialog)
};

/*============================================================================
================================ EditLabDialog ===============================
============================================================================*/

class EditLabDialog : public BDialog
{
    Q_OBJECT
public:
    explicit EditLabDialog(quint64 id, QWidget *parent = 0);
public:
    LabWidget *labWidget() const;
protected:
    void closeEvent(QCloseEvent *e);
private:
    LabWidget *mlabwgt;
private:
    Q_DISABLE_COPY(EditLabDialog)
};

/*============================================================================
================================ SelectUserDialog ============================
============================================================================*/

class SelectUserDialog : public BDialog
{
    Q_OBJECT
public:
    explicit SelectUserDialog(QWidget *parent = 0);
public:
    quint64 userId() const;
    QString userLogin() const;
private:
    QLineEdit *mledt;
    BInputField *mfield;
private slots:
    void buttonClicked(int id);
    void checkValidity();
private:
    Q_DISABLE_COPY(SelectUserDialog)
};

/*============================================================================
================================ ClabWidget ==================================
============================================================================*/

class ClabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClabWidget(MainWindow *window);
    ~ClabWidget();
public:
    QList<QAction *> toolBarActions() const;
signals:
    void message(const QString &msg);
private:
    static void setTableHeaderState(const QByteArray &state);
    static QByteArray tableHeaderState();
private:
    void showAddingLabFailedMessage(const QString &errorString = QString());
    void showEditingLabFailedMessage(const QString &errorString = QString());
private slots:
    void retranslateUi();
    void actRegisterTriggered();
    void actRecoverTriggered();
    void addUser();
    void editUser();
    void actInvitesTriggered();
    void editGroups();
    void addLab();
    void editLab();
    void deleteLab();
    void updateLabsList();
    void showLabInfo();
    void getLab();
    void showSettings();
    void showAccountSettings();
    void clientStateChanged(Client::State state);
    void clientAccessLevelChanged(int lvl);
    void tblvwDoubleClicked(const QModelIndex &index);
    void tblvwCustomContextMenuRequested(const QPoint &pos);
    void infoDialogFinished();
    void addDialogFinished();
    void editDialogFinished();
private:
    MainWindow *const Window;
private:
    LabsProxyModel *mproxyModel;
    quint64 mlastId;
    QMap< quint64, QPointer<QDialog> > minfoDialogMap;
    QMap<QPointer<QObject>, quint64> minfoDialogIdMap;
    QMap< quint64, QPointer<QDialog> > meditDialogMap;
    QMap<QPointer<QObject>, quint64> meditDialogIdMap;
    QPointer<AddLabDialog> maddDialog;
    //
    QToolBar *mtbar;
      ConnectionAction *mactConnection;
        QAction *mactConnect;
        QAction *mactDisconnect;
      QAction *mactUpdate;
      QAction *mactSend;
      QAction *mactTools;
        QAction *mactRegister;
        QAction *mactRecover;
        QAction *mactSettings;
        QAction *mactAccountSettings;
        QAction *mactAdministration;
          QAction *mactAddUser;
          QAction *mactEditUser;
          QAction *mactInvites;
          QAction *mactGroups;
    QGroupBox *mgboxSelect;
      QLabel *mlblSearch;
      QLineEdit *mledtSearch;
    QTableView *mtblvw;
private:
    Q_DISABLE_COPY(ClabWidget)
};

#endif // CLABWIDGET_H
