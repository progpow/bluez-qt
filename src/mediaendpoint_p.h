/*
 * BluezQt - Asynchronous Bluez wrapper library
 *
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
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

#ifndef BLUEZQT_MEDIAENDPOINT_P_H
#define BLUEZQT_MEDIAENDPOINT_P_H

#include <QDBusObjectPath>
#include <QVariantMap>

#include "mediaendpoint.h"

namespace BluezQt
{

class MediaEndpointPrivate
{
public:
    explicit MediaEndpointPrivate(const MediaEndpoint::Configuration &configuration);

    void init(const MediaEndpoint::Configuration &configuration);

    QVariantMap m_properties;
    MediaEndpoint::Configuration m_configuration;
    QDBusObjectPath m_objectPath;
};

} // namespace BluezQt

#endif // BLUEZQT_MEDIAENDPOINT_P_H
