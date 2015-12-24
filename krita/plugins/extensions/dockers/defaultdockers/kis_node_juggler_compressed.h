/*
 *  Copyright (c) 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __KIS_NODE_JUGGLER_COMPRESSED_H
#define __KIS_NODE_JUGGLER_COMPRESSED_H

#include <QObject>
#include <QScopedPointer>

#include <kritadefaultdockers_export.h>
#include <kundo2command.h>
#include "kis_types.h"


class KRITADEFAULTDOCKERS_EXPORT KisNodeJugglerCompressed : public QObject
{
    Q_OBJECT
public:
    KisNodeJugglerCompressed(const KUndo2MagicString &actionName, KisImageSP image, int timeout);
    ~KisNodeJugglerCompressed();

    void moveNode(KisNodeSP node, KisNodeSP parent, KisNodeSP above);
    void setAutoDelete(bool value);

    bool isEnded() const;

    void lowerNode(KisNodeSP node);
    void raiseNode(KisNodeSP node);

public Q_SLOTS:
    void end();

private Q_SLOTS:
    void slotUpdateTimeout();
    void slotEndStrokeRequested();

private:
    void startTimers();

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif /* __KIS_NODE_JUGGLER_COMPRESSED_H */