#include "MetadataTag.hpp"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *									METADATATAG PUBLIC METHODS 									 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


MetadataTag::MetadataTag(const MetadataTag &tag) : QObject() {
	*this = tag;
}


/**
 * Initializes MetadataTag with name, value remains empty and null.
 */
MetadataTag::MetadataTag(const QString &_name, QObject *parent) : QObject(parent), name(_name) {}


/**
 * Initializes MetadataTag with name and value.
 */
MetadataTag::MetadataTag(const QString &_name, const QString &_value, QObject *parent)
		: QObject(parent), name(_name), value(_value) {}


/**
 * MetadataTag getter methods.
 */
QString MetadataTag::getName() const { return name; }
QString MetadataTag::getValue() const { return value; }


void MetadataTag::setValue(const QString &_value) {
	QString old_value(value);
	value = _value;
	emit changed(*this, old_value);
}

/**
 * If name is null, then tag is null.
 */
bool MetadataTag::isNull() const { return name.isNull(); }


MetadataTag & MetadataTag::operator=(const MetadataTag &tag) {
	name	=	tag.name;
	value	=	tag.value;

	return *this;
}