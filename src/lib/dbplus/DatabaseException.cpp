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

#include <boost/lexical_cast.hpp>

#include <dbplus/DatabaseException.hpp>

DBPLUS_NS_BEGIN

DatabaseException::DatabaseException(const Code code,
                                     const string &file,
                                     const string &function,
                                     const int line,
                                     const string &message) throw()
{
	_message = "[DatabaseException] In file " + file + " "
		"at line " + boost::lexical_cast<string>(line) + ": "
		"[" + boost::lexical_cast<string>(code) + "] " + message;
}

DatabaseException::~DatabaseException() throw()
{
}

const char* DatabaseException::what() const throw()
{
	return _message.c_str();
}

DBPLUS_NS_END
