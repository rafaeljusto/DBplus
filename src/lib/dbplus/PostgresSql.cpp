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

#include <boost/lexical_cast.hpp>

#include <dbplus/DatabaseException.hpp>
#include <dbplus/PostgresSql.hpp>
#include <dbplus/PostgresSqlResult.hpp>

//#define SHOW_NOTICES

DBPLUS_NS_BEGIN

PostgresSql::PostgresSql() :
	_transactionMode(TransactionMode::AUTO_COMMIT),
	_affectedRows(0)
{
}

PostgresSql::~PostgresSql()
{
	disconnect();
}

void PostgresSql::connect(const string &database,
                          const string &user,
                          const string &password,
                          const string &server,
                          const unsigned int port)
{
	string connection = "host='" + server + "' "
		"port='" + boost::lexical_cast<string>(port) + "' "
		"dbname='" + database + "' "
		"user='" + user + "' "
		"password='" + password + "'";

	_postgres = PQconnectdb(connection.c_str());
	if (_postgres == NULL) {
		throw DATABASE_EXCEPTION(DatabaseException::CONNECTION_ERROR,
		                         "Could not allocate memory for "
		                         "database connection");
	}

	if (PQstatus(_postgres) == CONNECTION_BAD) {
		throw DATABASE_EXCEPTION(DatabaseException::CONNECTION_ERROR,
		                         PQerrorMessage(_postgres));
	}

	PQsetNoticeReceiver(_postgres, noticeReceiver, NULL);

	setTransactionMode(_transactionMode);
}

void PostgresSql::disconnect()
{
	PQfinish(_postgres);
}

void PostgresSql::setTransactionMode(const TransactionMode::Value mode)
{
	_transactionMode = _transactionMode != mode ? mode : _transactionMode;

	switch(_transactionMode) {
	case TransactionMode::MANUAL_COMMIT:
		execute("BEGIN");
		break;
	case TransactionMode::AUTO_COMMIT:
		commit();
		break;
	}
}

PostgresSql::TransactionMode::Value PostgresSql::getTransactionMode() const
{
	return _transactionMode;
}

void PostgresSql::commit()
{
	execute("COMMIT");

	if (_transactionMode == TransactionMode::MANUAL_COMMIT) {
		execute("BEGIN");
	}
}

void PostgresSql::rollback()
{
	execute("ROLLBACK");

	if (_transactionMode == TransactionMode::MANUAL_COMMIT) {
		execute("BEGIN");
	}
}

string PostgresSql::escape(const string &value)
{
	char escapedValue[value.size() + 1];
	PQescapeStringConn(_postgres, escapedValue, 
	                   value.c_str(), value.size(), NULL);
	return static_cast<string>(escapedValue);
}

std::shared_ptr<Result> PostgresSql::execute(const string &query,
                                             const ResultMode::Value resultMode)
{
	PGresult *result = PQexec(_postgres, query.c_str());
	if (result == NULL) {
		throw DATABASE_EXCEPTION(DatabaseException::RESULT_ERROR,
		                         PQerrorMessage(_postgres));
	}

	if (PQresultStatus(result) != PGRES_TUPLES_OK &&
	    PQresultStatus(result) != PGRES_COMMAND_OK) {
		throw DATABASE_EXCEPTION(DatabaseException::EXECUTION_ERROR,
		                         PQresultErrorMessage(result));
	}

	string affectedRows = PQcmdTuples(result);
	if (affectedRows.empty()) {
		_affectedRows = 0;
	} else {
		_affectedRows = boost::lexical_cast<unsigned int>(affectedRows);
	}

	// TODO: result mode

	return std::shared_ptr<Result>(new PostgresSqlResult(result));
}

unsigned long long PostgresSql::affectedRows()
{
	return _affectedRows;
}

unsigned long long PostgresSql::lastInsertedId()
{
	// TODO!
	// http://archives.postgresql.org/pgsql-interfaces/2004-07/msg00035.php
	
	// Use currval:
	// http://www.postgresql.org/docs/current/static/functions-sequence.html
	
	// OR
	
	// Use PQoidValue:
	// http://www.postgresql.org/docs/8.2/static/libpq-exec.html
	
	return 0;
}

void PostgresSql::noticeReceiver(void *arg, const PGresult *result)
{
#ifdef SHOW_NOTICES
	printf("%s\n", PQresultErrorField(result, PG_DIAG_MESSAGE_PRIMARY));
#endif
}

DBPLUS_NS_END
