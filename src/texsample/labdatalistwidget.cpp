/****************************************************************************
**
** Copyright (C) 2013-2014 Andrey Bogdanov
**
** This file is part of CloudLab Client.
**
** CloudLab Client is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** CloudLab Client is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with CloudLab Client.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "labdatalistwidget.h"

#include "application.h"

#include <TLabApplication>
#include <TLabType>
#include <TLabData>
#include <TLabDataList>

#include <BDialog>
#include <BDirTools>
#include <BeQt>
#include <BGuiTools>
#include <BImageWidget>

#include <QByteArray>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSignalMapper>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QValidator>
#include <QVariant>
#include <QVariantMap>
#include <QWidget>

/*============================================================================
================================ LabDataListWidget ===========================
============================================================================*/

/*============================== Public constructors =======================*/

LabDataListWidget::LabDataListWidget(QWidget *parent) :
    QWidget(parent)
{
    mtype = 0;
    mvalid = false;
    mlastDir = QDir::homePath();
    QSignalMapper *mapperDir = new QSignalMapper(this);
    connect(mapperDir, SIGNAL(mapped(int)), this, SLOT(selectDir(int)));
    QSignalMapper *mapperFiles = new QSignalMapper(this);
    connect(mapperFiles, SIGNAL(mapped(int)), this, SLOT(selectFiles(int)));
    //
    QFormLayout *flt = new QFormLayout(this);
    mcmboxType = new QComboBox;
      static const QList<TLabType> LabTypes = QList<TLabType>() << TLabType::DesktopApplication
                                                                << TLabType::WebApplication << TLabType::Url;
      foreach (const TLabType &t, LabTypes)
          mcmboxType->addItem(t.toString(), int(t));
      mcmboxType->setCurrentIndex(0);
      connect(mcmboxType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmboxTypeCurrentIndexChanged(int)));
    flt->addRow(tr("Type:", "lbl text"), mcmboxType);
    QStringList labels;
    labels << tr("Windows application:", "lbl text");
    labels << tr("Linux application:", "lbl text");
    labels << tr("MacOS application:", "lbl text");
    labels << tr("Web application:", "lbl text");
    QStringList iconNames;
    iconNames << "windows_logo";
    iconNames << "linux_logo";
    iconNames << "macos_logo";
    iconNames << "network";
    foreach (int i, bRangeD(1, 4)) {
        QLineEdit *ledt = new QLineEdit;
        ledt->setReadOnly(true);
        QWidget *wgtLabel = new QWidget;
        QHBoxLayout *hlt = new QHBoxLayout(wgtLabel);
        hlt->setContentsMargins(0, 0, 0, 0);
        BImageWidget *imgwgt = new BImageWidget(Application::icon(iconNames.at(i - 1)));
        imgwgt->setScaledContents(true);
        imgwgt->setFixedSize(ledt->sizeHint().height(), ledt->sizeHint().height());
        hlt->addWidget(imgwgt);
        hlt->addWidget(new QLabel(labels.at(i - 1)));
        hlt = new QHBoxLayout;
        hlt->addWidget(ledt);
        QPushButton *btn = new QPushButton(tr("Select directory...", "btn text"));
        bSetMapping(mapperDir, btn, SIGNAL(clicked()), i);
        hlt->addWidget(btn);
        btn = new QPushButton(tr("Select files...", "btn text"));
        bSetMapping(mapperFiles, btn, SIGNAL(clicked()), i);
        hlt->addWidget(btn);
        flt->addRow(wgtLabel, hlt);
        mhltFields.insert(i, hlt);
        mledtFields.insert(i, ledt);
    }
    QLineEdit *ledt = new QLineEdit;
    QRegExp rx(".+\\..+");
    ledt->setValidator(new QRegExpValidator(rx, this));
    connect(ledt, SIGNAL(textChanged(QString)), this, SLOT(ledtUrlTextChanged(QString)));
    QWidget *wgtLabel = new QWidget;
    QHBoxLayout *hlt = new QHBoxLayout(wgtLabel);
    hlt->setContentsMargins(0, 0, 0, 0);
    BImageWidget *imgwgt = new BImageWidget(Application::icon("link"));
    imgwgt->setScaledContents(true);
    imgwgt->setFixedSize(ledt->sizeHint().height(), ledt->sizeHint().height());
    hlt->addWidget(imgwgt);
    hlt->addWidget(new QLabel(tr("External link:", "lbl text")));
    hlt = new QHBoxLayout;
    hlt->addWidget(ledt);
    flt->addRow(wgtLabel, hlt);
    mhltFields.insert(5, hlt);
    //
    cmboxTypeCurrentIndexChanged(0);
}

/*============================== Public methods ============================*/

const TLabDataList &LabDataListWidget::dataList() const
{
    return mdataList;
}

bool LabDataListWidget::hasValidInput() const
{
    return mvalid;
}

void LabDataListWidget::restoreState(const QByteArray &state)
{
    QVariantMap m = BeQt::deserialize(state).toMap();
    int index = mcmboxType->findData(m.value("type", 0).toInt());
    if (index >= 0)
        mcmboxType->setCurrentIndex(index);
    QString dir = m.value("last_dir").toString();
    if (!dir.isEmpty())
        mlastDir = dir;
}

QByteArray LabDataListWidget::saveState() const
{
    QVariantMap m;
    m.insert("type", mtype);
    m.insert("last_dir", mlastDir);
    return BeQt::serialize(m);
}

/*============================== Private methods ===========================*/

QString LabDataListWidget::selectMainFile(const QStringList &fileNames, bool *ok)
{
    BDialog dlg(this);
    dlg.setWindowTitle(tr("Select main file", "dlg windowTitle"));
    QListWidget *lstwgt = new QListWidget;
    lstwgt->addItems(fileNames);
    lstwgt->setCurrentRow(0, QItemSelectionModel::Select);
    dlg.setWidget(lstwgt);
    dlg.resize(600, 600);
    dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
    if (dlg.exec() != BDialog::Accepted)
        return bRet(ok, false, QString());
    QListWidgetItem *item = lstwgt->currentItem();
    if (!item || item->text().isEmpty())
        return bRet(ok, false, QString());
    return bRet(ok, true, item->text());
}

bool LabDataListWidget::setLabApplication(TLabApplication *app, int type)
{
    typedef QMap<int, BeQt::OSType> TypeMap;
    init_once(TypeMap, typeMap, TypeMap()) {
        typeMap.insert(WindowsField, BeQt::WindowsOS);
        typeMap.insert(LinuxField, BeQt::LinuxOS);
        typeMap.insert(MacosField, BeQt::MacOS);
    }
    if (!app)
        return false;
    if (TLabType::DesktopApplication == mtype) {
        foreach (int i, bRangeD(0, mdataList.size() - 1)) {
            if (mdataList.at(i).os() == typeMap.value(type)) {
                mdataList.removeAt(i);
                break;
            }
        }
        app->setOsType(typeMap.value(type));
        TLabData ld;
        ld.setApplication(*app);
        if (!ld.isValid())
            return false;
        mdataList << ld;
    } else {
        mdataList.clear();
        TLabData ld;
        ld.setApplication(*app);
        if (!ld.isValid())
            return false;
        mdataList << ld;
    }
    checkInputs();
    return true;
}

/*============================== Private slots =============================*/

void LabDataListWidget::checkInputs()
{
    bool v = !mdataList.isEmpty();
    if (v == mvalid)
        return;
    mvalid = v;
    emit inputValidityChanged(mvalid);
}

void LabDataListWidget::cmboxTypeCurrentIndexChanged(int index)
{
    int type = (index >= 0) ? mcmboxType->itemData(index).toInt() : 0;
    if (type == mtype)
        return;
    mtype = type;
    bool desktop = (TLabType::DesktopApplication == type);
    bool web = (TLabType::WebApplication == type);
    bool url = (TLabType::Url == type);
    BGuiTools::setRowVisible(mhltFields.value(WindowsField), desktop);
    BGuiTools::setRowVisible(mhltFields.value(LinuxField), desktop);
    BGuiTools::setRowVisible(mhltFields.value(MacosField), desktop);
    BGuiTools::setRowVisible(mhltFields.value(WebField), web);
    BGuiTools::setRowVisible(mhltFields.value(UrlField), url);
    mdataList.clear();
    foreach (QLineEdit *ledt, mledtFields.values())
        ledt->clear();
    checkInputs();
}

void LabDataListWidget::ledtUrlTextChanged(const QString &text)
{
    if (TLabType::Url != mtype)
        return;
    mdataList.clear();
    TLabData ld;
    ld.setUrl(text);
    if (ld.isValid())
        mdataList << ld;
    checkInputs();
}

void LabDataListWidget::selectDir(int type)
{
    QLineEdit *ledt = mledtFields.value(type);
    if (!ledt)
        return;
    QString dir = ledt->text();
    if (dir.isEmpty())
        dir = mlastDir;
    dir = QFileDialog::getExistingDirectory(this, tr("Select directory", "fdlg caption"), dir);
    if (dir.isEmpty())
        return;
    mlastDir = dir;
    QStringList fileNames = BDirTools::entryListRecursive(dir, QDir::Files);
    if (fileNames.isEmpty())
        return;
    foreach (int i, bRangeD(0, fileNames.size() - 1))
        fileNames[i].remove(0, dir.length());
    bool b = false;
    QString mainFileName = selectMainFile(fileNames, &b);
    if (!b)
        return;
    TLabApplication app;
    if (!app.load(dir, mainFileName, fileNames))
        return;
    if (setLabApplication(&app, type)) {
        ledt->setText(dir);
        ledt->setToolTip(dir);
    }
}

void LabDataListWidget::selectFiles(int type)
{
    QLineEdit *ledt = mledtFields.value(type);
    if (!ledt)
        return;
    QString dir = ledt->text();
    if (dir.isEmpty())
        dir = mlastDir;
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select files", "fdlg caption"), dir);
    if (fileNames.isEmpty())
        return;
    mlastDir = QFileInfo(fileNames.first()).path();
    foreach (int i, bRangeD(0, fileNames.size() - 1))
        fileNames[i].remove(0, mlastDir.length());
    bool b = false;
    QString mainFileName = selectMainFile(fileNames, &b);
    if (!b)
        return;
    TLabApplication app;
    if (!app.load(mlastDir, mainFileName, fileNames))
        return;
    if (setLabApplication(&app, type)) {
        ledt->setText(mlastDir);
        ledt->setToolTip(mlastDir);
    }
}
