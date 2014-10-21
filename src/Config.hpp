/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Config.hpp                                                                  *
 *                                                                             *
 * Copyright 2014 Tory Gaurnier <tory.gaurnier@linuxmail.org>                  *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published by *
 * the Free Software Foundation; version 3.                                    *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QDir>
#include <QCoreApplication>

#include "Exceptions.hpp"


/**
 * The Config class is a singleton object that represents the configuration data, when constructed
 * it checks if the configuration folders exist, and if not it creates them. The config object can
 * be used by other classes to get the specific directory locations for things like the program's
 * temp directory, thumbnail directory, etc, as well as any other config data. The main() portion of
 * the program needs to call 'Config::init()' and 'Config::destroy()'.
 */
class Config : public QObject {
	public:
		static void init();
		static void destroy();
		void save();
		const QString& getBinDir() const;
		QString getIconPath() const;
		QDir getRootDir() const;
		QDir getThumbDir() const;
		QDir getTempDir() const;
		QDir getComicDir() const;
		QDir getMangaDir() const;
		bool groupByPublisher() const;
		bool isEmpty() const;
		bool isComicEnabled() const;
		bool isMangaEnabled() const;
		QString getSelectedList() const;
		void setThumbDir(const QString &path);
		void setTempDir(const QString &path);
		void setComicDir(const QString &path);
		void setMangaDir(const QString &path);
		void setComicEnabled(const bool val);
		void setMangaEnabled(const bool val);
		void setSelectedList(const QString &val);

	private:
		QFile *file;
		QString icon_path;
		QString bin_dir;
		QDir root_dir;
		QDir thumb_dir;
		QDir temp_dir;
		QDir comic_dir; // Location of comics if comic_enabled == true
		QDir manga_dir; // Location of manga if manga_enabled == true
		bool group_by_publisher; // Should root level of library be grouped by publisher?
		bool comic_enabled; // Is comic enabled? (either comic, manga, or both must be enabled)
		bool manga_enabled; // Is manga enabled?
		bool empty; // Is configuration empty/missing data? (should only be true on first run)
		QString selected_list; // Remember last selected library/list on startup

		Config();
		~Config();
		void loadFromFile();
		void setDir(QDir &dir, const QString &path); // A convenience function for setting dirs
};

extern Config *config; // Points to singleton instance


#endif