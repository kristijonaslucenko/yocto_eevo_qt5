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

#include "finddialog.h"

FindDialog::FindDialog(QWidget* parent) : QDialog(parent), Ui::FindDialog()
{
	setupUi(this);
	connect(findNextButton_, SIGNAL(clicked()), this, SLOT(findNext()));
}

FindDialog::~FindDialog()
{
}

void FindDialog::accept()
{
	findNext();
	QDialog::accept();
}

void FindDialog::findNext()
{
	QString text;
	QTextDocument::FindFlags flags;
	
	text = textEdit_->text();
	if (text.isEmpty())
		return;
	
	flags = 0;
	if (backwardsCheck_->isChecked())
		flags |= QTextDocument::FindBackward;
	if (caseSensitiveCheck_->isChecked())
		flags |= QTextDocument::FindCaseSensitively;
	if (wholeWordsCheck_->isChecked())
		flags |= QTextDocument::FindWholeWords;
	
	emit find(text, flags);
}
