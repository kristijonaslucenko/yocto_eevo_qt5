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

#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QLabel>

class WikiWidget;

/**
 * The main window.
 * Implements a single-document interface, built around a WikiWidget object.
 * @author  Elad Lahav
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

public slots:
	void openFile(const QString&);
	void openHelp();
	
protected:
	virtual void closeEvent(QCloseEvent*);
	
private:
	/**
	 * The central widget.
	 */
	WikiWidget* wiki_;
	
	/**
	 * A status-bar label that shows the current directory.
	 */
	QLabel* dirStatus_;
	
	void setupActions();
	void writeSettings();
	void readSettings();
	
private slots:
	void newFile();
	void openFile();
	void chooseEditorFont();
	void configExternal();
	void configBrowserCSS();
	void openExternal(const QUrl&);
	void about();
	void fileLoaded(const QUrl&);
};

#endif // __MAINWINDOW_H
