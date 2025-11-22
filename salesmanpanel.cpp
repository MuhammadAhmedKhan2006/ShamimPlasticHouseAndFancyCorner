#include "SalesmanPanel.h"
#include <QApplication>
#include <QSqlQuery>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include <QHeaderView>

SalesmanPanel::SalesmanPanel(QWidget *parent)
    : QMainWindow(parent)
    , m_selectedProductId(0)
{
    m_slipPrinter = new SlipPrinter(this);
    setupUI();

    loadCategories();
    loadProducts();

    setWindowTitle("Sales Panel - Shamim Plastic House Management System");
    setMinimumSize(1200, 800);
    showMaximized();
}

void SalesmanPanel::setupUI()
{
    // Set modern application-wide stylesheet
    setStyleSheet(R"(
        QMainWindow {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f5f7fa, stop: 1 #c3cfe2);
        }
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #3498db;
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            background: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 5px;
            color: #2c3e50;
        }
        QTableWidget {
            background: white;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            gridline-color: #ecf0f1;
            font-size: 12px;
            selection-background-color: #3498db;
            selection-color: white;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QTableWidget::item:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
        }
        QHeaderView::section {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #34495e, stop: 1 #2c3e50);
            color: white;
            padding: 10px;
            border: none;
            font-weight: bold;
            font-size: 13px;
        }
        QLineEdit, QComboBox {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 8px;
            font-size: 13px;
            background: white;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 2px solid #3498db;
            background: #ecf0f1;
        }
        QTextEdit {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 8px;
            font-size: 13px;
            background: white;
        }
        QTextEdit:focus {
            border: 2px solid #3498db;
        }
    )");

    setCentralWidget(new QWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Header with gradient and shadow effect
    QLabel *headerLabel = new QLabel("🛒 SALES POINT SYSTEM");
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

    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    m_mainSplitter->setStyleSheet("QSplitter::handle { background: #bdc3c7; }");

    setupProductBrowser();
    setupCartSection();

    m_mainSplitter->addWidget(m_productBrowserWidget);
    m_mainSplitter->addWidget(m_cartWidget);
    m_mainSplitter->setStretchFactor(0, 2);
    m_mainSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(m_mainSplitter);

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
    statusBar()->showMessage("✅ Ready for sales - All systems operational");
}

void SalesmanPanel::setupProductBrowser()
{
    m_productBrowserWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(m_productBrowserWidget);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    // Controls
    QGroupBox *controlGroup = new QGroupBox("📦 PRODUCT BROWSER - Available Items Only");
    controlGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 15px;
            border: 3px solid #3498db;
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
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(12);

    // Category and search
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->setSpacing(10);
    
    QLabel *catLabel = new QLabel("Category:");
    catLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #2c3e50;");
    filterLayout->addWidget(catLabel);
    
    m_categoryCombo = new QComboBox;
    m_categoryCombo->setMinimumWidth(220);
    m_categoryCombo->setStyleSheet(R"(
        QComboBox {
            border: 2px solid #3498db;
            border-radius: 8px;
            padding: 10px;
            font-size: 13px;
            background: white;
            min-height: 20px;
        }
        QComboBox:hover {
            border: 2px solid #2980b9;
            background: #ecf0f1;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 8px solid #3498db;
            margin-right: 10px;
        }
    )");
    filterLayout->addWidget(m_categoryCombo);

    QLabel *searchLabel = new QLabel("Search:");
    searchLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #2c3e50;");
    filterLayout->addWidget(searchLabel);
    
    m_productSearchEdit = new QLineEdit;
    m_productSearchEdit->setPlaceholderText("🔍 Search products by name or category...");
    m_productSearchEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #3498db;
            border-radius: 8px;
            padding: 10px;
            font-size: 13px;
            background: white;
        }
        QLineEdit:focus {
            border: 3px solid #2980b9;
            background: #ecf0f1;
        }
    )");
    filterLayout->addWidget(m_productSearchEdit);

    QPushButton *searchBtn = new QPushButton("🔍 Search");
    searchBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 25px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }
        QPushButton:pressed {
            background: #21618c;
        }
    )");
    filterLayout->addWidget(searchBtn);
    filterLayout->addStretch();

    // Product table
    m_productTable = new QTableWidget;
    m_productTable->setColumnCount(6);
    QStringList headers = {"ID", "Name", "Category", "Subcategory", "Price (Rs)", "Stock"};
    m_productTable->setHorizontalHeaderLabels(headers);
    m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_productTable->setAlternatingRowColors(true);
    m_productTable->horizontalHeader()->setStretchLastSection(true);
    m_productTable->setColumnHidden(0, true); // Hide ID column
    m_productTable->setStyleSheet(R"(
        QTableWidget {
            background: white;
            border: 2px solid #bdc3c7;
            border-radius: 10px;
            gridline-color: #ecf0f1;
            font-size: 13px;
            selection-background-color: #3498db;
            selection-color: white;
        }
        QTableWidget::item {
            padding: 10px;
        }
        QTableWidget::item:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
        }
        QHeaderView::section {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #667eea, stop: 1 #764ba2);
            color: white;
            padding: 12px;
            border: none;
            font-weight: bold;
            font-size: 14px;
        }
    )");

    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout;
    actionLayout->setSpacing(10);
    
    m_addToCartBtn = new QPushButton("➕ Add to Cart");
    m_addToCartBtn->setStyleSheet(R"(
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
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    m_addToCartBtn->setEnabled(false);

    m_quickAddBtn = new QPushButton("⚡ Quick Add");
    m_quickAddBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f39c12, stop: 1 #e67e22);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e67e22, stop: 1 #d35400);
        }
        QPushButton:pressed {
            background: #d35400;
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    m_quickAddBtn->setEnabled(false);

    actionLayout->addWidget(m_addToCartBtn);
    actionLayout->addWidget(m_quickAddBtn);
    actionLayout->addStretch();

    controlLayout->addLayout(filterLayout);
    controlLayout->addWidget(m_productTable);
    controlLayout->addLayout(actionLayout);

    layout->addWidget(controlGroup);

    // Connect signals
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SalesmanPanel::onCategoryChanged);
    connect(searchBtn, &QPushButton::clicked, this, &SalesmanPanel::onSearchProducts);
    connect(m_productSearchEdit, &QLineEdit::returnPressed, this, &SalesmanPanel::onSearchProducts);
    connect(m_productTable, &QTableWidget::itemDoubleClicked, this, &SalesmanPanel::onProductDoubleClicked);
    connect(m_productTable, &QTableWidget::itemSelectionChanged, [this]() {
        bool hasSelection = !m_productTable->selectedItems().isEmpty();
        m_addToCartBtn->setEnabled(hasSelection);
        m_quickAddBtn->setEnabled(hasSelection);

        if (hasSelection) {
            int row = m_productTable->selectedItems().first()->row();
            m_selectedProductId = m_productTable->item(row, 0)->text().toInt();
        }
    });
    connect(m_addToCartBtn, &QPushButton::clicked, this, &SalesmanPanel::onAddToCart);
    connect(m_quickAddBtn, &QPushButton::clicked, this, &SalesmanPanel::onQuickAddProduct);
}

void SalesmanPanel::setupCartSection()
{
    m_cartWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(m_cartWidget);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    QGroupBox *cartGroup = new QGroupBox("🛒 SHOPPING CART");
    cartGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 15px;
            border: 3px solid #e74c3c;
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
    QVBoxLayout *cartLayout = new QVBoxLayout(cartGroup);
    cartLayout->setSpacing(12);

    // Cart table
    m_cartTable = new QTableWidget;
    m_cartTable->setColumnCount(5);
    QStringList cartHeaders = {"Product", "Qty", "Unit Price", "Total", "Actions"};
    m_cartTable->setHorizontalHeaderLabels(cartHeaders);
    m_cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_cartTable->horizontalHeader()->setStretchLastSection(true);
    m_cartTable->setMaximumHeight(350);
    m_cartTable->setStyleSheet(R"(
        QTableWidget {
            background: white;
            border: 2px solid #bdc3c7;
            border-radius: 10px;
            gridline-color: #ecf0f1;
            font-size: 13px;
            selection-background-color: #e74c3c;
            selection-color: white;
        }
        QTableWidget::item {
            padding: 10px;
        }
        QTableWidget::item:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
            color: white;
        }
        QHeaderView::section {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
            color: white;
            padding: 12px;
            border: none;
            font-weight: bold;
            font-size: 14px;
        }
    )");

    // Total display
    QFrame *totalFrame = new QFrame;
    totalFrame->setFrameStyle(QFrame::StyledPanel);
    totalFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                                      stop: 0 #667eea, stop: 1 #764ba2);
            border-radius: 12px;
            padding: 15px;
            border: 3px solid #5a67d8;
        }
    )");
    QHBoxLayout *totalLayout = new QHBoxLayout(totalFrame);
    QLabel *totalTextLabel = new QLabel("💰 TOTAL:");
    totalTextLabel->setStyleSheet("color: white; font-weight: bold; font-size: 18px;");
    m_totalLabel = new QLabel("Rs. 0.00");
    m_totalLabel->setStyleSheet("color: white; font-weight: bold; font-size: 24px;");
    totalLayout->addWidget(totalTextLabel);
    totalLayout->addStretch();
    totalLayout->addWidget(m_totalLabel);

    // Customer note
    QLabel *noteLabel = new QLabel("📝 Customer Note:");
    noteLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #2c3e50;");
    m_customerNoteEdit = new QTextEdit;
    m_customerNoteEdit->setMaximumHeight(90);
    m_customerNoteEdit->setPlaceholderText("Optional customer note or special instructions...");
    m_customerNoteEdit->setStyleSheet(R"(
        QTextEdit {
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 10px;
            font-size: 13px;
            background: white;
        }
        QTextEdit:focus {
            border: 3px solid #3498db;
            background: #ecf0f1;
        }
    )");

    // Cart action buttons
    QHBoxLayout *cartButtonLayout = new QHBoxLayout;
    cartButtonLayout->setSpacing(10);
    
    m_removeFromCartBtn = new QPushButton("❌ Remove Item");
    m_removeFromCartBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ec7063, stop: 1 #e74c3c);
        }
        QPushButton:pressed {
            background: #c0392b;
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    m_removeFromCartBtn->setEnabled(false);

    m_clearCartBtn = new QPushButton("🗑️ Clear Cart");
    m_clearCartBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f39c12, stop: 1 #e67e22);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e67e22, stop: 1 #d35400);
        }
        QPushButton:pressed {
            background: #d35400;
        }
    )");

    cartButtonLayout->addWidget(m_removeFromCartBtn);
    cartButtonLayout->addWidget(m_clearCartBtn);

    // Final action buttons
    QVBoxLayout *finalButtonLayout = new QVBoxLayout;
    finalButtonLayout->setSpacing(10);
    
    m_finalizeSaleBtn = new QPushButton("✅ FINALIZE SALE");
    m_finalizeSaleBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            border: none;
            border-radius: 12px;
            padding: 18px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2ecc71, stop: 1 #27ae60);
        }
        QPushButton:pressed {
            background: #229954;
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    m_finalizeSaleBtn->setEnabled(false);

    m_printInvoiceBtn = new QPushButton("🖨️ Print Invoice");
    m_printInvoiceBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #8e44ad, stop: 1 #7d3c98);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 12px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #a569bd, stop: 1 #8e44ad);
        }
        QPushButton:pressed {
            background: #7d3c98;
        }
        QPushButton:disabled {
            background: #95a5a6;
            color: #7f8c8d;
        }
    )");
    m_printInvoiceBtn->setEnabled(false);

    finalButtonLayout->addWidget(m_finalizeSaleBtn);
    finalButtonLayout->addWidget(m_printInvoiceBtn);

    cartLayout->addWidget(m_cartTable);
    cartLayout->addWidget(totalFrame);
    cartLayout->addWidget(noteLabel);
    cartLayout->addWidget(m_customerNoteEdit);
    cartLayout->addLayout(cartButtonLayout);
    cartLayout->addLayout(finalButtonLayout);

    layout->addWidget(cartGroup);

    // Connect cart signals
    connect(m_cartTable, &QTableWidget::itemSelectionChanged, [this]() {
        m_removeFromCartBtn->setEnabled(!m_cartTable->selectedItems().isEmpty());
    });
    connect(m_cartTable, &QTableWidget::cellChanged, this, &SalesmanPanel::onUpdateCartQuantity);
    connect(m_removeFromCartBtn, &QPushButton::clicked, this, &SalesmanPanel::onRemoveFromCart);
    connect(m_clearCartBtn, &QPushButton::clicked, this, &SalesmanPanel::onClearCart);
    connect(m_finalizeSaleBtn, &QPushButton::clicked, this, &SalesmanPanel::onFinalizeSale);
    connect(m_printInvoiceBtn, &QPushButton::clicked, this, &SalesmanPanel::onPrintInvoice);
}

void SalesmanPanel::setupActionButtons()
{
    // This function is for future expansion if needed
}

void SalesmanPanel::loadCategories()
{
    m_categoryCombo->clear();
    m_categoryCombo->addItem("All Categories", "");

    QSqlQuery query = DatabaseManager::instance().getCategories();
    while (query.next()) {
        m_categoryCombo->addItem(query.value("category").toString());
    }
}

void SalesmanPanel::loadProducts()
{
    // Salesman can only see products with stock > 0 (available products)
    QSqlQuery query = DatabaseManager::instance().getAvailableProducts();

    m_productTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_productTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(query.value("category").toString()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(query.value("subcategory").toString()));

        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(query.value("price").toDouble(), 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_productTable->setItem(row, 4, priceItem);

        QTableWidgetItem *stockItem = new QTableWidgetItem(query.value("stock_quantity").toString());
        stockItem->setTextAlignment(Qt::AlignCenter);
        m_productTable->setItem(row, 5, stockItem);

        // Highlight low stock items
        int stock = query.value("stock_quantity").toInt();
        if (stock <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(255, 193, 7, 120));
                m_productTable->item(row, col)->setForeground(QColor(139, 69, 19));
            }
        } else {
            // Green highlight for good stock
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(200, 230, 201, 80));
            }
        }

        row++;
    }

    statusBar()->showMessage(QString("Loaded %1 available products").arg(row));
}

void SalesmanPanel::loadProductsByCategory()
{
    QString category = m_categoryCombo->currentText();

    if (category == "All Categories") {
        loadProducts();
        return;
    }

    // Salesman can only see available products
    QSqlQuery query = DatabaseManager::instance().getAvailableProductsByCategory(category);

    m_productTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_productTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(query.value("category").toString()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(query.value("subcategory").toString()));

        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(query.value("price").toDouble(), 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_productTable->setItem(row, 4, priceItem);

        QTableWidgetItem *stockItem = new QTableWidgetItem(query.value("stock_quantity").toString());
        stockItem->setTextAlignment(Qt::AlignCenter);
        m_productTable->setItem(row, 5, stockItem);

        // Highlight low stock items
        int stock = query.value("stock_quantity").toInt();
        if (stock <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(255, 193, 7, 120));
                m_productTable->item(row, col)->setForeground(QColor(139, 69, 19));
            }
        } else {
            // Green highlight for good stock
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(200, 230, 201, 80));
            }
        }

        row++;
    }

    statusBar()->showMessage(QString("Loaded %1 available products for category: %2").arg(row).arg(category));
}

void SalesmanPanel::updateCartTotal()
{
    double total = 0.0;

    for (int row = 0; row < m_cartTable->rowCount(); row++) {
        QTableWidgetItem *totalItem = m_cartTable->item(row, 3);
        if (totalItem) {
            total += totalItem->text().toDouble();
        }
    }

    m_totalLabel->setText(QString("Rs. %1").arg(total, 0, 'f', 2));
    m_finalizeSaleBtn->setEnabled(total > 0);
}

void SalesmanPanel::clearCart()
{
    m_cartTable->setRowCount(0);
    m_currentSale.clearItems();
    updateCartTotal();
    m_printInvoiceBtn->setEnabled(false);
}

void SalesmanPanel::onCategoryChanged()
{
    loadProductsByCategory();
}

void SalesmanPanel::onSearchProducts()
{
    QString searchTerm = m_productSearchEdit->text().trimmed();

    if (searchTerm.isEmpty()) {
        if (m_categoryCombo->currentText() == "All Categories") {
            loadProducts();
        } else {
            loadProductsByCategory();
        }
        return;
    }

    // Salesman can only search available products
    QSqlQuery query = DatabaseManager::instance().searchAvailableProducts(searchTerm);

    m_productTable->setRowCount(0);
    int row = 0;

    while (query.next()) {
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        m_productTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(query.value("category").toString()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(query.value("subcategory").toString()));

        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(query.value("price").toDouble(), 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_productTable->setItem(row, 4, priceItem);

        QTableWidgetItem *stockItem = new QTableWidgetItem(query.value("stock_quantity").toString());
        stockItem->setTextAlignment(Qt::AlignCenter);
        m_productTable->setItem(row, 5, stockItem);

        // Highlight low stock items
        int stock = query.value("stock_quantity").toInt();
        if (stock <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(255, 193, 7, 120));
                m_productTable->item(row, col)->setForeground(QColor(139, 69, 19));
            }
        } else {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(200, 230, 201, 80));
            }
        }

        row++;
    }

    statusBar()->showMessage(QString("Found %1 available products matching '%2'").arg(row).arg(searchTerm));
}

void SalesmanPanel::onProductDoubleClicked()
{
    onAddToCart();
}

void SalesmanPanel::onAddToCart()
{
    if (m_selectedProductId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a product.");
        return;
    }

    int row = m_productTable->selectedItems().first()->row();
    QString productName = m_productTable->item(row, 1)->text();
    double unitPrice = m_productTable->item(row, 4)->text().toDouble();
    int availableStock = m_productTable->item(row, 5)->text().toInt();

    if (availableStock <= 0) {
        QMessageBox::warning(this, "Warning", "This product is out of stock.");
        return;
    }

    // Check if product already in cart
    for (int cartRow = 0; cartRow < m_cartTable->rowCount(); cartRow++) {
        QTableWidgetItem *nameItem = m_cartTable->item(cartRow, 0);
        if (nameItem && nameItem->data(Qt::UserRole).toInt() == m_selectedProductId) {
            // Update quantity
            QTableWidgetItem *qtyItem = m_cartTable->item(cartRow, 1);
            int currentQty = qtyItem->text().toInt();
            qtyItem->setText(QString::number(currentQty + 1));

            // Update total for this row
            double total = (currentQty + 1) * unitPrice;
            m_cartTable->item(cartRow, 3)->setText(QString::number(total, 'f', 2));

            updateCartTotal();
            return;
        }
    }

    // Add new item to cart
    int cartRow = m_cartTable->rowCount();
    m_cartTable->insertRow(cartRow);

    QTableWidgetItem *nameItem = new QTableWidgetItem(productName);
    nameItem->setData(Qt::UserRole, m_selectedProductId);
    m_cartTable->setItem(cartRow, 0, nameItem);

    QTableWidgetItem *qtyItem = new QTableWidgetItem("1");
    qtyItem->setFlags(qtyItem->flags() | Qt::ItemIsEditable);
    qtyItem->setTextAlignment(Qt::AlignCenter);
    m_cartTable->setItem(cartRow, 1, qtyItem);

    QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(unitPrice, 'f', 2));
    priceItem->setFlags(priceItem->flags() | Qt::ItemIsEditable);
    priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_cartTable->setItem(cartRow, 2, priceItem);

    QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(unitPrice, 'f', 2));
    totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_cartTable->setItem(cartRow, 3, totalItem);

    QPushButton *updateBtn = new QPushButton("Update");
    updateBtn->setStyleSheet("QPushButton { padding: 5px 10px; }");
    connect(updateBtn, &QPushButton::clicked, [this]() {
        onUpdateCartPrice();
    });
    m_cartTable->setCellWidget(cartRow, 4, updateBtn);

    updateCartTotal();
    statusBar()->showMessage(QString("Added %1 to cart").arg(productName));
}

void SalesmanPanel::onQuickAddProduct()
{
    if (m_selectedProductId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a product.");
        return;
    }

    int row = m_productTable->selectedItems().first()->row();
    QString productName = m_productTable->item(row, 1)->text();
    double unitPrice = m_productTable->item(row, 4)->text().toDouble();
    int availableStock = m_productTable->item(row, 5)->text().toInt();

    if (availableStock <= 0) {
        QMessageBox::warning(this, "Warning", "This product is out of stock.");
        return;
    }

    bool ok;
    int quantity = QInputDialog::getInt(this, "Quick Add",
                                        QString("Enter quantity for %1:").arg(productName),
                                        1, 1, availableStock, 1, &ok);

    if (!ok) return;

    // Check if product already in cart
    for (int cartRow = 0; cartRow < m_cartTable->rowCount(); cartRow++) {
        QTableWidgetItem *nameItem = m_cartTable->item(cartRow, 0);
        if (nameItem && nameItem->data(Qt::UserRole).toInt() == m_selectedProductId) {
            // Update quantity
            QTableWidgetItem *qtyItem = m_cartTable->item(cartRow, 1);
            int currentQty = qtyItem->text().toInt();
            qtyItem->setText(QString::number(currentQty + quantity));

            // Update total for this row
            double total = (currentQty + quantity) * unitPrice;
            m_cartTable->item(cartRow, 3)->setText(QString::number(total, 'f', 2));

            updateCartTotal();
            return;
        }
    }

    // Add new item to cart
    int cartRow = m_cartTable->rowCount();
    m_cartTable->insertRow(cartRow);

    QTableWidgetItem *nameItem = new QTableWidgetItem(productName);
    nameItem->setData(Qt::UserRole, m_selectedProductId);
    m_cartTable->setItem(cartRow, 0, nameItem);

    QTableWidgetItem *qtyItem = new QTableWidgetItem(QString::number(quantity));
    qtyItem->setFlags(qtyItem->flags() | Qt::ItemIsEditable);
    qtyItem->setTextAlignment(Qt::AlignCenter);
    m_cartTable->setItem(cartRow, 1, qtyItem);

    QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(unitPrice, 'f', 2));
    priceItem->setFlags(priceItem->flags() | Qt::ItemIsEditable);
    priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_cartTable->setItem(cartRow, 2, priceItem);

    QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(quantity * unitPrice, 'f', 2));
    totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_cartTable->setItem(cartRow, 3, totalItem);

    QPushButton *updateBtn = new QPushButton("Update");
    updateBtn->setStyleSheet("QPushButton { padding: 5px 10px; }");
    connect(updateBtn, &QPushButton::clicked, [this]() {
        onUpdateCartPrice();
    });
    m_cartTable->setCellWidget(cartRow, 4, updateBtn);

    updateCartTotal();
    statusBar()->showMessage(QString("Added %1 x %2 to cart").arg(quantity).arg(productName));
}

void SalesmanPanel::onRemoveFromCart()
{
    QList<QTableWidgetItem*> selectedItems = m_cartTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select an item to remove.");
        return;
    }

    int row = selectedItems.first()->row();
    QString productName = m_cartTable->item(row, 0)->text();

    int ret = QMessageBox::question(this, "Confirm Remove",
                                    QString("Remove %1 from cart?").arg(productName),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        m_cartTable->removeRow(row);
        updateCartTotal();
        statusBar()->showMessage(QString("Removed %1 from cart").arg(productName));
    }
}

void SalesmanPanel::onUpdateCartQuantity()
{
    // This is called when cart table cells are edited
    for (int row = 0; row < m_cartTable->rowCount(); row++) {
        QTableWidgetItem *qtyItem = m_cartTable->item(row, 1);
        QTableWidgetItem *priceItem = m_cartTable->item(row, 2);
        QTableWidgetItem *totalItem = m_cartTable->item(row, 3);

        if (qtyItem && priceItem && totalItem) {
            int quantity = qtyItem->text().toInt();
            double unitPrice = priceItem->text().toDouble();
            double total = quantity * unitPrice;

            totalItem->setText(QString::number(total, 'f', 2));
        }
    }

    updateCartTotal();
}

void SalesmanPanel::onUpdateCartPrice()
{
    // Manual price update (called by Update button)
    onUpdateCartQuantity();
}

void SalesmanPanel::onClearCart()
{
    if (m_cartTable->rowCount() == 0) {
        return;
    }

    int ret = QMessageBox::question(this, "Confirm Clear",
                                    "Clear all items from cart?",
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        clearCart();
        statusBar()->showMessage("Cart cleared");
    }
}

void SalesmanPanel::onFinalizeSale()
{
    if (m_cartTable->rowCount() == 0) {
        QMessageBox::warning(this, "Warning", "Cart is empty.");
        return;
    }

    // Prepare sale data
    QList<QPair<int, QPair<int, double>>> saleItems;
    double total = 0.0;

    m_currentSale.clearItems();
    m_currentSale.setDate(QDateTime::currentDateTime());
    m_currentSale.setCustomerNote(m_customerNoteEdit->toPlainText());

    for (int row = 0; row < m_cartTable->rowCount(); row++) {
        int productId = m_cartTable->item(row, 0)->data(Qt::UserRole).toInt();
        QString productName = m_cartTable->item(row, 0)->text();
        int quantity = m_cartTable->item(row, 1)->text().toInt();
        double unitPrice = m_cartTable->item(row, 2)->text().toDouble();
        double itemTotal = m_cartTable->item(row, 3)->text().toDouble();

        // Add to sale items list for database
        saleItems.append(qMakePair(productId, qMakePair(quantity, unitPrice)));

        // Add to current sale object for printing
        SaleItem saleItem(productId, productName, quantity, unitPrice);
        m_currentSale.addItem(saleItem);

        total += itemTotal;
    }

    m_currentSale.setTotal(total);

    // Save to database
    QString dateString = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if (DatabaseManager::instance().addSale(dateString, total, saleItems)) {
        QMessageBox::information(this, "Success",
                                 QString("Sale completed successfully!\nTotal: Rs. %1").arg(total, 0, 'f', 2));

        m_printInvoiceBtn->setEnabled(true);
        statusBar()->showMessage(QString("Sale completed - Total: Rs. %1").arg(total, 0, 'f', 2));

        // Refresh product list to show updated stock
        if (m_categoryCombo->currentText() == "All Categories") {
            loadProducts();
        } else {
            loadProductsByCategory();
        }

    } else {
        QMessageBox::critical(this, "Error", "Failed to save sale to database.");
    }
}

void SalesmanPanel::onPrintInvoice()
{
    if (m_currentSale.getItems().isEmpty()) {
        QMessageBox::warning(this, "Warning", "No sale data available to print.");
        return;
    }

    m_slipPrinter->previewSalesInvoice(m_currentSale, this);
}
