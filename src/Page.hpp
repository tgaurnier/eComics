/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Page.hpp                                                               *
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


#ifndef PAGE_HPP
#define PAGE_HPP


#include <QList>

#include "MetadataTag.hpp"


/**
 * This class contains metadata about a single page, each metadata is stored as a QString in this
 * object, but they have specific types they represent.
 *
 * Page Type Strings:
 * -"FrontCover"
 * -"InnerCover"
 * -"Roundup"
 * -"Story"
 * -"Advertisment"
 * -"Editorial"
 * -"Letters"
 * -"Preview"
 * -"BackCover"
 * -"Other"
 * -"Deleted"
 */
class Page {
	public:
		Page() {}
		Page(const Page &page);
		Page(const QString &_image);
		QString getImage() const;
		QString getType() const;
		QString getDoublePage() const;
		QString getImageSize() const;
		QString getKey() const;
		QString getImageWidth() const;
		QString getImageHeight() const;
		void setImage(const QString &value);
		void setType(const QString &value);
		void setDoublePage(const QString &value);
		void setImageSize(const QString &value);
		void setKey(const QString &value);
		void setImageWidth(const QString &value);
		void setImageHeight(const QString &value);
		Page & operator=(const Page &page);

	private:
		MetadataTag image; // <Image> tag in xml, represents image number in comic file as an int
		MetadataTag type; // <Type>... value as 1 of 11 strings
		MetadataTag double_page; // <DoublePage>... value as a bool
		MetadataTag image_size; // <ImageSize>... value as a long
		MetadataTag key; // <Key>... value as a string
		MetadataTag image_width; // <ImageWidth>... value as an int
		MetadataTag image_height; // <ImageHeight>... value as an int
};


/**
 * PageList is just a QList of Pages.
 */
class PageList : public QList<Page> {
	public:
		int getFrontCover() const;
};


#endif