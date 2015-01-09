#include "Library.hpp"

#include <QApplication>
#include <QErrorMessage>
#include <QFileDialog>
#include <QProgressDialog>

#include "ConfirmationDialog.hpp"
#include "LibraryView.hpp"
#include "MainWindow.hpp"
#include "SplashScreen.hpp"


Library *library = nullptr;
LibraryWorker *Library::worker;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARY PUBLIC METHODS 										 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void Library::init() {
	if(library == nullptr) {
		library = new Library;

		// Start splash screen and connect it to library
		SplashScreen::start();
		connect(library, SIGNAL(startedWorker(const QString &)),
				splash_screen, SLOT(update(const QString &)));

		// Prepare event loop
		QEventLoop loop;
		connect(library->thread, SIGNAL(finished()), &loop, SLOT(quit()));

		// The rest of this stuff must be done in init() rather than constructor to avoid segfaults
		// Check for config file and load it if it exists
		if(library->file->exists() && library->file->size() > 0) {
			// Connect worker method to thread
			connect(library->thread, SIGNAL(started()), library->worker, SLOT(loadLibrary()));

			// Start thread and event loop
			library->thread->start();
			loop.exec();

			// When finished make sure to disconnect worker method from thread
			disconnect(library->thread, SIGNAL(started()), library->worker, SLOT(loadLibrary()));
		}

		// Scan directories to add any missing comics and remove comics that do not exist on disk
		// Connect worker method to event loop
		connect(library->thread, SIGNAL(started()), library->worker, SLOT(scanDirectories()));

		// Start thread and event loop
		library->thread->start();
		loop.exec();

		// When finished make sure to disconnect worker method from thread
		disconnect(library->thread, SIGNAL(started()), library->worker, SLOT(scanDirectories()));

		splash_screen->finish();
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


int Library::removeAll(const ComicFile &comic) {
	int result = QList::removeAll(comic);
	save();
	return result;
}


void Library::removeAt(int i) {
	QList::removeAt(i);
	save();
}


void Library::removeFirst() {
	QList::removeFirst();
	save();
}


void Library::removeLast() {
	QList::removeLast();
	save();
}


bool Library::removeOne(const ComicFile &comic) {
	bool result = QList::removeOne(comic);
	save();
	return result;
}


void Library::replace(int i, const ComicFile &comic) {
	QList<ComicFile>::replace(i, comic);
	emit (*this)[comic.getPath()].addedToLibrary();
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


/**
 * Open a files select dialog, copy selected comics to comics or manga dir, then add to library.
 */
void Library::addComics() {
	QStringList path_list	=	QFileDialog::getOpenFileNames(
								main_window,
								"Select comics to import",
								QDir::homePath(),
								"Comic files(*.cb7 *.7z *.cbr *.rar *.cbz *zip *.pdf)");

	for(QString path : path_list) {
		ComicFile comic(path);

		if(comic.isNull()) {
			QErrorMessage err_msg(main_window);
			err_msg.showMessage(QString("Failed to open ") + path);
		} else {
			comic.move();
			this->append(comic);
			library_view->refreshModel();
		}
	}
}


/**
 * Moves files to appropriate directories, deletes all empty directories, then saves everything to
 * library.
 */
void Library::cleanupFiles() {
	// Prepare progress dialog
	QProgressDialog progress_dialog(tr("Moving files..."), 0, 0, this->size(), main_window);
	progress_dialog.setWindowModality(Qt::WindowModal);
	progress_dialog.setMinimumDuration(500);
	progress_dialog.setCancelButton(0);
	progress_dialog.show();
	progress_dialog.setValue(0);
	int i = 0;

	// Move each comic to it's appropriate directory
	for(ComicFile &comic : *this) {
		comic.move();
		progress_dialog.setValue(++i);
	}

	// Update progress dialog and set indeterminate
	progress_dialog.setLabelText(tr("Deleting empty folders..."));
	progress_dialog.setMaximum(0);
	progress_dialog.show();
	progress_dialog.setValue(0);

	// Scan directories for empty folders and delete them
	QDirIterator *cur = nullptr;

	// 2x loop, once for comics, another for manga
	for(int count = 1; count <= 2; count++) {
		QApplication::processEvents();
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
					goto done_cleaning;
				}
		}

		// Loop through current directory iterator getting comics
		while(cur->hasNext()) {
			QApplication::processEvents();
			cur->next();

			// If not a dir, skip
			if(!cur->fileInfo().isDir()) continue;

			QDir cur_dir = cur->fileInfo().dir();

			// If dir is empty, delete it, and any empty parent directories
			if(cur_dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0) {
				cur_dir.rmpath(cur_dir.absolutePath());
			}
		}

		// Reset cur
		delete cur;
		cur = nullptr;
	}

	done_cleaning:
		progress_dialog.setLabelText(tr("Saving library..."));
		QApplication::processEvents();
		save();
}


/**
 * Deletes selected comics from library and disk.
 */
void Library::deleteSelectedComics() {
	ReferenceList<ComicFile> selected_list = library_view->getSelectedComics();

	if(selected_list.isEmpty()) {
		qDebug() << "Logic error: 'Delete' should be grayed out if no comics selected.";
		return;
	}

	QString title	=	"Confirm delete";
	QString msg		=	"Are you sure you want to delete ";
	msg += (
		(selected_list.size() > 1) ? QString::number(selected_list.size()) + " comics?" :
		selected_list[0].getPath() + "?"
	);
	msg += "\n\nCan not be undone!";

	if(ConfirmationDialog::exec(title, msg, main_window)) {
		for(ComicFile comic : selected_list) {
			QString path = comic.getPath().mid(0, comic.getPath().lastIndexOf("/"));
			this->removeOne(comic);
			comic.remove();

			// Remove folders if empty
			if(QDir(path).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0) {
				QDir root_dir;
				if(path.contains(config->getComicPath())) {
					root_dir = config->getComicDir();
				} if(path.contains(config->getMangaPath())) {
					root_dir = config->getMangaDir();
				}

				root_dir.rmpath(path);
			}
		}

		library_view->refreshModel();
	}
}


/**
 * Removes selected comics from library, and moves files to desktop.
 */
void Library::removeSelectedComics() {
	ReferenceList<ComicFile> selected_list = library_view->getSelectedComics();

	if(selected_list.isEmpty()) {
		qDebug() << "Logic error: 'Delete' should be grayed out if no comics selected.";
		return;
	}

	QString title	=	"Confirm remove";
	QString msg		=	"Are you sure you want to remove ";
	msg += (
		(selected_list.size() > 1) ? QString::number(selected_list.size())+" comics from library?" :
		selected_list[0].getPath() + " from library?"
	);
	msg += "\n\nFile(s) will be moved to desktop.";

	if(ConfirmationDialog::exec(title, msg, main_window)) {
		for(ComicFile comic : selected_list) {
			QString path = comic.getPath().mid(0, comic.getPath().lastIndexOf("/"));
			QString name = comic.getPath().mid(comic.getPath().lastIndexOf("/") + 1);
			this->removeOne(comic);

			// Move to desktop
			QDir("/").rename(comic.getPath(), QDir::homePath() + "/Desktop/" + name);

			// Remove folders if empty
			if(QDir(path).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0) {
				QDir root_dir;
				if(path.contains(config->getComicPath())) {
					root_dir = config->getComicDir();
				} if(path.contains(config->getMangaPath())) {
					root_dir = config->getMangaDir();
				}

				root_dir.rmpath(path);
			}
		}

		library_view->refreshModel();
	}
}


/**
 * Start worker thread and start it's scanDirectories() method, with an indeterminate progress
 * dialog waiting the main thread.
 */
void Library::scanDirectories() {
	// Prepare progress dialog
	QProgressDialog progress_dialog(tr("Scanning directories..."), 0, 0, 0, main_window);
	progress_dialog.setWindowModality(Qt::WindowModal);
	progress_dialog.setMinimumDuration(1500);
	progress_dialog.setCancelButton(0);
	connect(thread, SIGNAL(finished()), &progress_dialog, SLOT(accept()));

	// Connect worker method to event loop
	connect(thread, SIGNAL(started()), worker, SLOT(scanDirectories()));

	// Start thread and event loop
	thread->start();
	progress_dialog.exec();

	// When finished make sure to disconnect worker method from thread
	disconnect(thread, SIGNAL(started()), worker, SLOT(scanDirectories()));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									LIBRARY PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


Library::Library() {
	// Initialize null comic
	null = ComicFile();

	// Initialize file pointer
	file = new QFile(config->getRootDir().absolutePath() + "/library.xml");

	// Initialize library worker and it's thread
	worker	=	new LibraryWorker();
	thread	=	new QThread();
	worker->moveToThread(thread);

	// Connect signals for worker and it's thread
	connect(this, SIGNAL(finishedWorker(const QString &)), thread, SLOT(quit()));

	// Connect actions
	eComics::Actions *actions = eComics::actions;
	connect(actions->addComics(), SIGNAL(triggered()), this, SLOT(addComics()));
	connect(actions->cleanupFiles(), SIGNAL(triggered()), this, SLOT(cleanupFiles()));
	connect(actions->deleteFile(), SIGNAL(triggered()), this, SLOT(deleteSelectedComics()));
	connect(actions->remove(), SIGNAL(triggered()), this, SLOT(removeSelectedComics()));
	connect(actions->scanLibrary(), SIGNAL(triggered()), this, SLOT(scanDirectories()));
}


Library::~Library() {
	delete file;
	delete worker;
	delete thread;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *								LIBRARYWORKER PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void LibraryWorker::loadLibrary() {
	emit library->startedWorker(tr("Loading library..."));
	qDebug() << "Loading library...";

	QFile *file = library->file;

	try {
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

					// If not end element for metadata tag, check if element is a valid MetadataTag
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
								// Even though <Page/> is a single self closing element, it will
								// still recognize a closing </Page>, so we have to skip it
								if(!reader.isEndElement()) {
									QXmlStreamAttributes attributes = reader.attributes();
									Page page(attributes.value("Image").toString());

									// For the rest, if they are empty don't add them
									if(attributes.hasAttribute("Type")) {
										page.setType(attributes.value("Type").toString());
									}

									if(attributes.hasAttribute("DoublePage")) {
										page.setDoublePage(
												attributes.value("DoublePage").toString());
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

				// If ComicFile was modified and has different md5_hash, then mark library dirty
				if(comic.getMd5Hash() != md5_hash) library->dirty = true;

				// Append ComicFile to library
				(*library) << comic;
			}
		}

		file->close();

		// If any comics are different from what was loaded, then re-save library
		if(library->dirty) library->save();
	} catch(const eComics::Exception &e) { e.printMsg(); }

	emit library->finishedWorker(tr("Finished loading library"));
}


/**
 * Scans Comic and/or Manga directories defined in Config object, adding/removing comics/manga from
 * library as needed. save() will automatically be called after this.
 */
void LibraryWorker::scanDirectories() {
	emit library->startedWorker(tr("Scanning directories..."));
	qDebug() << "Scanning directories for changes...";

	// First scan library for non-existent comics, and remove them
	for(int i = 0; i < library->size(); i++) {
		if(!QFile::exists((*library)[i].getPath())) {
			library->removeAt(i);
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
			if(library->contains(cur->filePath())) {
				// If it exists in library, check if it's been modified by comparing the md5 hash
				if(library->at(cur->filePath()).getMd5Hash() == cur_file.getMd5Hash()) {
					continue;
				} else {
					// If file has been modified, then remove from library to re-add
					library->removeAt(library->indexOf(cur->filePath()));
				}
			}

			// Last add it to library and set dirty to true
			(*library) << cur_file;
			library->dirty = true;

			// Update debug log
			qDebug() << "Added to library:" << cur->filePath() << "\n";
		}

		// Reset cur
		delete cur;
		cur = nullptr;
	}

	done_scanning:
		if(library->dirty) try {
			library->save();
		} catch(const eComics::Exception &e) {
			e.printMsg();
		}

	emit library->finishedWorker(tr("Finished scanning directories"));
}