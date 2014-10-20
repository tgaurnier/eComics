#include "Page.hpp"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *										PAGE PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Copy constructor.
 */
Page::Page(const Page &page) {
	*this = page;
}


/**
 * Initialize page with image number (index number of image in comic file)
 */
Page::Page(const QString &_image) {
	image = MetadataTag("Image");
	type = MetadataTag("Type");
	double_page = MetadataTag("DoublePage");
	image_size = MetadataTag("ImageSize");
	key = MetadataTag("Key");
	image_width = MetadataTag("ImageWidth");
	image_height = MetadataTag("ImageHeight");

	image.setValue(_image);
}


/**
 * Page basic getters
 */
QString Page::getImage() const { return image.getValue(); }
QString Page::getType() const { return type.getValue(); }
QString Page::getDoublePage() const { return double_page.getValue(); }
QString Page::getImageSize() const { return image_size.getValue(); }
QString Page::getKey() const { return key.getValue(); }
QString Page::getImageWidth() const { return image_width.getValue(); }
QString Page::getImageHeight() const { return image_height.getValue(); }


/**
 * Page basic setters
 */
void Page::setImage(const QString &value) { image.setValue(value); }
void Page::setType(const QString &value) { type.setValue(value); }
void Page::setDoublePage(const QString &value) { double_page.setValue(value); }
void Page::setImageSize(const QString &value) { image_size.setValue(value); }
void Page::setKey(const QString &value) { key.setValue(value); }
void Page::setImageWidth(const QString &value) { image_width.setValue(value); }
void Page::setImageHeight(const QString &value) { image_height.setValue(value); }


Page & Page::operator=(const Page &page) {
	image			=	page.image;
	type			=	page.type;
	double_page		=	page.double_page;
	image_size		=	page.image_size;
	key				=	page.key;
	image_width		=	page.image_width;
	image_height	=	page.image_height;

	return *this;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									PAGELIST PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Returns the index of page with type "FrontCover". If no pages have a type of "FrontCover" then
 * just return 0 (we just assume the first image is the front cover).
 */
int PageList::getFrontCover() const {
	// Loop through pages checking for a FrontCover page
	for(int i = 0; i < size(); i++) {
		if((*this)[i].getType() == "FrontCover") {
			return i;
		}
	}

	// If none of the pages have a type of FrontCover,
	return 0;
}