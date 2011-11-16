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

#ifndef __DB_PLUS_RESULT_HPP__
#define __DB_PLUS_RESULT_HPP__

#include <string>

#include <dbplus/Dbplus.hpp>

using std::string;

DBPLUS_NS_BEGIN

/*! \class Result
 *  \brief Store result of database queries (interface).
 *
 * Base class to store results of a database query.
 */
class Result
{
public:
	virtual unsigned int size() const = 0;
	virtual bool fetch() = 0;
	virtual string get(const string &key) const = 0;
};

DBPLUS_NS_END

#endif // __DB_PLUS_RESULT_HPP__
