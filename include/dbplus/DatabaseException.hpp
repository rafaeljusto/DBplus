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

#ifndef __DB_PLUS_DATABASE_EXCEPTION_HPP__
#define __DB_PLUS_DATABASE_EXCEPTION_HPP__

#include <exception>
#include <string>

#include <dbplus/Dbplus.hpp>

using std::string;

DBPLUS_NS_BEGIN

/*! \class DatabaseException
 *  \brief DBplus exception class
 *
 * Class created to be throw whenever a fatal error occurs.
 */
class DatabaseException : public std::exception
{
public:
	enum Code {
		CONNECTION_ERROR,
		EXECUTION_ERROR,
		RESULT_ERROR,
		STATE_CHANGE_ERROR,
		TRANSACTION_ERROR,
		UNKNOW_KEY_ERROR
	};

	DatabaseException(const Code code, 
	                  const string &file, 
	                  const string &function, 
	                  const int line, 
	                  const string &message) throw();
	~DatabaseException() throw();

	const char* what() const throw();

private:
	string _message;
};

DBPLUS_NS_END

#endif // __DB_PLUS_DATABASE_EXCEPTION_HPP__
