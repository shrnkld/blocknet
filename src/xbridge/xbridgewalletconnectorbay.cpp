// Copyright (c) 2020 The Blocknet developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <xbridge/xbridgewalletconnectorbay.h>
#include <xbridge/util/logger.h>

#include <vector>

namespace xbridge {

bool bay_listUnspent(const std::string & rpcuser, const std::string & rpcpasswd, const std::string & rpcip, const std::string & rpcport,
        std::vector<wallet::UtxoEntry> & entries)
{
    const static std::string txid("txid");
    const static std::string vout("vout");
    const static std::string amount("liquidity");
    const static std::string scriptPubKey("scriptPubKey");
    const static std::string confirmations("confirmations");

    try {
        LOG() << "rpc call bay <listunspent>";

        json_spirit::Array params;
        json_spirit::Object reply = CallRPC(rpcuser, rpcpasswd, rpcip, rpcport, "listunspent", params);

        // Parse reply
        const json_spirit::Value & result = find_value(reply, "result");
        const json_spirit::Value & error  = find_value(reply, "error");

        if (error.type() != json_spirit::null_type) {
            LOG() << "error: " << write_string(error, false);
            return false;
        } else if (result.type() != json_spirit::array_type) {
            LOG() << "result not an array " <<
                  (result.type() == json_spirit::null_type ? "" :
                   result.type() == json_spirit::str_type  ? result.get_str() :
                   write_string(result, true));
            return false;
        }

        json_spirit::Array arr = result.get_array();
        for (const json_spirit::Value & val : arr) {
            if (val.type() == json_spirit::obj_type) {
                const json_spirit::Value & spendable = find_value(val.get_obj(), "spendable");
                if (spendable.type() == json_spirit::bool_type && !spendable.get_bool())
                    continue;

                wallet::UtxoEntry u;
                int confs = -1;

                json_spirit::Object o = val.get_obj();
                for (const auto & v : o) {
                    if (v.name_ == txid)
                        u.txId = v.value_.get_str();
                    else if (v.name_ == vout)
                        u.vout = v.value_.get_int();
                    else if (v.name_ == amount)
                        u.amount = v.value_.get_real();
                    else if (v.name_ == scriptPubKey)
                        u.scriptPubKey = v.value_.get_str();
                    else if (v.name_ == confirmations)
                        confs = v.value_.get_int();
                }

                if (!u.txId.empty() && u.amount > 0 && (confs == -1 || confs > 0))
                    entries.push_back(u);
            }
        }
    } catch (std::exception & e) {
        LOG() << "bay listunspent exception " << e.what();
        return false;
    }

    return true;
}

BayWalletConnector::BayWalletConnector() : BtcWalletConnector<BtcCryptoProvider>() { }

bool BayWalletConnector::getUnspent(std::vector<wallet::UtxoEntry> & inputs,
        const std::set<wallet::UtxoEntry> & excluded) const
{
    if (!bay_listUnspent(m_user, m_passwd, m_ip, m_port, inputs)) {
        LOG() << "rpc::listUnspent failed " << __FUNCTION__;
        return false;
    }

    // Remove all the excluded utxos
    inputs.erase(
        std::remove_if(inputs.begin(), inputs.end(), [&excluded, this](xbridge::wallet::UtxoEntry & u) {
            if (excluded.count(u))
                return true; // remove if in excluded list

            // Only accept p2pkh (like 76a91476bba472620ff0ecbfbf93d0d3909c6ca84ac81588ac)
            std::vector<unsigned char> script = ParseHex(u.scriptPubKey);
            if (script.size() == 25 &&
                script[0] == 0x76 && script[1] == 0xa9 && script[2] == 0x14 &&
                script[23] == 0x88 && script[24] == 0xac)
            {
                script.erase(script.begin(), script.begin()+3);
                script.erase(script.end()-2, script.end());
                u.address = fromXAddr(script);
                return false; // keep
            }

            return true; // remove if script invalid
        }),
        inputs.end()
    );

    return true;
}

}
