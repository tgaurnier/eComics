/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Exceptions.hpp                                                              *
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


#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP


#include <typeinfo>

#include <QXmlStreamReader>
#include <QDebug>


namespace eComics {
	enum ExceptionType {
		LOGIC_ERROR,
		FILE_ERROR,
		PROCESS_ERROR,
		DIR_ERROR,
		PDF_ERROR,
		XML_READ_ERROR,
		XML_WRITE_ERROR
	};

	/**
	 * This is a generic Exception class, the type of exception is stored in the enumerator
	 * ExceptionType type, this is to reduce the ammount of repetitive code, and simplify usage.
	 * Stores internal strings as QByteArrays rather than QStrings so that what() can simply return
	 * data(), if we use QStrings with toLocal8Bit().data(), then it falls out of scope printing
	 * junk data.
	 */
	class Exception : public std::exception {
		public:
			Exception() {}
			Exception(ExceptionType t, const QString &func, const QString &msg,
					const void *const obj = nullptr) {
				type = t;
				function = func.toLocal8Bit();

				switch(type) {
					case LOGIC_ERROR:
					case FILE_ERROR:
					case PROCESS_ERROR:
					case DIR_ERROR:
					case PDF_ERROR:
						err_msg = msg.toLocal8Bit();
						break;

					// This case goes in braces to stop cross-initialization error
					case XML_READ_ERROR: {
						const QXmlStreamReader *const reader =
								static_cast<const QXmlStreamReader *const>(obj);
						switch(reader->error()) {
							case QXmlStreamReader::NoError:
								err_msg = QByteArray("Unknown error in ") + msg.toLocal8Bit();

							case QXmlStreamReader::CustomError:
								err_msg = reader->errorString().toLocal8Bit() + " in " +
									msg.toLocal8Bit();

							case QXmlStreamReader::NotWellFormedError:
								err_msg = QByteArray("Badly formed xml in ") + msg.toLocal8Bit();

							case QXmlStreamReader::PrematureEndOfDocumentError:
								err_msg = QByteArray("Premature end of document in ") +
									msg.toLocal8Bit();

							case QXmlStreamReader::UnexpectedElementError:
								err_msg = QByteArray("Unexpected element in ") + msg.toLocal8Bit();
						}

						break;
					}

					case XML_WRITE_ERROR:
						err_msg = QByteArray("Failed to write xml to ") + msg.toLocal8Bit();
						break;
				}
			}
			~Exception() throw() {}
			const char* what() const throw() { return this->err_msg.data(); }
			QString getFunction() const { return QString(function); }
			ExceptionType getType() const { return type; }
			void printMsg() const {
				qDebug() << name[type] << "in" << function << ":\n" << what();
			}

		private:
			QByteArray err_msg;
			QByteArray function;
			ExceptionType type;
			const char *name[7] = {
				"Logic Error",
				"File Error",
				"Process Error",
				"Dir Error",
				"Pdf Error",
				"XML Read Error",
				"XML Write Error"
			};
	};
}


#endif