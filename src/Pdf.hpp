/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Pdf.hpp                                                                     *
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


#ifndef PDF_HPP
#define PDF_HPP


#include <podofo/podofo.h>
#include <poppler-qt5.h>
#include <cstddef>
#include <QDebug>
#include <QImage>
#include <QFile>

#include "Exceptions.hpp"
#include "Config.hpp"


/**
 * The Pdf object uses Poppler to extract entire pages, and to render pages for viewing, PoDoFo is
 * used for everything else.
 */
class Pdf {
	public:
		Pdf() {};
		Pdf(const Pdf &pdf);
		Pdf(const QString &_path);
		~Pdf();
		int getNumOfPages() const;
		bool hasComicInfo() const;
		QString getXmlBuf() const;
		void setComicInfo(const QByteArray &raw_xmp);
		QString extractPage(const int index) const;

	private:
		QString path;
};


#endif