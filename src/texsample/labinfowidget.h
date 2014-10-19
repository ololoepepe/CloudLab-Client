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

#ifndef LABINFOWIDGET_H
#define LABINFOWIDGET_H

class ExtraFileListWidget;
class LabDataListWidget;
class LabModel;

class TAbstractCache;
class TAuthorListWidget;
class TGroupListWidget;
class TNetworkClient;
class TTagWidget;

class BInputField;

class QByteArray;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QVariant;
class QVBoxLayout;

#include <QString>
#include <QWidget>

/*============================================================================
================================ LabInfoWidget ===============================
============================================================================*/

class LabInfoWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode
    {
        AddMode,
        EditMode,
        ShowMode
    };
private:
    static const QString DateTimeFormat;
    static const Qt::TextInteractionFlags TextInteractionFlags;
private:
    const Mode mmode;
private:
    TAbstractCache *mcache;
    TNetworkClient *mclient;
    quint64 mid;
    LabModel *mmodel;
    quint64 msenderId;
    bool mvalid;
    //
    QLineEdit *mledtTitle;
    BInputField *minputTitle;
    TTagWidget *mtgwgt;
    QLabel *mlblSender;
    QComboBox *mcmboxType;
    QLabel *mlblSize;
    QLabel *mlblCreationDT;
    QLabel *mlblUpdateDT;
    TAuthorListWidget *mlstwgtAuthors;
    QPlainTextEdit *mptedtDescription;
    TGroupListWidget *mlstwgtGroups;
    QCheckBox *mcboxEditData;
    LabDataListWidget *mldlwgt;
    ExtraFileListWidget *meflwgt;
public:
    explicit LabInfoWidget(Mode m, QWidget *parent = 0);
public:
    TAbstractCache *cache() const;
    TNetworkClient *client() const;
    QVariant createRequestData() const;
    bool hasValidInput() const;
    Mode mode() const;
    LabModel *model() const;
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;
    void setCache(TAbstractCache *cache);
    void setClient(TNetworkClient *client);
    bool setLab(quint64 labId);
    void setModel(LabModel *model);
    QString title() const;
private:
    void createAuthorsGroup(QHBoxLayout *hlt, bool readOnly = false);
    void createDescriptionGroup(QHBoxLayout *hlt, bool readOnly = false);
    void createEditDataField(QFormLayout *flt);
    void createExtraFileListGroup(QHBoxLayout *hlt, bool readOnly = false);
    void createExtraGroup(QHBoxLayout *hlt);
    void createGroupsGroup(QHBoxLayout *hlt, bool readOnly = false);
    void createLabDataListGroup(QHBoxLayout *hlt);
    void createMainGroup(QVBoxLayout *vlt, bool readOnly = false);
    void createTagsField(QFormLayout *flt, bool readOnly = false);
    void createTitleField(QFormLayout *flt, bool readOnly = false);
private slots:
    void checkInputs();
    void clientAuthorizedChanged(bool authorized);
    void showExtraFile(const QString &fileName);
    void showSenderInfo();
signals:
    void inputValidityChanged(bool valid);
};

#endif // LABINFOWIDGET_H
