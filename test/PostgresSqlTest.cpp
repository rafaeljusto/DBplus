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

#include <list>
#include <map>
#include <memory>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include <dbplus/DatabaseException.hpp>
#include <dbplus/PostgresSql.hpp>
#include <dbplus/PostgresSqlResult.hpp>

using std::list;
using std::map;
using std::shared_ptr;

using boost::any;
using boost::any_cast;
using boost::lexical_cast;
using boost::posix_time::ptime;
using boost::posix_time::time_from_string;

using dbplus::DatabaseException;
using dbplus::PostgresSql;
using dbplus::PostgresSqlResult;

// When you need to run only one test, compile only this file with the
// STAND_ALONE flag.
#ifdef STAND_ALONE
#define BOOST_TEST_MODULE DBplus
#endif

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(dbplusPostgresTests)

BOOST_AUTO_TEST_CASE(mustConnectToDatabase)
{
	PostgresSql postgres;
	BOOST_CHECK_NO_THROW(postgres.connect("dbplus", "root", 
	                                      "abc123", "127.0.0.1"));
}

BOOST_AUTO_TEST_CASE(mustNotConnectToDatabase)
{
	PostgresSql postgres;
	BOOST_CHECK_THROW(postgres.connect("dbplus", "root", "abc321", "127.0.0.1"),
	                  DatabaseException);
}

BOOST_AUTO_TEST_CASE(mustExecuteWithoutErrors)
{
	PostgresSql postgres;
	postgres.connect("dbplus", "root", "abc123", "127.0.0.1");

	string sql = "DROP TABLE IF EXISTS test";
	BOOST_CHECK_NO_THROW(postgres.execute(sql));

	sql = "CREATE TABLE test (id SERIAL PRIMARY KEY, "
		"value VARCHAR(255), date TIMESTAMP)";
	BOOST_CHECK_NO_THROW(postgres.execute(sql));
}

void createDatabaseAndTable(PostgresSql &postgres)
{
	postgres.connect("dbplus", "root", "abc123", "127.0.0.1");

	string sql = "DROP TABLE IF EXISTS test";
	postgres.execute(sql);

	sql = "CREATE TABLE test (id SERIAL PRIMARY KEY, "
		"value VARCHAR(255), date TIMESTAMP)";
	postgres.execute(sql);
}

BOOST_AUTO_TEST_CASE(mustInsertAndSelectData)
{
	PostgresSql postgres;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(postgres));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	BOOST_CHECK_EQUAL(postgres.affectedRows(), 1);

	sql = "SELECT id, value, date FROM test";
	shared_ptr<PostgresSqlResult> result =
		std::dynamic_pointer_cast<PostgresSqlResult>(postgres.execute(sql));

	BOOST_CHECK_EQUAL(result->size(), 1);

	while (result->fetch()) {
		long id = result->get<long>("id");
		string value = result->get<string>("value");
		ptime date = result->get<ptime>("date");

		BOOST_CHECK_EQUAL(id, 1);
		BOOST_CHECK_EQUAL(value, "This is a test");
		BOOST_CHECK_EQUAL(date, time_from_string("2011-11-11 11:11:11"));
	}
}

BOOST_AUTO_TEST_CASE(mustSelectAndBuildEachObject)
{
	PostgresSql postgres;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(postgres));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	class Object {
	public:
		Object() : id(0), value("") {}

		long id;
		string value;
		ptime date;
	};

	sql = "SELECT id, value, date FROM test";
	shared_ptr<PostgresSqlResult> result =
		std::dynamic_pointer_cast<PostgresSqlResult>(postgres.execute(sql));

	while (result->fetch()) {
		auto object = result->get<Object>([](map<string, any> row) {
				Object object;
				object.id = any_cast<long>(row["id"]);
				object.value = any_cast<string>(row["value"]);
				object.date = any_cast<ptime>(row["date"]);
				return object;
			});

		BOOST_CHECK_EQUAL(object.id, 1);
		BOOST_CHECK_EQUAL(object.value, "This is a test");
		BOOST_CHECK_EQUAL(object.date, time_from_string("2011-11-11 11:11:11"));
	}
}

BOOST_AUTO_TEST_CASE(mustSelectAndBuildAllObjects)
{
	PostgresSql postgres;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(postgres));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	sql = "INSERT INTO test(value, date) "
		"VALUES ('This is another test', '2011-12-12 11:11:11')";
	postgres.execute(sql);

	class Object {
	public:
		Object() : id(0), value("") {}

		long id;
		string value;
		ptime date;
	};

	sql = "SELECT id, value, date FROM test ORDER BY id";
	shared_ptr<PostgresSqlResult> result = 
		std::dynamic_pointer_cast<PostgresSqlResult>(postgres.execute(sql));

	BOOST_CHECK_EQUAL(result->size(), 2);

	list<Object> objects = result->getAll<Object>([](map<string, any> row) {
			Object object;
			object.id = any_cast<long>(row["id"]);
			object.value = any_cast<string>(row["value"]);
			object.date = any_cast<ptime>(row["date"]);
			return object;
		});

	Object object1 = objects.front();
	BOOST_CHECK_EQUAL(object1.id, 1);
	BOOST_CHECK_EQUAL(object1.value, "This is a test");
	BOOST_CHECK_EQUAL(object1.date, time_from_string("2011-11-11 11:11:11"));

	Object object2 = objects.back();
	BOOST_CHECK_EQUAL(object2.id, 2);
	BOOST_CHECK_EQUAL(object2.value, "This is another test");
	BOOST_CHECK_EQUAL(object2.date, time_from_string("2011-12-12 11:11:11"));
}

BOOST_AUTO_TEST_CASE(mustRollbackData)
{
	PostgresSql postgres;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(postgres));

	auto mode = PostgresSql::TransactionMode::MANUAL_COMMIT;
	BOOST_CHECK_NO_THROW(postgres.setTransactionMode(mode));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	BOOST_CHECK_NO_THROW(postgres.rollback());

	sql = "SELECT id, value, date FROM test";
	shared_ptr<PostgresSqlResult> result =
		std::dynamic_pointer_cast<PostgresSqlResult>(postgres.execute(sql));

	BOOST_CHECK_EQUAL(result->size(), 0);
}

BOOST_AUTO_TEST_CASE(mustCommitData)
{
	PostgresSql postgres;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(postgres));

	auto mode = PostgresSql::TransactionMode::AUTO_COMMIT;
	BOOST_CHECK_NO_THROW(postgres.setTransactionMode(mode));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	BOOST_CHECK_NO_THROW(postgres.commit());
	BOOST_CHECK_NO_THROW(postgres.rollback());

	sql = "SELECT id, value, date FROM test";
	shared_ptr<PostgresSqlResult> result =
		std::dynamic_pointer_cast<PostgresSqlResult>(postgres.execute(sql));

	BOOST_CHECK_EQUAL(result->size(), 1);
}

BOOST_AUTO_TEST_CASE(mustRetrieveLastInsertedId)
{
	PostgresSql postgres;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(postgres));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	BOOST_CHECK_EQUAL(postgres.lastInsertedId(), 1);

	sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	postgres.execute(sql);

	BOOST_CHECK_EQUAL(postgres.lastInsertedId(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
