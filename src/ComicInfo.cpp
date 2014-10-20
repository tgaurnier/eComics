#include "ComicInfo.hpp"

#include <QDebug>

#include "ComicFile.hpp"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									COMICINFO PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Instantiate a new blank ComicInfo object
 */
ComicInfo::ComicInfo(QObject *parent) : QObject(parent) {
	title			=	MetadataTag("Title", this);
	series			=	MetadataTag("Series", this);
	number			=	MetadataTag("Number", this);
	count			=	MetadataTag("Count", this);
	volume			=	MetadataTag("Volume", this);
	alt_series		=	MetadataTag("AlternateSeries", this);
	alt_number		=	MetadataTag("AlternateNumber", this);
	alt_count		=	MetadataTag("AlternateCount", this);
	summary			=	MetadataTag("Summary", this);
	notes			=	MetadataTag("Notes", this);
	year			=	MetadataTag("Year", this);
	month			=	MetadataTag("Month", this);
	writer			=	MetadataTag("Writer", this);
	penciller		=	MetadataTag("Penciller", this);
	inker			=	MetadataTag("Inker", this);
	colorist		=	MetadataTag("Colorist", this);
	letterer		=	MetadataTag("Letterer", this);
	cover_artist	=	MetadataTag("CoverArtist", this);
	editor			=	MetadataTag("Editor", this);
	publisher		=	MetadataTag("Publisher", this);
	imprint			=	MetadataTag("Imprint", this);
	genre			=	MetadataTag("Genre", this);
	web				=	MetadataTag("Web", this);
	page_count		=	MetadataTag("PageCount", this);
	language_iso	=	MetadataTag("LanguageISO", this);
	format			=	MetadataTag("Format", this);
	black_and_white	=	MetadataTag("BlackAndWhite", this);
	manga			=	MetadataTag("Manga", this);

	connectSignals();
}


ComicInfo::ComicInfo(const ComicInfo &info) : QObject() {
	*this = info;
	connectSignals();
}


void ComicInfo::clear() {
	for(int i = 0; i < size(); i++) (*this)[i].setValue("");
}


/**
 * Getters for all MetadataTags.
 */
QString ComicInfo::getTitle() const { return title.getValue(); }
QString ComicInfo::getSeries() const { return series.getValue(); }
QString ComicInfo::getNumber() const { return number.getValue(); }
QString ComicInfo::getCount() const { return count.getValue(); }
QString ComicInfo::getVolume() const { return volume.getValue(); }
QString ComicInfo::getAltSeries() const { return alt_series.getValue(); }
QString ComicInfo::getAltNumber() const { return alt_number.getValue(); }
QString ComicInfo::getAltCount() const { return alt_count.getValue(); }
QString ComicInfo::getSummary() const { return summary.getValue(); }
QString ComicInfo::getNotes() const { return notes.getValue(); }
QString ComicInfo::getYear() const { return year.getValue(); }
QString ComicInfo::getMonth() const { return month.getValue(); }
QString ComicInfo::getWriter() const { return writer.getValue(); }
QString ComicInfo::getPenciller() const { return penciller.getValue(); }
QString ComicInfo::getInker() const { return inker.getValue(); }
QString ComicInfo::getColorist() const { return colorist.getValue(); }
QString ComicInfo::getLetterer() const { return letterer.getValue(); }
QString ComicInfo::getCoverArtist() const { return cover_artist.getValue(); }
QString ComicInfo::getEditor() const { return editor.getValue(); }
QString ComicInfo::getPublisher() const { return publisher.getValue(); }
QString ComicInfo::getImprint() const { return imprint.getValue(); }
QString ComicInfo::getGenre() const { return genre.getValue(); }
QString ComicInfo::getWeb() const { return web.getValue(); }
QString ComicInfo::getPageCount() const { return page_count.getValue(); }
QString ComicInfo::getLanguageIso() const { return language_iso.getValue(); }
QString ComicInfo::getFormat() const { return format.getValue(); }
QString ComicInfo::getBlackAndWhite() const { return black_and_white.getValue(); }
QString ComicInfo::getManga() const { return manga.getValue(); }


/**
 * Setters for all MetadataTags.
 */
void ComicInfo::setTitle(const QString &value) { title.setValue(value); }
void ComicInfo::setSeries(const QString &value) { series.setValue(value); }
void ComicInfo::setNumber(const QString &value) { number.setValue(value); }
void ComicInfo::setCount(const QString &value) { count.setValue(value); }
void ComicInfo::setVolume(const QString &value) { volume.setValue(value); }
void ComicInfo::setAltSeries(const QString &value) { alt_series.setValue(value); }
void ComicInfo::setAltNumber(const QString &value) { alt_number.setValue(value); }
void ComicInfo::setAltCount(const QString &value) { alt_count.setValue(value); }
void ComicInfo::setSummary(const QString &value) { summary.setValue(value); }
void ComicInfo::setNotes(const QString &value) { notes.setValue(value); }
void ComicInfo::setYear(const QString &value) { year.setValue(value); }
void ComicInfo::setMonth(const QString &value) { month.setValue(value); }
void ComicInfo::setWriter(const QString &value) { writer.setValue(value); }
void ComicInfo::setPenciller(const QString &value) { penciller.setValue(value); }
void ComicInfo::setInker(const QString &value) { inker.setValue(value); }
void ComicInfo::setColorist(const QString &value) { colorist.setValue(value); }
void ComicInfo::setLetterer(const QString &value) { letterer.setValue(value); }
void ComicInfo::setCoverArtist(const QString &value) { cover_artist.setValue(value); }
void ComicInfo::setEditor(const QString &value) { editor.setValue(value); }
void ComicInfo::setPublisher(const QString &value) { publisher.setValue(value); }
void ComicInfo::setImprint(const QString &value) { imprint.setValue(value); }
void ComicInfo::setGenre(const QString &value) { genre.setValue(value); }
void ComicInfo::setWeb(const QString &value) { web.setValue(value); }
void ComicInfo::setPageCount(const QString &value) { page_count.setValue(value); }
void ComicInfo::setLanguageIso(const QString &value) { language_iso.setValue(value); }
void ComicInfo::setFormat(const QString &value) { format.setValue(value); }
void ComicInfo::setBlackAndWhite(const QString &value) { black_and_white.setValue(value); }
void ComicInfo::setManga(const QString &value) { manga.setValue(value); }


/**
 * If ComicInfo is null, then ALL of it's metadata tags have no value.
 */
bool ComicInfo::isNull() const {
	for(int i = 0; i < size(); i++) {
		if(!this->at(i).getValue().isEmpty()) return false;
	}

	return true;
}


/**
 * Returns number of metadata items.
 */
int ComicInfo::size() const {
	return 28;
}


/**
 * Returns const reference to MetadataTag at index i, returns null when index is out of bounds,
 * null.isNull() will return true. Can not be used to get path or page_list members. If returned
 * MetadataTag needs to be editable, then use operator [].
 */
const MetadataTag& ComicInfo::at(const int i) const {
	switch(i) {
		case 0: return title;
		case 1: return series;
		case 2: return number;
		case 3: return count;
		case 4: return volume;
		case 5: return alt_series;
		case 6: return alt_number;
		case 7: return alt_count;
		case 8: return summary;
		case 9: return notes;
		case 10: return year;
		case 11: return month;
		case 12: return writer;
		case 13: return penciller;
		case 14: return inker;
		case 15: return colorist;
		case 16: return letterer;
		case 17: return cover_artist;
		case 18: return editor;
		case 19: return publisher;
		case 20: return imprint;
		case 21: return genre;
		case 22: return web;
		case 23: return page_count;
		case 24: return language_iso;
		case 25: return format;
		case 26: return black_and_white;
		case 27: return manga;
		default: return null;
	}
}


/**
 * Returns const reference to MetadataTag with name, returns null if name is not found,
 * null.isNull() will return true. Can not be used to get path or page_list members.
 */
const MetadataTag& ComicInfo::at(const QString &name) const {
	return
		(name == "Title") ? title :
		(name == "Series") ? series :
		(name == "Number") ? number :
		(name == "Count") ? count :
		(name == "Volume") ? volume :
		(name == "AlternateSeries") ? alt_series :
		(name == "AlternateNumber") ? alt_number :
		(name == "AlternateCount") ? alt_count :
		(name == "Summary") ? summary :
		(name == "Notes") ? notes :
		(name == "Year") ? year :
		(name == "Month") ? month :
		(name == "Writer") ? writer :
		(name == "Penciller") ? penciller :
		(name == "Inker") ? inker :
		(name == "Colorist") ? colorist :
		(name == "Letter") ? letterer :
		(name == "CoverArtist") ? cover_artist :
		(name == "Editor") ? editor :
		(name == "Publisher") ? publisher :
		(name == "Imprint") ? imprint :
		(name == "Genre") ? genre :
		(name == "Web") ? web :
		(name == "PageCount") ? page_count :
		(name == "LanguageISO") ? language_iso :
		(name == "Format") ? format :
		(name == "BlackAndWhite") ? black_and_white :
		(name == "Manga") ? manga :
		null;
}


/**
 * Returns reference to MetadataTag at index i, returns null when index is out of bounds,
 * null.isNull() will return true. Can not be used to get path or page_list members.
 */
MetadataTag& ComicInfo::operator [](const int i) {
	switch(i) {
		case 0: return title;
		case 1: return series;
		case 2: return number;
		case 3: return count;
		case 4: return volume;
		case 5: return alt_series;
		case 6: return alt_number;
		case 7: return alt_count;
		case 8: return summary;
		case 9: return notes;
		case 10: return year;
		case 11: return month;
		case 12: return writer;
		case 13: return penciller;
		case 14: return inker;
		case 15: return colorist;
		case 16: return letterer;
		case 17: return cover_artist;
		case 18: return editor;
		case 19: return publisher;
		case 20: return imprint;
		case 21: return genre;
		case 22: return web;
		case 23: return page_count;
		case 24: return language_iso;
		case 25: return format;
		case 26: return black_and_white;
		case 27: return manga;
		default: return null;
	}
}


/**
 * Returns reference to MetadataTag with name, returns null if name is not found,
 * null.isNull() will return true. Can not be used to get path or page_list members.
 */
MetadataTag& ComicInfo::operator [](const QString &name) {
	return
		(name == "Title") ? title :
		(name == "Series") ? series :
		(name == "Number") ? number :
		(name == "Count") ? count :
		(name == "Volume") ? volume :
		(name == "AlternateSeries") ? alt_series :
		(name == "AlternateNumber") ? alt_number :
		(name == "AlternateCount") ? alt_count :
		(name == "Summary") ? summary :
		(name == "Notes") ? notes :
		(name == "Year") ? year :
		(name == "Month") ? month :
		(name == "Writer") ? writer :
		(name == "Penciller") ? penciller :
		(name == "Inker") ? inker :
		(name == "Colorist") ? colorist :
		(name == "Letter") ? letterer :
		(name == "CoverArtist") ? cover_artist :
		(name == "Editor") ? editor :
		(name == "Publisher") ? publisher :
		(name == "Imprint") ? imprint :
		(name == "Genre") ? genre :
		(name == "Web") ? web :
		(name == "PageCount") ? page_count :
		(name == "LanguageISO") ? language_iso :
		(name == "Format") ? format :
		(name == "BlackAndWhite") ? black_and_white :
		(name == "Manga") ? manga :
		null;
}


/**
 * Compares each value in ComicInfo objects, returns true if all match, false if not.
 */
bool ComicInfo::operator ==(const ComicInfo &info) const {
	// Loop through MetadataTags comparing values
	for(int i = 0; !this->at(i).isNull(); i++) {
		if(this->at(i).getValue() != info.at(i).getValue()) {
			return false;
		}
	}

	// Compare number of pages
	if(this->page_list.size() != info.page_list.size()) {
		return false;
	}

	// Loop through pages comparing values
	for(int i = 0; i < this->page_list.size(); i++) {
		const Page &page_lhs = this->page_list.at(i);
		const Page &page_rhs = info.page_list.at(i);
		if(
			page_lhs.getImage() != page_rhs.getImage() ||
			page_lhs.getType() != page_rhs.getType() ||
			page_lhs.getDoublePage() != page_rhs.getDoublePage() ||
			page_lhs.getImageSize() != page_rhs.getImageSize() ||
			page_lhs.getKey() != page_rhs.getKey() ||
			page_lhs.getImageWidth() != page_rhs.getImageWidth() ||
			page_lhs.getImageHeight() != page_rhs.getImageHeight()
		) {
			return false;
		}
	}

	// If false hasn't returned by now, then all values must match
	return true;
}


/**
 * Copies all metadata values, but leaves parent untouched.
 */
ComicInfo & ComicInfo::operator =(const ComicInfo &info) {
	// First copy all MetadataTags
	for(int i = 0; i < size(); i++) {
		(*this)[i] = info.at(i);
	}

	// next copy PageList
	page_list = info.page_list;

	return *this;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									COMICINFO PRIVATE METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void ComicInfo::connectSignals() {
	for(int i = 0; i < size(); i++) {
		connect(&(*this)[i], SIGNAL(changed(const MetadataTag &, const QString)), this,
			SIGNAL(tagChanged(const MetadataTag &, const QString)));
	}
}