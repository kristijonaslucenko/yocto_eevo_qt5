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

#include <QFileDialog>
#include "stylesheetdialog.h"

/**
 * Class constructor.
 * @param  path   The path of the current style sheet
 * @param  parent Owner widget
 */
StyleSheetDialog::StyleSheetDialog(const QString& path, QWidget* parent) :
	QDialog(parent), Ui::StyleSheetDialog()
{
	setupUi(this);
	
	connect(browseButton_, SIGNAL(clicked()), this, SLOT(browse()));
	
	// Update controls to reflect the current style sheet.
	if (path.isEmpty()) {
		defaultButton_->setChecked(true);
		
		// For some reason, this is not done automatically, despite the
		// setEnabled() slot being connected to the toggled() signal. It does
		// work when the user clicks on the radio button.
		pathEdit_->setEnabled(false);
		browseButton_->setEnabled(false);
	}
	else {
		fileButton_->setChecked(true);
		pathEdit_->setText(path);
	}
}

/**
 * Class destructor.
 */
StyleSheetDialog::~StyleSheetDialog()
{
}

/**
 * @return The path to the selected style sheet file, empty string to use the
 *         default
 */
QString StyleSheetDialog::path() const
{
	if (defaultButton_->isChecked())
		return QString();
	
	return pathEdit_->text();
}

/**
 * Displays a file dialogue for selecting a style sheet file.
 * This slot is conncected to the clicked() signal emitted by the "Browse"
 * button.
 */
void StyleSheetDialog::browse()
{
	QString path;
	
	path = QFileDialog::getOpenFileName(this, tr("Select File"));
	if (!path.isEmpty())
		pathEdit_->setText(path);
}
