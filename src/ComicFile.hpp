/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ComicFile.hpp                                                               *
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


#ifndef COMICFILE_HPP
#define COMICFILE_HPP


#include <QFile>

#include "Exceptions.hpp"
#include "ComicInfo.hpp"
#include "Archive.hpp"
#include "Config.hpp"
#include "MetadataTag.hpp"
#include "Pdf.hpp"


class QByteArray;


/**
 * class ComicFile
 *
 * Represents a comic file on disk with conveniece methods for dealing with file.
 */
class ComicFile : public QFile {
	Q_OBJECT

	public:
		ComicInfo info;

		ComicFile();
		ComicFile(const ComicFile &comic);
		ComicFile(const QString &path, const ComicInfo &info, const QByteArray &md5_hash);
		ComicFile(const QString &_path);
		~ComicFile();
		void extractPage(const int image, const QString &path, const QString &file_name,
				const int size = 0) const;
		QString getExtString() const;
		QString getSizeString();
		QString getNumOfPagesString() const;
		QString getExt() const;
		qint64 getSize();
		int getNumOfPages() const;
		QString getFileName() const;
		QString getPath() const;
		QString getThumbName(const ComicInfo &comic_info = 0) const;
		QString getThumbPath(const ComicInfo &comic_info = 0) const;
		QByteArray getMd5Hash() const;
		bool isNull() const;
		void move();
		void save();
		void startEditing();
		void finishEditing();
		ComicFile & operator =(const ComicFile &comic);
		bool operator ==(const ComicFile &comic);

	signals:
		void addedToLibrary();

	private:
		// With TYPE_UNSUPPORTED, ComicType can act like a bool
		enum ComicType {
			TYPE_UNSUPPORTED,
			TYPE_ARCHIVE,
			TYPE_PDF
		} type;

		ComicInfo original_info; // Used to keep track of when info is changed
		QByteArray md5_hash;
		QString ext;
		QString ns_uri; // Namespace when type is TYPE_PDF, stays blank when TYPE_ARCHIVE
		Archive *archive	=	nullptr; // Object for managing TYPE_ARCHIVE (zip, 7z, rar)
		Pdf *pdf			=	nullptr; // Object for managing TYPE_PDF
		bool dirty			=	false;
		bool editing		=	false;
		bool in_library;

		void connectSignals();
		QString getXmlBuf() const;
		void populateComicInfo();
		void parseAttributeLists();
		void loadComicInfo();
		bool hasComicInfo() const; // Checks for embedded ComicInfo xml
		void parseFilenameForInfo();
		void processError(QProcess::ProcessError error);
		void initMd5Hash();
		bool initFileType();
		void verifyThumb();

	private slots:
		void onInfoChanged(const MetadataTag &tag, const QString old_value);
		void onAddedToLibrary();
};

#endif