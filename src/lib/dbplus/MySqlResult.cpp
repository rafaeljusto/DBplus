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

#include <dbplus/Binary.hpp>
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
	//unsigned long *lengths = mysql_fetch_lengths(_result);

	for (unsigned int i = 0; i < numberOfFields; i++) {
		MYSQL_FIELD *field = mysql_fetch_field_direct(_result, i);
		if (field->flags & BINARY_FLAG) {
			//Binary data(reinterpret_cast<unsigned char*>(row[i]), lengths[i]);
			//_row[field->name] = data;

			if (row[i] != NULL) {
				_row[field->name] = static_cast<string>(row[i]);
			} else {
				_row[field->name] = string("");
			}

		} else if (field->flags & NUM_FLAG) {
			if (row[i] != NULL) {
				_row[field->name] = boost::lexical_cast<int>(row[i]);
			} else {
				_row[field->name] = 0;
			}

		} else {
			if (row[i] != NULL) {
				_row[field->name] = static_cast<string>(row[i]);
			} else {
				_row[field->name] = string("");
			}
		}
	}

	return true;
}

boost::any MySqlResult::get(const string &key) const
{
	auto result = _row.find(key);
	if (result == _row.end()) {
		throw DATABASE_EXCEPTION(DatabaseException::UNKNOW_KEY_ERROR, 
		                         "Column " + key + " not found in result set");
	}

	return result->second;
}

DBPLUS_NS_END
