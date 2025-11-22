#include "SlipPrinter.h"
#include <QPrintPreviewDialog>

SlipPrinter::SlipPrinter(QObject *parent)
    : QObject(parent)
{
}

void SlipPrinter::printSalesInvoice(const Sale& sale, QWidget *parentWidget)
{
    QString htmlContent = generateSalesInvoiceHtml(sale);
    printDocument(htmlContent, parentWidget);
}

void SlipPrinter::printPaymentSlip(const QString& supplierName, double totalPurchases,
                                   double totalPayments, double balance, QWidget *parentWidget)
{
    QString htmlContent = generatePaymentSlipHtml(supplierName, totalPurchases, totalPayments, balance);
    printDocument(htmlContent, parentWidget);
}

void SlipPrinter::previewSalesInvoice(const Sale& sale, QWidget *parentWidget)
{
    QString htmlContent = generateSalesInvoiceHtml(sale);
    previewDocument(htmlContent, parentWidget);
}

void SlipPrinter::previewPaymentSlip(const QString& supplierName, double totalPurchases,
                                     double totalPayments, double balance, QWidget *parentWidget)
{
    QString htmlContent = generatePaymentSlipHtml(supplierName, totalPurchases, totalPayments, balance);
    previewDocument(htmlContent, parentWidget);
}

void SlipPrinter::printDocument(const QString& htmlContent, QWidget *parentWidget)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    QPrintDialog printDialog(&printer, parentWidget);
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument document;
        document.setHtml(htmlContent);
        document.print(&printer);

        QMessageBox::information(parentWidget, "Print", "Document sent to printer successfully!");
    }
}

void SlipPrinter::previewDocument(const QString& htmlContent, QWidget *parentWidget)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    QPrintPreviewDialog previewDialog(&printer, parentWidget);

    connect(&previewDialog, &QPrintPreviewDialog::paintRequested, [=](QPrinter *printer) {
        QTextDocument document;
        document.setHtml(htmlContent);
        document.print(printer);
    });

    previewDialog.exec();
}

QString SlipPrinter::generateSalesInvoiceHtml(const Sale& sale)
{
    QString html = R"(
    <html>
    <head>
        <style>
            body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }
            .header { text-align: center; border-bottom: 2px solid #3498db; padding-bottom: 15px; margin-bottom: 20px; }
            .company-name { font-size: 24px; font-weight: bold; color: #2c3e50; margin: 0; }
            .company-subtitle { font-size: 16px; color: #3498db; margin: 5px 0; }
            .contact-info { font-size: 12px; color: #7f8c8d; margin: 10px 0; }
            .invoice-title { text-align: center; font-size: 20px; font-weight: bold; margin: 20px 0; color: #2c3e50; }
            .invoice-info { display: flex; justify-content: space-between; margin: 20px 0; }
            .invoice-info div { flex: 1; }
            table { width: 100%; border-collapse: collapse; margin: 20px 0; }
            th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
            th { background-color: #3498db; color: white; font-weight: bold; }
            .total-row { background-color: #f8f9fa; font-weight: bold; }
            .grand-total { background-color: #3498db; color: white; font-size: 18px; }
            .footer { text-align: center; margin-top: 30px; font-size: 12px; color: #7f8c8d; }
        </style>
    </head>
    <body>
        <div class="header">
            <h1 class="company-name">Shamim Plastic House</h1>
            <h2 class="company-subtitle">& Fancy Corner</h2>
            <div class="contact-info">
                Online Orders: 03333152019, 03333152018
            </div>
        </div>

        <div class="invoice-title">SALES INVOICE</div>

        <div class="invoice-info">
            <div>
                <strong>Invoice #:</strong> )" + QString::number(sale.getId()) + R"(<br>
                <strong>Date:</strong> )" + sale.getDate().toString("dd/MM/yyyy hh:mm AP") + R"(<br>
                <strong>Salesman ID:</strong> )" + QString::number(sale.getSalesmanId()) + R"(
            </div>
        </div>

        <table>
            <thead>
                <tr>
                    <th>Item</th>
                    <th>Quantity</th>
                    <th>Unit Price</th>
                    <th>Total</th>
                </tr>
            </thead>
            <tbody>)";

    for (const SaleItem& item : sale.getItems()) {
        html += R"(
                <tr>
                    <td>)" + item.productName + R"(</td>
                    <td>)" + QString::number(item.quantity) + R"(</td>
                    <td>Rs. )" + QString::number(item.unitPrice, 'f', 2) + R"(</td>
                    <td>Rs. )" + QString::number(item.totalPrice, 'f', 2) + R"(</td>
                </tr>)";
    }

    html += R"(
                <tr class="grand-total">
                    <td colspan="3"><strong>GRAND TOTAL</strong></td>
                    <td><strong>Rs. )" + QString::number(sale.getTotal(), 'f', 2) + R"(</strong></td>
                </tr>
            </tbody>
        </table>)";

    if (!sale.getCustomerNote().isEmpty()) {
        html += R"(
        <div style="margin-top: 20px;">
            <strong>Customer Note:</strong><br>
            )" + sale.getCustomerNote() + R"(
        </div>)";
    }

    html += R"(
        <div class="footer">
            <p>Thank you for your business!</p>
            <p>This is a computer generated invoice.</p>
        </div>
    </body>
    </html>)";

    return html;
}

QString SlipPrinter::generatePaymentSlipHtml(const QString& supplierName, double totalPurchases,
                                             double totalPayments, double balance)
{
    QString html = R"(
    <html>
    <head>
        <style>
            body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }
            .header { text-align: center; border-bottom: 2px solid #3498db; padding-bottom: 15px; margin-bottom: 20px; }
            .company-name { font-size: 24px; font-weight: bold; color: #2c3e50; margin: 0; }
            .company-subtitle { font-size: 16px; color: #3498db; margin: 5px 0; }
            .slip-title { text-align: center; font-size: 20px; font-weight: bold; margin: 20px 0; color: #2c3e50; }
            .slip-info { margin: 30px 0; }
            .info-row { display: flex; justify-content: space-between; padding: 10px 0; border-bottom: 1px solid #ddd; }
            .info-row:last-child { border-bottom: 2px solid #3498db; font-weight: bold; font-size: 18px; }
            .balance-positive { color: #27ae60; }
            .balance-negative { color: #e74c3c; }
            .footer { text-align: center; margin-top: 30px; font-size: 12px; color: #7f8c8d; }
        </style>
    </head>
    <body>
        <div class="header">
            <h1 class="company-name">Shamim Plastic House</h1>
            <h2 class="company-subtitle">& Fancy Corner</h2>
        </div>

        <div class="slip-title">SUPPLIER PAYMENT SLIP</div>

        <div class="slip-info">
            <div class="info-row">
                <span><strong>Supplier Name:</strong></span>
                <span>)" + supplierName + R"(</span>
            </div>
            <div class="info-row">
                <span><strong>Date/Time:</strong></span>
                <span>)" + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm AP") + R"(</span>
            </div>
            <div class="info-row">
                <span><strong>Total Purchases:</strong></span>
                <span>Rs. )" + QString::number(totalPurchases, 'f', 2) + R"(</span>
            </div>
            <div class="info-row">
                <span><strong>Total Payments:</strong></span>
                <span>Rs. )" + QString::number(totalPayments, 'f', 2) + R"(</span>
            </div>
            <div class="info-row )" + (balance >= 0 ? "balance-positive" : "balance-negative") + R"(">
                <span><strong>Remaining Balance:</strong></span>
                <span>Rs. )" + QString::number(balance, 'f', 2) + R"(</span>
            </div>
        </div>

        <div class="footer">
            <p>This is a computer generated slip.</p>
            <p>Generated on )" + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm AP") + R"(</p>
        </div>
    </body>
    </html>)";

    return html;
}

QString SlipPrinter::getCompanyHeader()
{
    return R"(
        <div class="header">
            <h1 class="company-name">Shamim Plastic House</h1>
            <h2 class="company-subtitle">& Fancy Corner</h2>
            <div class="contact-info">
                Online Orders: 03333152019, 03333152018
            </div>
        </div>
    )";
}
