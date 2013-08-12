/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "diffshoweditorfactory.h"
#include "diffshoweditor.h"
#include "diffeditorwidget.h"
#include "diffeditorconstants.h"

#include <QCoreApplication>

namespace DiffEditor {

namespace Internal {

DiffShowEditorFactory::DiffShowEditorFactory(QObject *parent)
    : IEditorFactory(parent)
{
    setId(Constants::DIFF_SHOW_EDITOR_ID);
    setDisplayName(qApp->translate("DiffEditorFactory", Constants::DIFF_SHOW_EDITOR_DISPLAY_NAME));
    setMimeTypes(QStringList() << QLatin1String(Constants::DIFF_EDITOR_MIMETYPE));
}

Core::IEditor *DiffShowEditorFactory::createEditor(QWidget *parent)
{
    DiffEditorWidget *editorWidget = new DiffEditorWidget(parent);
    DiffShowEditor *editor = new DiffShowEditor(editorWidget);
    return editor;
}

} // namespace Internal
} // namespace DiffEditor
