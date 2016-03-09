/***************************************************************************
 *   Copyright (C) 2008 by Elad Lahav
 *   elad_lahav@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "wikieditor.h"

/**
 * Class constructor.
 * @param  parent  The parent widget
 */
WikiEditor::WikiEditor(QWidget* parent) : QTextEdit(parent)
{
	// Wiki text is composed of plain text only.
	setAcceptRichText(false);
}

/**
 * Class destructor.
 */
WikiEditor::~WikiEditor()
{
}

/**
 * Opens a file for editing.
 * @param  path  The path of the file to open
 */
void WikiEditor::load(const QString& path)
{
	QFile file(path);
	
	// Nothing to do if the requested file is already open.
	if (path == path_)
		return;
	
	// Make sure we get a clean buffer even if loading fails.
	clear();
				
	// Try to open the file.
	if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		path_ = QString();
		QString msg = tr("The file '%1' cannot be edited").arg(path);
		QMessageBox::warning(0, tr("Failed to open file"), msg);
		return;
	}
	
	// Store the new path.
	path_ = path;
	
	// Load the file's contents into the editor.
	setText(file.readAll());
	
	// Even though the buffer's contents have changed, the document was not
	// modified (from a disk file point of view).
	document()->setModified(false);
}

/**
 * Writes the contents of the editor back to disk.
 */
void WikiEditor::save()
{
	// Nothing to do if no file is loaded.
	if (path_.isEmpty())
		return;
	
	// Open the file for writing.
	QFile file(path_);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QString msg = tr("The file '%1' cannot be saved").arg(path_);
		QMessageBox::warning(0, tr("Failed to open file"), msg);
		return;
	}
	
	// Write the buffer's data to file.
	QTextStream strm(&file);
	strm << toPlainText();
	
	// The document is no longer considered as modified.
	document()->setModified(false);
}
