#include "Library.hpp"


Library *library = nullptr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARY PUBLIC METHODS 										 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void Library::init() {
	if(library == nullptr) {
		library = new Library;

		// The rest of this stuff must be done in init() rather than constructor to avoid segfaults
		// Check for config file and load it if it exists
		if(library->file->exists() && library->file->size() > 0) {
			try { library->loadFromFile(); } catch(const eComics::Exception &e) { e.printMsg(); }
		}

		// Scan directories to add any missing comics and remove comics that do not exist on disk
		library->scanDirectories();
	}
}


void Library::destroy() {
	if(library != nullptr) {
		delete library;
		library = nullptr;
	}
}


void Library::append(const ComicFile &comic) {
	QList<ComicFile>::append(comic);
	emit (*this)[comic.getPath()].addedToLibrary();
}


void Library::append(const QList<ComicFile> &comic_list) {
	QList<ComicFile>::append(comic_list);
	for(ComicFile comic : comic_list) emit (*this)[comic.getPath()].addedToLibrary();
}


/**
 * Return list of comics from publisher.
 */
QList<ComicFile> Library::getComicsFromPublisher(const QString &publisher) const {
	QList<ComicFile> list;

	for(const ComicFile &comic : *this) {
		if(comic.info.getPublisher() == publisher) {
			list.append(comic);
		}
	}

	return list;
}


/**
 * Return list of comics from series.
 */
QList<ComicFile> Library::getComicsFromSeries(const QString &series, const QString &publisher)
		const {
	QList<ComicFile> list;

	for(const ComicFile &comic : *this) {
		if((comic.info.getPublisher() == publisher || !config->groupByPublisher()) &&
				comic.info.getSeries() == series) {
			list.append(comic);
		}
	}

	return list;
}


/**
 * Return list of comics from series and volume.
 */
QList<ComicFile> Library::getComicsFromVolume(const QString &series, const QString &volume,
		const QString &publisher) const {
	QList<ComicFile> list;

	for(const ComicFile &comic : *this) {
		if((comic.info.getPublisher() == publisher || !config->groupByPublisher()) &&
				comic.info.getSeries() == series && comic.info.getVolume() == volume) {
			list.append(comic);
		}
	}

	return list;
}


/**
 * Return list of pointers to comics from publisher.
 */
ReferenceList<ComicFile> Library::comicsFromPublisher(const QString &publisher) {
	ReferenceList<ComicFile> list;

	// If the range-based for loop is used with *this, then a reference must be const, therefore to
	// get non-const ComicFile &comic, we must use *library
	for(ComicFile &comic : *library) {
		if(comic.info.getPublisher() == publisher) {
			list.append(comic);
		}
	}

	return list;
}


/**
 * Return list of pointers to comics from series.
 */
ReferenceList<ComicFile> Library::comicsFromSeries(const QString &series, const QString &publisher) {
	ReferenceList<ComicFile> list;

	for(ComicFile &comic : *library) {
		if((comic.info.getPublisher() == publisher || !config->groupByPublisher()) &&
				comic.info.getSeries() == series) {
			list.append(comic);
		}
	}

	return list;
}


/**
 * Return list of pointers to  comics from series and volume.
 */
ReferenceList<ComicFile> Library::comicsFromVolume(const QString &series, const QString &volume,
		const QString &publisher) {
	ReferenceList<ComicFile> list;

	for(ComicFile &comic : *library) {
		if((comic.info.getPublisher() == publisher || !config->groupByPublisher()) &&
				comic.info.getSeries() == series && comic.info.getVolume() == volume) {
			list.append(comic);
		}
	}

	return list;
}


/**
 * Returns true if comic with md5 hash exists in library, otherwise returns false
 */
bool Library::contains(const QByteArray &md5_hash) const {
	for(int i = 0; i < this->length(); i++) {
		if((*this)[i].getMd5Hash() == md5_hash) {
			return true;
		}
	}

	return false;
}


/**
 * Reimplemented method, return true if comic with path exists in library, otherwise returns false.
 */
bool Library::contains(QString path) const {
	for(int i = 0; i < this->length(); i++) {
		if((*this)[i].getPath() == path) {
			return true;
		}
	}

	return false;
}


int Library::indexOf(const QByteArray &md5_hash) const {
	for(int i = 0; i <= this->size(); i++) {
		if((*this)[i].getMd5Hash() == md5_hash) {
			return i;
		}
	}

	return -1;
}


/**
 * Reimplemented method, return index of comic with path, returns -1 if it doesn't exist in library.
 */
int Library::indexOf(const QString &path) const {
	for(int i = 0; i <= this->size(); i++) {
		if((*this)[i].getPath() == path) {
			return i;
		}
	}

	return -1;
}


void Library::insert(int i, const ComicFile &comic) {
	QList<ComicFile>::insert(i, comic);
	emit (*this)[comic.getPath()].addedToLibrary();
}


typedef QList<ComicFile>::iterator iterator;
iterator Library::insert(iterator before, const ComicFile &comic) {
	iterator iter = QList<ComicFile>::insert(before, comic);
	emit iter->addedToLibrary();
	return iter;
}


void Library::push_back(const ComicFile &comic) {
	QList<ComicFile>::push_back(comic);
	emit (*this)[comic.getPath()].addedToLibrary();
}


void Library::push_front(const ComicFile &comic) {
	QList<ComicFile>::push_front(comic);
	emit (*this)[comic.getPath()].addedToLibrary();
}


void Library::replace(int i, const ComicFile &comic) {
	QList<ComicFile>::replace(i, comic);
	emit (*this)[comic.getPath()].addedToLibrary();
}


/**
 * Scans Comic and/or Manga directories defined in Config object, adding/removing comics/manga from
 * library as needed. save() will automatically be called after this.
 */
void Library::scanDirectories() {
	qDebug() << "Scanning directories for changes...";

	// First scan library for non-existent comics, and remove them
	for(int i = 0; i < this->size(); i++) {
		if(!QFile::exists((*this)[i].getPath())) {
			this->removeAt(i);
		}
	}

	// Next scan comic and/or manga directories to add any new comics/manga
	QDirIterator *cur = nullptr;

	// 2x loop, once for comics, another for manga
	for(int count = 1; count <= 2; count++) {
		// On first iteration set cur to comic dir if it's enabled, on second do manga
		switch(count) {
			case 1:
				if(config->isComicEnabled()) {
					cur = new QDirIterator(config->getComicDir(), QDirIterator::Subdirectories);
				} else {
					continue;
				}
				break;

			case 2:
				if(config->isMangaEnabled()) {
					cur = new QDirIterator(config->getMangaDir(), QDirIterator::Subdirectories);
				} else {
					goto done_scanning;
				}
		}

		// Loop through current directory iterator getting comics
		while(cur->hasNext()) {
			cur->next();

			// If not a file, skip
			if(!cur->fileInfo().isFile()) continue;

			ComicFile cur_file(cur->filePath());

			// First check if file is valid
			if(cur_file.isNull()) {
				continue;
			}

			// Next check if file at path already exists in library
			if(this->contains(cur->filePath())) {
				// If it exists in library, check if it's been modified by comparing the md5 hash
				if(this->at(cur->filePath()).getMd5Hash() == cur_file.getMd5Hash()) {
					continue;
				} else {
					// If file has been modified, then remove from library to re-add
					removeAt(indexOf(cur->filePath()));
				}
			}

			// Last add it to library and set dirty to true
			(*this) << cur_file;
			dirty = true;

			// Update debug log
			qDebug() << "Added to library:" << cur->filePath() << "\n";
		}

		// Reset cur
		delete cur;
		cur = nullptr;
	}

	done_scanning:
		if(dirty) try { save(); } catch(const eComics::Exception &e) { e.printMsg(); }
}


/**
 * Saves all comics/manga in library to library.xml.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if library file fails to open.
 */
void Library::save() {
	if(batch_editing) {
		dirty = true;
		return;
	}

	qDebug() << "Saving changes to library...";
	// Open file for writing
	if(!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		throw eComics::Exception(eComics::FILE_ERROR, "Library::save()",
			QString("Failed to open ") + file->fileName() + " for writing");
	}

	// Prepare xml writer, and start xml file
	QXmlStreamWriter writer(file);
	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1);
	writer.writeStartDocument("1.0");
	writer.writeStartElement("Library");

	writer.writeStartElement("Comics");

	// Loop through comics adding to library file
	for(int comic_index = 0; comic_index < this->size(); comic_index++) {
		const ComicFile &cur_comic = (*this)[comic_index];
		writer.writeStartElement("Comic");

		// Loop through all the metadata tags in comic
		for(int meta_index = 0; meta_index < cur_comic.info.size(); meta_index++) {
			// If current metadata value is not empty, add it
			if(!cur_comic.info.at(meta_index).getValue().isEmpty()) {
				writer.writeTextElement(
					cur_comic.info.at(meta_index).getName(),
					cur_comic.info.at(meta_index).getValue()
				);
			}
		}

		writer.writeStartElement("Pages");
		// Loop through pages adding to library
		for(int page_index = 0; page_index < cur_comic.info.page_list.size(); page_index++) {
			const Page &cur_page = cur_comic.info.page_list[page_index];
			writer.writeEmptyElement("Page");
			// Image is a required field, so it will always be filled
			writer.writeAttribute("Image", QString::number(page_index));

			// For the rest, if they are empty don't add them
			if(!cur_page.getType().isEmpty()) {
				writer.writeAttribute("Type", cur_page.getType());
			}

			if(!cur_page.getDoublePage().isEmpty()) {
				writer.writeAttribute("DoublePage", cur_page.getDoublePage());
			}

			if(!cur_page.getImageSize().isEmpty()) {
				writer.writeAttribute("ImageSize", cur_page.getImageSize());
			}

			if(!cur_page.getKey().isEmpty()) {
				writer.writeAttribute("Key", cur_page.getKey());
			}

			if(!cur_page.getImageWidth().isEmpty()) {
				writer.writeAttribute("ImageWidth", cur_page.getImageWidth());
			}

			if(!cur_page.getImageHeight().isEmpty()) {
				writer.writeAttribute("ImageHeight", cur_page.getImageHeight());
			}
		}

		writer.writeEndElement(); // </Pages>

		// Write path of comic in library
		writer.writeTextElement("Path", cur_comic.getPath());

		// Write md5 hash of comic in library
		if(!cur_comic.getMd5Hash().isEmpty()) {
			writer.writeTextElement("Md5Hash", cur_comic.getMd5Hash());
		}

		writer.writeEndElement(); // </Comic>
	}

	// Finish writing library file and close
	writer.writeEndElement(); // </Comics>
	writer.writeEndElement(); // </Library>
	file->close();
	dirty = false;
}


/**
 * Sets batch_editing to true, Library will not save until finishBatchEditing() is called. This
 * should be called if editing metadata of multiple comics, otherwise Library will save to disk on
 * every single comic that is changed, this way it will only save to disk once you are finished
 * editing comic metadata.
 */
void Library::startBatchEditing() {
	batch_editing = true;
}


/**
 * Sets batch_editing to false and saves library.
 */
void Library::finishBatchEditing() {
	batch_editing = false;
	if(dirty) save();
}


const ComicFile & Library::at(const QByteArray &md5_hash) const {
	if(this->contains(md5_hash)) {
		return this->at(this->indexOf(md5_hash));
	} else return null;
}


const ComicFile & Library::at(const QString &path) const {
	if(this->contains(path)) {
		return this->at(this->indexOf(path));
	} else return null;
}


Library & Library::operator+=(const QList<ComicFile> &comic_list) {
	QList<ComicFile>::operator+=(comic_list);
	for(ComicFile comic : comic_list) emit (*this)[comic.getPath()].addedToLibrary();
	return *this;
}


Library & Library::operator+=(const ComicFile &comic) {
	QList<ComicFile>::operator+=(comic);
	emit (*this)[comic.getPath()].addedToLibrary();
	return *this;
}


Library & Library::operator<<(const QList<ComicFile> &comic_list) {
	QList<ComicFile>::operator<<(comic_list);
	for(ComicFile comic : comic_list) emit (*this)[comic.getPath()].addedToLibrary();
	return *this;
}


Library & Library::operator<<(const ComicFile &comic) {
	QList<ComicFile>::operator<<(comic);
	emit (*this)[comic.getPath()].addedToLibrary();
	return *this;
}


ComicFile & Library::operator[](const QByteArray &md5_hash) {
	if(this->contains(md5_hash)) {
		return (*this)[this->indexOf(md5_hash)];
	} else return null;
}


const ComicFile & Library::operator[](const QByteArray &md5_hash) const {
	return this->at(md5_hash);
}


ComicFile & Library::operator[](const QString &path) {
	if(this->contains(path)) {
		return (*this)[this->indexOf(path)];
	} else return null;
}


const ComicFile & Library::operator[](const QString &path) const {
	return this->at(path);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARY PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


Library::Library() {
	// Initialize null comic
	null = ComicFile();

	// Initialize file pointer
	file = new QFile(config->getRootDir().absolutePath() + "/library.xml");
}


Library::~Library() {
	delete file;
}


void Library::loadFromFile() {
	qDebug() << "Loading library...";

	// Open file for reading
	if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw eComics::Exception(eComics::FILE_ERROR, "Library::loadFromFile()",
				QString("Failed to open ") + file->fileName() + " for reading");
	}

	// Setup xml reader
	QXmlStreamReader reader(file);

	// Loop
	while(!reader.atEnd()) {
		reader.readNextStartElement();

		// Skip end elements
		// Occasionally an unexpected blank element will be read, skip it
		if(reader.isEndElement() || reader.name() == "") {
			continue;
		}

		// If an error occurs, throw exception
		else if(reader.hasError()) {
			throw eComics::Exception(eComics::XML_READ_ERROR, "Library::loadFromFile()",
					file->fileName(), &reader);
		}

		// Next, if at a <Comic> element, loop through it's metadata tags
		if(reader.name() == "Comic") {
			// Start on first child of <Comic>
			reader.readNextStartElement();

			ComicInfo info;
			QString comic_path;
			QByteArray md5_hash;

			// Loop through elements until </Comic> is found
			while(!(reader.name() == "Comic" && reader.isEndElement())) {
				if(reader.hasError()) {
					throw eComics::Exception(eComics::XML_READ_ERROR, "Library::loadFromFile()",
							file->fileName(), &reader);
				}

				// If not an end element for metadata tag, Check if element is a valid MetadataTag
				else if(!reader.isEndElement()) {
					// Check if MetadataTag
					if(!info[reader.name().toString()].isNull()) {
						info[reader.name().toString()].setValue(reader.readElementText());
					}

					// Check if PageList
					else if(reader.name() == "Pages") {
						reader.readNextStartElement();

						// Loop through pages
						while(reader.name() == "Page") {
							// Even though <Page/> is a single self closing element, it will still
							// Recognize a closing </Page>, so we have to skip it
							if(!reader.isEndElement()) {
								QXmlStreamAttributes attributes = reader.attributes();
								Page page(attributes.value("Image").toString());

								// For the rest, if they are empty don't add them
								if(attributes.hasAttribute("Type")) {
									page.setType(attributes.value("Type").toString());
								}

								if(attributes.hasAttribute("DoublePage")) {
									page.setDoublePage(attributes.value("DoublePage").toString());
								}

								if(attributes.hasAttribute("ImageSize")) {
									page.setImageSize(attributes.value("ImageSize").toString());
								}

								if(attributes.hasAttribute("Key")) {
									page.setKey(attributes.value("Key").toString());
								}

								if(attributes.hasAttribute("ImageWidth")) {
									page.setImageWidth(
											attributes.value("ImageWidth").toString());
								}

								if(attributes.hasAttribute("ImageHeight")) {
									page.setImageHeight(
											attributes.value("ImageHeight").toString());
								}

								info.page_list << page;
							}

							reader.readNextStartElement();
						}
					}

					// Check if path
					else if(reader.name() == "Path") {
						comic_path = reader.readElementText();
					}

					// Check if md5 hash
					else if(reader.name() == "Md5Hash") {
						md5_hash = reader.readElementText().toLocal8Bit();
					}
				}

				reader.readNextStartElement();
			}

			// Load ComicFile, setting it's ComicInfo to info loaded from library
			ComicFile comic(comic_path, info, md5_hash);

			// If loaded ComicFile was modified and has different md5_hash, then mark library dirty
			if(comic.getMd5Hash() != md5_hash) dirty = true;

			// Append ComicFile to library
			(*this) << comic;
		}
	}

	file->close();

	// If any comics are different from what was loaded, then re-save library
	if(dirty) save();
}