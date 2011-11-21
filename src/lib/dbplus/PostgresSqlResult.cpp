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

#include <dbplus/DatabaseException.hpp>
#include <dbplus/PostgresSqlResult.hpp>

DBPLUS_NS_BEGIN

PostgresSqlResult::PostgresSqlResult(PGresult *result) :
	_result(result),
	_currentRow(-1)
{
}

PostgresSqlResult::~PostgresSqlResult()
{
	PQclear(_result);
}

unsigned int PostgresSqlResult::size() const
{
	string size = PQcmdTuples(_result);
	if (size.empty()) {
		return 0;
	}

	return boost::lexical_cast<unsigned int>(size);
}

bool PostgresSqlResult::fetch()
{
	_row.clear();
	_currentRow++;

	if (static_cast<unsigned int>(_currentRow) >= size()) {
		return false;
	}

	unsigned int numberOfFields = PQnfields(_result);
	for (unsigned int i = 0; i < numberOfFields; i++) {
		_row[PQfname(_result, i)] = 
			static_cast<string>(PQgetvalue(_result, _currentRow, i));
	}

	return true;
}

string PostgresSqlResult::get(const string &key) const
{
	auto result = _row.find(key);
	if (result == _row.end()) {
		throw DatabaseException(DatabaseException::UNKNOW_KEY_ERROR,
		                        __FILE__, __FUNCTION__, __LINE__,
		                        "Column " + key + " not found in result set");
	}

	return result->second;
}

DBPLUS_NS_END
