#include "AdminPanel.h"
#include <QApplication>
#include <QFileDialog>
#include <QTextStream>
#include <QCloseEvent>
#include <QSqlQuery>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include <QHeaderView>

AdminPanel::AdminPanel(QWidget *parent)
    : QMainWindow(parent)
    , m_selectedProductId(0)
    , m_selectedSupplierId(0)
{
    m_slipPrinter = new SlipPrinter(this);
    setupUI();

    // Load initial data
    loadProducts();
    loadSuppliers();
    loadTransactions();
    loadCategories();
    updateSupplierCombo();

    setWindowTitle("Admin Panel - Shamim Plastic House Management System");
    setMinimumSize(1200, 800);
    showMaximized();
}

void AdminPanel::setupUI()
{
    setCentralWidget(new QWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());

    // Create header with modern styling
    QLabel *headerLabel = new QLabel("⚙️ ADMIN CONTROL PANEL");
    headerLabel->setStyleSheet(R"(
        QLabel {
            font-size: 32px;
            font-weight: bold;
            color: white;
            padding: 20px;
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                                      stop: 0 #667eea, stop: 1 #764ba2);
            border-radius: 15px;
            margin-bottom: 10px;
        }
    )");
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setMinimumHeight(80);

    m_tabWidget = new QTabWidget;
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setStyleSheet(R"(
        QTabWidget::pane {
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            background: white;
        }
        QTabBar::tab {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ecf0f1, stop: 1 #bdc3c7);
            color: #2c3e50;
            border: 2px solid #95a5a6;
            border-bottom: none;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            min-width: 150px;
            padding: 12px 25px;
            font-size: 13px;
            font-weight: bold;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #667eea, stop: 1 #764ba2);
            color: white;
            border-color: #5a67d8;
        }
        QTabBar::tab:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #d5d9e0, stop: 1 #bdc3c7);
        }
    )");

    setupProductTab();
    setupSupplierTab();
    setupTransactionTab();
    setupDailySalesTab();
    setupReportsTab();

    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(m_tabWidget);

    // Status bar with modern styling
    statusBar()->setStyleSheet(R"(
        QStatusBar {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                                      stop: 0 #34495e, stop: 1 #2c3e50);
            color: white;
            font-weight: bold;
            padding: 5px;
        }
    )");
    statusBar()->showMessage("✅ Admin Panel Ready - All systems operational");
}

void AdminPanel::setupProductTab()
{
    m_productTab = new QWidget;

    QHBoxLayout *mainLayout = new QHBoxLayout(m_productTab);
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Left side - Product list
    QWidget *leftWidget = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);

    // Search
    QHBoxLayout *searchLayout = new QHBoxLayout;
    m_productSearchEdit = new QLineEdit;
    m_productSearchEdit->setPlaceholderText("Search products...");
    QPushButton *searchBtn = new QPushButton("🔍 Search");
    searchBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }
    )");
    searchLayout->addWidget(new QLabel("Search:"));
    searchLayout->addWidget(m_productSearchEdit);
    searchLayout->addWidget(searchBtn);

    // Product table
    m_productTable = new QTableWidget;
    m_productTable->setColumnCount(6);
    QStringList headers = {"ID", "Name", "Category", "Subcategory", "Price", "Stock"};
    m_productTable->setHorizontalHeaderLabels(headers);
    m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_productTable->setAlternatingRowColors(true);
    m_productTable->horizontalHeader()->setStretchLastSection(true);

    leftLayout->addLayout(searchLayout);
    leftLayout->addWidget(m_productTable);

    // Right side - Product form
    QWidget *rightWidget = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);

    QGroupBox *formGroup = new QGroupBox("Product Details");
    QFormLayout *formLayout = new QFormLayout(formGroup);

    m_productNameEdit = new QLineEdit;
    m_productCategoryCombo = new QComboBox;
    m_productCategoryCombo->setEditable(true);
    m_productSubcategoryEdit = new QLineEdit;
    m_productPriceSpinBox = new QDoubleSpinBox;
    m_productPriceSpinBox->setRange(0, 999999);
    m_productPriceSpinBox->setDecimals(2);
    m_productPriceSpinBox->setSuffix(" Rs");
    m_productStockSpinBox = new QSpinBox;
    m_productStockSpinBox->setRange(0, 99999);

    formLayout->addRow("Name:", m_productNameEdit);
    formLayout->addRow("Category:", m_productCategoryCombo);
    formLayout->addRow("Subcategory:", m_productSubcategoryEdit);
    formLayout->addRow("Price:", m_productPriceSpinBox);
    formLayout->addRow("Stock:", m_productStockSpinBox);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);
    
    m_addProductBtn = new QPushButton("➕ Add Product");
    m_addProductBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2ecc71, stop: 1 #27ae60);
        }
    )");
    
    m_editProductBtn = new QPushButton("✏️ Edit Product");
    m_editProductBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    
    m_deleteProductBtn = new QPushButton("❌ Delete Product");
    m_deleteProductBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ec7063, stop: 1 #e74c3c);
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    
    m_refreshProductBtn = new QPushButton("🔄 Refresh");
    m_refreshProductBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f39c12, stop: 1 #e67e22);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e67e22, stop: 1 #d35400);
        }
    )");

    m_editProductBtn->setEnabled(false);
    m_deleteProductBtn->setEnabled(false);

    buttonLayout->addWidget(m_addProductBtn);
    buttonLayout->addWidget(m_editProductBtn);
    buttonLayout->addWidget(m_deleteProductBtn);
    buttonLayout->addWidget(m_refreshProductBtn);

    rightLayout->addWidget(formGroup);
    rightLayout->addLayout(buttonLayout);
    rightLayout->addStretch();

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    m_tabWidget->addTab(m_productTab, "Products");

    // Connect signals
    connect(searchBtn, &QPushButton::clicked, this, &AdminPanel::onSearchProducts);
    connect(m_productSearchEdit, &QLineEdit::returnPressed, this, &AdminPanel::onSearchProducts);
    connect(m_addProductBtn, &QPushButton::clicked, this, &AdminPanel::onAddProduct);
    connect(m_editProductBtn, &QPushButton::clicked, this, &AdminPanel::onEditProduct);
    connect(m_deleteProductBtn, &QPushButton::clicked, this, &AdminPanel::onDeleteProduct);
    connect(m_refreshProductBtn, &QPushButton::clicked, this, &AdminPanel::onRefreshProducts);
    connect(m_productTable, &QTableWidget::itemSelectionChanged, this, &AdminPanel::onProductSelectionChanged);
}

void AdminPanel::setupSupplierTab()
{
    m_supplierTab = new QWidget;

    QHBoxLayout *mainLayout = new QHBoxLayout(m_supplierTab);
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Left side - Supplier list
    QWidget *leftWidget = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);

    m_supplierTable = new QTableWidget;
    m_supplierTable->setColumnCount(3);
    QStringList headers = {"ID", "Name", "Contact"};
    m_supplierTable->setHorizontalHeaderLabels(headers);
    m_supplierTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_supplierTable->setAlternatingRowColors(true);
    m_supplierTable->horizontalHeader()->setStretchLastSection(true);

    leftLayout->addWidget(new QLabel("Suppliers:"));
    leftLayout->addWidget(m_supplierTable);

    // Right side - Supplier form
    QWidget *rightWidget = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);

    QGroupBox *formGroup = new QGroupBox("Supplier Details");
    QFormLayout *formLayout = new QFormLayout(formGroup);

    m_supplierNameEdit = new QLineEdit;
    m_supplierContactEdit = new QLineEdit;

    formLayout->addRow("Name:", m_supplierNameEdit);
    formLayout->addRow("Contact:", m_supplierContactEdit);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);
    
    m_addSupplierBtn = new QPushButton("➕ Add Supplier");
    m_addSupplierBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2ecc71, stop: 1 #27ae60);
        }
    )");
    
    m_editSupplierBtn = new QPushButton("✏️ Edit Supplier");
    m_editSupplierBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    
    m_deleteSupplierBtn = new QPushButton("❌ Delete Supplier");
    m_deleteSupplierBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ec7063, stop: 1 #e74c3c);
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    
    m_refreshSupplierBtn = new QPushButton("🔄 Refresh");
    m_refreshSupplierBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f39c12, stop: 1 #e67e22);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e67e22, stop: 1 #d35400);
        }
    )");

    m_editSupplierBtn->setEnabled(false);
    m_deleteSupplierBtn->setEnabled(false);

    buttonLayout->addWidget(m_addSupplierBtn);
    buttonLayout->addWidget(m_editSupplierBtn);
    buttonLayout->addWidget(m_deleteSupplierBtn);
    buttonLayout->addWidget(m_refreshSupplierBtn);

    rightLayout->addWidget(formGroup);
    rightLayout->addLayout(buttonLayout);
    rightLayout->addStretch();

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    m_tabWidget->addTab(m_supplierTab, "Suppliers");

    // Connect signals
    connect(m_addSupplierBtn, &QPushButton::clicked, this, &AdminPanel::onAddSupplier);
    connect(m_editSupplierBtn, &QPushButton::clicked, this, &AdminPanel::onEditSupplier);
    connect(m_deleteSupplierBtn, &QPushButton::clicked, this, &AdminPanel::onDeleteSupplier);
    connect(m_refreshSupplierBtn, &QPushButton::clicked, this, &AdminPanel::onRefreshSuppliers);
    connect(m_supplierTable, &QTableWidget::itemSelectionChanged, this, &AdminPanel::onSupplierSelectionChanged);
}

void AdminPanel::setupTransactionTab()
{
    m_transactionTab = new QWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout(m_transactionTab);

    // Top section - Add transaction form
    QGroupBox *addGroup = new QGroupBox("Add New Transaction");
    QHBoxLayout *addLayout = new QHBoxLayout(addGroup);

    QFormLayout *formLayout = new QFormLayout;
    m_transactionSupplierCombo = new QComboBox;
    m_transactionDateEdit = new QDateEdit(QDate::currentDate());
    m_transactionDescEdit = new QLineEdit;
    m_transactionPurchaseSpinBox = new QDoubleSpinBox;
    m_transactionPurchaseSpinBox->setRange(0, 9999999);
    m_transactionPurchaseSpinBox->setDecimals(2);
    m_transactionPurchaseSpinBox->setSuffix(" Rs");
    m_transactionPaymentSpinBox = new QDoubleSpinBox;
    m_transactionPaymentSpinBox->setRange(0, 9999999);
    m_transactionPaymentSpinBox->setDecimals(2);
    m_transactionPaymentSpinBox->setSuffix(" Rs");

    formLayout->addRow("Supplier:", m_transactionSupplierCombo);
    formLayout->addRow("Date:", m_transactionDateEdit);
    formLayout->addRow("Description:", m_transactionDescEdit);
    formLayout->addRow("Purchases:", m_transactionPurchaseSpinBox);
    formLayout->addRow("Payments:", m_transactionPaymentSpinBox);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->setSpacing(10);
    
    m_addTransactionBtn = new QPushButton("➕ Add Transaction");
    m_addTransactionBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2ecc71, stop: 1 #27ae60);
        }
    )");
    
    m_printPaymentSlipBtn = new QPushButton("🖨️ Print Payment Slip");
    m_printPaymentSlipBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #8e44ad, stop: 1 #7d3c98);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #a569bd, stop: 1 #8e44ad);
        }
    )");
    buttonLayout->addWidget(m_addTransactionBtn);
    buttonLayout->addWidget(m_printPaymentSlipBtn);
    buttonLayout->addStretch();

    addLayout->addLayout(formLayout);
    addLayout->addLayout(buttonLayout);

    // Middle section - Filter
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Filter by Supplier:"));
    m_supplierFilterCombo = new QComboBox;
    m_supplierFilterCombo->addItem("All Suppliers", 0);
    filterLayout->addWidget(m_supplierFilterCombo);
    m_refreshTransactionBtn = new QPushButton("🔄 Refresh");
    m_refreshTransactionBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f39c12, stop: 1 #e67e22);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e67e22, stop: 1 #d35400);
        }
    )");
    filterLayout->addWidget(m_refreshTransactionBtn);
    filterLayout->addStretch();

    // Bottom section - Transactions table
    m_transactionTable = new QTableWidget;
    m_transactionTable->setColumnCount(7);
    QStringList headers = {"ID", "Supplier", "Date", "Description", "Purchases", "Payments", "Balance"};
    m_transactionTable->setHorizontalHeaderLabels(headers);
    m_transactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_transactionTable->setAlternatingRowColors(true);
    m_transactionTable->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(addGroup);
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_transactionTable);

    m_tabWidget->addTab(m_transactionTab, "Transactions");

    // Connect signals
    connect(m_addTransactionBtn, &QPushButton::clicked, this, &AdminPanel::onAddTransaction);
    connect(m_refreshTransactionBtn, &QPushButton::clicked, this, &AdminPanel::onRefreshTransactions);
    connect(m_supplierFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdminPanel::onSupplierFilterChanged);
    connect(m_printPaymentSlipBtn, &QPushButton::clicked, this, &AdminPanel::onPrintPaymentSlip);
}

void AdminPanel::setupReportsTab()
{
    m_reportsTab = new QWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout(m_reportsTab);

    // Report controls
    QGroupBox *controlGroup = new QGroupBox("Report Settings");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);

    m_reportTypeCombo = new QComboBox;
    m_reportTypeCombo->addItem("Sales Report", "sales");
    m_reportTypeCombo->addItem("Low Stock Report", "lowstock");
    m_reportTypeCombo->addItem("Supplier Balance Report", "suppliers");
    m_reportTypeCombo->addItem("Product Summary", "products");

    m_reportStartDateEdit = new QDateEdit(QDate::currentDate().addDays(-30));
    m_reportEndDateEdit = new QDateEdit(QDate::currentDate());

    m_generateReportBtn = new QPushButton("📊 Generate Report");
    m_generateReportBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 25px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }
    )");
    
    m_exportReportBtn = new QPushButton("💾 Export to CSV");
    m_exportReportBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #16a085, stop: 1 #138d75);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 25px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #138d75, stop: 1 #0e6655);
        }
    )");

    controlLayout->addWidget(new QLabel("Report Type:"));
    controlLayout->addWidget(m_reportTypeCombo);
    controlLayout->addWidget(new QLabel("From:"));
    controlLayout->addWidget(m_reportStartDateEdit);
    controlLayout->addWidget(new QLabel("To:"));
    controlLayout->addWidget(m_reportEndDateEdit);
    controlLayout->addWidget(m_generateReportBtn);
    controlLayout->addWidget(m_exportReportBtn);
    controlLayout->addStretch();

    // Reports table
    m_reportsTable = new QTableWidget;
    m_reportsTable->setAlternatingRowColors(true);
    m_reportsTable->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(controlGroup);
    mainLayout->addWidget(m_reportsTable);

    m_tabWidget->addTab(m_reportsTab, "Reports");

    // Connect signals
    connect(m_generateReportBtn, &QPushButton::clicked, this, &AdminPanel::onGenerateReports);
    connect(m_exportReportBtn, &QPushButton::clicked, this, &AdminPanel::onExportData);
}

void AdminPanel::setupDailySalesTab()
{
    m_dailySalesTab = new QWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout(m_dailySalesTab);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Header section with date selector
    QGroupBox *headerGroup = new QGroupBox("📊 DAILY SALES REPORT");
    headerGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 16px;
            border: 3px solid #27ae60;
            border-radius: 12px;
            margin-top: 12px;
            padding-top: 20px;
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ffffff, stop: 1 #f8f9fa);
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 20px;
            padding: 0 10px;
            color: #2c3e50;
        }
    )");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerGroup);
    headerLayout->setSpacing(15);

    QLabel *dateLabel = new QLabel("Select Date:");
    dateLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    headerLayout->addWidget(dateLabel);

    m_dailySalesDateEdit = new QDateEdit(QDate::currentDate());
    m_dailySalesDateEdit->setCalendarPopup(true);
    m_dailySalesDateEdit->setStyleSheet(R"(
        QDateEdit {
            border: 2px solid #27ae60;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            background: white;
            min-width: 200px;
        }
        QDateEdit:focus {
            border: 3px solid #229954;
            background: #ecf0f1;
        }
        QDateEdit::drop-down {
            border: none;
            width: 30px;
        }
    )");
    headerLayout->addWidget(m_dailySalesDateEdit);

    m_refreshDailySalesBtn = new QPushButton("🔄 Refresh Daily Sales");
    m_refreshDailySalesBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2ecc71, stop: 1 #27ae60);
        }
        QPushButton:pressed {
            background: #229954;
        }
    )");
    headerLayout->addWidget(m_refreshDailySalesBtn);
    headerLayout->addStretch();

    // Summary cards
    QHBoxLayout *summaryLayout = new QHBoxLayout;
    summaryLayout->setSpacing(15);

    QFrame *totalFrame = new QFrame;
    totalFrame->setFrameStyle(QFrame::StyledPanel);
    totalFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                                      stop: 0 #667eea, stop: 1 #764ba2);
            border-radius: 12px;
            padding: 20px;
            border: 3px solid #5a67d8;
        }
    )");
    QVBoxLayout *totalLayout = new QVBoxLayout(totalFrame);
    QLabel *totalTextLabel = new QLabel("💰 Total Sales");
    totalTextLabel->setStyleSheet("color: white; font-weight: bold; font-size: 16px;");
    m_dailySalesTotalLabel = new QLabel("Rs. 0.00");
    m_dailySalesTotalLabel->setStyleSheet("color: white; font-weight: bold; font-size: 28px;");
    totalLayout->addWidget(totalTextLabel);
    totalLayout->addWidget(m_dailySalesTotalLabel);

    QFrame *countFrame = new QFrame;
    countFrame->setFrameStyle(QFrame::StyledPanel);
    countFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                                      stop: 0 #f093fb, stop: 1 #f5576c);
            border-radius: 12px;
            padding: 20px;
            border: 3px solid #e91e63;
        }
    )");
    QVBoxLayout *countLayout = new QVBoxLayout(countFrame);
    QLabel *countTextLabel = new QLabel("📦 Total Transactions");
    countTextLabel->setStyleSheet("color: white; font-weight: bold; font-size: 16px;");
    m_dailySalesCountLabel = new QLabel("0");
    m_dailySalesCountLabel->setStyleSheet("color: white; font-weight: bold; font-size: 28px;");
    countLayout->addWidget(countTextLabel);
    countLayout->addWidget(m_dailySalesCountLabel);

    summaryLayout->addWidget(totalFrame);
    summaryLayout->addWidget(countFrame);
    summaryLayout->addStretch();

    // Daily sales table
    m_dailySalesTable = new QTableWidget;
    m_dailySalesTable->setColumnCount(5);
    QStringList headers = {"Sale ID", "Time", "Total Amount", "Items Count", "Details"};
    m_dailySalesTable->setHorizontalHeaderLabels(headers);
    m_dailySalesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dailySalesTable->setAlternatingRowColors(true);
    m_dailySalesTable->horizontalHeader()->setStretchLastSection(true);
    m_dailySalesTable->setStyleSheet(R"(
        QTableWidget {
            background: white;
            border: 2px solid #bdc3c7;
            border-radius: 10px;
            gridline-color: #ecf0f1;
            font-size: 13px;
            selection-background-color: #27ae60;
            selection-color: white;
        }
        QTableWidget::item {
            padding: 10px;
        }
        QTableWidget::item:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
        }
        QHeaderView::section {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            padding: 12px;
            border: none;
            font-weight: bold;
            font-size: 14px;
        }
    )");

    mainLayout->addWidget(headerGroup);
    mainLayout->addLayout(summaryLayout);
    mainLayout->addWidget(m_dailySalesTable);

    m_tabWidget->addTab(m_dailySalesTab, "📊 Daily Sales");

    // Connect signals
    connect(m_refreshDailySalesBtn, &QPushButton::clicked, this, &AdminPanel::onLoadDailySales);
    connect(m_dailySalesDateEdit, &QDateEdit::dateChanged, this, &AdminPanel::onLoadDailySales);

    // Load today's sales by default
    onLoadDailySales();
}

void AdminPanel::loadProducts()
{
    QSqlQuery query = DatabaseManager::instance().getAllProducts();

    m_productTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_productTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(query.value("category").toString()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(query.value("subcategory").toString()));
        m_productTable->setItem(row, 4, new QTableWidgetItem(QString::number(query.value("price").toDouble(), 'f', 2)));
        m_productTable->setItem(row, 5, new QTableWidgetItem(query.value("stock_quantity").toString()));

        // Highlight low stock items
        if (query.value("stock_quantity").toInt() <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(231, 76, 60, 100));
            }
        }

        row++;
    }

    statusBar()->showMessage(QString("Loaded %1 products").arg(row));
}

void AdminPanel::loadSuppliers()
{
    QSqlQuery query = DatabaseManager::instance().getAllSuppliers();

    m_supplierTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_supplierTable->insertRow(row);
        m_supplierTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_supplierTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
        m_supplierTable->setItem(row, 2, new QTableWidgetItem(query.value("contact").toString()));
        row++;
    }

    statusBar()->showMessage(QString("Loaded %1 suppliers").arg(row));
}

void AdminPanel::loadTransactions()
{
    QSqlQuery query = DatabaseManager::instance().getAllTransactions();

    m_transactionTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_transactionTable->insertRow(row);
        m_transactionTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_transactionTable->setItem(row, 1, new QTableWidgetItem(query.value("supplier_name").toString()));
        m_transactionTable->setItem(row, 2, new QTableWidgetItem(query.value("date").toString()));
        m_transactionTable->setItem(row, 3, new QTableWidgetItem(query.value("description").toString()));
        m_transactionTable->setItem(row, 4, new QTableWidgetItem(QString::number(query.value("purchases").toDouble(), 'f', 2)));
        m_transactionTable->setItem(row, 5, new QTableWidgetItem(QString::number(query.value("payments").toDouble(), 'f', 2)));
        m_transactionTable->setItem(row, 6, new QTableWidgetItem(QString::number(query.value("balance").toDouble(), 'f', 2)));

        // Color code balance
        double balance = query.value("balance").toDouble();
        QColor balanceColor = balance >= 0 ? QColor(39, 174, 96, 100) : QColor(231, 76, 60, 100);
        m_transactionTable->item(row, 6)->setBackground(balanceColor);

        row++;
    }
}

void AdminPanel::loadCategories()
{
    QSqlQuery query = DatabaseManager::instance().getCategories();

    m_productCategoryCombo->clear();
    while (query.next()) {
        m_productCategoryCombo->addItem(query.value("category").toString());
    }
}

void AdminPanel::clearProductForm()
{
    m_productNameEdit->clear();
    m_productCategoryCombo->setCurrentIndex(0);
    m_productSubcategoryEdit->clear();
    m_productPriceSpinBox->setValue(0.0);
    m_productStockSpinBox->setValue(0);
    m_selectedProductId = 0;
}

void AdminPanel::clearSupplierForm()
{
    m_supplierNameEdit->clear();
    m_supplierContactEdit->clear();
    m_selectedSupplierId = 0;
}

void AdminPanel::updateSupplierCombo()
{
    m_transactionSupplierCombo->clear();
    m_supplierFilterCombo->clear();
    m_supplierFilterCombo->addItem("All Suppliers", 0);

    QSqlQuery query = DatabaseManager::instance().getAllSuppliers();
    while (query.next()) {
        int id = query.value("id").toInt();
        QString name = query.value("name").toString();
        m_transactionSupplierCombo->addItem(name, id);
        m_supplierFilterCombo->addItem(name, id);
    }
}

void AdminPanel::onAddProduct()
{
    if (m_productNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter product name.");
        return;
    }

    if (DatabaseManager::instance().addProduct(
            m_productNameEdit->text().trimmed(),
            m_productCategoryCombo->currentText(),
            m_productSubcategoryEdit->text().trimmed(),
            m_productPriceSpinBox->value(),
            m_productStockSpinBox->value())) {

        QMessageBox::information(this, "Success", "Product added successfully!");
        clearProductForm();
        loadProducts();
        loadCategories();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add product.");
    }
}

void AdminPanel::onEditProduct()
{
    if (m_selectedProductId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a product to edit.");
        return;
    }

    if (m_productNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter product name.");
        return;
    }

    if (DatabaseManager::instance().updateProduct(
            m_selectedProductId,
            m_productNameEdit->text().trimmed(),
            m_productCategoryCombo->currentText(),
            m_productSubcategoryEdit->text().trimmed(),
            m_productPriceSpinBox->value(),
            m_productStockSpinBox->value())) {

        QMessageBox::information(this, "Success", "Product updated successfully!");
        clearProductForm();
        loadProducts();
        m_editProductBtn->setEnabled(false);
        m_deleteProductBtn->setEnabled(false);
    } else {
        QMessageBox::critical(this, "Error", "Failed to update product.");
    }
}

void AdminPanel::onDeleteProduct()
{
    if (m_selectedProductId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a product to delete.");
        return;
    }

    int ret = QMessageBox::question(this, "Confirm Delete",
                                    "Are you sure you want to delete this product?",
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteProduct(m_selectedProductId)) {
            QMessageBox::information(this, "Success", "Product deleted successfully!");
            clearProductForm();
            loadProducts();
            m_editProductBtn->setEnabled(false);
            m_deleteProductBtn->setEnabled(false);
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete product.");
        }
    }
}

void AdminPanel::onRefreshProducts()
{
    loadProducts();
    m_productSearchEdit->clear();
    clearProductForm();
    m_editProductBtn->setEnabled(false);
    m_deleteProductBtn->setEnabled(false);
}

void AdminPanel::onSearchProducts()
{
    QString searchTerm = m_productSearchEdit->text().trimmed();

    if (searchTerm.isEmpty()) {
        loadProducts();
        return;
    }

    QSqlQuery query = DatabaseManager::instance().searchProducts(searchTerm);

    m_productTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_productTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(query.value("category").toString()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(query.value("subcategory").toString()));
        m_productTable->setItem(row, 4, new QTableWidgetItem(QString::number(query.value("price").toDouble(), 'f', 2)));
        m_productTable->setItem(row, 5, new QTableWidgetItem(query.value("stock_quantity").toString()));
        row++;
    }

    statusBar()->showMessage(QString("Found %1 products matching '%2'").arg(row).arg(searchTerm));
}

void AdminPanel::onProductSelectionChanged()
{
    QList<QTableWidgetItem*> selectedItems = m_productTable->selectedItems();

    if (selectedItems.isEmpty()) {
        m_editProductBtn->setEnabled(false);
        m_deleteProductBtn->setEnabled(false);
        return;
    }

    int row = selectedItems.first()->row();
    m_selectedProductId = m_productTable->item(row, 0)->text().toInt();

    // Populate form with selected product data
    m_productNameEdit->setText(m_productTable->item(row, 1)->text());
    m_productCategoryCombo->setCurrentText(m_productTable->item(row, 2)->text());
    m_productSubcategoryEdit->setText(m_productTable->item(row, 3)->text());
    m_productPriceSpinBox->setValue(m_productTable->item(row, 4)->text().toDouble());
    m_productStockSpinBox->setValue(m_productTable->item(row, 5)->text().toInt());

    m_editProductBtn->setEnabled(true);
    m_deleteProductBtn->setEnabled(true);
}

void AdminPanel::onAddSupplier()
{
    if (m_supplierNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter supplier name.");
        return;
    }

    if (DatabaseManager::instance().addSupplier(
            m_supplierNameEdit->text().trimmed(),
            m_supplierContactEdit->text().trimmed())) {

        QMessageBox::information(this, "Success", "Supplier added successfully!");
        clearSupplierForm();
        loadSuppliers();
        updateSupplierCombo();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add supplier. Name might already exist.");
    }
}

void AdminPanel::onEditSupplier()
{
    if (m_selectedSupplierId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a supplier to edit.");
        return;
    }

    if (m_supplierNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter supplier name.");
        return;
    }

    if (DatabaseManager::instance().updateSupplier(
            m_selectedSupplierId,
            m_supplierNameEdit->text().trimmed(),
            m_supplierContactEdit->text().trimmed())) {

        QMessageBox::information(this, "Success", "Supplier updated successfully!");
        clearSupplierForm();
        loadSuppliers();
        updateSupplierCombo();
        m_editSupplierBtn->setEnabled(false);
        m_deleteSupplierBtn->setEnabled(false);
    } else {
        QMessageBox::critical(this, "Error", "Failed to update supplier.");
    }
}

void AdminPanel::onDeleteSupplier()
{
    if (m_selectedSupplierId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a supplier to delete.");
        return;
    }

    int ret = QMessageBox::question(this, "Confirm Delete",
                                    "Are you sure you want to delete this supplier? This will also delete all associated transactions.",
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteSupplier(m_selectedSupplierId)) {
            QMessageBox::information(this, "Success", "Supplier deleted successfully!");
            clearSupplierForm();
            loadSuppliers();
            updateSupplierCombo();
            loadTransactions();
            m_editSupplierBtn->setEnabled(false);
            m_deleteSupplierBtn->setEnabled(false);
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete supplier.");
        }
    }
}

void AdminPanel::onRefreshSuppliers()
{
    loadSuppliers();
    updateSupplierCombo();
    clearSupplierForm();
    m_editSupplierBtn->setEnabled(false);
    m_deleteSupplierBtn->setEnabled(false);
}

void AdminPanel::onSupplierSelectionChanged()
{
    QList<QTableWidgetItem*> selectedItems = m_supplierTable->selectedItems();

    if (selectedItems.isEmpty()) {
        m_editSupplierBtn->setEnabled(false);
        m_deleteSupplierBtn->setEnabled(false);
        return;
    }

    int row = selectedItems.first()->row();
    m_selectedSupplierId = m_supplierTable->item(row, 0)->text().toInt();

    // Populate form with selected supplier data
    m_supplierNameEdit->setText(m_supplierTable->item(row, 1)->text());
    m_supplierContactEdit->setText(m_supplierTable->item(row, 2)->text());

    m_editSupplierBtn->setEnabled(true);
    m_deleteSupplierBtn->setEnabled(true);
}

void AdminPanel::onAddTransaction()
{
    if (m_transactionSupplierCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Warning", "Please select a supplier.");
        return;
    }

    int supplierId = m_transactionSupplierCombo->currentData().toInt();
    QString date = m_transactionDateEdit->date().toString("yyyy-MM-dd");
    QString description = m_transactionDescEdit->text().trimmed();
    double purchases = m_transactionPurchaseSpinBox->value();
    double payments = m_transactionPaymentSpinBox->value();

    if (purchases == 0.0 && payments == 0.0) {
        QMessageBox::warning(this, "Warning", "Please enter either purchases or payments amount.");
        return;
    }

    if (DatabaseManager::instance().addTransaction(supplierId, date, description, purchases, payments)) {
        QMessageBox::information(this, "Success", "Transaction added successfully!");

        // Clear form
        m_transactionDescEdit->clear();
        m_transactionPurchaseSpinBox->setValue(0.0);
        m_transactionPaymentSpinBox->setValue(0.0);
        m_transactionDateEdit->setDate(QDate::currentDate());

        loadTransactions();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add transaction.");
    }
}

void AdminPanel::onRefreshTransactions()
{
    loadTransactions();
    m_supplierFilterCombo->setCurrentIndex(0);
}

void AdminPanel::onSupplierFilterChanged()
{
    int supplierId = m_supplierFilterCombo->currentData().toInt();

    if (supplierId == 0) {
        loadTransactions();
    } else {
        QSqlQuery query = DatabaseManager::instance().getTransactionsBySupplier(supplierId);

        m_transactionTable->setRowCount(0);
        int row = 0;

        while (query.next()) {
            m_transactionTable->insertRow(row);
            m_transactionTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
            m_transactionTable->setItem(row, 1, new QTableWidgetItem(m_supplierFilterCombo->currentText()));
            m_transactionTable->setItem(row, 2, new QTableWidgetItem(query.value("date").toString()));
            m_transactionTable->setItem(row, 3, new QTableWidgetItem(query.value("description").toString()));
            m_transactionTable->setItem(row, 4, new QTableWidgetItem(QString::number(query.value("purchases").toDouble(), 'f', 2)));
            m_transactionTable->setItem(row, 5, new QTableWidgetItem(QString::number(query.value("payments").toDouble(), 'f', 2)));
            m_transactionTable->setItem(row, 6, new QTableWidgetItem(QString::number(query.value("balance").toDouble(), 'f', 2)));

            // Color code balance
            double balance = query.value("balance").toDouble();
            QColor balanceColor = balance >= 0 ? QColor(39, 174, 96, 100) : QColor(231, 76, 60, 100);
            m_transactionTable->item(row, 6)->setBackground(balanceColor);

            row++;
        }
    }
}

void AdminPanel::onPrintPaymentSlip()
{
    if (m_transactionSupplierCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Warning", "Please select a supplier.");
        return;
    }

    int supplierId = m_transactionSupplierCombo->currentData().toInt();
    QString supplierName = m_transactionSupplierCombo->currentText();

    // Calculate totals for the supplier
    QSqlQuery query = DatabaseManager::instance().getTransactionsBySupplier(supplierId);
    double totalPurchases = 0.0;
    double totalPayments = 0.0;
    double balance = 0.0;

    while (query.next()) {
        totalPurchases += query.value("purchases").toDouble();
        totalPayments += query.value("payments").toDouble();
        balance = query.value("balance").toDouble(); // Last balance
    }

    m_slipPrinter->previewPaymentSlip(supplierName, totalPurchases, totalPayments, balance, this);
}

void AdminPanel::onGenerateReports()
{
    QString reportType = m_reportTypeCombo->currentData().toString();
    QDate startDate = m_reportStartDateEdit->date();
    QDate endDate = m_reportEndDateEdit->date();

    m_reportsTable->clear();

    if (reportType == "sales") {
        QStringList headers = {"Sale ID", "Date", "Total", "Items Count"};
        m_reportsTable->setColumnCount(headers.size());
        m_reportsTable->setHorizontalHeaderLabels(headers);

        QSqlQuery salesQuery = DatabaseManager::instance().getSalesByDateRange(
            startDate.toString("yyyy-MM-dd"),
            endDate.toString("yyyy-MM-dd"));

        m_reportsTable->setRowCount(0);
        int row = 0;

        while (salesQuery.next()) {
            m_reportsTable->insertRow(row);
            int saleId = salesQuery.value("id").toInt();

            // Get items count
            QSqlQuery itemsQuery = DatabaseManager::instance().getSaleItems(saleId);
            int itemsCount = 0;
            while (itemsQuery.next()) itemsCount++;

            m_reportsTable->setItem(row, 0, new QTableWidgetItem(QString::number(saleId)));
            m_reportsTable->setItem(row, 1, new QTableWidgetItem(salesQuery.value("date").toString()));
            m_reportsTable->setItem(row, 2, new QTableWidgetItem(QString::number(salesQuery.value("total").toDouble(), 'f', 2)));
            m_reportsTable->setItem(row, 3, new QTableWidgetItem(QString::number(itemsCount)));
            row++;
        }
    } else if (reportType == "lowstock") {
        QStringList headers = {"Product ID", "Name", "Category", "Stock Quantity"};
        m_reportsTable->setColumnCount(headers.size());
        m_reportsTable->setHorizontalHeaderLabels(headers);

        QSqlQuery query = DatabaseManager::instance().getLowStockProducts(10);

        m_reportsTable->setRowCount(0);
        int row = 0;

        while (query.next()) {
            m_reportsTable->insertRow(row);
            m_reportsTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
            m_reportsTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
            m_reportsTable->setItem(row, 2, new QTableWidgetItem(query.value("category").toString()));
            m_reportsTable->setItem(row, 3, new QTableWidgetItem(query.value("stock_quantity").toString()));

            // Highlight low stock
            for (int col = 0; col < m_reportsTable->columnCount(); col++) {
                m_reportsTable->item(row, col)->setBackground(QColor(231, 76, 60, 100));
            }

            row++;
        }
    } else if (reportType == "suppliers") {
        QStringList headers = {"Supplier", "Total Purchases", "Total Payments", "Balance"};
        m_reportsTable->setColumnCount(headers.size());
        m_reportsTable->setHorizontalHeaderLabels(headers);

        QSqlQuery suppliersQuery = DatabaseManager::instance().getAllSuppliers();

        m_reportsTable->setRowCount(0);
        int row = 0;

        while (suppliersQuery.next()) {
            int supplierId = suppliersQuery.value("id").toInt();
            QString supplierName = suppliersQuery.value("name").toString();

            QSqlQuery transQuery = DatabaseManager::instance().getTransactionsBySupplier(supplierId);
            double totalPurchases = 0.0;
            double totalPayments = 0.0;
            double balance = 0.0;

            while (transQuery.next()) {
                totalPurchases += transQuery.value("purchases").toDouble();
                totalPayments += transQuery.value("payments").toDouble();
                balance = transQuery.value("balance").toDouble();
            }

            m_reportsTable->insertRow(row);
            m_reportsTable->setItem(row, 0, new QTableWidgetItem(supplierName));
            m_reportsTable->setItem(row, 1, new QTableWidgetItem(QString::number(totalPurchases, 'f', 2)));
            m_reportsTable->setItem(row, 2, new QTableWidgetItem(QString::number(totalPayments, 'f', 2)));
            m_reportsTable->setItem(row, 3, new QTableWidgetItem(QString::number(balance, 'f', 2)));

            // Color code balance
            QColor balanceColor = balance >= 0 ? QColor(39, 174, 96, 100) : QColor(231, 76, 60, 100);
            m_reportsTable->item(row, 3)->setBackground(balanceColor);

            row++;
        }
    } else if (reportType == "products") {
        QStringList headers = {"Name", "Category", "Price", "Stock", "Value"};
        m_reportsTable->setColumnCount(headers.size());
        m_reportsTable->setHorizontalHeaderLabels(headers);

        QSqlQuery query = DatabaseManager::instance().getAllProducts();

        m_reportsTable->setRowCount(0);
        int row = 0;
        double totalValue = 0.0;

        while (query.next()) {
            double price = query.value("price").toDouble();
            int stock = query.value("stock_quantity").toInt();
            double value = price * stock;
            totalValue += value;

            m_reportsTable->insertRow(row);
            m_reportsTable->setItem(row, 0, new QTableWidgetItem(query.value("name").toString()));
            m_reportsTable->setItem(row, 1, new QTableWidgetItem(query.value("category").toString()));
            m_reportsTable->setItem(row, 2, new QTableWidgetItem(QString::number(price, 'f', 2)));
            m_reportsTable->setItem(row, 3, new QTableWidgetItem(QString::number(stock)));
            m_reportsTable->setItem(row, 4, new QTableWidgetItem(QString::number(value, 'f', 2)));
            row++;
        }

        // Add total row
        m_reportsTable->insertRow(row);
        m_reportsTable->setItem(row, 0, new QTableWidgetItem("TOTAL"));
        m_reportsTable->setItem(row, 4, new QTableWidgetItem(QString::number(totalValue, 'f', 2)));

        for (int col = 0; col < m_reportsTable->columnCount(); col++) {
            if (m_reportsTable->item(row, col)) {
                m_reportsTable->item(row, col)->setBackground(QColor(52, 152, 219, 100));
                m_reportsTable->item(row, col)->setFont(QFont("", -1, QFont::Bold));
            }
        }
    }

    statusBar()->showMessage(QString("Report generated with %1 records").arg(m_reportsTable->rowCount()));
}

void AdminPanel::onExportData()
{
    if (m_reportsTable->rowCount() == 0) {
        QMessageBox::warning(this, "Warning", "No data to export. Please generate a report first.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Export Report",
                                                    QString("report_%1.csv").arg(QDate::currentDate().toString("yyyy-MM-dd")),
                                                    "CSV Files (*.csv)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open file for writing.");
        return;
    }

    QTextStream stream(&file);

    // Write headers
    QStringList headers;
    for (int col = 0; col < m_reportsTable->columnCount(); col++) {
        headers << m_reportsTable->horizontalHeaderItem(col)->text();
    }
    stream << headers.join(",") << "\n";

    // Write data
    for (int row = 0; row < m_reportsTable->rowCount(); row++) {
        QStringList rowData;
        for (int col = 0; col < m_reportsTable->columnCount(); col++) {
            QTableWidgetItem *item = m_reportsTable->item(row, col);
            rowData << (item ? item->text() : "");
        }
        stream << rowData.join(",") << "\n";
    }

    QMessageBox::information(this, "Success", "Report exported successfully!");
}

void AdminPanel::onLoadDailySales()
{
    QString selectedDate = m_dailySalesDateEdit->date().toString("yyyy-MM-dd");
    QSqlQuery query = DatabaseManager::instance().getDailySales(selectedDate);

    m_dailySalesTable->setRowCount(0);
    int row = 0;
    double totalSales = 0.0;
    int transactionCount = 0;

    while (query.next()) {
        m_dailySalesTable->insertRow(row);
        
        int saleId = query.value("id").toInt();
        QString dateTime = query.value("date").toString();
        double total = query.value("total").toDouble();
        int itemsCount = query.value("items_count").toInt();

        m_dailySalesTable->setItem(row, 0, new QTableWidgetItem(QString::number(saleId)));
        m_dailySalesTable->setItem(row, 1, new QTableWidgetItem(dateTime));
        
        QTableWidgetItem *totalItem = new QTableWidgetItem(QString("Rs. %1").arg(total, 0, 'f', 2));
        totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        totalItem->setForeground(QColor(27, 142, 96));
        m_dailySalesTable->setItem(row, 2, totalItem);
        
        m_dailySalesTable->setItem(row, 3, new QTableWidgetItem(QString::number(itemsCount)));

        // Details button
        QPushButton *detailsBtn = new QPushButton("View Details");
        detailsBtn->setStyleSheet(R"(
            QPushButton {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                          stop: 0 #3498db, stop: 1 #2980b9);
                color: white;
                border: none;
                border-radius: 6px;
                padding: 6px 15px;
                font-size: 12px;
                font-weight: bold;
            }
            QPushButton:hover {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                          stop: 0 #2980b9, stop: 1 #21618c);
            }
        )");
        connect(detailsBtn, &QPushButton::clicked, [this, saleId]() {
            QSqlQuery itemsQuery = DatabaseManager::instance().getSaleItems(saleId);
            QString details = QString("Sale ID: %1\n\nItems:\n").arg(saleId);
            double itemTotal = 0.0;
            while (itemsQuery.next()) {
                QString name = itemsQuery.value("name").toString();
                int qty = itemsQuery.value("quantity").toInt();
                double price = itemsQuery.value("unit_price").toDouble();
                double total = itemsQuery.value("total_price").toDouble();
                details += QString("%1 x %2 @ Rs. %3 = Rs. %4\n")
                    .arg(qty).arg(name).arg(price, 0, 'f', 2).arg(total, 0, 'f', 2);
                itemTotal += total;
            }
            details += QString("\nTotal: Rs. %1").arg(itemTotal, 0, 'f', 2);
            QMessageBox::information(this, "Sale Details", details);
        });
        m_dailySalesTable->setCellWidget(row, 4, detailsBtn);

        totalSales += total;
        transactionCount++;
        row++;
    }

    // Update summary labels
    m_dailySalesTotalLabel->setText(QString("Rs. %1").arg(totalSales, 0, 'f', 2));
    m_dailySalesCountLabel->setText(QString::number(transactionCount));

    statusBar()->showMessage(QString("Loaded %1 sales for %2 - Total: Rs. %3")
                             .arg(transactionCount)
                             .arg(selectedDate)
                             .arg(totalSales, 0, 'f', 2));
}

void AdminPanel::onLowStockAlert()
{
    QSqlQuery query = DatabaseManager::instance().getLowStockProducts(10);

    QStringList lowStockItems;
    while (query.next()) {
        lowStockItems << QString("%1 (Stock: %2)")
        .arg(query.value("name").toString())
            .arg(query.value("stock_quantity").toInt());
    }

    if (!lowStockItems.isEmpty()) {
        QMessageBox::warning(this, "Low Stock Alert",
                             "The following items are low in stock:\n\n" + lowStockItems.join("\n"));
    }
}
