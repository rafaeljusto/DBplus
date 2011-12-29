/*
  DBplus Copyright (C) 2011 Rafael Dantas Justo

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

#include <dbplus/Dbplus.hpp>

DBPLUS_NS_BEGIN

class Binary
{
public:
	Binary(const unsigned char *data, const unsigned long dataSize);

	unsigned char* getData() const;
	unsigned long getDataSize() const;

private:
	unsigned char *_data;
	unsigned long _dataSize;
};

DBPLUS_NS_END

#endif // __DB_PLUS_BINARY_HPP__
