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

#ifndef __WIKIVIEW_H
#define __WIKIVIEW_H

#include <QTextBrowser>
#include <QRegExp>
#include <QList>

/**
 * An HTML-based browser that displays formatted Wiki text.
 * @author  Elad Lahav
 */
class WikiView : public QTextBrowser
{
	Q_OBJECT

public:
	WikiView(QWidget* parent = 0);
	~WikiView();

	QUrl source() const { return url_; }
	QString path() const { return url_.path(); }
	void loadStyleSheet(const QString&);
	
public slots:
	virtual void setSource(const QUrl&);
	virtual void reload();
	virtual void backward();
	virtual void forward();
	void exportHtml();

signals:
	/**
	 * Emitted whenever a new file is loaded into the widget.
	 * @param  editable  true if the file can be edited, false otherwise
	 */
	void canEdit(bool editable);
	
	/**
	 * Called when a user clicks on a link leading to a non-local file (such
	 * as an HTTP address).
	 * @param  url  The URL to open
	 */
	void externalLink(const QUrl& url);
	
private:
	/**
	 * A Wiki-to-HTML replacement rule.
	 * Holds a regular expression and a destination string. The apply()
	 * method can be used to match the expression to a given string, and
	 * perform text replacement according to the destination string.
	 * @author  Elad Lahav
	 */
	class Rule
	{
	public:
		/**
		 * Class constructor.
		 * @param  pattern  A regular expression specification
		 * @param  dest     The destination string
		 * @param  role     Identifies the semantic role of text matched by
		 *                  this rule
		 */
		Rule(const QString& pattern, const QString& dest, int role)
			: re_(pattern), dest_(dest), role_(role) {}

		/**
		 * Copy constructor.
		 * @param  other  The Rule object to copy
		 */
		Rule(const Rule& other)
			: re_(other.re_), dest_(other.dest_), role_(other.role_) {}

		/**
		 * Replaces an occurance of the regular expression in the given text
		 * with the destination string.
		 * @param  text  The text to match
		 * @return true if a match was found in the text, false otherwise
		 */
		bool apply(QString& text) {
			if (re_.indexIn(text) == -1)
				return false;
		
			text.replace(re_, dest_);
			return true;
		}

		/**
		 * Accessor for the regular expression's pattern text.
		 * @return The pattern string
		 */
		QString pattern() { return re_.pattern(); }
		
		/**
		 * Returns the list of strings captured by parentheses in the regular
		 * expression during the last invocation of apply().
		 * @return The list of captured strings
		 */
		QStringList captureList() { return re_.capturedTexts(); }
		
		/**
		 * Accessor for the role value.
		 * @return The semantic role of text matched by this rule
		 */
		int role() const { return role_; }

	private:
		/**
		 * A regular expression used to match text against this rule.
		 */
		QRegExp re_;
		
		/**
		 * Destination string.
		 * Matched text is replaced with this string (which can contain numbered
		 * place holders for captured text).
		 */
		QString dest_;
		
		/**
		 * A user-supplied value used to identify the semantic role of text
		 * matched by this rule.
		 */
		int role_;
	};
	
	/**
	 * Semantic roles supplied to the rules.
	 */
	enum {
		/** Pre-formatted block. */
		Pre,
		/** Any kind of heading. */
		Title,
		/** Simple text-formatting directives. */
		TextFormat,
		/** List item. */
		ListItem,
		/** Links to other documents. */
		Link,
		/** Blank line. */
		Blank
	};
	
	/**
	 * An ordered list of replacement rules.
	 */
	QList<Rule> reList_;
	
	/**
	 * Environment blocks.
	 */
	enum Environment {
		/** None (e.g., titles) */
		None,
		/** Paragraphs (<p>...</p>) */
		Paragraph,
		/** Lists (<ul>...</ul> or <ol>...</ol>)*/
		List,
		/** Preformatted blocks (<pre>...</pre>) */
		PreBlock
	};
		
	/**
	 * The current environment block.
	 * Changes to this variable trigger the opening or closing of blocks.
	 */
	Environment env_;
	
	/**
	 * Holds a list item string (a combination of '*' and '#') that identifies
	 * the current list level.
	 * Used for opening and closing list environments, whenever the new list
	 * level does not match the current one.
	 */
	QString curListLevel_;
	
	/**
	 * The URL of the currently-viewed document.
	 */
	QUrl url_;
	
	/**
	 * An ordered list of recently-visited document paths.
	 */
	QList<QUrl> history_;
	
	/**
	 * The current position in the history list.
	 */
	int histPos_;
	
	/**
	 * Set to false to prevent documents from being added to the history.
	 * Useful, e.g., when reloading the current document.
	 */
	bool histEnabled_;

	QString formatLine(const QString&);
	QString adjustListLevel(const QString&);
	void addToHistory(const QUrl&);
};

#endif // __WIKIVIEW_H
