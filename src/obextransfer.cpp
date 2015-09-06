/*
 * BluezQt - Asynchronous Bluez wrapper library
 *
 * Copyright (C) 2014 David Rosca <nowrep@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "obextransfer.h"
#include "obextransfer_p.h"
#include "pendingcall.h"
#include "utils.h"
#include "macros.h"

#include <QDBusServiceWatcher>

namespace BluezQt
{

static ObexTransfer::Status stringToStatus(const QString &status)
{
    if (status == QLatin1String("queued")) {
        return ObexTransfer::Queued;
    } else if (status == QLatin1String("active")) {
        return ObexTransfer::Active;
    } else if (status == QLatin1String("suspended")) {
        return ObexTransfer::Suspended;
    } else if (status == QLatin1String("complete")) {
        return ObexTransfer::Complete;
    } else if (status == QLatin1String("error")) {
        return ObexTransfer::Error;
    }
    return ObexTransfer::Unknown;
}

ObexTransferPrivate::ObexTransferPrivate(const QString &path, const QVariantMap &properties)
    : QObject()
    , m_dbusProperties(0)
    , m_status(ObexTransfer::Error)
    , m_time(0)
    , m_size(0)
    , m_transferred(0)
    , m_suspendable(false)
{
    m_bluezTransfer = new BluezTransfer(Strings::orgBluezObex(), path, DBusConnection::orgBluezObex(), this);

    QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(Strings::orgBluezObex(), DBusConnection::orgBluezObex(),
            QDBusServiceWatcher::WatchForUnregistration, this);

    connect(serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &ObexTransferPrivate::orgBluezObexUnregistered);

    init(properties);
}

void ObexTransferPrivate::init(const QVariantMap &properties)
{
    m_dbusProperties = new DBusProperties(Strings::orgBluezObex(), m_bluezTransfer->path(),
                                          DBusConnection::orgBluezObex(), this);

    connect(m_dbusProperties, &DBusProperties::PropertiesChanged,
            this, &ObexTransferPrivate::propertiesChanged, Qt::QueuedConnection);

    // Init properties
    m_status = stringToStatus(properties.value(QStringLiteral("Status")).toString());
    m_name = properties.value(QStringLiteral("Name")).toString();
    m_type = properties.value(QStringLiteral("Type")).toString();
    m_time = properties.value(QStringLiteral("Time")).toUInt();
    m_size = properties.value(QStringLiteral("Size")).toUInt();
    m_transferred = properties.value(QStringLiteral("Transferred")).toUInt();
    m_fileName = properties.value(QStringLiteral("Filename")).toString();
}

void ObexTransferPrivate::propertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated)
{
    Q_UNUSED(invalidated)

    if (interface != Strings::orgBluezObexTransfer1()) {
        return;
    }

    QVariantMap::const_iterator i;
    for (i = changed.constBegin(); i != changed.constEnd(); ++i) {
        const QVariant &value = i.value();
        const QString &property = i.key();

        if (property == QLatin1String("Status")) {
            PROPERTY_CHANGED2(m_status, stringToStatus(value.toString()), statusChanged);
        } else if (property == QLatin1String("Transferred")) {
            PROPERTY_CHANGED(m_transferred, toUInt, transferredChanged);
        } else if (property == QLatin1String("Filename")) {
            PROPERTY_CHANGED(m_fileName, toString, fileNameChanged);
        }
    }
}

void ObexTransferPrivate::orgBluezObexUnregistered()
{
    // Change status to Error if org.bluez.obex crashes
    if (m_status != ObexTransfer::Complete && m_status != ObexTransfer::Error) {
        m_status = ObexTransfer::Error;
        Q_EMIT q.data()->statusChanged(m_status);
    }
}

ObexTransfer::ObexTransfer(const QString &path, const QVariantMap &properties)
    : QObject()
    , d(new ObexTransferPrivate(path, properties))
{
}

ObexTransfer::~ObexTransfer()
{
    delete d;
}

ObexTransferPtr ObexTransfer::toSharedPtr() const
{
    return d->q.toStrongRef();
}

QDBusObjectPath ObexTransfer::objectPath() const
{
    return QDBusObjectPath(d->m_bluezTransfer->path());
}

ObexTransfer::Status ObexTransfer::status() const
{
    return d->m_status;
}

QString ObexTransfer::name() const
{
    return d->m_name;
}

QString ObexTransfer::type() const
{
    return d->m_type;
}

quint64 ObexTransfer::time() const
{
    return d->m_time;
}

quint64 ObexTransfer::size() const
{
    return d->m_size;
}

quint64 ObexTransfer::transferred() const
{
    return d->m_transferred;
}

QString ObexTransfer::fileName() const
{
    return d->m_fileName;
}

bool ObexTransfer::isSuspendable() const
{
    return d->m_suspendable;
}

PendingCall *ObexTransfer::cancel()
{
    return new PendingCall(d->m_bluezTransfer->Cancel(), PendingCall::ReturnVoid, this);
}

PendingCall *ObexTransfer::suspend()
{
    return new PendingCall(d->m_bluezTransfer->Suspend(), PendingCall::ReturnVoid, this);
}

PendingCall *ObexTransfer::resume()
{
    return new PendingCall(d->m_bluezTransfer->Resume(), PendingCall::ReturnVoid, this);
}

} // namespace BluezQt
