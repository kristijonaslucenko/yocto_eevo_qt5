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

#ifndef __VERSION_H
#define __VERSION_H

/**
 * Generates version information.
 * @todo Would like to automate generation of version from external source
 *       (e.g., CVS).
 * @author  Elad Lahav
 */
struct Version {
	/** Major version number. */
	int major_;
	
	/** Minor version number. */
	int minor_;
	
	/**
	 * Class constructor.
	 * @param  major  Major version number
	 * @param  minor  Minor version number
	 */
	Version(int major, int minor) : major_(major), minor_(minor) {}
	
	/**
	 * Translates the version numbers into a string.
	 * The string is of the form "MAJOR.MINOR (BUILD_DATE)"
	 * @return  The formatted version
	 */
	QString toString() const {
		return QString("%1.%2 (Built %3)").arg(major_).arg(minor_)
		       .arg(__DATE__);
	}
};

/**
 * Specifies the application version.
 */
static const Version appVersion(0, 4);

#endif // __VERSION_H
