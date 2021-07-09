// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef BITCOIN_QT_RECEIVEPAGE_H
#define BITCOIN_QT_RECEIVEPAGE_H

#include <qt/guiutil.h>

#include <QWidget>
#include <QDialog>
#include <QHeaderView>
#include <QItemSelection>
#include <QKeyEvent>
#include <QMenu>
#include <QPoint>
#include <QVariant>
#include <QVBoxLayout>

#include <qt/blocknetaddresseditor.h>
#include <qt/blocknetformbtn.h>
#include <qt/blocknetsendfundsutil.h>

#include <qt/walletmodel.h>

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <qt/blocknetguiutil.h>
#include <qt/blockneticonbtn.h>


class PlatformStyle;
class WalletModel;

class ReceivePage : public QWidget
{
    Q_OBJECT

public:
    explicit ReceivePage(const PlatformStyle *platformStyle, WalletModel *model = nullptr, QWidget *parent = nullptr);
    ~ReceivePage();

public Q_SLOTS:
    void setModel(WalletModel *model);

protected:
    void focusInEvent(QFocusEvent *event) override;

private:
    WalletModel *walletModel;
    const PlatformStyle *platformStyle;
    QVBoxLayout *layout;
    QLabel *titleLbl, *subtitleLbl;
    QLabel *lblQRCode;
    BlocknetAddressEditor *addressTi;
    BlocknetFormBtn *leftBtn, *rightBtn;
    BlocknetIconBtn *selectAddressBtn, *newAddressBtn;
    

Q_SIGNALS:
    void dashboard();

private Q_SLOTS:
    void makeQrCode(QString&);
    void showView1();
    void showView2();
    void openAddressBook();
    void onAddAddress();
};

#endif // BITCOIN_QT_RECEIVEPAGE_H


