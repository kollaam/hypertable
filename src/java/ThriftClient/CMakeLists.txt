#
# Copyright(C) 2008 Doug Judd (Zvents, Inc.)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

# We should be able to run the test even if we didn't build jars
if (JAVAC_VERSION)
  add_test(ThriftClient-java env java -cp
          ${INSTALL_DIR}/lib/java/hypertable-${VERSION}.jar:${INSTALL_DIR}/lib/java/log4j-1.2.13.jar:${INSTALL_DIR}/lib/java/libthrift.jar
          org.hypertable.thrift.BasicClientTest)
endif ()
