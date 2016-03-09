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

#ifndef __FINDDIALOG_H
#define __FINDDIALOG_H

#include <QDialog>
#include <QTextDocument>
#include "ui_finddialog.h"

/**
 * A simple text-search dialogue.
 * @author Elad Lahav
 */
class FindDialog : public QDialog, private Ui::FindDialog
{
	Q_OBJECT

public:
	FindDialog(QWidget* parent = 0);
	~FindDialog();

signals:
	/**
	 * Emitted when the user clicks either the "OK" or the "Find Next" buttons.
	 * @param  text  The text to find
	 * @param  flags Text search options
	 */
	void find(const QString& text, QTextDocument::FindFlags flags);
	
protected slots:
	virtual void accept();
	
private slots:
	void findNext();
};

#endif  // FINDDIALOG_H
