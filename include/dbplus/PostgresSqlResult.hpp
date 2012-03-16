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

#ifndef __DB_PLUS_POSTGRES_SQL_RESULT_HPP__
#define __DB_PLUS_POSTGRES_SQL_RESULT_HPP__

extern "C" {
#include <postgresql/libpq-fe.h>
}

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
	 *
	 * @param result Result set with the raw data
	 * @param types List of know types
	 */
	explicit PostgresSqlResult(PGresult *result,
	                           const std::map<Oid, string> &types);

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
	 * @todo Store result according to the column type
	 */
	bool fetch();
	
	/*! Returns the value of a given column name.
	 *
	 * @param key Column name
	 * @return column Value in the current row
	 * @throw DatabaseException on error
	 */
	boost::any get(const string &key) const;
	
private:
	PGresult *_result;
	int _currentRow;
	std::map<Oid, string> _types;
};

DBPLUS_NS_END

#endif // __DB_PLUS_POSTGRES_SQL_RESULT_HPP__
