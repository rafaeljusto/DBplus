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
#ifdef SHOW_NOTICES
#include <iostream>
#endif

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
	buildTypesCache();

	PGresult *result = PQexec(_postgres, query.c_str());
	if (result == NULL) {
		throw DATABASE_EXCEPTION(DatabaseException::RESULT_ERROR,
		                         PQerrorMessage(_postgres));
	}

	if (PQresultStatus(result) != PGRES_TUPLES_OK &&
	    PQresultStatus(result) != PGRES_COMMAND_OK) {
		PQclear(result);
		throw DATABASE_EXCEPTION(DatabaseException::EXECUTION_ERROR,
		                         PQresultErrorMessage(result));
	}

	string affectedRows = PQcmdTuples(result);
	if (affectedRows.empty()) {
		_affectedRows = 0;
	} else {
		_affectedRows = boost::lexical_cast<unsigned int>(affectedRows);
	}

	// TODO: Result mode. We can use cursor, but we neede to do it in a
	// transaction and we must close the cursor after usage. How we are
	// going to know where we close the cursor?
	//
	// http://www.postgresql.org/docs/8.0/static/libpq-example.html

	return std::shared_ptr<Result>(new PostgresSqlResult(result, _types));
}

unsigned long long PostgresSql::affectedRows()
{
	return _affectedRows;
}

unsigned long long PostgresSql::lastInsertedId()
{
	string query = "SELECT lastval()";
	std::shared_ptr<Result> result = execute(query);
	
	if (result->fetch() == false) {
		return 0;
	}

	return boost::any_cast<long long>(result->get("lastval"));
}

void PostgresSql::buildTypesCache()
{
	if (_types.empty() == false) {
		return;
	}

	string query = "SELECT typelem, typname FROM pg_type";

	PGresult *result = PQexec(_postgres, query.c_str());
	if (result == NULL) {
		throw DATABASE_EXCEPTION(DatabaseException::RESULT_ERROR,
		                         PQerrorMessage(_postgres));
	}

	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
		PQclear(result);
		throw DATABASE_EXCEPTION(DatabaseException::EXECUTION_ERROR,
		                         PQresultErrorMessage(result));
	}

	for (int i = 0; i < PQntuples(result); i++) {
		Oid oid = boost::lexical_cast<Oid>(PQgetvalue(result, i, 0));
		string type = PQgetvalue(result, i, 1);

		_types[oid] = type;
	}

	PQclear(result);
}

void PostgresSql::noticeReceiver(void *arg, const PGresult *result)
{
#ifdef SHOW_NOTICES
	std::cout << PQresultErrorField(result, PG_DIAG_MESSAGE_PRIMARY) << std::endl;
#endif
}

DBPLUS_NS_END
