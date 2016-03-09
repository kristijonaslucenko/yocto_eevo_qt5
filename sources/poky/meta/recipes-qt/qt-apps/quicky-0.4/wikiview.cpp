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
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QtDebug>
#include "wikiview.h"

/**
 * Class constructor.
 * @param  parent  The parent widget
 */
WikiView::WikiView(QWidget* parent) : QTextBrowser(parent),
	env_(None),
	histPos_(0),
	histEnabled_(true)
{
	setReadOnly(true);
	
	// Pre-formatted text.
	// A line that begins with a space is not formatted.
	// This rule has to be first on the list.
	reList_ << Rule("^[ ]", "", Pre);
	
	// Titles:
	// = My Title =
	// With an equal number of '=' on either side, determining the level.
	reList_ << Rule("^=([^=]*)=\\s*$", "<h1>\\1</h1>", Title);
	reList_ << Rule("^==([^=]*)==\\s*$", "<h2>\\1</h2>", Title);
	reList_ << Rule("^===([^=]*)===\\s*$", "<h3>\\1</h3>", Title);
	reList_ << Rule("^====([^=]*)====\\s*$", "<h4>\\1</h4>", Title);
	
	// Lists:
	// * Bullet list item
	// # Numbered list item
	// We use the first item on the capture list to determine the list level.
	reList_ << Rule("^([\\*\\#]+)(.*)$", "<li>\\2</li>", ListItem);
	
	// Links:
	// [[URL]] or [[URL|Link Text]]
	QString urlExp = "([^\\s\\(\\)\\[\\]|]+)";
	QString textExp = "([^\\]]+)";
	QString linkExp("\\[\\[" + urlExp + "\\]\\]");
	QString linkTextExp("\\[\\[" + urlExp + "\\s*\\|\\s*" + textExp
	                    + "\\]\\]");
	reList_ << Rule(linkExp, "<a href=\"\\1\">\\1</a>", Link);
	reList_ << Rule(linkTextExp, "<a href=\"\\1\">\\2</a>", Link);
	
	// Text formatting:
	// ''Italic'' or '''Bold'''.
	reList_ << Rule("'''([^']*)'''", "<b>\\1</b>", TextFormat);
	reList_ << Rule("''([^']*)''", "<i>\\1</i>", TextFormat);

	// Blank line:
	// Ends a paragraph.
	reList_ << Rule("^\\s*$", "", Blank);
}

/**
 * Class destructor.
 */
WikiView::~WikiView()
{
}

/**
 * Sets a new default style sheet for the browser.
 * The CSS style sheet is used to format the HTML contents of the browser.
 * @param  path  The path to a CSS file
 */
void WikiView::loadStyleSheet(const QString& path)
{
	QFile file(path);
	QString styleSheet;
	
	qDebug() << path;
	
	if (!file.exists())
		file.setFileName(":/default.css");
		
	// Open the CSS file for reading.
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString msg = tr("Failed to load style sheet '%1'")
		              .arg(file.fileName());
		QMessageBox::warning(0, tr("Style Sheet"), msg);
		return;
	}
	
	styleSheet = file.readAll();
	qDebug() << styleSheet;
	
	// Set the style sheet.
	document()->setDefaultStyleSheet(styleSheet);
}

/**
 * Loads a source file into the view.
 * If the file exists, it is formatted into HTML and displayed. Otherwise,
 * an empty page is shown, with only the file's name formatted as HTML
 * heading.
 * If the given URL is external, no changes are made, and the openExternal()
 * signal is emitted.
 * This function is called manually (e.g., when the user requests a file to
 * open) or automatically when the user clicks on a link.
 * @param  url  The URL of the file to open
 */
void WikiView::setSource(const QUrl& url)
{
	QString path;
	bool writable;
	
	// Check for external links.
	// These are links of the form [[scheme://path]].
	if (!url.scheme().isEmpty()) {
		// Handle internal help files.
		// These are files of the form help:///FileName, which we need to map
		// to :/help/FileName.
		// NOTE: We cannot pass :/help/FileName directly, as QUrl will omit 
		// the colon.
		if (url.scheme() == "help") {
			path = ":/help" + url.path();
		}
		else {
			// Handle external links.
			emit externalLink(url);
			return;
		}
	}
	else {
		// Handle intenal links.
		path = url.path();
	}
	
	qDebug() << "setSource:" << path;
	
	QFileInfo fi(path);
	QString html, formattedText;
	
	// Set the file name as the page's main title.
	html += QString("<h1>%1</h1>\n").arg(fi.fileName());
	
	// Try to read the file (may fail if file does not exist, but that's OK).
	QFile file(path);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Read and format each line.
		// The result is added to the HTML string.
		QTextStream strm(&file);
		while (!strm.atEnd())
			html += formatLine(strm.readLine()) + "\n";
		
		// Add an empty line, to close any open environment.
		html += formatLine(QString());
	}
	else {
		// Try to create a new file.
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QString msg = tr("Cannot create the file '%1' ('%2')")
			              .arg(path).arg(fi.absoluteFilePath());
			QMessageBox::warning(0, tr("Create Failed"), msg);
			return;
		}
	}
	
	file.close();
	
	// Formatting finished.
	// Display the resulting HTML string.
	qDebug() << html;
	setHtml(html);
	
	// NOTE:
	// For some reason, isWritable() sometimes changes from true to false
	// after we emit the sourceChanged() signal (perhaps due to the 
	// construction of another QFileInfo for the same file by a slot?).
	// To fix this problem, we store the value of isWritable() now.
	writable = fi.isWritable();
	qDebug() << "Writable:" << writable;
	
	// If the URL is a local file, store its absolute path.
	if (url.scheme().isEmpty())
		url_ = fi.absoluteFilePath();
	else
		url_ = url;
	
	addToHistory(url_);
	emit sourceChanged(url_);
	emit canEdit(writable);
}

/**
 * Reloads the current file.
 */
void WikiView::reload()
{
	// Do not change the history, as we are simply refreshing the currently
	// displayed file.
	histEnabled_ = false;
	
	// Reread and reformat the current file.
	if (url_.isValid())
		setSource(url_);
	
	histEnabled_ = true;
}

/**
 * Loads the previous document on the history list.
 * This slot handles the "Navigate->Previous" action.
 */
void WikiView::backward()
{
	// Nothing to do if at the beginning of the history list.
	if (histPos_ == 0)
		return;
	
	// Load the previous document.
	histPos_--;
	histEnabled_ = false;
	qDebug() << history_.at(histPos_);
	setSource(history_.at(histPos_));
	histEnabled_ = true;
		
	// Update the status of the "Previous" and "Next" actions.
	if (histPos_ == 0)
		emit backwardAvailable(false);
		
	emit forwardAvailable(true);
}

/**
 * Loads the next document on the history list.
 * This slot handles the "Navigate->Next" action.
 */
void WikiView::forward()
{
	// Nothing to do if at the end of the history list.
	if (histPos_ == (history_.size() - 1))
		return;
	
	// Load the next document.
	histPos_++;
	histEnabled_ = false;
	setSource(history_.at(histPos_));
	histEnabled_ = true;

	// Update the status of the "Previous" and "Next" actions.
	if (histPos_ == (history_.size() - 1))
		emit forwardAvailable(false);
		
	emit backwardAvailable(true);
}

/**
 * Writes the current contents of the view (formatted as HTML) to a file.
 * This slot handles the "File->Export HTML" action.
 */
void WikiView::exportHtml()
{
	QString path;
	
	// Prompt the user for a file to write to.
	path = QFileDialog::getSaveFileName(0, "Export as HTML");
	if (path.isEmpty())
		return;
	
	// Open the file for writing.
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QString msg = tr("Failed to open '%1' for writing").arg(path);
		QMessageBox::warning(0, "Export Failed", msg);
		return;
	}
	
	// Write the HTML contents.
	QTextStream strm(&file);
	strm << toHtml();
}

/**
 * Converts a single line of text from Wiki text to HTML.
 * The given line is matched against each rule in the rule list, and the
 * appropriate substitutions take place.
 * @param  line  A line of Wiki text
 * @return The line converted to HTML
 */
QString WikiView::formatLine(const QString& line)
{
	QString result;
	QList<Rule>::iterator itr;
	bool loop;
	Environment env;
	
	result = line;
	loop = true;
	env = Paragraph;
	
	// Apply each rule in sequence.
	for (itr = reList_.begin(); loop && itr != reList_.end(); ++itr) {
		if (!(*itr).apply(result))
			continue;

		switch ((*itr).role()) {
		case Pre:
			// Stop processing.
			env = PreBlock;
			loop = false;
			break;
			
		case Title:
			// Do not apply any other rules to titles.
			// TODO: Is this the correct behaviour?
			loop = false;
			env = None;
			break;

		case ListItem:
			// A list item.
			// Open/close list environments, as needed.
			result = adjustListLevel((*itr).captureList()[1]) + result;
			env = List;
			break;

		case Blank:
			// A blank line.
			// Resets the environment.
			env = None;
			break;
			
		default:
			;
		}
	}
	
	// Handle changes to the environment.
	// The order by which the commands inside this block are written matters!
	// We do not want to open an environment before closing one.
	if (env != env_) {
		// Open a new environment.
		switch (env) {
		case Paragraph:
			result = "<p>\n" + result;
			break;
			
		case PreBlock:
			result = "<pre>\n" + result;
			break;
			
		default:
			;
		}
		
		// Close an environment.
		switch (env_) {
		case Paragraph:
			result = "</p>\n" + result;
			break;
			
		case PreBlock:
			result = "</pre>\n" + result;
			break;
	
		case List:
			result = adjustListLevel("") + result;
			break;
			
		default:
			;
		}
		
		// Set the new environment.
		env_ = env;
	}
	
	return result;
}

/**
 * Compares a new list level to the existing one, and opens/closes environments
 * as required.
 * A list level is indicated by a string comprising '*' (bullet) and '#'
 * (number) characters. If the new string differs from the current one, than
 * the list environment changes, and the appropriate HTML tags need to be added.
 * @param
 */
QString WikiView::adjustListLevel(const QString& level)
{
	QString result;
	int minLength;
	int i, common;
	
	// Nothing to do if the new list level is the same as the current one.
	// Return an empty string.
	if (level == curListLevel_)
		return result;
	
	// Find the length of the common prefix of the two list strings.
	minLength = qMin(level.length(), curListLevel_.length());
	for (i = 0; (i < minLength) && (level.at(i) == curListLevel_.at(i)); i++)
		;
	common = i;
	
	// Close list environments that appear on the current list string, but not
	// on the new one.
	for (i = common; i < curListLevel_.length(); i++) {
		if (curListLevel_.at(i) == QChar('*'))
			result += "</ul>";
		else
			result += "</ol>";
	}
	
	// Open list environments that appear on the new list string, but not
	// on the current one.
	for (i = common; i < level.length(); i++) {
		if (level.at(i) == QChar('*'))
			result += "<ul>";
		else
			result += "<ol>";
	}
	
	// Set the new list level string.
	curListLevel_ = level;
	
	return result;
}

/**
 * Appends the given URL at the end of the history list.
 * If the current position is not at the end of the list, then all succeeding
 * items are removed.
 * @param  url  The URL to add
 */
void WikiView::addToHistory(const QUrl& url)
{
	// Do not update if history tracking is disabled.
	if (!histEnabled_)
		return;
	
	// Don't add if the new URL is the same as the current one.
	if ((histPos_ < history_.size()) && (url == history_.at(histPos_)))
		return;
	
	// Remove succeeding items.
	while (histPos_ < (history_.size() - 1))
		history_.removeLast();
		
	// Append to the history list and update the position index.
	history_.append(url);
	histPos_ = history_.size() - 1;
		
	// Update the status of the "Previous" and "Next" actions.
	emit backwardAvailable(histPos_ > 0);
	emit forwardAvailable(false);
}
