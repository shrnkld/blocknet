// Copyright (c) 2020 The Blocknet developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BLOCKNET_XBRIDGE_UTIL_RPCUTIL_H
#define BLOCKNET_XBRIDGE_UTIL_RPCUTIL_H

#include <event2/buffer.h>
#include <rpc/client.h>
#include <rpc/protocol.h>
#include <support/events.h>
#include <util/strencodings.h>
#include <util/system.h>

#include <json/json_spirit.h>
#include <json/json_spirit_reader_template.h>
#include <json/json_spirit_writer_template.h>
#include <univalue.h>

namespace xbridge {

/** Reply structure for request_done to fill in */
struct HTTPReply {
    HTTPReply(): status(0), error(-1) {}
    int status;
    int error;
    std::string body;
};

const char *http_errorstring(int code);
void http_request_done(struct evhttp_request *req, void *ctx);
#if LIBEVENT_VERSION_NUMBER >= 0x02010300
void http_error_cb(enum evhttp_request_error err, void *ctx);
#endif

UniValue XBridgeJSONRPCRequestObj(const std::string & strMethod, const UniValue & params,
                                  const UniValue & id, const std::string & jsonver="");

json_spirit::Object CallRPC(const std::string & rpcuser, const std::string & rpcpasswd,
                            const std::string & rpcip, const std::string & rpcport,
                            const std::string & strMethod, const json_spirit::Array & params,
                            const std::string & jsonver="", const std::string & contenttype="");

}

#endif //BLOCKNET_XBRIDGE_UTIL_RPCUTIL_H
