/*
  DBplus Copyright (C) 2012 Rafael Dantas Justo

  This file is part of DBplus.

  DBplus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  DBplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DBplus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DB_PLUS_BINARY_HPP__
#define __DB_PLUS_BINARY_HPP__

#include <string>

#include <dbplus/Dbplus.hpp>

using std::string;

DBPLUS_NS_BEGIN

/*! \class Binary
 *  \brief Container to store data/size
 *
 * Class to store binary data.
 */
class Binary
{
public:
	/*! Constructor.
	 *
	 * @param data Binary data
	 * @param size Binary data size
	 */
	Binary(const unsigned char *data, const unsigned long size);

	/*! Constructor. Used to create a binary data from a string.
	 *
	 * @param data Binary data
	 */
	Binary(const string &data);

	/*! Copy constructor.
	 *
	 * @param binary Other Binary object
	 */
	Binary(const Binary &binary);

	/*! Destructor. Free internal array.
	 */
	~Binary();

	/*! Assignment operator. Used to copy another Binary object.
	 *
	 * @param binary Other Binary object
	 * @return The current Binary object
	 */
	Binary& operator=(const Binary &binary);

	/*! Compare two Binary objects
	 *
	 * @param binary Other Binary object
	 * @return True if the Binary objects are equal or false otherwise
	 */
	bool operator==(const Binary &binary) const;

	/*! Returns internal data array
	 *
	 * @return Internal data array
	 */
	unsigned char* getData() const;

	/*! Returns internal data array size
	 *
	 * @return Internal data array size
	 */
	unsigned long getSize() const;

private:
	unsigned char *_data;
	unsigned long _size;
};

DBPLUS_NS_END

#endif // __DB_PLUS_BINARY_HPP__
