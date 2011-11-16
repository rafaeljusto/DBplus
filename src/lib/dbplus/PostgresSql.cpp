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

#include <boost/lexical_cast.hpp>

#include <dbplus/DatabaseException.hpp>
#include <dbplus/PostgresSql.hpp>
#include <dbplus/PostgresSqlResult.hpp>

//#define SHOW_NOTICES

DBPLUS_NS_BEGIN

static void noticeReceiver(void *arg, const PGresult *result)
{
#ifdef SHOW_NOTICES
	printf("%s\n", PQresultErrorField(result, PG_DIAG_MESSAGE_PRIMARY));
#endif
}

PostgresSql::PostgresSql() :
	_transactionMode(AUTO_COMMIT)
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
		throw DatabaseException(DatabaseException::CONNECTION_ERROR,
		                        __FILE__, __FUNCTION__, __LINE__,
		                        "Could not allocate memory for "
		                        "database connection");
	}

	if (PQstatus(_postgres) == CONNECTION_BAD) {
		throw DatabaseException(DatabaseException::CONNECTION_ERROR,
		                        __FILE__, __FUNCTION__, __LINE__,
		                        PQerrorMessage(_postgres));
	}

	PQsetNoticeReceiver(_postgres, noticeReceiver, NULL);

	setTransactionMode(_transactionMode);
}

void PostgresSql::disconnect()
{
	PQfinish(_postgres);
}

void PostgresSql::setTransactionMode(const TransactionMode mode)
{
	_transactionMode = _transactionMode != mode ? mode : _transactionMode;

	switch(_transactionMode) {
	case MANUAL_COMMIT:
		if (_transactionMode != MANUAL_COMMIT) {
			execute("BEGIN");
		}
		break;
	case AUTO_COMMIT:
		commit();
		break;
	}
}

PostgresSql::TransactionMode PostgresSql::getTransactionMode() const
{
	return _transactionMode;
}

void PostgresSql::commit()
{
	execute("COMMIT");

	if (_transactionMode == MANUAL_COMMIT) {
		execute("BEGIN");
	}
}

void PostgresSql::rollback()
{
	execute("ROLLBACK");

	if (_transactionMode == MANUAL_COMMIT) {
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
                                             const ResultMode resultMode)
{
	PGresult *result = PQexec(_postgres, query.c_str());
	if (result == NULL) {
		throw DatabaseException(DatabaseException::RESULT_ERROR,
		                        __FILE__, __FUNCTION__, __LINE__,
		                        PQerrorMessage(_postgres));
	}

	if (PQresultStatus(result) != PGRES_TUPLES_OK &&
	    PQresultStatus(result) != PGRES_COMMAND_OK) {
		throw DatabaseException(DatabaseException::EXECUTION_ERROR,
		                        __FILE__, __FUNCTION__, __LINE__,
		                        PQresultErrorMessage(result));
	}

	// TODO: result mode

	return std::shared_ptr<Result>(new PostgresSqlResult(result));
}

unsigned long long PostgresSql::affectedRows()
{
	// TODO: Maybe in postgres the affected rows is always stored in the
	// result set!
	return 0;
}

unsigned long long PostgresSql::lastInsertedId()
{
	// TODO! Use currval :
	// http://www.postgresql.org/docs/current/static/functions-sequence.html
	return 0;
}

DBPLUS_NS_END
