/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Archive.hpp                                                                 *
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


#ifndef ARCHIVE_HPP
#define ARCHIVE_HPP

#include <exception>
#include <QProcess>
#include <QDebug>

#include "FileTypeList.hpp"
#include "Exceptions.hpp"
#include "Config.hpp"


class Archive : public QObject {
	Q_OBJECT

	public:
		Archive() {};
		Archive(const Archive &archive);
		Archive(const QString &_path);
		~Archive();
		int getNumOfPages() const;
		bool hasComicInfo() const;
		QString getXmlBuf() const;
		void setComicInfo(const QByteArray &raw_xml);
		QString extractPage(const int index) const;

	private:
		QProcess *process;
		QStringList file_list;
		QString path;
		QString type;
		FileTypeList supported_image_types; // Stores list of supported image file types

		void run(const QString &program, const QStringList &args) const;

	private slots:
		void processError(QProcess::ProcessError error);
		void outputStdError();
};


#endif