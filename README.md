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

  To run the unit tests install the mysql-server, using abc123 as root
  password and create the "dbplus" database.

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

  // TODO

Contact
-------

  Rafael Dantas Justo  
  <adm@rafael.net.br>

License
-------

    CGIplus Copyright (C) 2011 Rafael Dantas Justo

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
