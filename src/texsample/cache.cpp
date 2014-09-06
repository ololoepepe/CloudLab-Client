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

#include "cache.h"

#include <TeXSample/TeXSampleCore>

#include <BDirTools>
#include <BeQt>
#include <BSqlDatabase>
#include <BSqlQuery>
#include <BSqlResult>
#include <BSqlWhere>
#include <BUuid>

#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ Cache =======================================
============================================================================*/

/*============================== Public constructors =======================*/

Cache::Cache(const QString &location) : Location(location)
{
    menabled = true;
    QString schemaFileName = BDirTools::findResource("texsample/db/cache.schema", BDirTools::GlobalOnly);
    mdb = new BSqlDatabase("QSQLITE", BUuid::createUuid().toString(true));
    mdb->setDatabaseName(location + "/cache.sqlite");
    mdb->setOnOpenQuery("PRAGMA foreign_keys = ON");
    if (!mdb->open() || !mdb->initializeFromSchemaFile(schemaFileName, "UTF-8")) {
        delete mdb;
        mdb = 0;
    }
}

Cache::~Cache()
{
    delete mdb;
}

/*============================== Public methods ============================*/

void Cache::clear()
{
    if (!mdb)
        return;
    if (!mdb->transaction())
        return;
    if (!mdb->deleteFrom("users") || !mdb->deleteFrom("invite_codes") || !mdb->deleteFrom("groups")
            || !mdb->deleteFrom("labs") || !mdb->deleteFrom("last_request_date_times") || !mdb->exec("VACUUM")) {
        bRet(mdb->rollback());
    }
    mdb->commit();
}

QVariant Cache::data(const QString &, const QVariant &) const
{
    return QVariant();
}

TGroupInfoList Cache::groupInfoList() const
{
    TGroupInfoList list;
    if (!menabled || !mdb)
        return list;
    BSqlResult result = getSelf()->mdb->select("groups", "info");
    if (!result.success())
        return list;
    foreach (const QVariantMap &m, result.values())
        list << BeQt::deserialize(m.value("info").toByteArray()).value<TGroupInfo>();
    return list;
}

TInviteInfoList Cache::inviteInfoList() const
{
    TInviteInfoList list;
    if (!menabled || !mdb)
        return list;
    BSqlResult result = getSelf()->mdb->select("invite_codes", "info");
    if (!result.success())
        return list;
    foreach (const QVariantMap &m, result.values())
        list << BeQt::deserialize(m.value("info").toByteArray()).value<TInviteInfo>();
    return list;
}

bool Cache::isEnabled() const
{
    return menabled;
}

QDateTime Cache::lastRequestDateTime(RequestType type, const quint64) const
{
    if (!menabled || !mdb)
        return QDateTime();
    if (!bRangeD(GroupListRequest, UserListRequest).contains(type))
        return QDateTime();
    QDateTime dt;
    dt.setTimeSpec(Qt::UTC);
    BSqlWhere where("request_type = :request_type", ":request_type", int(type));
    BSqlResult result = mdb->select("last_request_date_times", "date_time", where);
    if (!result.success() || result.values().isEmpty())
        return QDateTime();
    dt.setMSecsSinceEpoch(result.value("date_time").toLongLong());
    return dt;
}

void Cache::removeData(const QString &operation, const QVariant &id)
{
    typedef QMap<QString, RemoveDataFunction> RemoveDataFunctionMap;
    init_once(RemoveDataFunctionMap, functionMap, RemoveDataFunctionMap()) {
        functionMap.insert(TOperation::DeleteGroup, &Cache::handleDeleteGroup);
        functionMap.insert(TOperation::DeleteInvites, &Cache::handleDeleteInvites);
        functionMap.insert(TOperation::DeleteLab, &Cache::handleDeleteLab);
        functionMap.insert(TOperation::DeleteUser, &Cache::handleDeleteUser);
    }
    if (!menabled || !mdb)
        return;
    RemoveDataFunction f = functionMap.value(operation);
    if (!f)
        return;
    if (!mdb->transaction())
        return;
    if (!(this->*f)(id))
        return bRet(mdb->rollback());
    mdb->commit();
}

TLabInfoList Cache::labInfoList() const
{
    TLabInfoList list;
    if (!menabled || !mdb)
        return list;
    BSqlResult result = getSelf()->mdb->select("labs", "info");
    if (!result.success())
        return list;
    foreach (const QVariantMap &m, result.values())
        list << BeQt::deserialize(m.value("info").toByteArray()).value<TLabInfo>();
    return list;
}

void Cache::setData(const QString &operation, const QDateTime &requestDateTime, const QVariant &data,
                    const QVariant &id)
{
    typedef QMap<QString, SetDataFunction> SetDataFunctionMap;
    init_once(SetDataFunctionMap, functionMap, SetDataFunctionMap()) {
        functionMap.insert(TOperation::AddGroup, &Cache::handleAddGroup);
        functionMap.insert(TOperation::AddLab, &Cache::handleAddLab);
        functionMap.insert(TOperation::AddUser, &Cache::handleAddUser);
        functionMap.insert(TOperation::GenerateInvites, &Cache::handleGenerateInvites);
        functionMap.insert(TOperation::EditGroup, &Cache::handleEditGroup);
        functionMap.insert(TOperation::EditLab, &Cache::handleEditLab);
        functionMap.insert(TOperation::EditSelf, &Cache::handleEditSelf);
        functionMap.insert(TOperation::EditUser, &Cache::handleEditUser);
        functionMap.insert(TOperation::GetGroupInfoList, &Cache::handleGetGroupInfoList);
        functionMap.insert(TOperation::GetInviteInfoList, &Cache::handleGetInviteInfoList);
        functionMap.insert(TOperation::GetLabInfoList, &Cache::handleGetLabInfoList);
        functionMap.insert(TOperation::GetSelfInfo, &Cache::handleGetSelfInfo);
        functionMap.insert(TOperation::GetUserInfo, &Cache::handleGetUserInfo);
        functionMap.insert(TOperation::GetUserInfoAdmin, &Cache::handleGetUserInfoAdmin);
        functionMap.insert(TOperation::GetUserInfoListAdmin, &Cache::handleGetUserInfoListAdmin);
    }
    if (!menabled || !mdb)
        return;
    SetDataFunction f = functionMap.value(operation);
    if (!f)
        return;
    if (!mdb->transaction())
        return;
    if (!(this->*f)(requestDateTime, data, id))
        return bRet(mdb->rollback());
    mdb->commit();
}

void Cache::setEnabled(bool enabled)
{
    menabled = enabled;
}

TUserInfoList Cache::userInfoList() const
{
    TUserInfoList list;
    if (!menabled || !mdb)
        return list;
    BSqlResult result = getSelf()->mdb->select("users", "info");
    if (!result.success())
        return list;
    foreach (const QVariantMap &m, result.values())
        list << BeQt::deserialize(m.value("info").toByteArray()).value<TUserInfo>();
    return list;
}

/*============================== Private methods ===========================*/

Cache *Cache::getSelf() const
{
    return const_cast<Cache *>(this);
}

bool Cache::handleAddGroup(const QDateTime &, const QVariant &v, const QVariant &)
{
    TAddGroupReplyData data = v.value<TAddGroupReplyData>();
    TGroupInfo info = data.groupInfo();
    if (!mdb->insert("groups", "id", info.id(), "info", BeQt::serialize(info)).success())
        return false;
    return true;
}

bool Cache::handleAddLab(const QDateTime &, const QVariant &v, const QVariant &)
{
    TAddLabReplyData data = v.value<TAddLabReplyData>();
    TLabInfo info = data.labInfo();
    if (!mdb->insert("labs", "id", info.id(), "info", BeQt::serialize(info)).success())
        return false;
    return true;
}

bool Cache::handleAddUser(const QDateTime &, const QVariant &v, const QVariant &)
{
    TAddUserReplyData data = v.value<TAddUserReplyData>();
    TUserInfo info = data.userInfo();
    if (!mdb->insert("users", "id", info.id(), "info", BeQt::serialize(info)).success())
        return false;
    return true;
}

bool Cache::handleDeleteGroup(const QVariant &v)
{
    quint64 groupId = v.toULongLong();
    if (!mdb->deleteFrom("groups", BSqlWhere("id = :id", ":id", groupId)).success())
        return false;
    return true;
}

bool Cache::handleDeleteInvites(const QVariant &v)
{
    foreach (quint64 id, v.value<TIdList>()) {
        if (mdb->deleteFrom("invite_codes", BSqlWhere("id = :id", ":id", id)))
            return false;
    }
    return true;
}

bool Cache::handleDeleteLab(const QVariant &v)
{
    return mdb->deleteFrom("labs", BSqlWhere("id = :id", ":id", v.toULongLong()));
}

bool Cache::handleDeleteUser(const QVariant &v)
{
    return mdb->deleteFrom("users", BSqlWhere("id = :id", ":id", v.toULongLong()));
}

bool Cache::handleEditGroup(const QDateTime &, const QVariant &v, const QVariant &)
{
    TEditGroupReplyData data = v.value<TEditGroupReplyData>();
    TGroupInfo info = data.groupInfo();
    if (!mdb->update("groups", "info", BeQt::serialize(info), BSqlWhere("id = :id", ":id", info.id())).success())
        return false;
    return true;
}

bool Cache::handleEditLab(const QDateTime &, const QVariant &v, const QVariant &)
{
    TEditLabReplyData data = v.value<TEditLabReplyData>();
    TLabInfo info = data.labInfo();
    BSqlWhere where("id = :id", ":id", info.id());
    if (!mdb->update("labs", "info", BeQt::serialize(info), where).success())
        return false;
    return true;
}

bool Cache::handleEditSelf(const QDateTime &, const QVariant &v, const QVariant &)
{
    TEditSelfReplyData data = v.value<TEditSelfReplyData>();
    TUserInfo info = data.userInfo();
    if (!mdb->update("users", "info", BeQt::serialize(info), BSqlWhere("id = :id", ":id", info.id())).success())
        return false;
    return true;
}

bool Cache::handleEditUser(const QDateTime &, const QVariant &v, const QVariant &)
{
    TEditUserReplyData data = v.value<TEditUserReplyData>();
    TUserInfo info = data.userInfo();
    if (!mdb->update("users", "info", BeQt::serialize(info), BSqlWhere("id = :id", ":id", info.id())).success())
        return false;
    return true;
}

bool Cache::handleGenerateInvites(const QDateTime &, const QVariant &v, const QVariant &)
{
    TGenerateInvitesReplyData data = v.value<TGenerateInvitesReplyData>();
    foreach (const TInviteInfo &info, data.generatedInvites()) {
        if (!mdb->insert("invite_codes", "id", info.id(), "info", BeQt::serialize(info)).success())
            return false;
    }
    return true;
}

bool Cache::handleGetGroupInfoList(const QDateTime &dt, const QVariant &v, const QVariant &)
{
    TGetGroupInfoListReplyData data = v.value<TGetGroupInfoListReplyData>();
    QVariantMap values;
    values.insert("request_type", int(GroupListRequest));
    values.insert("date_time", dt.toUTC().toMSecsSinceEpoch());
    if (!mdb->insertOrReplace("last_request_date_times", values).success())
        return false;
    foreach (quint64 groupId, data.deletedGroups()) {
        if (!mdb->deleteFrom("groups", BSqlWhere("id = :id", ":id", groupId)))
            return false;
    }
    foreach (const TGroupInfo &info, data.newGroups()) {
        if (!mdb->insertOrReplace("groups", "id", info.id(), "info", BeQt::serialize(info)))
            return false;
    }
    return true;
}

bool Cache::handleGetInviteInfoList(const QDateTime &dt, const QVariant &v, const QVariant &)
{
    TGetInviteInfoListReplyData data = v.value<TGetInviteInfoListReplyData>();
    QVariantMap values;
    values.insert("request_type", int(InviteListRequest));
    values.insert("date_time", dt.toUTC().toMSecsSinceEpoch());
    if (!mdb->insertOrReplace("last_request_date_times", values).success())
        return false;
    foreach (quint64 inviteId, data.deletedInvites()) {
        if (!mdb->deleteFrom("invite_codes", BSqlWhere("id = :id", ":id", inviteId)))
            return false;
    }
    foreach (const TInviteInfo &info, data.newInvites()) {
        if (!mdb->insertOrReplace("invite_codes", "id", info.id(), "info", BeQt::serialize(info)))
            return false;
    }
    return true;
}

bool Cache::handleGetLabInfoList(const QDateTime &dt, const QVariant &v, const QVariant &)
{
    TGetLabInfoListReplyData data = v.value<TGetLabInfoListReplyData>();
    QVariantMap values;
    values.insert("request_type", int(LabListRequest));
    values.insert("date_time", dt.toUTC().toMSecsSinceEpoch());
    if (!mdb->insertOrReplace("last_request_date_times", values).success())
        return false;
    foreach (quint64 labId, data.deletedLabs()) {
        if (!mdb->deleteFrom("labs", BSqlWhere("id = :id", ":id", labId)))
            return false;
    }
    foreach (const TLabInfo &info, data.newLabs()) {
        if (!mdb->insertOrReplace("labs", "id", info.id(), "info", BeQt::serialize(info)))
            return false;
    }
    return true;
}

bool Cache::handleGetUserInfo(const QDateTime &, const QVariant &v, const QVariant &)
{
    TGetUserInfoReplyData data = v.value<TGetUserInfoReplyData>();
    TUserInfo info = data.userInfo();
    if (!mdb->insertOrReplace("users", "id", info.id(), "info", BeQt::serialize(info)).success())
        return false;
    return true;
}

bool Cache::handleGetSelfInfo(const QDateTime &, const QVariant &v, const QVariant &)
{
    TGetSelfInfoReplyData data = v.value<TGetSelfInfoReplyData>();
    TUserInfo info = data.userInfo();
    if (!mdb->insertOrReplace("users", "id", info.id(), "info", BeQt::serialize(info)).success())
        return false;
    return true;
}

bool Cache::handleGetUserInfoAdmin(const QDateTime &, const QVariant &v, const QVariant &)
{
    TGetUserInfoAdminReplyData data = v.value<TGetUserInfoAdminReplyData>();
    TUserInfo info = data.userInfo();
    if (!mdb->insertOrReplace("users", "id", info.id(), "info", BeQt::serialize(info)).success())
        return false;
    return true;
}

bool Cache::handleGetUserInfoListAdmin(const QDateTime &dt, const QVariant &v, const QVariant &)
{
    TGetUserInfoListAdminReplyData data = v.value<TGetUserInfoListAdminReplyData>();
    QVariantMap values;
    values.insert("request_type", int(UserListRequest));
    values.insert("date_time", dt.toUTC().toMSecsSinceEpoch());
    if (!mdb->insertOrReplace("last_request_date_times", values).success())
        return false;
    foreach (quint64 userId, data.deletedUsers()) {
        if (!mdb->deleteFrom("users", BSqlWhere("id = :id", ":id", userId)))
            return false;
    }
    foreach (const TUserInfo &info, data.newUsers()) {
        if (!mdb->insertOrReplace("users", "id", info.id(), "info", BeQt::serialize(info)))
            return false;
    }
    return true;
}
