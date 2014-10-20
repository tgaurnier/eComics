/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ComicInfo.hpp                                                               *
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
 *                                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                             *
 * This program follows a metadata standard set by ComicRack. ComicRack is the *
 * single best comic book manager/viewer application on Windows.               *
 *                                                                             *
 * For more info: http://comicrack.cyolito.com/                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef COMICINFO_HPP
#define COMICINFO_HPP


#include "MetadataTag.hpp"
#include "Page.hpp"


/**
 * ComicInfo contains MetadataTags which can be iterated through with [] operator (like any list or
 * array), and a QList of Pages. An out of bounds [index] will return a metadata tag that
 * isNull().
 */
class ComicInfo : public QObject {
	Q_OBJECT

	public:
		PageList page_list; // To avoid being overcomplex, we need direct access to page_list

		ComicInfo(QObject *parent = 0);
		ComicInfo(const ComicInfo &info);
		void clear();
		QString getTitle() const;
		QString getSeries() const;
		QString getNumber() const;
		QString getCount() const;
		QString getVolume() const;
		QString getAltSeries() const;
		QString getAltNumber() const;
		QString getAltCount() const;
		QString getSummary() const;
		QString getNotes() const;
		QString getYear() const;
		QString getMonth() const;
		QString getWriter() const;
		QString getPenciller() const;
		QString getInker() const;
		QString getColorist() const;
		QString getLetterer() const;
		QString getCoverArtist() const;
		QString getEditor() const;
		QString getPublisher() const;
		QString getImprint() const;
		QString getGenre() const;
		QString getWeb() const;
		QString getPageCount() const;
		QString getLanguageIso() const;
		QString getFormat() const;
		QString getBlackAndWhite() const;
		QString getManga() const;
		void setTitle(const QString &value);
		void setSeries(const QString &value);
		void setNumber(const QString &value);
		void setCount(const QString &value);
		void setVolume(const QString &value);
		void setAltSeries(const QString &value);
		void setAltNumber(const QString &value);
		void setAltCount(const QString &value);
		void setSummary(const QString &value);
		void setNotes(const QString &value);
		void setYear(const QString &value);
		void setMonth(const QString &value);
		void setWriter(const QString &value);
		void setPenciller(const QString &value);
		void setInker(const QString &value);
		void setColorist(const QString &value);
		void setLetterer(const QString &value);
		void setCoverArtist(const QString &value);
		void setEditor(const QString &value);
		void setPublisher(const QString &value);
		void setImprint(const QString &value);
		void setGenre(const QString &value);
		void setWeb(const QString &value);
		void setPageCount(const QString &value);
		void setLanguageIso(const QString &value);
		void setFormat(const QString &value);
		void setBlackAndWhite(const QString &value);
		void setManga(const QString &value);
		bool isNull() const;
		int size() const;
		const MetadataTag& at(const int i) const;
		const MetadataTag& at(const QString &name) const;
		MetadataTag& operator [](const int i);
		MetadataTag& operator [](const QString& name);
		bool operator ==(const ComicInfo &info) const;
		ComicInfo & operator =(const ComicInfo &info);

	signals:
		void tagChanged(const MetadataTag &tag, const QString &old_value);

	private:
		MetadataTag title;
		MetadataTag series;
		MetadataTag number;
		MetadataTag count;
		MetadataTag volume;
		MetadataTag alt_series;
		MetadataTag alt_number;
		MetadataTag alt_count;
		MetadataTag summary;
		MetadataTag notes;
		MetadataTag year;
		MetadataTag month;
		MetadataTag writer;
		MetadataTag penciller;
		MetadataTag inker;
		MetadataTag colorist;
		MetadataTag letterer;
		MetadataTag cover_artist;
		MetadataTag editor;
		MetadataTag publisher;
		MetadataTag imprint;
		MetadataTag genre;
		MetadataTag web;
		MetadataTag page_count;
		MetadataTag language_iso;
		MetadataTag format;
		MetadataTag black_and_white;
		MetadataTag manga;
		MetadataTag null;

		void connectSignals();
};


#endif