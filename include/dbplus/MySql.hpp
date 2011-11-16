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

#ifndef __DB_PLUS_MYSQL_HPP__
#define __DB_PLUS_MYSQL_HPP__

extern "C" {
#include <mysql/mysql.h>
}

#include <dbplus/Dbplus.hpp>

#include "RelationalDatabase.hpp"

DBPLUS_NS_BEGIN

/*! \class Mysql
 *  \brief MySQL client interface.
 *
 * Connect to MySQL database.
 */
class MySql : public RelationalDatabase
{
public:
	MySql();
	~MySql();

	void connect(const string &database, 
	             const string &user, 
	             const string &password, 
	             const string &server, 
	             const unsigned int port = 3306);
	void disconnect();

	void setTransactionMode(const TransactionMode mode);
	TransactionMode getTransactionMode() const;

	void commit();
	void rollback();

	string escape(const string &value);

	std::shared_ptr<Result>	
	execute(const string &query, const ResultMode resultMode = STORE_RESULT);

	unsigned long long affectedRows();
	unsigned long long lastInsertedId();

private:
	MYSQL _mysql;
	TransactionMode _transactionMode;
};

DBPLUS_NS_END

#endif // __DB_PLUS_MYSQL_HPP__
