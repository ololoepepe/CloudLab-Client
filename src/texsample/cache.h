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

#ifndef CACHE_H
#define CACHE_H

class TGroupInfoList;
class TInviteInfoList;
class TLabInfoList;
class TUserInfoList;

class BSqlDatabase;

class QDateTime;
class QString;

#include <TAbstractCache>

#include <QVariant>

/*============================================================================
================================ Cache =======================================
============================================================================*/

class Cache : public TAbstractCache
{
public:
    enum RequestType
    {
        GroupListRequest = 1,
        InviteListRequest,
        LabListRequest,
        UserListRequest
    };
public:
    const QString Location;
public:
    BSqlDatabase *mdb;
    bool menabled;
public:
    explicit Cache(const QString &location);
    ~Cache();
public:
    void clear();
    QVariant data(const QString &operation, const QVariant &id = QVariant()) const;
    TGroupInfoList groupInfoList() const;
    TInviteInfoList inviteInfoList() const;
    bool isEnabled() const;
    QDateTime lastRequestDateTime(RequestType type, const quint64 id = 0) const;
    void removeData(const QString &operation, const QVariant &id = QVariant());
    TLabInfoList labInfoList() const;
    void setData(const QString &operation, const QDateTime &requestDateTime, const QVariant &data = QVariant(),
                 const QVariant &id = QVariant());
    void setEnabled(bool enabled);
    TUserInfoList userInfoList() const;
private:
    typedef QVariant (Cache::*GetDataFunction)(const QVariant &id);
    typedef bool (Cache::*RemoveDataFunction)(const QVariant &id);
    typedef bool (Cache::*SetDataFunction)(const QDateTime &requestDateTime, const QVariant &data, const QVariant &id);
private:
    Cache *getSelf() const;
    bool handleAddGroup(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleAddLab(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleAddUser(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleEditGroup(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleEditLab(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleEditSelf(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleEditUser(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleDeleteGroup(const QVariant &v);
    bool handleDeleteInvites(const QVariant &v);
    bool handleDeleteLab(const QVariant &v);
    bool handleDeleteUser(const QVariant &v);
    bool handleGenerateInvites(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetGroupInfoList(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetInviteInfoList(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetLabInfoList(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetSelfInfo(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetUserInfo(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetUserInfoAdmin(const QDateTime &dt, const QVariant &v, const QVariant &id);
    bool handleGetUserInfoListAdmin(const QDateTime &dt, const QVariant &v, const QVariant &id);
};

#endif // CACHE_H
