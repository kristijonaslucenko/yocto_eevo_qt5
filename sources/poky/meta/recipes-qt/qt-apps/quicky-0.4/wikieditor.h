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

#ifndef WIKIEDITOR_H
#define WIKIEDITOR_H

#include <QTextEdit>

/**
 * An editor for Wiki text.
 * @author Elad Lahav
 */
class WikiEditor : public QTextEdit
{
	Q_OBJECT

public:
	WikiEditor(QWidget* parent = 0);
	~WikiEditor();
	
	void load(const QString&);
	void save();
	
	/**
	 * Accessor to the path of the file currently loaded.
	 * @return The current file path
	 */
	QString path() { return path_; }
	
private:
	/**
	 * The path of the currently loaded document.
	 */
	QString path_;
};

#endif  // WIKIEDITOR_H
