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

#ifndef EXTRAFILEWIDGET_H
#define EXTRAFILEWIDGET_H

class QFormLayout;
class QHBoxLayout;
class QLabel;
class QToolButton;

#include <QString>
#include <QWidget>

/*============================================================================
================================ ExtraFileWidget =============================
============================================================================*/

class ExtraFileWidget : public QWidget
{
    Q_OBJECT
private:
    bool mdeleted;
    QString mfileName;
    QFormLayout *mflt;
    QHBoxLayout *mhlt;
    quint64 mlabId;
    QLabel *mlbl;
    bool mreadOnly;
    int msize;
    QToolButton *mtbtnDelete;
    QToolButton *mtbtnToggle;
public:
    explicit ExtraFileWidget(QFormLayout *flt, const QString &fileName, QWidget *parent = 0);
    explicit ExtraFileWidget(QFormLayout *flt, quint64 labId, const QString &fileName, int size, QWidget *parent = 0);
    explicit ExtraFileWidget(QFormLayout *flt, quint64 labId, const QString &fileName, int size, bool readOnly,
                             QWidget *parent = 0);
    ~ExtraFileWidget();
public:
    QString fileName() const;
    bool isDeleted() const;
    bool isReadOnly() const;
    void setReadOnly(bool ro);
signals:
    void deletePermanently();
private:
    void init();
private slots:
    void showFile();
    void toggleDeleted();
private:
    Q_DISABLE_COPY(ExtraFileWidget)
};

#endif // EXTRAFILEWIDGET_H
