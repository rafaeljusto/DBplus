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

#ifndef __DB_PLUS_POSTGRES_SQL_RESULT_HPP__
#define __DB_PLUS_POSTGRES_SQL_RESULT_HPP__

extern "C" {
#include <postgresql/libpq-fe.h>
}

#include <list>
#include <map>

#include <boost/lexical_cast.hpp>

#include <dbplus/Dbplus.hpp>

#include "Result.hpp"

DBPLUS_NS_BEGIN

/*! \class PostgresSqlResult
 *  \brief Store result of database queries (interface).
 *
 * Base class to store results of a database query.
 */
class PostgresSqlResult : public Result
{
public:
	/*! Constructor receives the raw postgreSQL structure already
	 * containing the result.
	 */
	explicit PostgresSqlResult(PGresult *result);

	/*! Release memory from raw PostgreSQL structures.
	 */
	~PostgresSqlResult();

	/*! Returns the number of rows found in result.
	 *
	 * @return Number of rows in result
	 */
	unsigned int size() const;

	/*! Move to the next row.
	 *
	 * @return True if there's a next row, false otherwise
	 */
	bool fetch();
	
	/*! Returns the value of a given column name.
	 *
	 * @param key Column name
	 * @return column Value in the current row
	 * @throw DatabaseException on error
	 */
	boost::any get(const string &key) const;
	
	/*! Find and convert the column data into some type.
	 *
	 * @tparam T Type of the data that is going to be returned
	 * @param key Column name
	 * @param converter Method that converts into the desired type
	 * @return Column value in the desired format
	 * @throw DatabaseException on error
	 */
	template<class T> 
	T get(const string &key, T (*converter)(const boost::any&) = 
	      boost::any_cast<T>) const
	{
		return converter(get(key));
	}

	/*! Converts an entire line in one object.
	 *
	 * @tparam T Type of the object that represents the result row
	 * @param converter Method that converts into the desired type
	 * @return Desired object that represents the result row
	 * @throw DatabaseException on error
	 */
	template<class T> 
	T get(T (*converter)(std::map<string, boost::any>)) const
	{
		return converter(_row);
	}

	/*! Converts all rows into a list of objects.
	 *
	 * @tparam T Type of the object that represents the result row
	 * @param converter Method that converts into the desired type
	 * @return List of the desired object that represents the result set
	 * @throw DatabaseException on error
	 */
	template<class T> 
	std::list<T> getAll(T (*converter)(std::map<string, boost::any>))
	{
		std::list<T> results;
		
		while(fetch()) {
			results.push_back(get<T>(converter));
		}

		return results;
	}

private:
	PGresult *_result;
	int _currentRow;
	std::map<string, boost::any> _row;
};

DBPLUS_NS_END

#endif // __DB_PLUS_POSTGRES_SQL_RESULT_HPP__
