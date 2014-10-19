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

#ifndef LABDATALISTWIDGET_H
#define LABDATALISTWIDGET_H

class TLabApplication;

class QByteArray;
class QComboBox;
class QHBoxLayout;
class QLineEdit;
class QStringList;

#include <TLabDataList>

#include <QMap>
#include <QString>
#include <QWidget>

/*============================================================================
================================ LabDataListWidget ===========================
============================================================================*/

class LabDataListWidget : public QWidget
{
    Q_OBJECT
private:
    enum FieldType
    {
        WindowsField = 1,
        LinuxField,
        MacosField,
        WebField,
        UrlField
    };
private:
    QComboBox *mcmboxType;
    TLabDataList mdataList;
    QMap<int, QHBoxLayout *> mhltFields;
    QString mlastDir;
    QMap<int, QLineEdit *> mledtFields;
    int mtype;
    bool mvalid;
public:
    explicit LabDataListWidget(QWidget *parent = 0);
public:
    const TLabDataList &dataList() const;
    bool hasValidInput() const;
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;
signals:
    void inputValidityChanged(bool valid);
private:
    QString selectMainFile(const QStringList &fileNames, bool *ok = 0);
    bool setLabApplication(TLabApplication *app, int type);
private slots:
    void checkInputs();
    void cmboxTypeCurrentIndexChanged(int index);
    void ledtUrlTextChanged(const QString &text);
    void selectDir(int type);
    void selectFiles(int type);
};

#endif // LABDATALISTWIDGET_H
