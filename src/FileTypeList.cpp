#include "FileTypeList.hpp"


/**
 * Returns true if file_name contains extension from list, otherwise returns false.
 */
bool FileTypeList::isSupported(const QString &file_name) const {
	for(int i = 0; i < this->size(); i++) {
		if(file_name.endsWith(this->at(i), Qt::CaseInsensitive)) {
			return true;
		}
	}

	return false;
}