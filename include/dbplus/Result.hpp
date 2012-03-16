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

#ifndef __DB_PLUS_RESULT_HPP__
#define __DB_PLUS_RESULT_HPP__

#include <list>
#include <map>
#include <string>

#include <boost/any.hpp>

#include <dbplus/DatabaseException.hpp>
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
	/*! Returns the number of rows found in result.
	 *
	 * @return Number of rows in result
	 */
	virtual unsigned int size() const = 0;

	/*! Move to the next row.
	 *
	 * @return True if there's a next row, false otherwise
	 */
	virtual bool fetch() = 0;

	/*! Returns the value of a given column name.
	 *
	 * @param key Column name
	 * @return column Value in the current row
	 */
	virtual boost::any get(const string &key) const = 0;

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
		try {
			return converter(get(key));
		} catch (boost::bad_any_cast) {
			throw DATABASE_EXCEPTION(DatabaseException::CONVERSION_ERROR, 
			                         "Conversion error. "
			                         "Data is in a different format");
		}
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

protected:
	std::map<string, boost::any> _row;
};

DBPLUS_NS_END

#endif // __DB_PLUS_RESULT_HPP__
