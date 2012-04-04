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

#include <map>
#include <memory>
#include <vector>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <dbplus/Binary.hpp>
#include <dbplus/DatabaseException.hpp>
#include <dbplus/MySql.hpp>
#include <dbplus/Result.hpp>

using std::map;
using std::shared_ptr;
using std::vector;

using boost::any;
using boost::any_cast;
using boost::posix_time::ptime;
using boost::posix_time::time_from_string;

using dbplus::Binary;
using dbplus::DatabaseException;
using dbplus::MySql;
using dbplus::Result;

// When you need to run only one test, compile only this file with the
// STAND_ALONE flag.
#ifdef STAND_ALONE
#define BOOST_TEST_MODULE DBplus
#endif

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(dbplusMysqlTests)

BOOST_AUTO_TEST_CASE(mustConnectToDatabase)
{
	MySql mysql;
	BOOST_CHECK_NO_THROW(mysql.connect("dbplus", "root", "abc123", "127.0.0.1"));
}

BOOST_AUTO_TEST_CASE(mustNotConnectToDatabase)
{
	MySql mysql;
	BOOST_CHECK_THROW(mysql.connect("dbplus", "root", "abc321", "127.0.0.1"), 
	                  DatabaseException);
}

BOOST_AUTO_TEST_CASE(mustExecuteWithoutErrors)
{
	MySql mysql;
	mysql.connect("dbplus", "root", "abc123", "127.0.0.1");

	string sql = "DROP TABLE IF EXISTS test";
	BOOST_CHECK_NO_THROW(mysql.execute(sql));

	sql = "CREATE TABLE test (id INT(11) PRIMARY KEY AUTO_INCREMENT, "
		"value VARCHAR(255), date DATETIME)";
	BOOST_CHECK_NO_THROW(mysql.execute(sql));
}

void createDatabaseAndTable(MySql &mysql)
{
	mysql.connect("dbplus", "root", "abc123", "127.0.0.1");

	string sql = "DROP TABLE IF EXISTS test";
	mysql.execute(sql);

	sql = "CREATE TABLE test (id INT(11) PRIMARY KEY AUTO_INCREMENT, "
		"value VARCHAR(255), date DATETIME) ENGINE=InnoDB";
	mysql.execute(sql);
}

BOOST_AUTO_TEST_CASE(mustInsertAndSelectData)
{
	MySql mysql;
	
	BOOST_CHECK_NO_THROW(createDatabaseAndTable(mysql));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	mysql.execute(sql);

	BOOST_CHECK_EQUAL(mysql.affectedRows(), 1);

	sql = "SELECT id, value, date FROM test";
	shared_ptr<Result> result = mysql.execute(sql);

	BOOST_CHECK_EQUAL(result->size(), 1);

	while (result->fetch()) {
		int id = result->get<long>("id");
		string value = result->get<string>("value");
		ptime date = result->get<ptime>("date");

		BOOST_CHECK_EQUAL(id, 1);
		BOOST_CHECK_EQUAL(value, "This is a test");
		BOOST_CHECK_EQUAL(date, time_from_string("2011-11-11 11:11:11"));
	}
}

BOOST_AUTO_TEST_CASE(mustSelectAndBuildEachObject)
{
	MySql mysql;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(mysql));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	mysql.execute(sql);

	class Object {
	public:
		Object() : id(0), value("") {}

		long id;
		string value;
		ptime date;
	};

	sql = "SELECT id, value, date FROM test";
	shared_ptr<Result> result = mysql.execute(sql);

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
	MySql mysql;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(mysql));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	mysql.execute(sql);

	sql = "INSERT INTO test(value, date) "
		"VALUES ('This is another test', '2011-12-12 11:11:11')";
	mysql.execute(sql);

	class Object {
	public:
		Object() : id(0), value("") {}

		long id;
		string value;
		ptime date;
	};

	sql = "SELECT id, value, date FROM test ORDER BY id";
	shared_ptr<Result> result = mysql.execute(sql);

	BOOST_CHECK_EQUAL(result->size(), 2);

	vector<Object> objects = result->getAll<Object>([](map<string, any> row) {
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
	MySql mysql;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(mysql));

	auto mode = MySql::TransactionMode::MANUAL_COMMIT;
	BOOST_CHECK_NO_THROW(mysql.setTransactionMode(mode));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	mysql.execute(sql);

	BOOST_CHECK_NO_THROW(mysql.rollback());

	sql = "SELECT id, value, date FROM test";
	shared_ptr<Result> result = mysql.execute(sql);

	BOOST_CHECK_EQUAL(result->size(), 0);
}

BOOST_AUTO_TEST_CASE(mustCommitData)
{
	MySql mysql;

	BOOST_CHECK_NO_THROW(createDatabaseAndTable(mysql));

	auto mode = MySql::TransactionMode::AUTO_COMMIT;
	BOOST_CHECK_NO_THROW(mysql.setTransactionMode(mode));

	string sql = "INSERT INTO test(value, date) "
		"VALUES ('This is a test', '2011-11-11 11:11:11')";
	mysql.execute(sql);

	BOOST_CHECK_NO_THROW(mysql.commit());
	BOOST_CHECK_NO_THROW(mysql.rollback());

	sql = "SELECT id, value, date FROM test";
	shared_ptr<Result> result = mysql.execute(sql);

	BOOST_CHECK_EQUAL(result->size(), 1);
}

BOOST_AUTO_TEST_CASE(mustRetrieveAllKindsOfData)
{
	MySql mysql;
	mysql.connect("dbplus", "root", "abc123", "127.0.0.1");

	string sql = "DROP TABLE IF EXISTS test";
	mysql.execute(sql);

	// Unsupported fields for now: BIT, BOOLEAN, DECIMAL, DATE,
	// TIMESTAMP, TIME, ENUM, SET

	sql = "CREATE TABLE test ("
		"value1 TINYINT, "
		"value2 SMALLINT, "
		"value3 MEDIUMINT, "
		"value4 INT, "
		"value5 BIGINT, "
		"value6 FLOAT, "
		"value7 DOUBLE, "
		"value8 DATETIME, "
		"value9 YEAR, "
		"value10 VARCHAR(255), "
		"value11 TINYBLOB, "
		"value12 BLOB, "
		"value13 MEDIUMBLOB, "
		"value14 LONGBLOB) ENGINE=InnoDB";
	mysql.execute(sql);

	sql = "INSERT INTO test (value1, value2, value3, value4, value5, value6, "
		"value7, value8, value9, value10, value11, value12, value13, value14) "
		"VALUES ("
		"100, "
		"1000, "
		"10000, "
		"100000, "
		"1000000, "
		"1.00000001, "
		"1.0000001, "
		"'2012-01-03 11:00:00', "
		"2012, "
		"'Test', "
		"'This is a blob', "
		"'This is another blob', "
		"'One more blob', "
		"'The last blob')";
	mysql.execute(sql);

	sql = "SELECT value1, value2, value3, value4, value5, value6, "
		"value7, value8, value9, value10, value11, value12, value13, value14 "
		"FROM test";
	shared_ptr<Result> result = mysql.execute(sql);

	BOOST_CHECK_EQUAL(result->size(), 1);

	while (result->fetch()) {
		uint8_t value1 = result->get<uint8_t>("value1");
		short value2 = result->get<short>("value2");
		uint32_t value3 = result->get<uint32_t>("value3");
		long value4 = result->get<long>("value4");
		long long value5 = result->get<long long>("value5");
		float value6 = result->get<float>("value6");
		double value7 = result->get<double>("value7");
		ptime value8 = result->get<ptime>("value8");
		int value9 = result->get<int>("value9");
		string value10 = result->get<string>("value10");
		Binary value11 = result->get<Binary>("value11");
		Binary value12 = result->get<Binary>("value12");
		Binary value13 = result->get<Binary>("value13");
		Binary value14 = result->get<Binary>("value14");

		BOOST_CHECK_EQUAL(value1, static_cast<uint8_t>(100));
		BOOST_CHECK_EQUAL(value2, static_cast<short>(1000));
		BOOST_CHECK_EQUAL(value3, static_cast<uint32_t>(10000));
		BOOST_CHECK_EQUAL(value4, static_cast<long>(100000));
		BOOST_CHECK_EQUAL(value5, static_cast<long long>(1000000));
		BOOST_CHECK_EQUAL(value6, static_cast<float>(1.00000001));
		BOOST_CHECK_EQUAL(value7, static_cast<double>(1.0000001));
		BOOST_CHECK_EQUAL(value8, time_from_string("2012-01-03 11:00:00"));
		BOOST_CHECK_EQUAL(value9, 2012);
		BOOST_CHECK_EQUAL(value10, "Test");
		BOOST_CHECK(value11 == Binary(string("This is a blob")));
		BOOST_CHECK(value12 == Binary(string("This is another blob")));
		BOOST_CHECK(value13 == Binary(string("One more blob")));
		BOOST_CHECK(value14 == Binary(string("The last blob")));
	}
}

BOOST_AUTO_TEST_SUITE_END()
