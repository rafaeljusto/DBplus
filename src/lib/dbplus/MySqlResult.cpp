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
#include <dbplus/MySqlResult.hpp>

DBPLUS_NS_BEGIN

MySqlResult::MySqlResult(MYSQL_RES *result) :
	_result(result)
{
}

MySqlResult::~MySqlResult()
{
	mysql_free_result(_result);
}

unsigned int MySqlResult::size() const
{
	return mysql_num_rows(_result);
}

bool MySqlResult::fetch()
{
	_row.clear();

	MYSQL_ROW row = mysql_fetch_row(_result);
	if (row == NULL) {
		return false;
	}

	unsigned int numberOfFields = mysql_num_fields(_result);
	for (unsigned int i = 0; i < numberOfFields; i++) {
		MYSQL_FIELD *field = mysql_fetch_field_direct(_result, i);
		// TODO: You cannot treat these as null-terminated strings if
		// field values may contain binary data, because such values may
		// contain null bytes internally
		_row[field->name] = static_cast<string>(row[i]);
	}

	return true;
}

string MySqlResult::get(const string &key) const
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
