//*****************************************************************************
//*****************************************************************************

#ifndef XBRIDGEEXCHANGE_H
#define XBRIDGEEXCHANGE_H

#include "uint256.h"
#include "xbridgetransaction.h"
#include "xbridgewallet.h"

#include <string>
#include <set>
#include <map>
#include <list>

#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>

//******************************************************************************
//******************************************************************************
namespace xbridge
{

//*****************************************************************************
//*****************************************************************************
typedef std::pair<std::string, std::string> StringPair;

//*****************************************************************************
//*****************************************************************************
class Exchange
{
public:
    static Exchange & instance();

protected:
    Exchange();
    ~Exchange();

public:
    bool init();

    bool isEnabled();
    bool isStarted();

    bool haveConnectedWallet(const std::string & walletName);
    std::vector<std::string> connectedWallets() const;

    // std::vector<unsigned char> walletAddress(const std::string & walletName);

    bool createTransaction(const uint256                    & id,
                           const std::vector<unsigned char> & sourceAddr,
                           const std::string                & sourceCurrency,
                           const uint64_t                   & sourceAmount,
                           const std::vector<unsigned char> & destAddr,
                           const std::string                & destCurrency,
                           const uint64_t                   & destAmount,
                           uint256                          & pendingId,
                           bool                             & isCreated);

    bool acceptTransaction(const uint256                    & id,
                           const std::vector<unsigned char> & sourceAddr,
                           const std::string                & sourceCurrency,
                           const uint64_t                   & sourceAmount,
                           const std::vector<unsigned char> & destAddr,
                           const std::string                & destCurrency,
                           const uint64_t                   & destAmount,
                           uint256                          & transactionId);

    bool deletePendingTransactions(const uint256 & id);
    bool deleteTransaction(const uint256 & id);

    bool updateTransactionWhenHoldApplyReceived(const TransactionPtr & tx,
                                                const std::vector<unsigned char> & from);
    bool updateTransactionWhenInitializedReceived(const TransactionPtr & tx,
                                                  const std::vector<unsigned char> & from,
                                                  const uint256 & datatxid,
                                                  const std::vector<unsigned char> & pk);
    bool updateTransactionWhenCreatedReceived(const TransactionPtr & tx,
                                              const std::vector<unsigned char> & from,
                                              const std::string & binTxId,
                                              const std::vector<unsigned char> & innerScript);
    bool updateTransactionWhenConfirmedReceived(const TransactionPtr & tx,
                                                const std::vector<unsigned char> & from);

    bool updateTransaction(const uint256 & hash);

    const TransactionPtr transaction(const uint256 & hash);
    const TransactionPtr pendingTransaction(const uint256 & hash);
    std::list<TransactionPtr> pendingTransactions() const;
    std::list<TransactionPtr> transactions() const;
    std::list<TransactionPtr> finishedTransactions() const;
    std::list<TransactionPtr> transactionsHistory() const;
    void addToTransactionsHistory(const uint256 & id);

private:
    std::list<TransactionPtr> transactions(bool onlyFinished) const;

private:
    // connected wallets
    typedef std::map<std::string, WalletParam> WalletList;
    WalletList                               m_wallets;

    mutable boost::mutex                     m_pendingTransactionsLock;
    std::map<uint256, TransactionPtr> m_pendingTransactions;

    mutable boost::mutex                     m_transactionsLock;
    std::map<uint256, TransactionPtr> m_transactions;

    mutable boost::mutex                     m_transactionsHistoryLock;
    std::map<uint256, TransactionPtr> m_transactionsHistory;

    mutable boost::mutex                     m_unconfirmedLock;
    std::map<std::string, uint256>           m_unconfirmed;

    // TODO use deque and limit size
    std::set<uint256>                        m_walletTransactions;

    mutable boost::mutex                     m_knownTxLock;
    std::set<uint256>                        m_knownTransactions;
};

} // namespace xbridge

#endif // XBRIDGEEXCHANGE_H
