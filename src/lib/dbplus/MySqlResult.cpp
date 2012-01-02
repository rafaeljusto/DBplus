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

#include <cstdint>

#include <boost/date_time/posix_time/posix_time.hpp>

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
	unsigned long *lengths = mysql_fetch_lengths(_result);

	for (unsigned int i = 0; i < numberOfFields; i++) {
		if (row[i] == NULL) {
			continue;
		}

		MYSQL_FIELD *field = mysql_fetch_field_direct(_result, i);

		switch (field->type) {
		case MYSQL_TYPE_TINY:
			_row[field->name] = boost::lexical_cast<uint8_t>(row[i]);
			break;
		case MYSQL_TYPE_SHORT:
			_row[field->name] = boost::lexical_cast<short>(row[i]);
			break;
		case MYSQL_TYPE_LONG:
			_row[field->name] = boost::lexical_cast<long>(row[i]);
			break;
		case MYSQL_TYPE_INT24:
			_row[field->name] = boost::lexical_cast<uint32_t>(row[i]);
			break;
		case MYSQL_TYPE_LONGLONG:
			_row[field->name] = boost::lexical_cast<long long>(row[i]);
			break;
		case MYSQL_TYPE_DECIMAL:
			// TODO
			break;
		case MYSQL_TYPE_NEWDECIMAL:
			// TODO
			break;
		case MYSQL_TYPE_FLOAT:
			_row[field->name] = boost::lexical_cast<float>(row[i]);
			break;
		case MYSQL_TYPE_DOUBLE:
			_row[field->name] = boost::lexical_cast<double>(row[i]);
			break;
		case MYSQL_TYPE_BIT:
			// TODO
			break;
		case MYSQL_TYPE_TIMESTAMP:
			// TODO
			break;
		case MYSQL_TYPE_DATE:
			// TODO
			break;
		case MYSQL_TYPE_NEWDATE:
			// TODO
			break;
		case MYSQL_TYPE_TIME:
			// TODO
			break;
		case MYSQL_TYPE_DATETIME:
			_row[field->name] = boost::posix_time::time_from_string(row[i]);
			break;
		case MYSQL_TYPE_YEAR:
			_row[field->name] = boost::lexical_cast<int>(row[i]);
			break;
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_VARCHAR:
			_row[field->name] = static_cast<string>(row[i]);
			break;
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			_row[field->name] =
				Binary(reinterpret_cast<unsigned char*>(row[i]), lengths[i]);;
			break;
		case MYSQL_TYPE_SET:
			// TODO
			break;
		case MYSQL_TYPE_ENUM:
			// TODO
			break;
		case MYSQL_TYPE_GEOMETRY:
			// TODO
			break;
		case MYSQL_TYPE_NULL:
			// TODO
			break;
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
