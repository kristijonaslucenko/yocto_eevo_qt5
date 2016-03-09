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

#ifndef WIKIWIDGET_H
#define WIKIWIDGET_H

#include <QWidget>
#include "wikiview.h"
#include "wikieditor.h"
#include "ui_wikiwidget.h"

/**
 * A dual-view widget for browsing and editing Wiki text.
 * The widget serves as host to two widgets: a WikiView for browsing formatted
 * Wiki text, and a WikiEdit for modifying such text.
 * @author Elad Lahav
 */
class WikiWidget : public QWidget, private Ui::WikiWidget
{
	Q_OBJECT

public:
	WikiWidget(QWidget* parent = 0);
	~WikiWidget();
	
	/**
	 * Accessor to the view widget.
	 * @return A pointer to the view widget
	 */
	WikiView* view() const { return view_; }
	
	/**
	 * Accessor to the editor widget.
	 * @return A poiner to the editor widget
	 */
	WikiEditor* editor() const { return editor_; }
	
	bool canClose();
	
public slots:
	void setMode(bool);
	void load(const QString&);
	void save();
	void findText();
	void findText(const QString&, QTextDocument::FindFlags);
	
signals:
	void modeChanged(bool isEdit);
};

#endif  // WIKIWIDGET_H
