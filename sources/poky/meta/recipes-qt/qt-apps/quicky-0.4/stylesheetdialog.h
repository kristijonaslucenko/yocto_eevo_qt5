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

#ifndef __STYLESHEETDIALOG_H
#define __STYLESHEETDIALOG_H

#include <QDialog>
#include "ui_stylesheetdialog.h"

/**
 * A dialogue for selecting a style sheet for use with the browser.
 * @author Elad Lahav
 */
class StyleSheetDialog : public QDialog, public Ui::StyleSheetDialog
{
	Q_OBJECT

public:
	StyleSheetDialog(const QString&, QWidget* parent = 0);
	~StyleSheetDialog();
	
	QString path() const;
	
private slots:
	void browse();	
};

#endif
