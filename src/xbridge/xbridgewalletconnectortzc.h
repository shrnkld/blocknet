// Copyright (c) 2017-2019 The Blocknet developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BLOCKNET_XBRIDGE_XBRIDGEWALLETCONNECTORTZC_H
#define BLOCKNET_XBRIDGE_XBRIDGEWALLETCONNECTORTZC_H

#include <xbridge/xbridgecryptoproviderbtc.h>
#include <xbridge/xbridgewalletconnectorbtc.h>

//*****************************************************************************
//*****************************************************************************
namespace xbridge
{

//******************************************************************************
//******************************************************************************
class TzcWalletConnector : public BtcWalletConnector<BtcCryptoProvider>
{
public:
    TzcWalletConnector();

};

} // namespace xbridge

#endif // BLOCKNET_XBRIDGE_XBRIDGEWALLETCONNECTORTZC_H
