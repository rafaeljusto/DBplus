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
#include <dbplus/MySql.hpp>
#include <dbplus/MySqlResult.hpp>

DBPLUS_NS_BEGIN

MySql::MySql() :
	_transactionMode(TransactionMode::AUTO_COMMIT)
{
}

MySql::~MySql()
{
	disconnect();
}

void MySql::connect(const string &database,
                    const string &user,
                    const string &password,
                    const string &server,
                    const unsigned int port)
{
	if (mysql_init(&_mysql) == NULL) {
		throw DATABASE_EXCEPTION(DatabaseException::CONNECTION_ERROR, 
		                         mysql_error(&_mysql));
	}

	if (mysql_real_connect(&_mysql,
	                       server.c_str(), 
	                       user.c_str(), 
	                       password.c_str(), 
	                       database.c_str(), 
	                       port, NULL, 0) != &_mysql) {
		throw DATABASE_EXCEPTION(DatabaseException::CONNECTION_ERROR, 
		                         mysql_error(&_mysql));
	}

	setTransactionMode(_transactionMode);
}

void MySql::disconnect()
{
	mysql_close(&_mysql);
}

void MySql::setTransactionMode(const TransactionMode::Value mode)
{
	_transactionMode = _transactionMode != mode ? mode : _transactionMode;

	switch(_transactionMode) {
	case TransactionMode::MANUAL_COMMIT:
		if (mysql_autocommit(&_mysql, 0) != 0) {
			throw DATABASE_EXCEPTION(DatabaseException::STATE_CHANGE_ERROR, 
			                         mysql_error(&_mysql));
		}
		break;
	case TransactionMode::AUTO_COMMIT:
		if (mysql_autocommit(&_mysql, 1) != 0) {
			throw DATABASE_EXCEPTION(DatabaseException::STATE_CHANGE_ERROR, 
			                         mysql_error(&_mysql));
		}
		break;
	}
}

MySql::TransactionMode::Value MySql::getTransactionMode() const
{
	return _transactionMode;
}

void MySql::commit()
{
	if (mysql_commit(&_mysql) != 0) {
		throw DATABASE_EXCEPTION(DatabaseException::TRANSACTION_ERROR, 
		                         mysql_error(&_mysql));
	}
}

void MySql::rollback()
{
	if (mysql_rollback(&_mysql) != 0) {
		throw DATABASE_EXCEPTION(DatabaseException::TRANSACTION_ERROR, 
		                         mysql_error(&_mysql));
	}
}

string MySql::escape(const string &value)
{
	char escapedValue[value.size() + 1];
	mysql_real_escape_string(&_mysql, escapedValue, value.c_str(), value.size());
	return static_cast<string>(escapedValue);
}

std::shared_ptr<Result> MySql::execute(const string &query, 
                                       const ResultMode::Value resultMode)
{
	if (mysql_real_query(&_mysql, query.c_str(), query.size()) != 0) {
		throw DATABASE_EXCEPTION(DatabaseException::EXECUTION_ERROR, 
		                         mysql_error(&_mysql));
	}

	if (mysql_field_count(&_mysql) == 0) {
		return std::shared_ptr<Result>();
	}

	MYSQL_RES *result = NULL;
	
	switch(resultMode) {
	case ResultMode::USE_RESULT:
		result = mysql_use_result(&_mysql);
		break;
	case ResultMode::STORE_RESULT:
		result = mysql_store_result(&_mysql);
		break;
	}

	if (result == NULL) {
		throw DATABASE_EXCEPTION(DatabaseException::RESULT_ERROR, 
		                         mysql_error(&_mysql));
	}

	return std::shared_ptr<Result>(new MySqlResult(result));
}

unsigned long long MySql::affectedRows()
{
	return mysql_affected_rows(&_mysql);
}

unsigned long long MySql::lastInsertedId()
{
	return mysql_insert_id(&_mysql);
}

DBPLUS_NS_END
