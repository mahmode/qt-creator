/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "clangsupport_global.h"

#include <utf8stringvector.h>

#include <QDataStream>

namespace ClangBackEnd {

class UpdateVisibleTranslationUnitsMessage
{
public:
    UpdateVisibleTranslationUnitsMessage() = default;
    UpdateVisibleTranslationUnitsMessage(const Utf8String &currentEditorFilePath,
                                         const Utf8StringVector &visibleEditorFilePaths)
        : currentEditorFilePath(currentEditorFilePath),
          visibleEditorFilePaths(visibleEditorFilePaths)
    {
    }

    friend QDataStream &operator<<(QDataStream &out, const UpdateVisibleTranslationUnitsMessage &message)
    {
        out << message.currentEditorFilePath;
        out << message.visibleEditorFilePaths;

        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, UpdateVisibleTranslationUnitsMessage &message)
    {
        in >> message.currentEditorFilePath;
        in >> message.visibleEditorFilePaths;

        return in;
    }

    friend bool operator==(const UpdateVisibleTranslationUnitsMessage &first, const UpdateVisibleTranslationUnitsMessage &second)
    {
        return first.currentEditorFilePath == second.currentEditorFilePath
            && first.visibleEditorFilePaths == second.visibleEditorFilePaths;
    }

public:
    Utf8String currentEditorFilePath;
    Utf8StringVector visibleEditorFilePaths;
};

CLANGSUPPORT_EXPORT QDebug operator<<(QDebug debug, const UpdateVisibleTranslationUnitsMessage &message);

DECLARE_MESSAGE(UpdateVisibleTranslationUnitsMessage)
} // namespace ClangBackEnd
