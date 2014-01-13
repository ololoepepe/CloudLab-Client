#ifndef LABWIDGET_H
#define LABWIDGET_H

class FilesWidget;

class TLabInfo;
class TLabProject;
class TTagsWidget;
class TListWidget;

class BInputField;

class QLineEdit;
class QComboBox;
class QSpinBox;
class QPlainTextEdit;
class QHBoxLayout;
class QPushButton;
class QButtonGroup;
class QVBoxLayout;
class QLabel;
class QToolButton;
class QSignalMapper;

#include <BTranslation>

#include <QDialog>
#include <QVariantMap>
#include <QMap>
#include <QString>

/*============================================================================
================================ FilesWidget =================================
============================================================================*/

class FilesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilesWidget(bool readOnly, QWidget *parent = 0);
public:
    void addFile(const QString &fn);
    void addFiles(const QStringList &list);
    QStringList files() const;
    bool isReadOnly() const;
signals:
    void getFile(const QString &fileName);
private:
    void deleteLastDeleted();
private slots:
    void mapped(const QString &fn);
    void addFile();
private:
    struct Line
    {
        QHBoxLayout *hlt;
        QLabel *lbl;
        QToolButton *tbtn;
        bool deleted;
    public:
        Line();
    };
private:
    static const BTranslation deleteToolTip;
    static const BTranslation undeleteToolTip;
private:
    static QString labelText(const QString &fileName);
private:
    const bool ReadOnly;
private:
    QSignalMapper *mapper;
    bool hasDeleted;
    QVBoxLayout *vlt;
    QMap<QString, Line> lines;
    QToolButton *tbtn;
    QString lastDir;
private:
    Q_DISABLE_COPY(FilesWidget)
};

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
    QStringList extraAttachedFiles() const;
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
    void getFile(const QString &fileName);
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
    FilesWidget *flswgt;
};

#endif // LABWIDGET_H
