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

#include <cstring>

#include <dbplus/Binary.hpp>

DBPLUS_NS_BEGIN

Binary::Binary(const unsigned char *data, const unsigned long size) :
	_data(0),
	_size(size)
{
	_data = new unsigned char[_size];
	memcpy(_data, data, size);
}

Binary::Binary(const string &data) :
	_data(0),
	_size(data.size())
{
	_data = new unsigned char[_size];
	memcpy(_data, data.c_str(), data.size());
}

Binary::Binary(const Binary &binary) :
	_data(0),
	_size(0)
{
	*this = binary;
}

Binary::~Binary()
{
	delete[] _data;
}

Binary& Binary::operator=(const Binary &binary)
{
	delete[] _data;

	_size = binary._size;
	_data = new unsigned char[_size];
	memcpy(_data, binary._data, binary._size);

	return *this;
}

bool Binary::operator==(const Binary &binary) const
{
	if (_size != binary._size) {
		return false;
	}

	for (unsigned long i = 0; i < _size; i++) {
		if (_data[i] != binary._data[i]) {
			return false;
		}
	}

	return true;
}

unsigned char* Binary::getData() const
{
	return _data;
}

unsigned long Binary::getSize() const
{
	return _size;
}

DBPLUS_NS_END
