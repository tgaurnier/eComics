#include "Pdf.hpp"


/**
 * NOTE *
 *
 * Keep in mind PoDoFo methods use Pascal case
 */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										PDF PUBLIC METHODS 										 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Copy constructor.
 */
Pdf::Pdf(const Pdf &pdf) {
	path = pdf.path;
}


/**
 * Initializes PdfMemDocument with path, and any variables that need initialization.
 */
Pdf::Pdf(const QString &_path) : path(_path) {}


Pdf::~Pdf() {}


/**
 * Simply returns number of pages
 */
int Pdf::getNumOfPages() const {
	PoDoFo::PdfMemDocument mem_doc(path.toLocal8Bit().data());
	return mem_doc.GetPageCount();
}


/**
 * Checks for xmp metadata stream, return true if it exists, return false if it doesn't.
 */
bool Pdf::hasComicInfo() const {
	PoDoFo::PdfMemDocument mem_doc(path.toLocal8Bit().data());

	// First check for ComicInfo metadata object
	if(!mem_doc.GetCatalog()->GetDictionary().HasKey(PoDoFo::PdfName("ComicInfo"))) {
		return false;
	}

	// Next get PdfObject pointer from dictionary
	PoDoFo::PdfObject *xmp_obj = mem_doc.GetCatalog()->GetIndirectKey(PoDoFo::PdfName("ComicInfo"));

	// Check if ComicInfo object has a PdfStream
	if(!xmp_obj->HasStream()) {
		return false;
	}

	return true;
}


/**
 * Get xmp packet from ComicInfo metadata object, and return as a QString, if it doesn't exist then
 * QString will be null.
 */
QString Pdf::getXmlBuf() const {
	PoDoFo::PdfMemDocument mem_doc(path.toLocal8Bit().data());
	QString xmp_str;

	// Check if ComicInfo exists and has a stream
	if(mem_doc.GetCatalog()->GetDictionary().HasKey(PoDoFo::PdfName("ComicInfo"))) {
		PoDoFo::PdfObject *xmp_obj = mem_doc.GetCatalog()->GetIndirectKey(
				PoDoFo::PdfName("ComicInfo"));

		// Check if ComicInfo object has a stream
		if(xmp_obj->HasStream()) {
			char *xmp_buf;
			PoDoFo::pdf_long xmp_len;

			// Get PdfStream from Metadata object, and write the stream to the ouput buffer
			xmp_obj->GetStream()->GetFilteredCopy(&xmp_buf, &xmp_len);
			// Create QString from xmp_buf
			xmp_str = QString(xmp_buf);
			// Free xmp_buf
			free(xmp_buf);
		}
	}

	return xmp_str;
}


/**
 * If ComicInfo object doesn't already exist, then create it. Add raw_xmp packet to ComicInfo
 * object's stream. If the ComicInfo object already has a stream it's overwritten.
 */
void Pdf::setComicInfo(const QByteArray &raw_xmp) {
	PoDoFo::PdfMemDocument mem_doc(path.toLocal8Bit().data());
	PoDoFo::PdfObject *xmp_obj;

	// If ComicInfo object already exists, get it
	if(mem_doc.GetCatalog()->GetDictionary().HasKey(PoDoFo::PdfName("ComicInfo"))) {
		xmp_obj = mem_doc.GetCatalog()->GetIndirectKey(PoDoFo::PdfName("ComicInfo"));
	}

	// Else create object
	else {
		xmp_obj = mem_doc.GetObjects().CreateObject("Metadata");
	}

	// Add appropriate keys and add xmp packet to stream
	xmp_obj->GetDictionary().AddKey(PoDoFo::PdfName("Subtype"), PoDoFo::PdfName("XML"));
	xmp_obj->GetStream()->Set(raw_xmp.data(), raw_xmp.length());
	mem_doc.GetCatalog()->GetDictionary().AddKey(PoDoFo::PdfName("ComicInfo"),
			xmp_obj->Reference());

	// Write pdf to new file
	QString new_path = path;
	new_path.replace(".pdf", "_NEW.pdf", Qt::CaseInsensitive);
	mem_doc.Write(new_path.toLocal8Bit().data());

	// Remove old pdf and rename new pdf
	QFile::remove(path);
	QFile::rename(new_path, path);

	// Last open new pdf in this object
	mem_doc.Load(path.toLocal8Bit().data());
}


/**
 * Extract page at index to temp dir, returns full path of extracted page.
 *
 * Possible Exceptions:
 * - PDF_ERROR may be thrown if Poppler fails to load file, if pdf is locked, or if Poppler fails
 * to render page.
 * - LOGIC_ERROR may be thrown if page at index doesn't exist.
 * - FILE_ERROR may be thrown if writing page to image file fails.
 */
QString Pdf::extractPage(const int index) const {
	QString new_path = config->getTempDir().absolutePath() + "/extracted_image.jpg";

	// Open document in poppler
	Poppler::Document *pop_doc = Poppler::Document::load(path);
	if(!pop_doc || pop_doc->isLocked()) {
		delete pop_doc;
		if(!pop_doc) {
			throw eComics::Exception(eComics::PDF_ERROR, "Pdf::extractPage()",
					QString("Poppler failed to load ") + path);
		} else {
			throw eComics::Exception(eComics::PDF_ERROR, "Pdf::extractPage()", QString("Pdf at ") +
					path + " is locked");
		}
	}

	// Get page from pdf
	Poppler::Page *pdf_page = pop_doc->page(index);
	if(!pdf_page) {
		throw eComics::Exception(eComics::LOGIC_ERROR, "Pdf::extractPage()",
				QString("Page at index ") + QString::number(index) + " doesn't exist in " + path);
	}

	// Render index from page
	QImage image = pdf_page->renderToImage();
	if(image.isNull()) {
		throw eComics::Exception(eComics::PDF_ERROR, "Pdf::extractPage()",
				QString("Failed to render QImage from pdf ") + "page in " + path);
	}

	// Save index to temp dir
	if(!image.save(new_path, "JPEG")) {
		throw eComics::Exception(eComics::FILE_ERROR, "Pdf::extractPage()",
				QString("Failed to save QImage to ") + new_path);
	}

	// Free pdf_page and return full path to extracted index
	delete pdf_page;
	delete pop_doc;
	return new_path;
}