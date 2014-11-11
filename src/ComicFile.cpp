#include "ComicFile.hpp"

#include <cmath>
#include <QDate>
#include <QByteArray>
#include <QCryptographicHash>
#include <QXmlStreamReader>

#include "Library.hpp"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									COMICFILE PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Initialize a blank null ComicFile.
 */
ComicFile::ComicFile() {
	type = TYPE_UNSUPPORTED;
}


ComicFile::ComicFile(const ComicFile &comic) : QFile(comic.getPath()) {
	*this = comic;
}


/**
 * Initializes file with path, path must be FULL path to file. If ComicInfo.xml exists in comic,
 * then it's content is set to doc. Else creates a new ComicInfo.xml file and tries to parse file
 * name for metadata. If file is not a valid comic file, then it is left empty with type of
 * TYPE_UNSUPPORTED.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if file at path doesn't exist.
 * - PROCESS_ERROR may be thrown if shell command in Archive fails for any reason.
 */
ComicFile::ComicFile(const QString &path) : QFile(path) {
	if(!initFileType()) return;

	try { initMd5Hash(); } catch(const eComics::Exception &e) { e.printMsg(); }

	// Check if current comic is already in library using path
	if(library->contains(path)) {
		in_library = true;

		// If md5 hash matches then just copy metadata, otherwise parse it from file
		const ComicFile &comic = library->at(path);
		if(getMd5Hash() == comic.getMd5Hash()) {
			info = comic.info;
		} else populateComicInfo();

		// Make sure that appropriate attributes are in one of the lists
		parseAttributeLists();
	} else {
		// If not in library just parse file for info
		in_library = false;
		populateComicInfo();
	}

	info.setParent(this);

	// Make sure thumbnail exists
	verifyThumb();

	connectSignals();
}


/**
 * Initialize a ComicFile with ComicInfo info if md5_hash matches file, otherwise just get info from
 * file itself.
 */
ComicFile::ComicFile(const QString &path, const ComicInfo &_info, const QByteArray &md5_hash)
		: QFile(path) {
	if(!initFileType()) return;
	try { initMd5Hash(); } catch(const eComics::Exception &e) { e.printMsg(); }

	// Get comic info
	if(this->md5_hash == md5_hash) info = _info;
	else populateComicInfo();

	info.setParent(this);

	if(library->contains(path)) {
		in_library = true;

		// Make sure that appropriate attributes are in one of the lists
		parseAttributeLists();
	} else in_library = false;

	// Make sure thumbnail exists
	verifyThumb();

	connectSignals();
}


ComicFile::~ComicFile() {
	if(archive != nullptr) {
		delete archive;
	}

	if(pdf != nullptr) {
		delete pdf;
	}
}


/**
 * This is a convenience method, moves file to either comics or manga directory, if
 * config->manageFiles() is true then also renames file as appropriate.
 */
void ComicFile::move() {
	QDir dir("/");
	QString new_dir_path;
	QString new_file_name;

	// Dir path for comics
	if((config->isComicEnabled() && !config->isMangaEnabled()) ||
			(config->isComicEnabled() && info.getManga() == "No")) {
		new_dir_path = config->getComicDir().absolutePath() + "/";
	}

	// Dir path for manga
	else if((config->isMangaEnabled() && !config->isComicEnabled()) ||
			(config->isMangaEnabled() && info.getManga() == "Yes")) {

	}

	// If managing files, add directories as appropriate, and rename file
	if(config->manageFiles()) {
		// comic|manga_dir/publisher/series/volume/filename.ext
		new_dir_path	+=	info.getPublisher() + "/" + info.getSeries() + "/" +
							((info.getVolume().isEmpty()) ? "Unknown Volume" : info.getVolume()) +
							"/";
		new_file_name	=	info.getSeries();
		if(!info.getVolume().isEmpty()) new_file_name += QString(" Vol. ") + info.getVolume();
		if(!info.getNumber().isEmpty()) new_file_name += QString(" No. ") + info.getNumber();
		new_file_name += QString(" - ") + info.getTitle();
		new_file_name += getPath().mid(getPath().lastIndexOf('.'));
	} else {
		// Else keep in root of comic|manga_dir, and don't rename file
		new_file_name = getPath().mid(getPath().lastIndexOf("/") + 1);
	}

	// Ensure new path exists, then move file
	dir.mkpath(new_dir_path);
	dir.rename(getPath(), new_dir_path + new_file_name);
	setFileName(new_dir_path);
}


/**
 * Extract image with index to path with name file_name, detects filetype from the extension in
 * file_name, and converts image if necessary. Resizes width and height to size, preserving aspect
 * ratio, if size is 0 (default), then it is not resized.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if ComicFile is an unsupported type, or if page at index doesn't
 * exist.
 * - FILE_ERROR may be thrown if saving extracted pdf page to file fails, or if opening, saving, or
 * removing the temp image file fails.
 * - PROCESS_ERROR may be thrown if shell command for Archive fails in any way.
 */
void ComicFile::extractPage(const int index, const QString &path, const QString &file_name,
		const int size) const {
	QString temp_image_path;

	switch(type) {
		case TYPE_ARCHIVE:
			// This may throw a LOGIC_ERROR() or PROCESS_ERROR()
			temp_image_path = archive->extractPage(index);
			break;

		case TYPE_PDF:
			// This may throw a few exceptions
			temp_image_path = pdf->extractPage(index);
			break;

		case TYPE_UNSUPPORTED:
		default:
			throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile::extractPage()",
					this->fileName() + " is an unsupported type");
	}

	// Convert image if necessary, and move to path
	QImage image = (size) ? QImage(temp_image_path).scaled(size, size, Qt::KeepAspectRatio) :
			QImage(temp_image_path);
	if(image.isNull()) {
		throw eComics::Exception(eComics::FILE_ERROR, "ComicFile::extractPage()",
				QString("Failed to open ") + temp_image_path);
	}

	if(!image.save(path + "/" + file_name)) {
		throw eComics::Exception(eComics::FILE_ERROR, "ComicFile::extractPage()",
				QString("Failed to save ") + path + "/" + file_name);
	}

	// Delete temp image
	if(!QFile::remove(temp_image_path)) {
		throw eComics::Exception(eComics::FILE_ERROR, "ComicFile::extractPage()",
				QString("Failed to remove ") + temp_image_path);
	}
}


QString ComicFile::getExtString() const {
	if(ext == "zip" || ext == "cbz") return "Comic Book Archive (ZIP/cbz)";
	else if(ext == "7z" || ext == "cb7") return "Comic Book Archive (7z/cb7)";
	else if(ext == "rar" || ext == "cbr") return "Comic Book Archive (RAR/cbr)";
	else if(ext == "pdf") return "Portable Document Format (PDF)";
	else return ext;
}


QString ComicFile::getSizeString() {
	try {
		qint64 file_size = getSize();

		// Support sizes up to 1023 yobibytes because fuckit
		if(file_size < 1024) {
			return QString::number(file_size) + " Bytes";
		} else if(file_size >= 1024 && file_size < pow(1024, 2)) {
			return QString::number(file_size / 1024) + " KiB";
		} else if(file_size >= pow(1024, 2) && file_size < pow(1024, 3)) {
			return QString::number(file_size / pow(1024, 2)) + " MiB";
		} else if(file_size >= pow(1024, 3) && file_size < pow(1024, 4)) {
			return QString::number(file_size / pow(1024, 3)) + " GiB";
		} else if(file_size >= pow(1024, 4) && file_size < pow(1024, 5)) {
			return QString::number(file_size / pow(1024, 4)) + " TiB";
		} else if(file_size >= pow(1024, 5) && file_size < pow(1024, 6)) {
			return QString::number(file_size / pow(1024, 5)) + " PiB";
		} else if(file_size >= pow(1024, 6) && file_size < pow(1024, 7)) {
			return QString::number(file_size / pow(1024, 6)) + " EiB";
		} else if(file_size >= pow(1024, 7) && file_size < pow(1024, 8)) {
			return QString::number(file_size / pow(1024, 7)) + " ZiB";
		} else if(file_size >= pow(1024, 8) && file_size < pow(1024, 9)) {
			return QString::number(file_size / pow(1024, 8)) + " YiB";
		} else if(file_size >= pow(1024, 9)) {
			return "1023 YiB+";
		} else return QString::number(file_size);
	} catch(const eComics::Exception &e) {
		e.printMsg();
		return "Error: can't open file to get size";
	}
}


QString ComicFile::ComicFile::getNumOfPagesString() const {
	return QString::number(getNumOfPages()) + ((getNumOfPages() == 1) ? " Page" : " Pages");
}


QString ComicFile::getExt() const { return ext; }


/**
 * Returns the size of comic file in bytes.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if file fails to open.
 */
qint64 ComicFile::getSize() {
	if(!open(QIODevice::ReadOnly)) {
		throw eComics::Exception(eComics::FILE_ERROR, "ComicFile::getSize()",
				QString("Failed to open ") + this->fileName() + " for reading");
	}

	qint64 file_size = size();
	close();
	return file_size;
}


/**
 * Returns number of pages.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if ComicFile is an unsupported type.
 * - PROCESS_ERROR may be thrown if shell command fails in Archive.
 */
int ComicFile::getNumOfPages() const {
	// No need for breaks since each case returns
	switch(type) {
		case TYPE_ARCHIVE:
			return archive->getNumOfPages();

		case TYPE_PDF:
			return pdf->getNumOfPages();

		case TYPE_UNSUPPORTED:
		default:
			throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile::getNumOfPages()",
					this->fileName() + " is an unsupported type");
	}
}


QString ComicFile::getFileName() const { return getPath().mid(getPath().lastIndexOf('/') + 1); }
QString ComicFile::getPath() const { return fileName(); }
QByteArray ComicFile::getMd5Hash() const { return md5_hash; }


/**
 * Generate name of thumbnail based on metadata.
 */
QString ComicFile::getThumbName(const ComicInfo &_info) const {
	QString thumb_file_name;

	// If no ComicInfo object passed as argument, use internal info to generate thumb path
	if(_info.isNull()) {
		thumb_file_name	=	info.getSeries() + "-" + info.getVolume() + "-" +
							info.getNumber() + "-" + info.getTitle() + ".jpg";
	} else {
		thumb_file_name	=	_info.getSeries() + "-" + _info.getVolume() + "-" +
							_info.getNumber() + "-" + _info.getTitle() + ".jpg";
	}

	return thumb_file_name;
}


/**
 * This is a convenience function that returns full path to thumbnail (including it's file name).
 */
QString ComicFile::getThumbPath(const ComicInfo &_info) const {
	return config->getThumbDir().absolutePath() + "/" + getThumbName(_info);
}


bool ComicFile::isNull() const {
	return !type;
}


/**
 * Saves ComicInfo to xml in comic file.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if ComicFile is an unsupported type.
 * - FILE_ERROR may be thrown from Archive::setComicInfo().
 * - PROCESS_ERROR may be thrown if shell command from Archive fails in any way.
 */
void ComicFile::save() {
	// Prepare buff to be written
	QByteArray xml_buff;

	// Prepare xml writer and start document
	QXmlStreamWriter writer(&xml_buff);
	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1);
	writer.writeStartDocument("1.0");

	// If PDF make sure to create namespace
	if(type == TYPE_PDF) {
		writer.writeNamespace(ns_uri, "ci");
	}

	writer.writeStartElement(ns_uri, "ComicInfo");

	// Create an element for each meta field that was filled
	for(int i = 0; i < info.size(); i++) {
		if(!info[i].getValue().isEmpty()) {
			writer.writeTextElement(ns_uri, info[i].getName(), info[i].getValue());
		}
	}

	// Create Pages element, and fill with a Page for each image in ComicFile
	writer.writeStartElement(ns_uri, "Pages");
	for(int i = 0; i < info.page_list.size(); i++) {
		writer.writeEmptyElement(ns_uri, "Page");
		// Image is a required field, so it will always be filled
		writer.writeAttribute(ns_uri, "Image", info.page_list[i].getImage());

		// For the rest, if they are empty don't add them
		if(!info.page_list[i].getType().isEmpty()) {
			writer.writeAttribute(ns_uri, "Type", info.page_list[i].getType());
		}

		if(!info.page_list[i].getDoublePage().isEmpty()) {
			writer.writeAttribute(ns_uri, "DoublePage", info.page_list[i].getDoublePage());
		}

		if(!info.page_list[i].getImageSize().isEmpty()) {
			writer.writeAttribute(ns_uri, "ImageSize", info.page_list[i].getImageSize());
		}

		if(!info.page_list[i].getKey().isEmpty()) {
			writer.writeAttribute(ns_uri, "Key", info.page_list[i].getKey());
		}

		if(!info.page_list[i].getImageWidth().isEmpty()) {
			writer.writeAttribute(ns_uri, "ImageWidth", info.page_list[i].getImageWidth());
		}

		if(!info.page_list[i].getImageHeight().isEmpty()) {
			writer.writeAttribute(ns_uri, "ImageHeight", info.page_list[i].getImageHeight());
		}
	}

	writer.writeEndElement(); // </Pages>

	writer.writeEndElement(); // </ComicInfo>
	writer.writeEndDocument();

	switch(type) {
		case TYPE_ARCHIVE:
			archive->setComicInfo(xml_buff);
			break;

		case TYPE_PDF:
			pdf->setComicInfo(xml_buff);
			break;

		case TYPE_UNSUPPORTED:
		default:
			throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile::save()", this->fileName() +
					" is an unsupported type");
	}

	// Update MD5 Hash
	initMd5Hash();
	dirty = false;
}


void ComicFile::startEditing() {
	editing = true;
	original_info = info;
}


void ComicFile::finishEditing() {
	if(dirty) {
		// Make sure thumb is updated properly
		verifyThumb();

		// Save
		save();
		library->save();
	}

	editing = false;
	original_info.clear();
}


ComicFile & ComicFile::operator =(const ComicFile &comic) {
	if(getPath().isEmpty()) QFile(comic.getPath());

	info		=	comic.info;
	type		=	comic.type;
	ext			=	comic.ext;
	md5_hash	=	comic.md5_hash;
	ns_uri		=	comic.ns_uri;
	dirty		=	comic.dirty;
	if(comic.pdf != nullptr) pdf = new Pdf(*comic.pdf);
	if(comic.archive != nullptr) archive = new Archive(*comic.archive);
	connectSignals();

	return *this;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									COMICFILE PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicFile::connectSignals() {
	connect(&info, SIGNAL(tagChanged(const MetadataTag &, const QString)), this,
		SLOT(onInfoChanged(const MetadataTag &, const QString)));
	connect(this, SIGNAL(addedToLibrary()), this, SLOT(onAddedToLibrary()));
}


/**
 * Gets md5 hash from comic file.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if file fails to open, or if file fails to be read.
 */
void ComicFile::initMd5Hash() {
	if(!this->open(QIODevice::ReadOnly)) {
		throw eComics::Exception(eComics::FILE_ERROR, "ComicFile::initMd5Hash()",
				QString("Failed to open ") + this->fileName() + " for reading");
	}

	QCryptographicHash hash(QCryptographicHash::Md5);

	if(!hash.addData(this)) {
		this->close();
		throw eComics::Exception(eComics::FILE_ERROR, "ComicFile::initMd5Hash()",
				QString("Failed to read ") + this->fileName());
	}

	md5_hash = hash.result().toHex();
	this->close();
}


/**
 * Detects file type by extension, returns true if supported file type, not not then sets type to
 * TYPE_UNSUPPORTED and returns false.
 */
bool ComicFile::initFileType() {
	// First make sure path is valid
	if(!this->exists() || QFileInfo(*this).isDir()) {
		// File doesn't exist, orubt nessage abd return
		qDebug() << getPath() + " doesn't exist or isn't supported, not constructing ComicFile";
		type = TYPE_UNSUPPORTED;
		return false;
	}

	// Get file extension
	if(getPath().endsWith("zip", Qt::CaseInsensitive)) ext	=	"zip";
	if(getPath().endsWith("cbz", Qt::CaseInsensitive)) ext	=	"cbz";
	if(getPath().endsWith("7z", Qt::CaseInsensitive)) ext	=	"7z";
	if(getPath().endsWith("cb7", Qt::CaseInsensitive)) ext	=	"cb7";
	if(getPath().endsWith("rar", Qt::CaseInsensitive)) ext	=	"rar";
	if(getPath().endsWith("cbr", Qt::CaseInsensitive)) ext	=	"cbr";
	if(getPath().endsWith("pdf", Qt::CaseInsensitive)) ext	=	"pdf";

	// Set file type based on file extension
	if(ext == "zip" || ext == "cbz" || ext == "7z" || ext == "cb7" || ext == "rar" ||
			ext == "cbr") {
		type	=	TYPE_ARCHIVE;
		archive	=	new Archive(getPath());
	} else if(ext == "pdf") {
		type	=	TYPE_PDF;
		pdf		=	new Pdf(getPath());
		ns_uri	=	QString("http://comicrack.cyolito.com/downloads/ComicRack/Support-Files/")
				+	QString("ComicInfoSchema.zip");
	}

	// Filetype not recognized
	else {
		type = TYPE_UNSUPPORTED;
		return false;
	}

	return true;
}


/**
 * Gets xml from file.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if ComicFile is an unsupported type.
 * - PROCESS_ERROR may be thrown if shell command in Archive fails.
 */
QString ComicFile::getXmlBuf() const {
	switch(type) {
		case TYPE_ARCHIVE:
			return archive->getXmlBuf();

		case TYPE_PDF:
			return pdf->getXmlBuf();

		case TYPE_UNSUPPORTED:
		default:
			throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile::getXmlBuf()",
					this->fileName() + " is an unsupported type");
	}
}


/**
 * Checks comic file for ComicInfo.xml, if it exists returns true, otherwise returns false.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if ComicFile is an unsupported type.
 */
bool ComicFile::hasComicInfo() const {
	// No need for breaks since each case returns true or false
	switch(type) {
		case TYPE_ARCHIVE:
			return archive->hasComicInfo();

		case TYPE_PDF:
			return pdf->hasComicInfo();

		case TYPE_UNSUPPORTED:
		default:
			throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile()::hasComicInfo()",
					this->fileName() + " is an unsupported type");
	}
}


/**
 * Check each appropriate metadata tag, and make sure it's value is in an attribute list in library.
 */
void ComicFile::parseAttributeLists() {
	QString series			=	info.getSeries();
	QString alt_series		=	info.getAltSeries();
	QString writer			=	info.getWriter();
	QString penciller		=	info.getPenciller();
	QString inker			=	info.getInker();
	QString colorist		=	info.getColorist();
	QString letterer		=	info.getLetterer();
	QString cover_artist	=	info.getCoverArtist();
	QString editor			=	info.getEditor();
	QString publisher		=	info.getPublisher();
	QString imprint			=	info.getImprint();
	QString genre			=	info.getGenre();
	QString language_iso	=	info.getLanguageIso();
	QString format			=	info.getFormat();

	if(!series.isEmpty() && !library->series_list.contains(series)) {
		library->series_list.append(series);
	}

	if(!alt_series.isEmpty() && !library->alt_series_list.contains(alt_series)) {
		library->alt_series_list.append(alt_series);
	}

	if(!writer.isEmpty() && !library->writer_list.contains(writer)) {
		library->writer_list.append(writer);
	}

	if(!penciller.isEmpty() && !library->penciller_list.contains(penciller)) {
		library->penciller_list.append(penciller);
	}

	if(!inker.isEmpty() && !library->inker_list.contains(inker)) {
		library->inker_list.append(inker);
	}

	if(!colorist.isEmpty() && !library->colorist_list.contains(colorist)) {
		library->colorist_list.append(colorist);
	}

	if(!letterer.isEmpty() && !library->letterer_list.contains(letterer)) {
		library->letterer_list.append(letterer);
	}

	if(!cover_artist.isEmpty() && !library->cover_artist_list.contains(cover_artist)) {
		library->cover_artist_list.append(cover_artist);
	}

	if(!editor.isEmpty() && !library->editor_list.contains(editor)) {
		library->editor_list.append(editor);
	}

	if(!publisher.isEmpty() && !library->publisher_list.contains(publisher)) {
		library->publisher_list.append(publisher);
	}

	if(!imprint.isEmpty() && !library->imprint_list.contains(imprint)) {
		library->imprint_list.append(imprint);
	}

	// If there is more than one genre then it is a comma delimited list of genres
	for(QString string : genre.split(',', QString::SkipEmptyParts)) {
		if(!library->genre_list.contains(string.trimmed())) {
			library->genre_list.append(string.trimmed());
		}
	}

	if(!language_iso.isEmpty() && !library->language_iso_list.contains(language_iso)) {
		library->language_iso_list.append(language_iso);
	}

	if(!format.isEmpty() && !library->format_list.contains(format)) {
		library->format_list.append(format);
	}
}


/**
 * Populates ComicInfo in any way possible, first checks file for existing info, then loads from
 * xml, if that doesn't work or there is no xml, then it parses file name to try to get as much info
 * as possible.
 */
void ComicFile::populateComicInfo() {
	try {
		// If file has ComicInfo.xml, get it and populate comic info
		if(hasComicInfo() && getXmlBuf().size() > 0) {
			// Try to read XML and get metadata, if reading XML fails then parse name for metadata
			try {
				loadComicInfo();
			} catch (const eComics::Exception &e) {
				e.printMsg();
				qDebug() << "Parsing file name for metadata instead...";
				parseFilenameForInfo();
				dirty = true;
			}
		}

		// Else ComicInfo did not exist for comic file, so create it, and try to parse file name for
		// metadata.
		else {
			parseFilenameForInfo(); // Get ComicInfo from file name
			dirty = true;
		}

		// Try to verify if page data is correct
		if(info.page_list.size() != getNumOfPages()) {
			// If page_list is incorrect, clear it out and redo it
			info.page_list.clear();
			for(int i = 0; i < getNumOfPages(); i++) {
				info.page_list << Page(QString::number(i));
			}

			dirty = true;
			qDebug() << "Corrected Page tags for" << getPath();
		}
	} catch(const eComics::Exception &e) { e.printMsg(); }

	// Make sure that Publisher, Series, Volume, and Title are not empty
	if(info.getPublisher().isEmpty()) {
		info.setPublisher("Unknown Publisher");
		dirty = true;
		qDebug() << "Added generic Publisher tag";
	}
	if(info.getSeries().isEmpty()) {
		info.setSeries("Unknown Series");
		dirty = true;
		qDebug() << "Added generic Series tag";
	}
	if(info.getVolume().isEmpty()) {
		info.setVolume("?");
		dirty = true;
		qDebug() << "Added generic Volume tag";
	}
	if(info.getTitle().isEmpty()) {
		info.setTitle("Untitled");
		dirty = true;
		qDebug() << "Added generic Title tag";
	}

	// Verify Manga value
	if(config->isMangaEnabled()) {
		if(getPath().contains(config->getMangaDir().absolutePath())) {
			if(info.getManga() != "Yes") {
				info.setManga("Yes");
				dirty = true;
				qDebug() << "Corrected Manga tag for" << getPath();
			}
		} else {
			if(info.getManga() != "No") {
				info.setManga("No");
				dirty = true;
				qDebug() << "Corrected Manga tag for" << getPath();
			}
		}
	}

	if(dirty) {
		try { save(); } catch(const eComics::Exception &e) { e.printMsg(); }
		qDebug() << "Saved corrected Metadata to" << getPath();
	}
}


/**
 * Populates ComicInfo object from raw xml, hasComicInfo() MUST be true, and xml MUST NOT be empty.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if ComicFile is an unsupported type.
 * - PROCESS_ERROR may be thrown if shell command fails in Archive in getXmlBuf().
 * - XML_READ_ERROR may be thrown if fails to read xml for any reason.
 */
void ComicFile::loadComicInfo() {
	QXmlStreamReader reader;

	// getXmlBuf may throw LOGIC_ERROR or PROCESS_ERROR on failure
	try {
		reader.addData(getXmlBuf());
	} catch(const eComics::Exception &e) { e.printMsg(); }

	// Loop through xml getting values for metadata
	while(!reader.atEnd()) {
		reader.readNextStartElement();
		// Skip end elements
		// Occasionally an unexpected blank element will be read, skip it
		if(reader.isEndElement() || reader.name() == "") {
			continue;
		}

		// If an error occurs, throw an exception
		 if(reader.hasError()) {
			throw eComics::Exception(eComics::XML_READ_ERROR, "ComicFile::populateComicInfo()",
					getPath(), &reader);
		}
		QString cur_name = reader.name().toString();
		// If ComicInfo object has metadata with name, set it's content
		if(!info[cur_name].isNull()) {
			QString cur_text = reader.readElementText();
			info[cur_name].setValue(cur_text);
		}

		// Check if PageList
		else if(reader.name() == "Pages") {
			reader.readNextStartElement();
			int page_num = 0; // Used for fallback if Image attribute is blank

			// Loop through pages
			while(reader.name() == "Page") {
				// Even though <Page/> is a single self closing element, it will still
				// Recognize a closing </Page>, so we have to skip it
				if(!reader.isEndElement()) {
					QXmlStreamAttributes attributes = reader.attributes();
					Page page;

					// Loop through page attributes
					for(int i = 0; i < attributes.size(); i++) {
						QString attrib_name		=	attributes[i].name().toString();
						QString attrib_value	=	attributes[i].value().toString();

						if(attrib_name == "Image") {
							page.setImage(attrib_value);
						}

						if(attrib_name == "Type") {
							page.setType(attrib_value);
						}

						if(attrib_name == "DoublePage") {
							page.setDoublePage(attrib_value);
						}

						if(attrib_name == "ImageSize") {
							page.setImageSize(attrib_value);
						}

						if(attrib_name == "Key") {
							page.setKey(attrib_value);
						}

						if(attrib_name == "ImageWidth") {
							page.setImageWidth(attrib_value);
						}

						if(attrib_name == "ImageHeight") {
							page.setImageHeight(attrib_value);
						}
					}

					// Validate Image tag
					if(page.getImage() != QString::number(page_num)) {
						page = Page(QString::number(page_num));
						dirty = true;
						qDebug() << "Corrected Page Image" << page_num;
					}

					info.page_list << page; // Append page to page list
					page_num++;
				}

				reader.readNextStartElement();
			}
		}
	}
}


/**
 * Parses filename to get as much information as possible from it
 */
void ComicFile::parseFilenameForInfo() {
	// First get name from path
	QString name;


	// If no directory separater in path, throw	exception, full path is a requirement
	if(getPath().lastIndexOf('/') == -1) {
		throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile::parseFilenameForInfo()",
				QString("'/'") + " not found in " + getPath() + ", absolute path required");
	}

	// If no '.' in path, throw exception, not valid file name without a file extension
	if(getPath().lastIndexOf('.') == -1) {
		throw eComics::Exception(eComics::LOGIC_ERROR, "ComicFile::parseFilenameForInfo()",
				QString("No file extension found in ") + getPath());
	}

	// Loop through characters after last '/' adding to name, until we reach index of file extension
	for(int i = getPath().lastIndexOf('/') + 1; i < getPath().lastIndexOf('.'); i++) {
		name += getPath()[i];
	}

	// Next tokenize name into a QStringList
	QStringList tok = name.split(QRegExp("\\b"));

	// Create list of months for checking if a month is contained in name
	QStringList months( {"January", "Jan", "February", "Feb", "March", "Mar", "31	Spring",
			"April", "Apr", "May", "May", "June", "Jun", "July", "Jul", "August", "Aug",
			"September", "Sep", "Autumn", "October", "Oct", "November", "Nov", "December",
			"Dec"} );

	// Prepare strings for values
	QString series;
	QString title;
	QString volume;
	QString number;
	QString year;
	QString month;

	// Create a string pointer to keep track of which tag we are getting
	QString *cur_meta = &series;

	// Loop through tok figuring out what metadata they belong to, or if they are garbage
	for(int i = 0; i < tok.size(); i++) {
		// Find out if current or next token is an int
		bool is_int = false;
		bool next_is_int = false;
		tok[i].toInt(&is_int);
		if(i + 1 < tok.size()) {
			tok[i + 1].toInt(&next_is_int);
			if(!next_is_int && i + 2 < tok.size()) {
				tok[i + 2].toInt(&next_is_int);
			}
		}

		// Check if current token might be a comic number
		if(!series.isEmpty() && (
				tok[i].compare("No", Qt::CaseInsensitive) == 0 ||
				tok[i].compare("Number", Qt::CaseInsensitive) == 0 ||
				tok[i].contains('#')
		) && next_is_int) {
			cur_meta = &number;
		}

		// Check if current token might be a comic volume
		else if(!series.isEmpty() && (
				tok[i].compare("Vol", Qt::CaseInsensitive) == 0 ||
				tok[i].compare("Volume", Qt::CaseInsensitive) == 0
		) && next_is_int) {
			cur_meta = &volume;
		}

		// If were in volume or number and tok is an int, append
		else if((cur_meta == &number ||
				cur_meta == &volume) && is_int) {
			(*cur_meta) = tok[i];
		}

		// Check if current token might be a month
		else if(!series.isEmpty() && months.contains(tok[i])) {
			cur_meta = &month;
			(*cur_meta) = tok[i];
		}

		// Check if current token might be a year
		else if(!series.isEmpty() && is_int && tok[i].toInt() >= 1800 &&
				tok[i].toInt() <= QDate::currentDate().year()) {
			cur_meta = &year;
			(*cur_meta) = tok[i];
		}

		// Else if series isn't blank but number is blank, and tok is an int, probably number
		else if(!series.isEmpty() && number.isEmpty() && is_int) {
			cur_meta = &number;
			(*cur_meta) = tok[i];
		}

		// If there is a " - " then it is most likely separating title
		else if(!series.isEmpty() && tok[i].compare(" - ") == 0) {
			cur_meta = &title;
		}

		// If there is a white space alternative, and we are in series or title, append space
		else if((tok[i] == "." || tok[i] == "_") &&
				(cur_meta == &series || cur_meta == &title)) {
			(*cur_meta) += ' ';
		}

		// Else if we are in series or title just append token
		else if(cur_meta == &series || cur_meta == &title) {
			(*cur_meta) += tok[i];
		}
	}

	// Set MetadataTags in info, make sure Series and Title don't have any trailing whitespace
	info.setSeries(series.trimmed());
	info.setTitle(title.trimmed());
	info.setVolume(volume);
	info.setNumber(number);
	info.setYear(year);
	info.setMonth(month);

	// Try to get pages info
	try {
		info.page_list.clear();
		for(int i = 0; i < getNumOfPages(); i++) {
			info.page_list << Page(QString::number(i));
		}
	} catch(const eComics::Exception &e) { e.printMsg(); }

	// Verify Manga value
	if(config->isMangaEnabled()) {
		if(getPath().contains(config->getMangaDir().absolutePath())) {
			info.setManga("Yes");
		} else {
			info.setManga("No");
		}
	}
}


/**
 * Make sure thumbnail is named properly (if ComicInfo changed), and it's the proper image (if page
 * data is changed, reordered, cover changed, etc).
 */
void ComicFile::verifyThumb() {
	// Check if comic info has been changed, and rename thumbnail accordingly
	if(!original_info.isNull() && getThumbPath(original_info) != getThumbPath() &&
			QFile::exists(getThumbPath(original_info))) {
		QFile::rename(getThumbPath(original_info), getThumbPath());
	}

	// Check if pages have been reordered, or if Cover was changed to a different page
	//TODO: CHECK IF PAGES HAVE BEEN CHANGED, IF NECESSARY DELETE OLD THUMB AND CREATE NEW ONE

	// Check if thumbnail even exists, if not create it
	if(!QFile::exists(getThumbPath())) {
		// Try to Extract thumbnail to THUMBDIR/SERIES-VOLUME-NUMBER-TITLE.jpg
		try{
			extractPage(
				info.page_list.getFrontCover(),
				config->getThumbDir().absolutePath(),
				getThumbName(), 512
			);
		} catch(const eComics::Exception &e) { e.printMsg(); }
	}
}


void ComicFile::onAddedToLibrary() {
	in_library = true;
	parseAttributeLists();
}


void ComicFile::onInfoChanged(const MetadataTag &tag, const QString old_value) {
	if(!editing) {
		// Populate original_info with values, including old_value
		original_info[tag.getName()].setValue(old_value);
		for(int i = 0; i < info.size(); i++) {
			if(info[i].getName() != tag.getName()) {
				original_info[i].setValue(info[i].getValue());
			}
		}

		// Verify thumb in case it needs to be updated
		verifyThumb();

		// Clear original_info
		original_info.clear();

		// Save info to file and library
		save();
		if(in_library) library->save();
	} else {
		dirty = true;
	}
}