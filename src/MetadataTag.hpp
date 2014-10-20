/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MetadataTag.hpp                                                               *
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


#ifndef METADATATAG_HPP
#define METADATATAG_HPP


#include <QString>
#include <QObject>


/**
 * For this class to be used, it must be constructed with a tag name, once created it will be
 * impossible to edit tag name. If constructed without a tag name, then the MetadataTag will be
 * null. A tag name should never need to be changed, you can create a new MetadataTag with a new
 * name, and copy the value over if need be. Values in metadata tags are ALWAYS stored as QStrings,
 * even if it is a number, this is to simplify reading and writing XML. QString makes it VERY easy
 * to convert value to number, just use value.toLong(), value.toInt(), etc. If type should be bool,
 * it's just as easy to say if(value == "true").
 */
class MetadataTag : public QObject {
	Q_OBJECT

	public:
		MetadataTag() {};
		MetadataTag(const MetadataTag &tag);
		MetadataTag(const QString &_name, QObject *parent = 0);
		MetadataTag(const QString &_name, const QString &_value, QObject *parent = 0);
		QString getName() const;
		QString getValue() const;
		void setValue(const QString &_value);
		bool isNull() const;
		MetadataTag & operator=(const MetadataTag &tag);

	signals:
		void changed(const MetadataTag &tag, const QString old_value);

	private:
		QString name;
		QString value;
};


#endif