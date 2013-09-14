#ifndef LABWIDGET_H
#define LABWIDGET_H

class TLabInfo;
class TLabProject;
class TTagsWidget;
class TListWidget;

class BInputField;

class QLineEdit;
class QLabel;
class QComboBox;
class QSpinBox;
class QString;
class QPlainTextEdit;
class QHBoxLayout;
class QPushButton;
class QButtonGroup;

#include <QDialog>
#include <QVariantMap>
#include <QMap>

/*============================================================================
================================ LabWidget ===================================
============================================================================*/

class LabWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode
    {
        AddMode,
        EditMode,
        ShowMode
    };
public:
    explicit LabWidget(Mode m, QWidget *parent = 0);
public:
    void setInfo(const TLabInfo &info);
    void setCheckSourceValidity(bool b);
    void setClabGroups(const QStringList &list);
    void restoreState(const QByteArray &state);
    Mode mode() const;
    TLabInfo info() const;
    bool checkSourceValidity() const;
    TLabProject webProject() const;
    TLabProject linuxProject() const;
    TLabProject macProject() const;
    TLabProject winProject() const;
    QString url() const;
    QStringList clabGroups() const;
    QByteArray saveState() const;
    bool isValid() const;
public slots:
    void clear();
    void setFocus();
private slots:
    void checkInputs();
    void showSenderInfo();
    void cmboxTypeCurrentIndexChanged(int index);
    void selectFile(int id);
signals:
    void validityChanged(bool valid);
private:
    static const QString DateTimeFormat;
private:
    const Mode mmode;
private:
    bool mvalid;
    bool mcheckSource;
    quint64 mid;
    quint64 msenderId;
    QString msenderLogin;
    QString msenderRealName;
    QLineEdit *mledtTitle;
    BInputField *minputTitle;
    TTagsWidget *mtgswgt;
    QComboBox *mcmboxType;
    QLabel *mlblSender;
    QLabel *mlblCreationDT;
    QLabel *mlblUpdateDT;
    TListWidget *mlstwgtAuthors;
    QPlainTextEdit *mptedtComment;
    TListWidget *mlstwgtGroups;
    QMap<int, QHBoxLayout *> mhltFile;
    QMap<int, QLineEdit *> mledtFile;
    QPushButton *mbtnSearch;
};

#endif // LABWIDGET_H
