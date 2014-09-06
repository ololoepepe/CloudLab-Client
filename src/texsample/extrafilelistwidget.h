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

#ifndef EXTRAFILELISTWIDGET_H
#define EXTRAFILELISTWIDGET_H

class ExtraFileWidget;

class TBinaryFileList;
class TFileInfoList;

class QByteArray;
class QFormLayout;
class QObject;
class QSignalMapper;
class QToolButton;

#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidget>

/*============================================================================
================================ ExtraFileListWidget =========================
============================================================================*/

class ExtraFileListWidget : public QWidget
{
    Q_OBJECT
private:
    QStringList mdeletedFileNames;
    QSignalMapper *mdeleteMapper;
    QMap<QWidget *, ExtraFileWidget *> mwidgets;
    QStringList mfileNames;
    QFormLayout *mflt;
    quint64 mlabId;
    QString mlastDir;
    bool mreadOnly;
    QToolButton *mtbtnAdd;
public:
    explicit ExtraFileListWidget(QWidget *parent = 0);
    explicit ExtraFileListWidget(bool readOnly, QWidget *parent = 0);
public:
    QStringList deletedFileList() const;
    bool isReadOnly() const;
    TBinaryFileList newFileList() const;
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;
    void setFileInfos(const TFileInfoList &list);
    void setLabId(quint64 id);
    void setReadOnly(bool ro);
private:
    void addFile(const QString &fileName, int size = 0);
    void init();
private slots:
    void addFiles();
    void deletePermanently(QWidget *widget);
private:
    Q_DISABLE_COPY(ExtraFileListWidget)
};

#endif // EXTRAFILELISTWIDGET_H
