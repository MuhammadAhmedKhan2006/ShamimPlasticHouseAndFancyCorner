#ifndef SLIPPRINTER_H
#define SLIPPRINTER_H

#include <QObject>
#include <QString>
#include <QWidget>

class Sale;

class SlipPrinter : public QObject
{
    Q_OBJECT

public:
    explicit SlipPrinter(QObject *parent = nullptr);

    // Print methods
    void printSalesInvoice(const Sale& sale, QWidget *parentWidget = nullptr);
    void printPaymentSlip(const QString& supplierName, double totalPurchases,
                          double totalPayments, double balance, QWidget *parentWidget = nullptr);

    // Preview methods
    void previewSalesInvoice(const Sale& sale, QWidget *parentWidget = nullptr);
    void previewPaymentSlip(const QString& supplierName, double totalPurchases,
                            double totalPayments, double balance, QWidget *parentWidget = nullptr);

private:
    void printDocument(const QString& htmlContent, QWidget *parentWidget = nullptr);
    void previewDocument(const QString& htmlContent, QWidget *parentWidget = nullptr);
    QString generateSalesInvoiceHtml(const Sale& sale);
    QString generatePaymentSlipHtml(const QString& supplierName, double totalPurchases,
                                    double totalPayments, double balance);
    QString getCompanyHeader();
};

#endif // SLIPPRINTER_H
