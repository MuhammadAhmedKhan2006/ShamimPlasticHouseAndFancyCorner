#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QSplitter>
#include <QStatusBar>
#include <QSqlQuery>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include "DatabaseManager.h"
#include "slipprinter.h"

class AdminPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminPanel(QWidget *parent = nullptr);

private slots:
    // Product management slots
    void onAddProduct();
    void onEditProduct();
    void onDeleteProduct();
    void onRefreshProducts();
    void onSearchProducts();
    void onProductSelectionChanged();

    // Supplier management slots
    void onAddSupplier();
    void onEditSupplier();
    void onDeleteSupplier();
    void onRefreshSuppliers();
    void onSupplierSelectionChanged();

    // Transaction management slots
    void onAddTransaction();
    void onRefreshTransactions();
    void onSupplierFilterChanged();
    void onPrintPaymentSlip();

    // Reports slots
    void onGenerateReports();
    void onExportData();
    void onLoadDailySales();

    // Other slots
    void onLowStockAlert();

private:
    void setupUI();
    void setupProductTab();
    void setupSupplierTab();
    void setupTransactionTab();
    void setupReportsTab();
    void setupDailySalesTab();

    void loadProducts();
    void loadSuppliers();
    void loadTransactions();
    void loadCategories();

    void clearProductForm();
    void clearSupplierForm();
    void updateSupplierCombo();

    // Main tabs
    QTabWidget *m_tabWidget;

    // Product management widgets
    QWidget *m_productTab;
    QTableWidget *m_productTable;
    QLineEdit *m_productNameEdit;
    QComboBox *m_productCategoryCombo;
    QLineEdit *m_productSubcategoryEdit;
    QDoubleSpinBox *m_productPriceSpinBox;
    QSpinBox *m_productStockSpinBox;
    QLineEdit *m_productSearchEdit;
    QPushButton *m_addProductBtn;
    QPushButton *m_editProductBtn;
    QPushButton *m_deleteProductBtn;
    QPushButton *m_refreshProductBtn;

    // Supplier management widgets
    QWidget *m_supplierTab;
    QTableWidget *m_supplierTable;
    QLineEdit *m_supplierNameEdit;
    QLineEdit *m_supplierContactEdit;
    QPushButton *m_addSupplierBtn;
    QPushButton *m_editSupplierBtn;
    QPushButton *m_deleteSupplierBtn;
    QPushButton *m_refreshSupplierBtn;

    // Transaction management widgets
    QWidget *m_transactionTab;
    QTableWidget *m_transactionTable;
    QComboBox *m_transactionSupplierCombo;
    QComboBox *m_supplierFilterCombo;
    QDateEdit *m_transactionDateEdit;
    QLineEdit *m_transactionDescEdit;
    QDoubleSpinBox *m_transactionPurchaseSpinBox;
    QDoubleSpinBox *m_transactionPaymentSpinBox;
    QPushButton *m_addTransactionBtn;
    QPushButton *m_refreshTransactionBtn;
    QPushButton *m_printPaymentSlipBtn;

    // Reports widgets
    QWidget *m_reportsTab;
    QTableWidget *m_reportsTable;
    QComboBox *m_reportTypeCombo;
    QDateEdit *m_reportStartDateEdit;
    QDateEdit *m_reportEndDateEdit;
    QPushButton *m_generateReportBtn;
    QPushButton *m_exportReportBtn;

    // Daily Sales widgets
    QWidget *m_dailySalesTab;
    QTableWidget *m_dailySalesTable;
    QDateEdit *m_dailySalesDateEdit;
    QPushButton *m_refreshDailySalesBtn;
    QLabel *m_dailySalesTotalLabel;
    QLabel *m_dailySalesCountLabel;

    // Utilities
    SlipPrinter *m_slipPrinter;

    // Current selections
    int m_selectedProductId;
    int m_selectedSupplierId;
};

#endif // ADMINPANEL_H
