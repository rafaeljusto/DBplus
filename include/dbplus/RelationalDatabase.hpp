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

#ifndef __DB_PLUS_RELATIONAL_DATABASE_HPP__
#define __DB_PLUS_RELATIONAL_DATABASE_HPP__

#include <memory>
#include <string>

#include <dbplus/Dbplus.hpp>

using std::string;

DBPLUS_NS_BEGIN

class Result;

/*! \class RelationalDatabase
 *  \brief Relational database interface.
 *
 * Base class to perform operations over a relational database.
 */
class RelationalDatabase
{
public:
	/*! \class TransactionMode
	 *  \brief Possible transaction modes
	 *
	 * In AUTO_COMMIT mode, every transaction is persisted
	 * on-the-fly. In MANUAL_COMMIT the transactions are only persisted
	 * when the commit method is called.
	 */
	class TransactionMode
	{
	public:
		/*! List all transaction modes
		 */
		enum Value {
			AUTO_COMMIT,
			MANUAL_COMMIT
		};
	};

	/*! \class ResultMode
	 *  \brief Possible result stote modes
	 *
	 * To avoid memory issues there are two possible result modes. The
	 * USE_RESULT mode store all result data in server side. The
	 * STORE_RESULT mode brings all result data to client side.
	 */
	class ResultMode
	{
	public:
		/*! List all result modes
		 */
		enum Value {
			USE_RESULT,
			STORE_RESULT
		};
	};

	/*! Connect to the database and keeps connected until the disconnect
	 * method is called.
	 *
	 * @param database Name of the database
	 * @param user Username used for connection
	 * @param password Username's password
	 * @param server Hostname or IP address of the database
	 * @param port Database server's port
	 * @see disconnect
	 */
	virtual void connect(const string &database, 
	                     const string &user, 
	                     const string &password, 
	                     const string &server, 
	                     const unsigned int port = 3306) = 0;

	/*! Disconnect from the database
	 */
	virtual void disconnect() = 0;

	/*! Sets transaction mode. Possible values are defined in
	 * RelationalDatabase::TransactionMode::Value.
	 *
	 * @param mode Transaction mode
	 */
	virtual void setTransactionMode(const TransactionMode::Value mode) = 0;

	/*! Gets transaction mode. Possible values are defined in
	 * RelationalDatabase::TransactionMode::Value.
	 *
	 * @return Transaction mode
	 */
	virtual TransactionMode::Value getTransactionMode() const = 0;

	/*! In transaction mode MANUAL_COMMIT, this method is responsable
	 * for persisting every query made since the last call of the
	 * methods commit or rollback.
	 *
	 * @see rollback
	 */
	virtual void commit() = 0;

	/*! In transaction mode MANUAL_COMMIT, this method is responsable
	 * for rolling back all changes made since the last call of the
	 * methods commit or rollback.
	 *
	 * @see commit
	 */
	virtual void rollback() = 0;

	/*! To avoid SQL injection over database queries, this method
	 * removes all dangerous characters.
	 *
	 * @param value String to be escaped
	 * @return Value escaped
	 */
	virtual string escape(const string &value) = 0;

	/*! Execute a SQL query.
	 *
	 * @param query SQL query
	 * @param resultMode Define where the result is going to be stored
	 * @return Result object with all the dataset
	 */
	virtual std::shared_ptr<Result> 
	execute(const string &query, 
	        const ResultMode::Value resultMode = ResultMode::STORE_RESULT) = 0;

	/*! Returns the number of rows effected by the last query.
	 *
	 * @return Number of rows effected
	 */
	virtual unsigned long long affectedRows() = 0;

	/*! Returns the last inserted id due to SQL insert commands.
	 *
	 * @return Last inserted id
	 */
	virtual unsigned long long lastInsertedId() = 0;
};

DBPLUS_NS_END

#endif // __DBPLUS_DATABASE_HPP__
