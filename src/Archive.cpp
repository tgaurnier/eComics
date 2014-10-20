#define ARCHIVE_CPP
#include "Archive.hpp"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									ARCHIVE PUBLIC METHODS 										 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Copy constructor.
 */
Archive::Archive(const Archive &archive) : QObject() {
	process					=	new QProcess(this);
	file_list				=	archive.file_list;
	path					=	archive.path;
	type					=	archive.type;
	supported_image_types	=	archive.supported_image_types;
}


/**
 * Initializes QProcess, gets list of archive contents, and stores in file_list.
 *
 * Possible Exceptions:
 * - PROCESS_ERROR may be thrown if shell command fails.
 */
Archive::Archive(const QString &_path) : path(_path) {
	supported_image_types = FileTypeList( {"jpg", "jpeg", "png", "tif", "tiff", "gif", "bmp"} );

	QString output; // Used to capture output from 7z command
	QStringList output_list; // List of lines from output (output split at "\n")
	QString column_header; // String that "7z l" uses as column header for content list
	QString out_body_sep; // A string that the "7z l" command uses to separate it's list of contents
	// While looping through output_list, are we in actual content list?
	bool in_content_list = false;
	int name_index = 0;

	process = new QProcess(this);

	// Connect to signal for error handling
	connect(process, SIGNAL(error(QProcess::ProcessError)),
			SLOT(processError(QProcess::ProcessError)));

	// Connect to signal for ErrorOutput
	connect(process, SIGNAL(readyReadStandardError()), SLOT(outputStdError()));

	// Set archive type
	if(path.endsWith("7z", Qt::CaseInsensitive) || path.endsWith("cb7", Qt::CaseInsensitive)) {
		type = "7z";
	}

	if(path.endsWith("zip", Qt::CaseInsensitive) || path.endsWith("cbz", Qt::CaseInsensitive)) {
		type = "zip";
	}

	if(path.endsWith("rar", Qt::CaseInsensitive) || path.endsWith("cbr", Qt::CaseInsensitive)) {
		type = "rar";
	}

	// Get list of archive contents
	run("7z", {"l", path} );

	output = process->readAllStandardOutput();

	// Create QStringList from output separating by whitespace
	output_list = output.split(QRegExp("\\n"));
	// Prepare strings for comparison
	column_header = "   Date      Time    Attr         Size   Compressed  Name";
	out_body_sep = "------------------- ----- ------------ ------------  ------------------------";

	// Loop through output list getting all image names
	for(int i = 0; i < output_list.size(); i++) {
		// When at "column header" line, get index where Name is
		if(output_list[i] == column_header) {
			name_index = output_list[i].indexOf("Name");
		}

		// If line is body separater, decide if we are in content list or not
		else if(output_list[i] == out_body_sep) {
			if(in_content_list) {
				in_content_list = false;
			}

			else {
				in_content_list = true;
			}
		}

		// Else, if we are in content list, use name_index to get actual file name from line
		else if(in_content_list) {
			file_list << output_list[i].mid(name_index);
		}
	}
}


Archive::~Archive() {
	delete process;
}


int Archive::getNumOfPages() const {
	int num = 0;

	// Loop through output list getting all image names
	for(int i = 0; i < file_list.size(); i++) {
		if(supported_image_types.isSupported(file_list[i])) {
			num++;
		}
	}

	return num;
}


bool Archive::hasComicInfo() const {
	if(file_list.contains("ComicInfo.xml", Qt::CaseInsensitive)) {
		return true;
	}

	else {
		return false;
	}
}


/**
 * Gets ComicInfo.xml and returns entire file contents as QString. If ComicInfo.xml does not exist,
 * then a blank/null QString is returned.
 *
 * Possible Exceptions:
 * - PROCESS_ERROR may be thrown if shell command fails.
 */
QString Archive::getXmlBuf() const {
	// Exec 7z command to extract file (7z can extract any archive format)
	if(hasComicInfo()) {
		run(QString("7z"), {"e", "-so", path, "ComicInfo.xml"} );
		return process->readAllStandardOutput();
	} else {
		return QString();
	}
}


/**
 * Writes raw_xml to ComicInfo.xml in archive, replaces file if it exists.
 *
 * Possible Exceptions:
 * - FILE_ERROR may be thrown if it fails to open, write to, or remove a file.
 * - PROCESS_ERROR may be thrown if the shell command fails in any way.
 */
void Archive::setComicInfo(const QByteArray &raw_xml) {
	// Prepare path string for temp ComicInfo.xml
	QString tmp_file_path = config->getTempDir().absolutePath() + "/ComicInfo.xml";
	// Prepare temp file for writing
	QFile file(tmp_file_path);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		throw eComics::Exception(eComics::FILE_ERROR, "Archive::setComicInfo()",
				QString("Failed to Open ") + tmp_file_path);
	}

	// Write xml to tmp file
	if(file.write(raw_xml) == -1) {
		throw eComics::Exception(eComics::FILE_ERROR, "Archive::setComicInfo()",
				QString("Failed to Write ") + tmp_file_path);
	}

	// Close file so it can be added to archive
	file.close();

	// Add temp file to archive, if archive is RAR, then use rar command
	if(type == "rar") {
		run("rar", {"u", "-ep", path, tmp_file_path} );
	} else {
		// For all other archive types use 7zip
		run("7z", {"u", QString("-t") + type, path, tmp_file_path} );
	}

	if(!file.remove()) {
		throw eComics::Exception(eComics::FILE_ERROR, "Archive::setComicInfo()",
				QString("Failed to Remove ") + tmp_file_path);
	}
}


/**
 * Extracts page at index to temp dir, returns full path to extracted page.
 *
 * Possible Exceptions:
 * - LOGIC_ERROR may be thrown if page at index doesn't exist.
 * - PROCESS_ERROR may be thrown if shell command fails in any way.
 */
QString Archive::extractPage(const int index) const {
	int count = 0; // Used to keep track of how many images we have counted

	for(int i = 0; i < file_list.size(); i++) {
		// If image is in subdirectory of archive, remove directory name from file_name
		QString file_name = (file_list[i].contains('/')) ? file_list[i].split('/').last() :
				file_list[i];

		if(supported_image_types.isSupported(file_list[i])) {
			if(count == index) {
				run("7z", {"e", path, file_list[i], "-aoa", QString("-o") +
							config->getTempDir().absolutePath()} );
				return config->getTempDir().absolutePath() + "/" + file_name;
			} else {
				count++;
			}
		}
	}

	throw eComics::Exception(eComics::LOGIC_ERROR, "Archive::extractPage()",
			QString("Page at index ") + QString::number(index) + QString(" doesn't exist in ") +
			path);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									ARCHIVE PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * This is a conveniece function for QProcess::start(). Simply runs a command with arguments and
 * waits for it to finish, if it fails then an exception is thrown.
 *
 * Possible Exceptions:
 * - PROCESS_ERROR may be thrown if shell command fails in any way.
 */
void Archive::run(const QString &program, const QStringList &args) const {
	process->start(program, args);

	// Wait for process to finish before continuing
	if(!process->waitForFinished(5000)) {
		// If process fails to finish, throw exception
		throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", program +
				" Failed to Finish");
	}
}


/**
 * Slot which connects to error signal from QProcess, throws appropriate exception.
 */
void Archive::processError(QProcess::ProcessError error) {
	switch(error) {
		case QProcess::FailedToStart:
			throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", process->program() +
					" failed to start");

		case QProcess::Crashed:
			throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", process->program() +
					" crashed");

		case QProcess::Timedout:
			throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", process->program() +
					" timedout");

		case QProcess::WriteError:
			throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", process->program() +
					" write error");

		case QProcess::ReadError:
			throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", process->program() +
					" read error");

		case QProcess::UnknownError:
			throw eComics::Exception(eComics::PROCESS_ERROR, "Archive::run()", process->program() +
					" unknown error");


	}
}


void Archive::outputStdError() {
	qDebug() << process->readAllStandardError();
}