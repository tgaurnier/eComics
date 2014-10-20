/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FileTypeList.hpp                                                            *
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


#ifndef FILETYPELIST_HPP
#define FILETYPELIST_HPP


#include <QStringList>
#include <QDebug>


/**
 * This is a convenience object, it stores a list of file extensions that are "supported file
 * types", and can be used for checking if a filename is a "supported type".
 */
class FileTypeList : QStringList {
	public:
		FileTypeList() {}
		FileTypeList(const QStringList &list) : QStringList(list) {}
		bool isSupported(const QString &file_name) const;
};


#endif