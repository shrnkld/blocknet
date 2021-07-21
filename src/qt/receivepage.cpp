// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/wallet.h>


#include <qt/receivepage.h>
#include <QString>
#include <QTimer>
#include <QSettings>

#ifdef USE_QRCODE
#include <qrencode.h>
#include <qt/guiconstants.h>
#include <QPixmap>
#include <QLabel>
#include <QImage>
#include <QPainter>
#endif

#include <qt/addressbookpage.h>
#include <qt/blocknetaddressedit.h>
#include <qt/blocknetaddressbook.h>
#include <qt/addresstablemodel.h>
#include <qt/bitcoinunits.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/recentrequeststablemodel.h>
#include <qt/walletmodel.h>

ReceivePage::ReceivePage(const PlatformStyle *_platformStyle, WalletModel *model, QWidget *parent) :
    QWidget(parent),
    walletModel(model),
    platformStyle(_platformStyle)
{
    //initialize screen layout and objects
    layout = new QVBoxLayout;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(layout);
    layout->setContentsMargins(BGU::spi(45), BGU::spi(10), BGU::spi(35), BGU::spi(30));

    titleLbl = new QLabel;
    titleLbl->setObjectName("h4");

    subtitleLbl = new QLabel;
    subtitleLbl->setObjectName("h2");
    
    addressTi = new BlocknetAddressEditor(BGU::spi(675));
    addressTi->setPlaceholderText(tr("Enter Blocknet Address..."));
    addressTi->setAddressValidator([this](QString &addr) -> bool {
        if (walletModel == nullptr)
            return false;
        auto isValid = walletModel->validateAddress(addr);
        leftBtn->setDisabled(!isValid);
        return isValid;
    });

    //remove visual effects around valid address in the address editor field
    connect(addressTi, &BlocknetAddressEditor::textChanged,
    [this]()
    {
        if(!addressTi->getAddresses().values().isEmpty())
        {
            QString address = addressTi->getAddresses().values().first();
            if(walletModel->validateAddress(address))
            {
                addressTi->clearData();
                addressTi->textCursor().insertText(address);
            }
        }
    });

    selectAddressBtn = new BlocknetIconBtn(QString(tr("Select from\n address book")), ":/redesign/QuickActions/AddressBookIcon.png");
    newAddressBtn = new BlocknetIconBtn(QString(tr("Create new\n address")), ":/redesign/QuickActions/AddressButtonIcon.png");

    connect(selectAddressBtn, &BlocknetIconBtn::clicked, this, &ReceivePage::openAddressBook);
    connect(newAddressBtn, &BlocknetIconBtn::clicked, this, &ReceivePage::onAddAddress);
    
    lblQRCode = new QLabel;

    auto *hdiv = new QLabel;
    hdiv->setFixedHeight(1);
    hdiv->setObjectName("hdiv");

    leftBtn = new BlocknetFormBtn;
    rightBtn = new BlocknetFormBtn;

    layout->addWidget(titleLbl, 0, Qt::AlignTop | Qt::AlignLeft);
    layout->addSpacing(BGU::spi(30));
    layout->addWidget(subtitleLbl, 0, Qt::AlignTop);
    layout->addSpacing(BGU::spi(25));
    layout->addWidget(addressTi);
    layout->addSpacing(BGU::spi(20));
    
    auto *hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(selectAddressBtn);
    hlayout1->addWidget(newAddressBtn);
    auto *spacer1 = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    hlayout1->addSpacerItem(spacer1);
    layout->addLayout(hlayout1);
    layout->addWidget(lblQRCode);
    layout->addSpacing(BGU::spi(35));
    layout->addWidget(hdiv);
    layout->addSpacing(BGU::spi(40));
    
    auto *hlayout2 = new QHBoxLayout;
    hlayout2->addWidget(leftBtn);
    hlayout2->addWidget(rightBtn);
    auto *spacer2 = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    hlayout2->setSpacing(30);
    hlayout2->addSpacerItem(spacer2);
    layout->addLayout(hlayout2);
    layout->addStretch(1);

    showView1();
}

void ReceivePage::openAddressBook() {
    BlocknetAddressBookDialog dlg(walletModel, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    connect(&dlg, &BlocknetAddressBookDialog::send, [this](const QString &address) {
        addressTi->clearData();
        //addressTi->addAddress(address);
        addressTi->textCursor().insertText(address);
    });
    dlg.exec();
}

void ReceivePage::onAddAddress(){
    BlocknetAddressAddDialog dlg(walletModel->getAddressTableModel(), walletModel, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    dlg.setStyleSheet(GUIUtil::loadStyleSheet());
    dlg.form->setMinimalView(true);
    if(dlg.exec() == QDialog::Accepted)
    {
        addressTi->clearData();
        //addressTi->addAddress(dlg.form->getAddress());
        addressTi->textCursor().insertText(dlg.form->getAddress());
    }
}

void ReceivePage::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    addressTi->setFocus();
}

void ReceivePage::showView1()
{
    QSettings settings;
    QString address = settings.value("receive_page_address").toString();
    if(address.isEmpty())
    {
        addressTi->clear();
        leftBtn->setDisabled(true);
    }
    else
    {
        addressTi->addAddress(address);
        leftBtn->setDisabled(false);
    }
    
    titleLbl->setText(tr("Receive Funds"));
    subtitleLbl->setText(tr("Enter an address to receive funds to:"));
    addressTi->setReadOnly(false);

    addressTi->setFocus();
    selectAddressBtn->setVisible(true);
    newAddressBtn->setVisible(true);
    leftBtn->setVisible(true);//not needed
    leftBtn->setText(tr("Continue"));
    rightBtn->setVisible(false);
    lblQRCode->setVisible(false);
    
    //reconnect button handler
    leftBtn->disconnect();
    connect(leftBtn, &QPushButton::clicked, this, &ReceivePage::showView2);
}
    
void ReceivePage::showView2()
{
    /*QString address = (addressTi->getAddresses().isEmpty()) ? "" 
    : addressTi->getAddresses().values().first();*/
    QString address = addressTi->toPlainText();

    //recheck again that address is correct
    if(!walletModel->validateAddress(address))
    {
        leftBtn->setDisabled(true);
        addressTi->setFocus();
        return;
    }

    #ifdef USE_QRCODE
    makeQrCode(address);
    lblQRCode->setVisible(true);
    #endif

    subtitleLbl->setText(tr("Receive funds to adddress:"));
    //addressTi->setEnabled(false);
    addressTi->setReadOnly(true);
    selectAddressBtn->setVisible(false);
    newAddressBtn->setVisible(false);
    leftBtn->setVisible(true);//not needed
    leftBtn->setText(tr("Return to Dashboard"));
    leftBtn->setDisabled(false);
    rightBtn->setVisible(true);
    rightBtn->setText(tr("Start over"));//can move to constructor
    
    QSettings settings;
    settings.setValue("receive_page_address", address);

    //reconnect button handlers
    leftBtn->disconnect();
    rightBtn->disconnect();
    connect(leftBtn, &QPushButton::clicked, [=]()
    {
        Q_EMIT dashboard();
    });

    connect(rightBtn, &QPushButton::clicked, [=]()
    {
        QSettings settings;
        settings.setValue("receive_page_address", "");
        showView1();
    });
    
    //connect(rightBtn, &QPushButton::clicked, this, &ReceivePage::showView1);
}

void ReceivePage::makeQrCode(QString &input)
{
    #ifdef USE_QRCODE
            //QString uri = "yJR1YAoxKSjSJTRrpPG3UR2YUhNQGR1doJ"; //test key
            QString uri = input;
            QRcode *code = QRcode_encodeString(uri.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
            if (!code)
            {
                this->lblQRCode->setText(tr("Error encoding URI into QR Code."));
                return;
            }
            QImage qrImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
            qrImage.fill(0xffffff);
            unsigned char *p = code->data;
            for (int y = 0; y < code->width; y++)
            {
                for (int x = 0; x < code->width; x++)
                {
                    qrImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                    p++;
                }
            }
            QRcode_free(code);

            #define QR_IMAGE_SIZE 200
            QImage qrAddrImage = QImage(QR_IMAGE_SIZE, QR_IMAGE_SIZE, QImage::Format_RGB32);
            qrAddrImage.fill(0xffffff);
            QPainter painter(&qrAddrImage);
            painter.drawImage(0, 0, qrImage.scaled(QR_IMAGE_SIZE, QR_IMAGE_SIZE));
            /*QFont font = GUIUtil::fixedPitchFont();
            QRect paddedRect = qrAddrImage.rect();

            // calculate ideal font size
            qreal font_size = GUIUtil::calculateIdealFontSize(paddedRect.width() - 20, info.address, font);
            font.setPointSizeF(font_size);

            painter.setFont(font);
            paddedRect.setHeight(QR_IMAGE_SIZE+12);
            painter.drawText(paddedRect, Qt::AlignBottom|Qt::AlignCenter, info.address);*/
            painter.end();
            lblQRCode->setPixmap(QPixmap::fromImage(qrAddrImage));
#endif
}

void ReceivePage::setModel(WalletModel *model)
{
    walletModel = model;
}

ReceivePage::~ReceivePage()
{
}