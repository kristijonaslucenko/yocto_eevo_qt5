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

#include <QMessageBox>
#include <QtDebug>
#include "wikiwidget.h"
#include "finddialog.h"

/**
 * Class constructor.
 * @param  parent  The parent widget
 */
WikiWidget::WikiWidget(QWidget* parent) : QWidget(parent), Ui::WikiWidget()
{
	setupUi(this);
}

/**
 * Class destructor.
 */
WikiWidget::~WikiWidget()
{
}

/**
 * Prompts the user to save any modified contents in the editor.
 * This method is called before the main window closes.
 * @return true if the main window can close, false to stay open
 */
bool WikiWidget::canClose()
{
	// Can close if the editor's contents are not modified.
	if (!editor_->document()->isModified())
		return true;
	
	// Prompt the user.
	QString path = editor_->path();
	QString msg = tr("The contents of '%1' have been modified.\n"
	                 "Would you like to save the file now?").arg(path);
	switch (QMessageBox::question(0, tr("Save Changes?"), msg,
	        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
	case QMessageBox::Yes:
		// Save and close the main window.
		editor_->save();
		return true;
		
	case QMessageBox::No:
		// Skip saving and close the main window.
		return true;
		
	default:
		// Cancel: keep the main window open.
		;
	}
	
	return false;
}

/**
 * Switches between browsing and editing modes.
 * This slot handles the "Edit->Edit mode" action.
 * @note Switching to browsing mode automatically saves the contents of the
 * editor. This is because browsing mode allows the user to navigate to other
 * documents, which may result in accidental loss of unsaved editor contents.
 * @param  editMode  true to switch to edit mode, false to switch to browsing
 *                   mode
 */
void WikiWidget::setMode(bool editMode)
{
	if (editMode) {
		// Show the editor widget.
		// Make sure the file is loaded in the editor before switching.
		editor_->load(view_->path());
		stack_->setCurrentWidget(editor_);
	}
	else {
		// Show the view widget.
		// Save the editor contents and reload the file in the view widget.
		editor_->save();
		view_->reload();
		stack_->setCurrentWidget(view_);
	}
	
	emit modeChanged(editMode);
}

/**
 * Loads a file in the browser.
 * @param  path  The file path to load
 */
void WikiWidget::load(const QString& path)
{
	qDebug() << "load" << path;
	stack_->setCurrentWidget(view_);
	view_->setSource(path);
}

/**
 * Saves the contents of the editor.
 */
void WikiWidget::save()
{
	editor_->save();
}

/**
 * Shows a dialogue for searching text in current view (browser or editor).
 * This slot handles the "Edit->Find" action.
 */
void WikiWidget::findText()
{
	FindDialog dlg;
	
	connect(&dlg, SIGNAL(find(const QString&, QTextDocument::FindFlags)),
		    this, SLOT(findText(const QString&, QTextDocument::FindFlags)));
	
	dlg.exec();
}

/**
 * Searches for text in the current view (browser or editor).
 * This slot is connected to the find() signal of the search dialogue.
 * @param  text  The text to search for
 * @param  flags Search options
 */
void WikiWidget::findText(const QString& text, QTextDocument::FindFlags flags)
{
	if (stack_->currentWidget() == editor_)
		editor_->find(text, flags);
	else
		view_->find(text, flags);
}
