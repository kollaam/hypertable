/** -*- c++ -*-
 * Copyright (C) 2009 Doug Judd (Zvents, Inc.)
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the
 * License, or any later version.
 *
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef HYPERTABLE_MASTER_H
#define HYPERTABLE_MASTER_H

extern "C" {
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
}

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "Common/Thread.h"
#include "Common/ReferenceCount.h"
#include "Common/String.h"
#include "Common/StringExt.h"
#include "Common/Properties.h"

#include "AsyncComm/ApplicationQueue.h"
#include "AsyncComm/Comm.h"
#include "AsyncComm/ConnectionManager.h"
#include "AsyncComm/Event.h"
#include "AsyncComm/ResponseCallback.h"

#include "Hyperspace/Session.h"

#include "Hypertable/Lib/Filesystem.h"
#include "Hypertable/Lib/Table.h"
#include "Hypertable/Lib/Types.h"
#include "Hypertable/Lib/Schema.h"

#include "HyperspaceSessionHandler.h"
#include "RangeServerState.h"
#include "ResponseCallbackGetSchema.h"
#include "MasterGc.h"

namespace Hypertable {
  using namespace Hyperspace;

  class Master : public ReferenceCount {
  public:
    Master(PropertiesPtr &, ConnectionManagerPtr &, ApplicationQueuePtr &);
    ~Master();

    void create_table(ResponseCallback *cb, const char *tablename,
                      const char *schemastr);
    void alter_table(ResponseCallback *cb, const char *tablename,
                      const char *schemastr);
    void get_schema(ResponseCallbackGetSchema *cb, const char *tablename);
    void register_server(ResponseCallback *cb, const char *location,
                         const sockaddr_in &addr);
    void report_split(ResponseCallback *cb, const TableIdentifier &table,
                      const RangeSpec &range, const char *transfer_log_dir,
                      uint64_t soft_limit);
    void drop_table(ResponseCallback *cb, const char *table_name,
                    bool if_exists);
    void close(ResponseCallback *cb);
    void shutdown(ResponseCallback *cb);

    void server_joined(const String &location);
    void server_left(const String &location);

    void join();

  protected:
    void create_table(const char *tablename, const char *schemastr);

  private:
    bool initialize();
    void scan_servers_directory();
    bool create_hyperspace_dir(const String &dir);
    void wait_for_root_metadata_server();

    Mutex        m_mutex;
    PropertiesPtr m_props_ptr;
    ConnectionManagerPtr m_conn_manager_ptr;
    ApplicationQueuePtr m_app_queue_ptr;
    bool m_verbose;
    Hyperspace::SessionPtr m_hyperspace_ptr;
    Filesystem *m_dfs_client;
    atomic_t m_last_table_id;
    HyperspaceSessionHandler m_hyperspace_session_handler;
    uint64_t m_master_file_handle;
    uint64_t m_servers_dir_handle;
    LockSequencer m_master_file_sequencer;
    HandleCallbackPtr m_servers_dir_callback_ptr;
    TablePtr m_metadata_table_ptr;
    uint64_t m_max_range_bytes;

    /** temporary vairables **/
    bool m_initialized;
    Mutex m_root_server_mutex;
    boost::condition  m_root_server_cond;
    bool m_root_server_connected;
    String m_root_server_location;

    typedef hash_map<String, RangeServerStatePtr> ServerMap;
    typedef hash_map<QualifiedRangeSpec, struct sockaddr_in,
                     QualifiedRangeHash, QualifiedRangeEqual> RangeToAddrMap;

    ServerMap  m_server_map;
    ServerMap::iterator m_server_map_iter;
    boost::condition  m_no_servers_cond;

    RangeToAddrMap m_range_to_server_map;

    ThreadGroup m_threads;
    static const uint32_t MAX_ALTER_TABLE_RETRIES = 3;
  };

  typedef intrusive_ptr<Master> MasterPtr;

} // namespace Hypertable

#endif // HYPERTABLE_MASTER_H
