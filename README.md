DBplus
=======

**DBplus** is a C++ database interface to use the most common database
clients, such as MySQL <http://mysql.com> and PostgreSQL
<http://postgresql.org>.

Prerequisits
------------

  * g++ 4.6 - <http://gcc.gnu.org/>
  * python 2.7 - <http://www.python.org/>
  * scons 2.0 - <http://www.scons.org/>
  * libboost-system-dev 1.4 - <http://www.boost.org>
  * libboost-test-dev 1.4 - <http://www.boost.org>
  * libboost-date-time-dev 1.4 - <http://www.boost.org>
  * libmysqlclient-dev 5.1 - <http://mysql.com/>
  * libpq-dev 5 - <http://postgresql.org>

  The project was compiled using the above compilers and libraries,
  higher versions should work well.

  To run the unit tests:

  * install mysql-server package (MySQL)
  * install postgresql package (PostgreSQL)
  * create a root user with the password "abc123" on each database server
  * create a database with the name "dbplus" on each database server

Installation
------------

  Define the compiler using the CXX environment variable, by default
  is used "g++".

    # scons
    # scons install

  For API documentation:

    # scons doc

Usage
-----

  Source code (using MySQL):

    #include <iostream>
    #include <list>
    #include <map>
    #include <memory>
    #include <string>

    #include <boost/any.hpp>
    #include <boost/date_time/posix_time/posix_time.hpp>

    #include <dbplus/MySql.hpp>
    #include <dbplus/MySqlResult.hpp>

    class Object
    {
    public:
      Object() : _id(0), _value("") {}

      void print()
      {
        std::cout << "-----------------" << std::endl;
        std::cout << "Id: "    << _id    << std::endl;
        std::cout << "Value: " << _value << std::endl;
        std::cout << "Date: "  << _date  << std::endl;
      }

      void setId(const long id)
      {
        _id = id;
      }

      void setValue(const std::string &value)
      {
        _value = value;
      }

      void setDate(const  boost::posix_time::ptime &date)
      {
        _date = date;
      }

    private:
      long _id;
      std::string _value;
      boost::posix_time::ptime _date;
    };

    int main()
    {
      dbplus::MySql mysql;
      mysql.connect("dbplus", "root", "abc123", "127.0.0.1");

      std::string sql = "SELECT id, value, date FROM test";
      std::shared_ptr<dbplus::MySqlResult> result = 
        std::dynamic_pointer_cast<dbplus::MySqlResult>(mysql.execute(sql));

      std::list<Object> objects = 
        result->getAll<Object>([](std::map<string, boost::any> row) {
          Object object;
          object.setId(boost::any_cast<long>(row["id"]));
          object.setValue(boost::any_cast<string>(row["value"]));
          object.setDate(boost::any_cast<boost::posix_time::ptime>(row["date"]));
          return object;
        });

      for (Object object: objects) {
        object.print();
      }
    }

  Compiling:

    g++ -std=c++0x test.cpp -o test -ldbplus -lboost_date_time -lmysqlclient

  Output:

    -----------------
    Id: 1
    Value: This is a test
    Date: 2011-Nov-11 11:11:11

Contact
-------

  Rafael Dantas Justo  
  <adm@rafael.net.br>

License
-------

    CGIplus Copyright (C) 2012 Rafael Dantas Justo

    This file is part of CGIplus.

    CGIplus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CGIplus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CGIplus.  If not, see <http://www.gnu.org/licenses/>.
