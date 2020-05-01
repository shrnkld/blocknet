// Copyright (c) 2020 The Blocknet developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BLOCKNET_XBRIDGE_XBRIDGEWALLETCONNECTORBAY_H
#define BLOCKNET_XBRIDGE_XBRIDGEWALLETCONNECTORBAY_H

#include <xbridge/xbridgewalletconnectorbtc.h>
#include <xbridge/xbridgecryptoproviderbtc.h>

namespace xbridge {

class BayWalletConnector : public BtcWalletConnector<BtcCryptoProvider> {
public:
    BayWalletConnector();
    bool getUnspent(std::vector<wallet::UtxoEntry> & inputs, const std::set<wallet::UtxoEntry> & excluded) const;
};

}


#endif //BLOCKNET_XBRIDGE_XBRIDGEWALLETCONNECTORBAY_H
