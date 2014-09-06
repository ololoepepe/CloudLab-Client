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

class TBinaryFileList;
class TFileInfoList;

class QStringList;

#include <QWidget>

/*============================================================================
================================ ExtraFileListWidget =========================
============================================================================*/

class ExtraFileListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExtraFileListWidget(QWidget *parent = 0);
    explicit ExtraFileListWidget(bool readOnly, QWidget *parent = 0);
public:
    //void addFile(const QString &fn);
    //void addFiles(const QStringList &list);
    //QStringList files() const;
    QStringList deletedFileList() const;
    bool isReadOnly() const;
    TBinaryFileList newFileList() const;
    void setFileInfos(const TFileInfoList &list);
    void setReadOnly(bool ro);
private:
    void init();
//signals:
//    void getFile(const QString &fileName);
//private:
//    void deleteLastDeleted();
//private slots:
//    void mapped(const QString &fn);
//    void addFile();
/*private:
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
    QString lastDir;*/
private:
    Q_DISABLE_COPY(ExtraFileListWidget)
};

#endif // EXTRAFILELISTWIDGET_H
