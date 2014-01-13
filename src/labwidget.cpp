#include "labwidget.h"
#include "client.h"
#include "application.h"

#include <TUserInfo>
#include <TOperationResult>
#include <TLabInfo>
#include <TLabProject>
#include <TTagsWidget>
#include <TListWidget>
#include <TUserWidget>

#include <BInputField>
#include <BDialog>
#include <BInputField>
#include <BTranslation>

#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QLineEdit>
#include <QLabel>
#include <QList>
#include <QComboBox>
#include <QVariant>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>
#include <QRegExp>
#include <QFileInfo>
#include <QVariantMap>
#include <QFileDialog>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QSignalMapper>
#include <QDir>
#include <QToolButton>
#include <QPair>
#include <QDesktopServices>
#include <QUrl>

#include <QDebug>

/*============================================================================
================================ FilesWidget =================================
============================================================================*/

FilesWidget::Line::Line()
{
    hlt = new QHBoxLayout;
    lbl = new QLabel;
    hlt->addWidget(lbl);
    hlt->addStretch();
    tbtn = 0;
    deleted = false;
}

/*============================== Public constructors =======================*/

FilesWidget::FilesWidget(bool readOnly, QWidget *parent) :
    QWidget(parent), ReadOnly(readOnly)
{
    hasDeleted = false;
    lastDir = QDir::homePath();
    vlt = new QVBoxLayout(this);
    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(mapped(QString)));
    if (!ReadOnly)
    {
        QHBoxLayout *hlt = new QHBoxLayout;
          hlt->addStretch();
          tbtn = new QToolButton;
            tbtn->setIcon(Application::icon("edit_add"));
            tbtn->setToolTip(tr("Add file", "tbtn toolTip"));
            connect(tbtn, SIGNAL(clicked()), this, SLOT(addFile()));
          hlt->addWidget(tbtn);
        vlt->addLayout(hlt);
    }
}

/*============================== Public methods ============================*/

void FilesWidget::addFile(const QString &fn)
{
    if (fn.isEmpty() || lines.contains(fn))
        return;
    deleteLastDeleted();
    Line l;
    l.lbl->setText(labelText(fn));
    l.lbl->setToolTip(fn);
    connect(l.lbl, SIGNAL(linkActivated(QString)), this, SIGNAL(getFile(QString)));
    if (!ReadOnly)
    {
        l.tbtn = new QToolButton;
        l.tbtn->setIcon(Application::icon("editdelete"));
        l.tbtn->setToolTip(deleteToolTip);
        l.hlt->addWidget(l.tbtn);
        bSetMapping(mapper, l.tbtn, SIGNAL(clicked()), fn);
    }
    lines.insert(fn, l);
    vlt->insertLayout(vlt->count() - 1, l.hlt);
}

void FilesWidget::addFiles(const QStringList &list)
{
    foreach (const QString &fn, list)
        addFile(fn);
}

QStringList FilesWidget::files() const
{
    QStringList list;
    foreach (const QString &fn, lines.keys())
        if (!lines.value(fn).deleted)
            list << fn;
    return list;
}

bool FilesWidget::isReadOnly() const
{
    return ReadOnly;
}

/*============================== Static private constants ==================*/

const BTranslation FilesWidget::deleteToolTip = BTranslation::translate("FilesWidget", "Remove file", "tbtn toolTip");
const BTranslation FilesWidget::undeleteToolTip = BTranslation::translate("FilesWidget", "Cancel removal",
                                                                          "tbtn toolTip");

/*============================== Static private methods ====================*/

QString FilesWidget::labelText(const QString &fileName)
{
    if (fileName.isEmpty())
        return "";
    return "<a href=\"" + fileName + "\">" + QFileInfo(fileName).fileName() + "</a>";
}

/*============================== Private methods ===========================*/

void FilesWidget::deleteLastDeleted()
{
    if (!hasDeleted)
        return;
    foreach (const QString &ffn, lines.keys())
    {
        if (lines.value(ffn).deleted)
        {
            Line l = lines.take(ffn);
            delete l.lbl;
            delete l.tbtn;
            delete l.hlt;
            break;
        }
    }
    hasDeleted = false;
}

/*============================== Private slots =============================*/

void FilesWidget::mapped(const QString &fn)
{
    Line &l = lines[fn];
    if (l.deleted)
    {
        l.tbtn->setIcon(Application::icon("editdelete"));
        l.tbtn->setToolTip(deleteToolTip);
        l.lbl->setText(labelText(fn));
        hasDeleted = false;
    }
    else
    {
        deleteLastDeleted();
        l.tbtn->setIcon(Application::icon("reload"));
        l.tbtn->setToolTip(undeleteToolTip);
        l.lbl->setText(labelText(fn) + " [" + tr("deleted", "lbl text") + "]");
        hasDeleted = true;
    }
    l.deleted = !l.deleted;
}

void FilesWidget::addFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files", "fdlg caption"), lastDir);
    if (files.isEmpty())
        return;
    lastDir = QFileInfo(files.first()).path();
    addFiles(files);
}

/*============================================================================
================================ LabWidget ===================================
============================================================================*/

/*============================== Public constructors =======================*/

LabWidget::LabWidget(Mode m, QWidget *parent) :
    QWidget(parent), mmode(m)
{
    mvalid = false;
    mcheckSource = false;
    mid = 0;
    msenderId = 0;
    Qt::TextInteractionFlags tiflags = Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse
            | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard;
    QSignalMapper *mpr = new QSignalMapper(this);
    connect(mpr, SIGNAL(mapped(int)), this, SLOT(selectFile(int)));
    //
    QVBoxLayout *vlt = new QVBoxLayout(this);
      QFormLayout *flt = new QFormLayout;
        mledtTitle = new QLineEdit;
          mledtTitle->setReadOnly(ShowMode == mmode);
          mledtTitle->setMaxLength(120);
          connect(mledtTitle, SIGNAL(textChanged(QString)), this, SLOT(checkInputs()));
          minputTitle = new BInputField;
          minputTitle->addWidget(mledtTitle);
          minputTitle->setShowStyle(ShowMode == mmode ? BInputField::ShowNever : BInputField::ShowAlways);
        flt->addRow(tr("Title:", "lbl text"), minputTitle);
        mtgswgt = new TTagsWidget;
          mtgswgt->setReadOnly(ShowMode == mmode);
        flt->addRow(tr("Tags:", "lbl text"), mtgswgt);
        mcmboxType = new QComboBox;
          foreach (const TLabInfo::Type &t, TLabInfo::allTypes())
              mcmboxType->addItem(TLabInfo::typeToString(t, true), t);
          mcmboxType->setEnabled(ShowMode != mmode);
          mcmboxType->setCurrentIndex(mcmboxType->findData(TLabInfo::DesktopType));
          connect(mcmboxType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmboxTypeCurrentIndexChanged(int)));
        flt->addRow(tr("Type:", "lbl text"), mcmboxType);
      vlt->addLayout(flt);
      //TODO: files
      QHBoxLayout *hlt = new QHBoxLayout;
        flt = new QFormLayout;
          mlblSender = new QLabel;
            mlblSender->setTextInteractionFlags(tiflags);
            connect(mlblSender, SIGNAL(linkActivated(QString)), this, SLOT(showSenderInfo()));
          flt->addRow(tr("Sender:", "lbl text"), mlblSender);
          mlblCreationDT = new QLabel;
            mlblCreationDT->setTextInteractionFlags(tiflags);
          flt->addRow(tr("Created:", "lbl text"), mlblCreationDT);
          mlblUpdateDT = new QLabel;
            mlblUpdateDT->setTextInteractionFlags(tiflags);
          flt->addRow(tr("Updated:", "lbl text"), mlblUpdateDT);
        hlt->addLayout(flt);
        QGroupBox *gbox = new QGroupBox(tr("Authors", "gbox title"));
          QHBoxLayout *hltw = new QHBoxLayout(gbox);
            mlstwgtAuthors = new TListWidget;
              mlstwgtAuthors->setReadOnly(ShowMode == mmode);
              mlstwgtAuthors->setButtonsVisible(ShowMode != mmode);
            hltw->addWidget(mlstwgtAuthors);
        hlt->addWidget(gbox);
      vlt->addLayout(hlt);
      hlt = new QHBoxLayout;
        gbox = new QGroupBox(tr("Comment", "gbox title"));
          hltw = new QHBoxLayout(gbox);
            mptedtComment = new QPlainTextEdit;
              mptedtComment->setReadOnly(ShowMode == mmode);
            hltw->addWidget(mptedtComment);
        hlt->addWidget(gbox);
        gbox = new QGroupBox(tr("Groups", "gbox title"));
          hltw = new QHBoxLayout(gbox);
            mlstwgtGroups = new TListWidget;
              mlstwgtGroups->setReadOnly(ShowMode == mmode);
              mlstwgtGroups->setButtonsVisible(ShowMode != mmode);
              if (ShowMode != mmode)
              {
                  QStringList groups;
                  sClient->getClabGroupsList(groups, parent);
                  mlstwgtGroups->setAvailableItems(groups);
              }
            hltw->addWidget(mlstwgtGroups);
        hlt->addWidget(gbox);
      vlt->addLayout(hlt);
      gbox = new QGroupBox(tr("Source", "gbox title"));
        gbox->setVisible(ShowMode != mmode);
        flt = new QFormLayout(gbox);
        foreach (int t, bRangeD(BeQt::LinuxOS, BeQt::WindowsOS))
        {
            mhltFile.insert(t, new QHBoxLayout);
              mledtFile.insert(t, new QLineEdit);
                connect(mledtFile.value(t), SIGNAL(textChanged(QString)), this, SLOT(checkInputs()));
              mhltFile.value(t)->addWidget(mledtFile.value(t));
              if (BeQt::LinuxOS == t)
              {
                  mbtnSearch = new QPushButton(tr("Search..."));
                    bSetMapping(mpr, mbtnSearch, SIGNAL(clicked()), t);
                  mhltFile.value(t)->addWidget(mbtnSearch);
              }
              else
              {
                  QPushButton *btn = new QPushButton(tr("Search..."));
                    bSetMapping(mpr, btn, SIGNAL(clicked()), t);
                  mhltFile.value(t)->addWidget(btn);
              }
            flt->addRow(" ", mhltFile.value(t));
        }
      vlt->addWidget(gbox);
      gbox = new QGroupBox(tr("Attached files", "gbox title"));
        hltw = new QHBoxLayout(gbox);
          flswgt = new FilesWidget(ShowMode == mmode);
            connect(flswgt, SIGNAL(getFile(QString)), this, SLOT(getFile(QString)));
          hltw->addWidget(flswgt);
      vlt->addWidget(gbox);
    //
    cmboxTypeCurrentIndexChanged(0);
    Application::setRowVisible(mlblSender, AddMode != mmode);
    Application::setRowVisible(mlblCreationDT, AddMode != mmode);
    Application::setRowVisible(mlblUpdateDT, AddMode != mmode);
    checkInputs();
}

/*============================== Public methods ============================*/

void LabWidget::setInfo(const TLabInfo &info)
{
    mid = info.id();
    msenderId = info.sender().id();
    msenderLogin = info.sender().login();
    msenderRealName = info.sender().realName();
    mledtTitle->setText(info.title());
    if (!mledtTitle->hasAcceptableInput())
        mledtTitle->clear();
    mtgswgt->setTags(info.tags());
    mcmboxType->setCurrentIndex(mcmboxType->findData(info.type()));
    if (mcmboxType->currentIndex() < 0)
        mcmboxType->setCurrentIndex(0);
    if (!msenderLogin.isEmpty())
    {
        QString s = "<a href=x>" + msenderLogin + "</a>";
        s += !msenderRealName.isEmpty() ? (" (" + msenderRealName + ")") : QString();
        mlblSender->setText(s);
        mlblSender->setToolTip(tr("Click the link to see info about the sender", "lbl toolTip"));
    }
    else
    {
        mlblSender->clear();
        mlblSender->setToolTip("");
    }
    if (info.creationDateTime().isValid())
        mlblCreationDT->setText(info.creationDateTime(Qt::LocalTime).toString(DateTimeFormat));
    else
        mlblCreationDT->clear();
    if (info.updateDateTime().isValid())
        mlblUpdateDT->setText(info.updateDateTime(Qt::LocalTime).toString(DateTimeFormat));
    else
        mlblUpdateDT->clear();
    mlstwgtAuthors->setItems(info.authors());
    mlstwgtGroups->setItems(info.groups());
    mptedtComment->setPlainText(info.comment());
    flswgt->addFiles(info.extraAttachedFileNames());
    setFocus();
    checkInputs();
}

void LabWidget::setCheckSourceValidity(bool b)
{
    if (b == mcheckSource)
        return;
    mcheckSource = b;
    checkInputs();
}

void LabWidget::setClabGroups(const QStringList &list)
{
    mlstwgtGroups->setItems(list);
}

void LabWidget::restoreState(const QByteArray &state)
{
    QVariantMap m = BeQt::deserialize(state).toMap();
    mtgswgt->setAvailableTags(m.value("tags").toStringList());
    mlstwgtAuthors->setAvailableItems(m.value("authors").toStringList());
}

LabWidget::Mode LabWidget::mode() const
{
    return mmode;
}

TLabInfo LabWidget::info() const
{
    TLabInfo info;
    switch (mmode)
    {
    case AddMode:
        info.setContext(TLabInfo::AddContext);
        break;
    case EditMode:
        info.setContext(TLabInfo::EditContext);
        break;
    case ShowMode:
        info.setContext(TLabInfo::GeneralContext);
        break;
    default:
        break;
    }
    info.setId(mid);
    TUserInfo u(msenderId, TUserInfo::BriefInfoContext);
    u.setLogin(msenderLogin);
    u.setRealName(msenderRealName);
    info.setSender(u);
    info.setTitle(mledtTitle->text());
    info.setTags(mtgswgt->tags());
    info.setType(mcmboxType->itemData(mcmboxType->currentIndex()).toInt());
    //TODO: size
    info.setCreationDateTime(QDateTime::fromString(mlblCreationDT->text(), DateTimeFormat));
    info.setUpdateDateTime(QDateTime::fromString(mlblUpdateDT->text(), DateTimeFormat));
    info.setAuthors(mlstwgtAuthors->items());
    info.setGroups(mlstwgtGroups->items());
    info.setComment(mptedtComment->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
    info.setExtraAttachedFileNames(flswgt->files());
    return info;
}

bool LabWidget::checkSourceValidity() const
{
    return mcheckSource;
}

TLabProject LabWidget::webProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 1) ? mledtFile.value(BeQt::LinuxOS)->text() : QString());
    if (p.isExecutable())
        p.clear();
    return p;
}

TLabProject LabWidget::linuxProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 0) ? mledtFile.value(BeQt::LinuxOS)->text() : QString());
    if (!p.isExecutable())
        p.clear();
    return p;
}

TLabProject LabWidget::macProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 0) ? mledtFile.value(BeQt::MacOS)->text() : QString());
    if (!p.isExecutable())
        p.clear();
    return p;
}

TLabProject LabWidget::winProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 0) ? mledtFile.value(BeQt::WindowsOS)->text() : QString());
    if (!p.isExecutable())
        p.clear();
    return p;
}

QString LabWidget::url() const
{
    return (mcmboxType->currentIndex() == 2) ? mledtFile.value(BeQt::LinuxOS)->text() : QString();
}

QStringList LabWidget::extraAttachedFiles() const
{
    return flswgt->files();
}

QStringList LabWidget::clabGroups() const
{
    return mlstwgtGroups->items();
}

QByteArray LabWidget::saveState() const
{
    QVariantMap m;
    m.insert("tags", mtgswgt->availableTags());
    m.insert("authors", mlstwgtAuthors->availableItems());
    return BeQt::serialize(m);
}

bool LabWidget::isValid() const
{
    return mvalid;
}

/*============================== Public slots ==============================*/

void LabWidget::clear()
{
    setInfo(TLabInfo());
}

void LabWidget::setFocus()
{
    mledtTitle->setFocus();
    if (!mledtTitle->isReadOnly())
        mledtTitle->selectAll();
}

/*============================== Private slots =============================*/

void LabWidget::checkInputs()
{
    minputTitle->setValid(!mledtTitle->text().isEmpty() && mledtTitle->hasAcceptableInput());
    bool src = true;
    if (mcheckSource)
    {
        switch (mcmboxType->currentIndex())
        {
        case 0:
            src = linuxProject().isValid() || macProject().isValid() || winProject().isValid();
            break;
        case 1:
            src = webProject().isValid();
            break;
        case 2:
            src = !url().isEmpty();
            break;
        default:
            break;
        }
    }
    bool v = info().isValid() && (!mcheckSource || src);
    if (v == mvalid)
        return;
    mvalid = v;
    emit validityChanged(v);
}

void LabWidget::showSenderInfo()
{
    if (!msenderId)
        return;
    if (!sClient->isAuthorized())
        return;
    TUserInfo info;
    QStringList groups;
    if (!sClient->getUserInfo(msenderId, info, groups, this))
        return;
    BDialog dlg(this);
    dlg.setWindowTitle(tr("User:", "windowTitle") + " " + info.login());
    TUserWidget *uwgt = new TUserWidget(TUserWidget::ShowMode);
    uwgt->setInfo(info);
    uwgt->setClabGroups(groups);
    uwgt->setClabGroupsVisible(true);
    dlg.setWidget(uwgt);
    dlg.addButton(QDialogButtonBox::Close, SLOT(close()));
    dlg.setMinimumSize(600, dlg.sizeHint().height());
    dlg.exec();
}

void LabWidget::cmboxTypeCurrentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        Application::setRowVisible(mhltFile.value(BeQt::LinuxOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::MacOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::WindowsOS), true);
        Application::labelForField<QLabel>(mhltFile.value(BeQt::LinuxOS))->setText(tr("Main file (Linux):",
                                                                                      "lbl text"));
        Application::labelForField<QLabel>(mhltFile.value(BeQt::MacOS))->setText(tr("Main file (Mac OS):", "lbl text"));
        Application::labelForField<QLabel>(mhltFile.value(BeQt::WindowsOS))->setText(tr("Main file (Windows):",
                                                                                        "lbl text"));
        mbtnSearch->setVisible(true);
        break;
    case 1:
        Application::setRowVisible(mhltFile.value(BeQt::LinuxOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::MacOS), false);
        Application::setRowVisible(mhltFile.value(BeQt::WindowsOS), false);
        Application::labelForField<QLabel>(mhltFile.value(BeQt::LinuxOS))->setText(tr("Main file:", "lbl text"));
        mbtnSearch->setVisible(true);
        break;
    case 2:
        Application::setRowVisible(mhltFile.value(BeQt::LinuxOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::MacOS), false);
        Application::setRowVisible(mhltFile.value(BeQt::WindowsOS), false);
        Application::labelForField<QLabel>(mhltFile.value(BeQt::LinuxOS))->setText(tr("URL:", "lbl text"));
        mbtnSearch->setVisible(false);
        break;
    default:
        break;
    }
    checkInputs();
}

void LabWidget::selectFile(int id)
{
    QLineEdit *ledt = mledtFile.value(id);
    if (!ledt)
        return;
    QString dir = ledt->text();
    if (dir.isEmpty())
        dir = QDir::homePath();
    QString fn = QFileDialog::getOpenFileName(this, tr("Select file", "getOpenFileName"), dir);
    if (fn.isEmpty())
        return;
    ledt->setText(fn);
}

void LabWidget::getFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).isAbsolute())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    else
        sClient->getExtraAttachedFile(mid, fileName, this);
}

/*============================== Static private constants ==================*/

const QString LabWidget::DateTimeFormat = "dd MMMM yyyy hh:mm";
