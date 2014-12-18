/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Library.hpp                                                                 *
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


#ifndef LIBRARY_HPP
#define LIBRARY_HPP


#include <QDirIterator>
#include <QDebug>

#include "Actions.hpp"
#include "ComicInfo.hpp"
#include "ComicFile.hpp"
#include "Config.hpp"
#include "ReferenceList.hpp"


/**
 * The Library class is a singleton object and QList containing all the comics/manga as ComicFile
 * objects, when constructed it checks if the library.xml file exists, and if not it creates it.
 * When constructed this object assumes the root config dir already exists, and therefore the Config
 * object MUST be constructed AND filled with valid values FIRST. The Library object is also
 * responsible for watching the designated Comic and/or Manga directories defined in Config, and
 * keeping a thumbnail cache of all of the covers. The main() portion of the program needs to call
 * 'Library::init()' and 'Library::destroy()'.
 */
class Library : public QObject, public QList<ComicFile> {
	Q_OBJECT

	public:
		// Attribute lists for attributes that multiple comics will share
		QList<QString> series_list;
		QList<QString> alt_series_list;
		QList<QString> writer_list;
		QList<QString> penciller_list;
		QList<QString> inker_list;
		QList<QString> colorist_list;
		QList<QString> letterer_list;
		QList<QString> cover_artist_list;
		QList<QString> editor_list;
		QList<QString> publisher_list;
		QList<QString> imprint_list;
		QList<QString> genre_list;
		QList<QString> language_iso_list;
		QList<QString> format_list;

		static void init();
		static void destroy();
		void append(const ComicFile &comic);
		void append(const QList<ComicFile> &comic_list);
		QList<ComicFile> getComicsFromPublisher(const QString &publisher) const;
		QList<ComicFile> getComicsFromSeries(const QString &series,
			const QString &publisher = 0) const;
		QList<ComicFile> getComicsFromVolume(const QString &series, const QString &volume,
			const QString &publisher = 0) const;
		ReferenceList<ComicFile> comicsFromPublisher(const QString &publisher);
		ReferenceList<ComicFile> comicsFromSeries(const QString &series,
			const QString &publisher = 0);
		ReferenceList<ComicFile> comicsFromVolume(const QString &series, const QString &volume,
			const QString &publisher = 0);
		bool contains(const QByteArray &md5_hash) const;
		bool contains(QString path) const;
		int indexOf(const QByteArray &md5_hash) const;
		int indexOf(const QString &path) const;
		void insert(int i, const ComicFile &comic);
		iterator insert(iterator before, const ComicFile &comic);
		void push_back(const ComicFile &comic);
		void push_front(const ComicFile &comic);
		int removeAll(const ComicFile &comic);
		void removeAt(int i);
		void removeFirst();
		void removeLast();
		bool removeOne(const ComicFile &comic);
		void replace(int i, const ComicFile &comic);
		void save();
		void startBatchEditing();
		void finishBatchEditing();
		using QList::at;
		const ComicFile & at(const QByteArray &md5_hash) const;
		const ComicFile & at(const QString &path) const;
		Library & operator+=(const QList<ComicFile> &comic_list);
		Library & operator+=(const ComicFile &comic);
		Library & operator<<(const QList<ComicFile> &comic_list);
		Library & operator<<(const ComicFile &comic);
		using QList::operator[];
		ComicFile & operator[](const QByteArray &md5_hash);
		const ComicFile & operator[](const QByteArray &md5_hash) const;
		ComicFile& operator[](const QString &path);
		const ComicFile & operator[](const QString &path) const;

	public slots:
		void cleanupFiles();
		void scanDirectories();

	private:
		static Library *instance;
		QFile *file;
		ComicFile null;
		bool dirty			=	false;
		bool batch_editing	=	false;

		Library();
		~Library();
		void loadFromFile();
};

extern Library *library;


#endif