#ifndef SLIPPRINTER_H
#define SLIPPRINTER_H

#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QFont>
#include <QDateTime>
#include <QMessageBox>
#include "Sale.h"

class SlipPrinter : public QObject
{
    Q_OBJECT

public:
    explicit SlipPrinter(QObject *parent = nullptr);

    // Print sales invoice
    void printSalesInvoice(const Sale& sale, QWidget *parentWidget = nullptr);

    // Print payment slip for supplier
    void printPaymentSlip(const QString& supplierName, double totalPurchases,
                          double totalPayments, double balance, QWidget *parentWidget = nullptr);

    // Preview methods
    void previewSalesInvoice(const Sale& sale, QWidget *parentWidget = nullptr);
    void previewPaymentSlip(const QString& supplierName, double totalPurchases,
                            double totalPayments, double balance, QWidget *parentWidget = nullptr);

private:
    void printDocument(const QString& htmlContent, QWidget *parentWidget);
    void previewDocument(const QString& htmlContent, QWidget *parentWidget);
    QString generateSalesInvoiceHtml(const Sale& sale);
    QString generatePaymentSlipHtml(const QString& supplierName, double totalPurchases,
                                    double totalPayments, double balance);
    QString getCompanyHeader();
};

#endif // SLIPPRINTER_H
