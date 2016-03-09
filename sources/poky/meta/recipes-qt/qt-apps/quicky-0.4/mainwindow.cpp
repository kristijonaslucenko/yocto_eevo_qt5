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

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFontDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QProcess>
#include <QInputDialog>
#include <QtDebug>
#include "mainwindow.h"
#include "wikiwidget.h"
#include "stylesheetdialog.h"
#include "version.h"

/**
 * Class constructor.
 */
MainWindow::MainWindow() : QMainWindow()
{
	setWindowTitle("Quicky");
	
	// Create the central Wiki widget.
	wiki_ = new WikiWidget(this);
	setCentralWidget(wiki_);
	connect(wiki_->view(), SIGNAL(sourceChanged(const QUrl&)), this,
			SLOT(fileLoaded(const QUrl&)));
	connect(wiki_->view(), SIGNAL(externalLink(const QUrl&)), this,
			SLOT(openExternal(const QUrl&)));
	
	// Show highlighted (hovered) links in the status bar.
	connect(wiki_->view(), SIGNAL(highlighted(const QString&)), statusBar(),
	        SLOT(showMessage(const QString&)));
	
	// Show the current directory in the status bar.
	dirStatus_ = new QLabel(QDir::currentPath(), this);
	statusBar()->addPermanentWidget(dirStatus_);

	setupActions();
	readSettings();
}

/**
 * Class destructor.
 */
MainWindow::~MainWindow()
{
}

/**
 * Loads the given file into the Wiki widget.
 * @param  path  The file to open
 */
void MainWindow::openFile(const QString& path)
{
	wiki_->load(path);
}

/**
 * Loads the main help file into the Wiki widget.
 * This slot is connected to the "Help->Manual" action. It is also called if
 * no file is initially opened (via the command line), so that we never have
 * an empty file loaded.
 */
void MainWindow::openHelp()
{
	wiki_->load("help:///Welcome");
}

/**
 * Called before the main window closes.
 * Checks for unsaved changes in the Wiki widget's editor. If there are, the
 * user is prompted to save the file first.
 * @param  event  Information on the closing event
 */
void MainWindow::closeEvent(QCloseEvent* event)
{
	if (wiki_->canClose()) {
		writeSettings();
		event->accept();
	}
	else {
		event->ignore();
	}
}

/**
 * Creates the actions used to populate the menu and tool bars.
 */
void MainWindow::setupActions()
{
	QMenu* menu;
	QToolBar* toolBar;
	QAction* action;
	QActionGroup* group;

	toolBar = new QToolBar(this);
	addToolBar(toolBar);
	
	// File menu.
	menu = menuBar()->addMenu(tr("&File"));

	// New file.
	action = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	action->setShortcut(tr("Ctrl+N"));
	action->setStatusTip(tr("Create a new file"));
	connect(action, SIGNAL(triggered()), this, SLOT(newFile()));
	menu->addAction(action);
	toolBar->addAction(action);
	
	// Open file.
	action = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
	action->setShortcut(tr("Ctrl+O"));
	action->setStatusTip(tr("Open an existing file"));
	connect(action, SIGNAL(triggered()), this, SLOT(openFile()));
	menu->addAction(action);
	toolBar->addAction(action);
	
	// Save file.
	// Enabled only in editor mode, and only when there are unsaved changes.
	action = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	action->setShortcut(tr("Ctrl+S"));
	action->setStatusTip(tr("Save the current contents of the editor"));
	connect(wiki_->editor()->document(), SIGNAL(modificationChanged(bool)),
	        action, SLOT(setEnabled(bool)));
	connect(action, SIGNAL(triggered()), wiki_, SLOT(save()));
	action->setEnabled(false);
	menu->addAction(action);
	toolBar->addAction(action);
	
	// Export as HTML.
	action = new QAction(tr("&Export HTML"), this);
	action->setStatusTip(tr("Export the formatted file as HTML"));
	connect(action, SIGNAL(triggered()), wiki_->view(), SLOT(exportHtml()));
	menu->addAction(action);

	// Edit menu.
	menu = menuBar()->addMenu(tr("&Edit"));

	// Toggle View/Edit mode.
	action = new QAction(QIcon(":/images/edit.png"), tr("Edit &mode"), this);
	action->setCheckable(true);
	action->setShortcut(tr("Ctrl+E"));
	action->setStatusTip(tr("Toggle between browser and editor modes"));
	connect(action, SIGNAL(toggled(bool)), wiki_, SLOT(setMode(bool)));
	connect(wiki_->view(), SIGNAL(canEdit(bool)), action,
	        SLOT(setEnabled(bool)));
	menu->addAction(action);
	toolBar->addAction(action);

	// Find text in view/editor.
	action = new QAction(tr("&Find"), this);
	action->setShortcut(tr("Ctrl+F"));
	action->setStatusTip(tr("Find text in file"));
	connect(action, SIGNAL(triggered()), wiki_, SLOT(findText()));
	menu->addAction(action);

	// Navigate menu.
	menu = menuBar()->addMenu(tr("&Navigate"));
	
	// An action group for the previous and next actions.
	// Enabled in view mode, disabled in editor mode.
	group = new QActionGroup(this);
	connect(wiki_, SIGNAL(modeChanged(bool)), group, SLOT(setDisabled(bool)));

	// Go to previous document.
	action = new QAction(QIcon(":/images/prev.png"), tr("&Previous"), this);
	action->setShortcut(tr("Alt+Left"));
	action->setStatusTip(tr("Go to previous document"));
	connect(action, SIGNAL(triggered()), wiki_->view(), SLOT(backward()));
	connect(wiki_->view(), SIGNAL(backwardAvailable(bool)), action, 
			SLOT(setEnabled(bool)));
	action->setEnabled(false);
	menu->addAction(action);
	toolBar->addAction(action);
	group->addAction(action);

	// Go to next document.
	action = new QAction(QIcon(":/images/next.png"), tr("&Next"), this);
	action->setShortcut(tr("Alt+Right"));
	action->setStatusTip(tr("Go to next document"));
	connect(action, SIGNAL(triggered()), wiki_->view(), SLOT(forward()));
	connect(wiki_->view(), SIGNAL(forwardAvailable(bool)), action, 
			SLOT(setEnabled(bool)));
	action->setEnabled(false);
	menu->addAction(action);
	toolBar->addAction(action);
	group->addAction(action);

	// Settings menu.
	menu = menuBar()->addMenu(tr("&Settings"));

	// Change the editor font.
	action = new QAction(tr("&Editor Font"), this);
	action->setStatusTip(tr("Set the editor font"));
	connect(action, SIGNAL(triggered()), this, SLOT(chooseEditorFont()));
	menu->addAction(action);

	// Extarnal links command line.
	action = new QAction(tr("E&xternal Links"), this);
	action->setStatusTip(tr("Set command line for external links"));
	connect(action, SIGNAL(triggered()), this, SLOT(configExternal()));
	menu->addAction(action);

	// CSS Style sheet for formatting the browser.
	action = new QAction(tr("Browser &Style Sheet"), this);
	action->setStatusTip(tr("Select a style sheet for the browser"));
	connect(action, SIGNAL(triggered()), this, SLOT(configBrowserCSS()));
	menu->addAction(action);
	
	// Help menu.
	menu = menuBar()->addMenu(tr("&Help"));

	// Manual page.
	action = new QAction(tr("&Manual"), this);
	action->setShortcut(tr("F1"));
	action->setStatusTip(tr("Show the manual page"));
	connect(action, SIGNAL(triggered()), this, SLOT(openHelp()));
	menu->addAction(action);
	
	// About Quicky.
	action = new QAction(tr("&About Quicky"), this);
	action->setStatusTip(tr("Quicky version information"));
	connect(action, SIGNAL(triggered()), this, SLOT(about()));
	menu->addAction(action);
	
	// About Qt.
	action = new QAction(tr("About &Qt"), this);
	action->setStatusTip(tr("Qt version information"));
	connect(action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	menu->addAction(action);
}

/**
 * Stores configuration in the settings file.
 */
void MainWindow::writeSettings()
{
	QSettings settings;

	// Store main window position and size.
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
}

/**
 * Loads configuration from the settings file.
 */
void MainWindow::readSettings()
{
	QSettings settings;
	
	// Restore main window position and size.
	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(500, 400)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());
	settings.endGroup();
	
	// Set editor font.
	settings.beginGroup("Editor");
	wiki_->editor()->setFont(settings.value("font").value<QFont>());
	settings.endGroup();
	
	// Browser style sheet.
	settings.beginGroup("View");
	wiki_->view()->loadStyleSheet(settings.value("css").toString());
	settings.endGroup();
}

/**
 * Creates a new file.
 * Prompts the user for a file name before a file is created, as Quicky does
 * not work with unnamed files.
 * Handles the "File->New" action.
 */
void MainWindow::newFile()
{
	QString path;

	path = QFileDialog::getSaveFileName(0, tr("New File"));
	if (!path.isEmpty())
		openFile(path);
}

/**
 * Opens an existing file.
 * Prompts the user for a file name to load.
 * Handles the "File->Open" action.
 */
void MainWindow::openFile()
{
	QString path;

	path = QFileDialog::getOpenFileName(0, tr("Open File"));
	if (!path.isEmpty())
		openFile(path);
}

/**
 * Allows the user to set a new font for the editor.
 * Handles the "Settings->Editor Font" action.
 */
void MainWindow::chooseEditorFont()
{
	QFont font;
	bool ok;
	
	font = QFontDialog::getFont(&ok, wiki_->editor()->currentFont(), this);
	if (ok) {
		QSettings settings;
		
		wiki_->editor()->setCurrentFont(font);
		settings.setValue("Editor/font", font);
	}
}

/**
 * Allows the user to configure the command line for handling external links.
 */
void MainWindow::configExternal()
{
	QSettings settings;
	QString label;
	QString cmd;
	bool ok;
	
	label = tr("Enter a command to execute when clicking on an external "
	           "link\nUse %1 to indicate where the URL should be placed");
	cmd = settings.value("View/externalCmd").toString();
	cmd = QInputDialog::getText(0, "External Links Command", label,
								QLineEdit::Normal, cmd, &ok);
	if (ok)
		settings.setValue("View/externalCmd", cmd);
}

/**
 * Prompts the user for a CSS style sheet to use in the browser.
 */
void MainWindow::configBrowserCSS()
{
	QSettings settings;
	StyleSheetDialog dlg(settings.value("View/css").toString());
	
	if (dlg.exec() == QDialog::Accepted) {
		QString path = dlg.path();
		settings.setValue("View/css", path);
		wiki_->view()->loadStyleSheet(path);
		wiki_->view()->reload();
	}
}

/**
 * Launches an application for viewing an external link.
 * @param  url  The link to view
 */
void MainWindow::openExternal(const QUrl& url)
{
	QSettings settings;
	QString cmd;

	// Get the command line for viewing external links.
	cmd = settings.value("View/externalCmd").toString();
	if (cmd.isEmpty()) {
		QString msg = tr("No command was set for viewing external links");
		QMessageBox::warning(0, tr("External Link"), msg);
		return;
	}
	
	// Launch the external application.
	cmd = cmd.arg(url.toString());
	qDebug() << cmd;
	QProcess::startDetached(cmd);
}

/**
 * Displays information about this application.
 * Handles the "Help->About" action.
 */
void MainWindow::about()
{
	QString msg;
	
	msg = tr("<h1>Quicky</h1>\n"
	         "<h2>Simple Wiki-style note-taking application</h2>\n"
	         "Version %1<br>\n"
	         "Copyright (C) 2008 by Elad Lahav<br>\n"
	         "Licensed under the terms of the GPLv2 Free Software License\n")
	      .arg(appVersion.toString());
	QMessageBox::about(0, tr("About Quicky"), msg);
}

/**
 * This slot is connected to the sourceChanged() signal emitted by the 
 * browser.
 * Sets the current directory to that of the currently loaded file, and
 * adjusts the window title.
 * @param  url  The URL of the loaded file
 */
void MainWindow::fileLoaded(const QUrl& url)
{
	QFileInfo fi(url.path());
	QString title;
	
	// Set the current directory to that of the requested file.
	if (url.scheme().isEmpty() &&
	    !QDir::setCurrent(fi.absoluteDir().path())) {
		QString msg = tr("Cannot change directory to '%1'")
		              .arg(fi.absoluteDir().path());
		QMessageBox::warning(0, "Error", msg);
		return;
	}
	
	// Update the status bar.
	dirStatus_->setText(QDir::currentPath());	
	
	// Set the window title.	
	title = tr("Quicky - %1").arg(fi.fileName());
	setWindowTitle(title);
}
