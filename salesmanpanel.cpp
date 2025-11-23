#include "salesmanpanel.h"
#include <QApplication>
#include <QSqlQuery>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include <QHeaderView>
#include <QSpinBox>

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
            color: #2c3e50;
        }
        QTableWidget::item {
            padding: 8px;
            color: #2c3e50;
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
        QLineEdit, QComboBox, QSpinBox {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 8px;
            font-size: 13px;
            background: white;
            color: #2c3e50;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus {
            border: 2px solid #3498db;
            background: #ecf0f1;
        }
        QTextEdit {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 8px;
            font-size: 13px;
            background: white;
            color: #2c3e50;
        }
        QTextEdit:focus {
            border: 2px solid #3498db;
        }
    )");

    setCentralWidget(new QWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Header with back button
    QHBoxLayout *headerLayout = new QHBoxLayout;

    QPushButton *backBtn = new QPushButton("← Back to Login");
    backBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #95a5a6, stop: 1 #7f8c8d);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #7f8c8d, stop: 1 #5d6d7e);
        }
    )");
    backBtn->setMaximumWidth(200);
    connect(backBtn, &QPushButton::clicked, this, &QMainWindow::close);

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
        }
    )");
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setMinimumHeight(80);

    headerLayout->addWidget(backBtn);
    headerLayout->addWidget(headerLabel, 1);

    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    m_mainSplitter->setStyleSheet("QSplitter::handle { background: #bdc3c7; }");

    setupProductBrowser();
    setupCartSection();

    m_mainSplitter->addWidget(m_productBrowserWidget);
    m_mainSplitter->addWidget(m_cartWidget);
    m_mainSplitter->setStretchFactor(0, 2);
    m_mainSplitter->setStretchFactor(1, 1);

    mainLayout->addLayout(headerLayout);
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
    QGroupBox *controlGroup = new QGroupBox("📦 PRODUCT BROWSER - Available Items");
    controlGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 16px;
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
            font-size: 16px;
        }
    )");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(12);

    // Category and search
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->setSpacing(10);

    QLabel *catLabel = new QLabel("Category:");
    catLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    filterLayout->addWidget(catLabel);

    m_categoryCombo = new QComboBox;
    m_categoryCombo->setMinimumWidth(220);
    m_categoryCombo->setStyleSheet(R"(
        QComboBox {
            border: 2px solid #3498db;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            background: white;
            color: #2c3e50;
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
    )");
    filterLayout->addWidget(m_categoryCombo);

    QLabel *searchLabel = new QLabel("Search:");
    searchLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    filterLayout->addWidget(searchLabel);

    m_productSearchEdit = new QLineEdit;
    m_productSearchEdit->setPlaceholderText("🔍 Search products by name or category...");
    m_productSearchEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #3498db;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            background: white;
            color: #2c3e50;
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
    m_productTable->setColumnHidden(0, true);
    m_productTable->setStyleSheet(R"(
        QTableWidget {
            background: white;
            border: 2px solid #bdc3c7;
            border-radius: 10px;
            gridline-color: #ecf0f1;
            font-size: 13px;
            selection-background-color: #3498db;
            selection-color: white;
            color: #2c3e50;
        }
        QTableWidget::item {
            padding: 10px;
            color: #2c3e50;
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
            font-size: 16px;
            border: 3px solid #e74c3c;
            border-radius: 12px;
            margin-top: 12px;
            padding-top: 20px;
            background: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 20px;
            padding: 0 10px;
            color: #2c3e50;
            font-size: 16px;
        }
    )");
    QVBoxLayout *cartLayout = new QVBoxLayout(cartGroup);
    cartLayout->setSpacing(12);

    // Cart table - FIXED: White background, dark text, NO blue selection
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
            font-size: 14px;
            color: #2c3e50;
        }
        QTableWidget::item {
            padding: 10px;
            color: #2c3e50;
            background: white;
        }
        QTableWidget::item:selected {
            background: #ecf0f1;
            color: #2c3e50;
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
    noteLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    m_customerNoteEdit = new QTextEdit;
    m_customerNoteEdit->setMaximumHeight(90);
    m_customerNoteEdit->setPlaceholderText("Optional customer note or special instructions...");
    m_customerNoteEdit->setStyleSheet(R"(
        QTextEdit {
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            background: white;
            color: #2c3e50;
        }
        QTextEdit:focus {
            border: 3px solid #3498db;
            background: white;
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
    connect(m_removeFromCartBtn, &QPushButton::clicked, this, &SalesmanPanel::onRemoveFromCart);
    connect(m_clearCartBtn, &QPushButton::clicked, this, &SalesmanPanel::onClearCart);
    connect(m_finalizeSaleBtn, &QPushButton::clicked, this, &SalesmanPanel::onFinalizeSale);
    connect(m_printInvoiceBtn, &QPushButton::clicked, this, &SalesmanPanel::onPrintInvoice);
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
        priceItem->setForeground(QColor("#2c3e50"));
        m_productTable->setItem(row, 4, priceItem);

        QTableWidgetItem *stockItem = new QTableWidgetItem(query.value("stock_quantity").toString());
        stockItem->setTextAlignment(Qt::AlignCenter);
        stockItem->setForeground(QColor("#2c3e50"));
        m_productTable->setItem(row, 5, stockItem);

        int stock = query.value("stock_quantity").toInt();
        if (stock <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(255, 193, 7, 120));
                m_productTable->item(row, col)->setForeground(QColor(139, 69, 19));
            }
        } else {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(200, 230, 201, 80));
                m_productTable->item(row, col)->setForeground(QColor("#2c3e50"));
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
        priceItem->setForeground(QColor("#2c3e50"));
        m_productTable->setItem(row, 4, priceItem);

        QTableWidgetItem *stockItem = new QTableWidgetItem(query.value("stock_quantity").toString());
        stockItem->setTextAlignment(Qt::AlignCenter);
        stockItem->setForeground(QColor("#2c3e50"));
        m_productTable->setItem(row, 5, stockItem);

        int stock = query.value("stock_quantity").toInt();
        if (stock <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(255, 193, 7, 120));
                m_productTable->item(row, col)->setForeground(QColor(139, 69, 19));
            }
        } else {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(200, 230, 201, 80));
                m_productTable->item(row, col)->setForeground(QColor("#2c3e50"));
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
        priceItem->setForeground(QColor("#2c3e50"));
        m_productTable->setItem(row, 4, priceItem);

        QTableWidgetItem *stockItem = new QTableWidgetItem(query.value("stock_quantity").toString());
        stockItem->setTextAlignment(Qt::AlignCenter);
        stockItem->setForeground(QColor("#2c3e50"));
        m_productTable->setItem(row, 5, stockItem);

        int stock = query.value("stock_quantity").toInt();
        if (stock <= 10) {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(255, 193, 7, 120));
                m_productTable->item(row, col)->setForeground(QColor(139, 69, 19));
            }
        } else {
            for (int col = 0; col < m_productTable->columnCount(); col++) {
                m_productTable->item(row, col)->setBackground(QColor(200, 230, 201, 80));
                m_productTable->item(row, col)->setForeground(QColor("#2c3e50"));
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

    // Ask for quantity
    bool ok;
    int quantity = QInputDialog::getInt(this, "Add to Cart",
                                        QString("Enter quantity for %1\n(Available: %2)").arg(productName).arg(availableStock),
                                        1, 1, availableStock, 1, &ok);

    if (!ok) return;

    // Check if product already in cart
    for (int cartRow = 0; cartRow < m_cartTable->rowCount(); cartRow++) {
        QTableWidgetItem *nameItem = m_cartTable->item(cartRow, 0);
        if (nameItem && nameItem->data(Qt::UserRole).toInt() == m_selectedProductId) {
            // Update quantity
            QSpinBox *qtySpinBox = qobject_cast<QSpinBox*>(m_cartTable->cellWidget(cartRow, 1));
            if (qtySpinBox) {
                int newQty = qtySpinBox->value() + quantity;
                if (newQty > availableStock) {
                    QMessageBox::warning(this, "Warning",
                                         QString("Cannot add more. Total would exceed available stock (%1)").arg(availableStock));
                    return;
                }
                qtySpinBox->setValue(newQty);
                double total = newQty * unitPrice;
                m_cartTable->item(cartRow, 3)->setText(QString::number(total, 'f', 2));
                updateCartTotal();
                return;
            }
        }
    }

    // Add new item to cart
    int cartRow = m_cartTable->rowCount();
    m_cartTable->insertRow(cartRow);

    QTableWidgetItem *nameItem = new QTableWidgetItem(productName);
    nameItem->setData(Qt::UserRole, m_selectedProductId);
    nameItem->setForeground(QColor("#2c3e50"));
    m_cartTable->setItem(cartRow, 0, nameItem);

    // Quantity with SpinBox
    QSpinBox *qtySpinBox = new QSpinBox;
    qtySpinBox->setRange(1, availableStock);
    qtySpinBox->setValue(quantity);
    qtySpinBox->setStyleSheet(R"(
        QSpinBox {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 5px;
            font-size: 13px;
            background: white;
            color: #2c3e50;
        }
        QSpinBox:focus {
            border: 2px solid #3498db;
        }
    )");
    connect(qtySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, cartRow, unitPrice](int newQty) {
        double total = newQty * unitPrice;
        m_cartTable->item(cartRow, 3)->setText(QString::number(total, 'f', 2));
        updateCartTotal();
    });
    m_cartTable->setCellWidget(cartRow, 1, qtySpinBox);

    QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(unitPrice, 'f', 2));
    priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    priceItem->setForeground(QColor("#2c3e50"));
    m_cartTable->setItem(cartRow, 2, priceItem);

    QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(unitPrice * quantity, 'f', 2));
    totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    totalItem->setForeground(QColor("#2c3e50"));
    m_cartTable->setItem(cartRow, 3, totalItem);

    QPushButton *removeBtn = new QPushButton("Remove");
    removeBtn->setStyleSheet(R"(
        QPushButton {
            background: #e74c3c;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 5px 10px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #c0392b;
        }
    )");
    connect(removeBtn, &QPushButton::clicked, [this, cartRow]() {
        m_cartTable->removeRow(cartRow);
        updateCartTotal();
    });
    m_cartTable->setCellWidget(cartRow, 4, removeBtn);

    updateCartTotal();
    statusBar()->showMessage(QString("Added %1x %2 to cart").arg(quantity).arg(productName));
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

    // Quick add with quantity 1
    for (int cartRow = 0; cartRow < m_cartTable->rowCount(); cartRow++) {
        QTableWidgetItem *nameItem = m_cartTable->item(cartRow, 0);
        if (nameItem && nameItem->data(Qt::UserRole).toInt() == m_selectedProductId) {
            QSpinBox *qtySpinBox = qobject_cast<QSpinBox*>(m_cartTable->cellWidget(cartRow, 1));
            if (qtySpinBox) {
                int newQty = qtySpinBox->value() + 1;
                if (newQty > availableStock) {
                    QMessageBox::warning(this, "Warning", "Cannot add more. Stock limit reached.");
                    return;
                }
                qtySpinBox->setValue(newQty);
                double total = newQty * unitPrice;
                m_cartTable->item(cartRow, 3)->setText(QString::number(total, 'f', 2));
                updateCartTotal();
                return;
            }
        }
    }

    // Add new item with quantity 1
    int cartRow = m_cartTable->rowCount();
    m_cartTable->insertRow(cartRow);

    QTableWidgetItem *nameItem = new QTableWidgetItem(productName);
    nameItem->setData(Qt::UserRole, m_selectedProductId);
    nameItem->setForeground(QColor("#2c3e50"));
    m_cartTable->setItem(cartRow, 0, nameItem);

    QSpinBox *qtySpinBox = new QSpinBox;
    qtySpinBox->setRange(1, availableStock);
    qtySpinBox->setValue(1);
    qtySpinBox->setStyleSheet(R"(
        QSpinBox {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 5px;
            font-size: 13px;
            background: white;
            color: #2c3e50;
        }
        QSpinBox:focus {
            border: 2px solid #3498db;
        }
    )");
    connect(qtySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, cartRow, unitPrice](int newQty) {
        double total = newQty * unitPrice;
        m_cartTable->item(cartRow, 3)->setText(QString::number(total, 'f', 2));
        updateCartTotal();
    });
    m_cartTable->setCellWidget(cartRow, 1, qtySpinBox);

    QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(unitPrice, 'f', 2));
    priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    priceItem->setForeground(QColor("#2c3e50"));
    m_cartTable->setItem(cartRow, 2, priceItem);

    QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(unitPrice, 'f', 2));
    totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    totalItem->setForeground(QColor("#2c3e50"));
    m_cartTable->setItem(cartRow, 3, totalItem);

    QPushButton *removeBtn = new QPushButton("Remove");
    removeBtn->setStyleSheet(R"(
        QPushButton {
            background: #e74c3c;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 5px 10px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #c0392b;
        }
    )");
    connect(removeBtn, &QPushButton::clicked, [this, cartRow]() {
        m_cartTable->removeRow(cartRow);
        updateCartTotal();
    });
    m_cartTable->setCellWidget(cartRow, 4, removeBtn);

    updateCartTotal();
    statusBar()->showMessage(QString("Quick added %1 to cart").arg(productName));
}

void SalesmanPanel::onRemoveFromCart()
{
    QList<QTableWidgetItem*> selectedItems = m_cartTable->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();
    m_cartTable->removeRow(row);
    updateCartTotal();
}

void SalesmanPanel::onUpdateCartQuantity()
{
    updateCartTotal();
}

void SalesmanPanel::onUpdateCartPrice()
{
    updateCartTotal();
}

void SalesmanPanel::onClearCart()
{
    if (m_cartTable->rowCount() == 0) return;

    int ret = QMessageBox::question(this, "Clear Cart",
                                    "Are you sure you want to clear all items from the cart?",
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

    double total = 0.0;
    QList<QPair<int, QPair<int, double>>> items; // productId, <quantity, unitPrice>

    for (int row = 0; row < m_cartTable->rowCount(); row++) {
        int productId = m_cartTable->item(row, 0)->data(Qt::UserRole).toInt();
        QSpinBox *qtySpinBox = qobject_cast<QSpinBox*>(m_cartTable->cellWidget(row, 1));
        int quantity = qtySpinBox ? qtySpinBox->value() : 1;
        double unitPrice = m_cartTable->item(row, 2)->text().toDouble();
        double itemTotal = m_cartTable->item(row, 3)->text().toDouble();

        items.append(qMakePair(productId, qMakePair(quantity, unitPrice)));
        total += itemTotal;
    }

    // Save sale to database using the correct signature
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (DatabaseManager::instance().addSale(currentDate, total, items)) {
        QMessageBox::information(this, "Success",
                                 QString("Sale completed successfully!\nTotal: Rs. %1")
                                     .arg(total, 0, 'f', 2));

        // Create Sale object for printing
        m_currentSale = Sale(0, QDateTime::currentDateTime(), total, 1);
        for (int row = 0; row < m_cartTable->rowCount(); row++) {
            int productId = m_cartTable->item(row, 0)->data(Qt::UserRole).toInt();
            QString productName = m_cartTable->item(row, 0)->text();
            QSpinBox *qtySpinBox = qobject_cast<QSpinBox*>(m_cartTable->cellWidget(row, 1));
            int quantity = qtySpinBox ? qtySpinBox->value() : 1;
            double unitPrice = m_cartTable->item(row, 2)->text().toDouble();
            double itemTotal = m_cartTable->item(row, 3)->text().toDouble();

            SaleItem item;
            item.productId = productId;
            item.productName = productName;
            item.quantity = quantity;
            item.unitPrice = unitPrice;
            item.totalPrice = itemTotal;
            m_currentSale.addItem(item);
        }

        m_printInvoiceBtn->setEnabled(true);
        clearCart();
        loadProducts(); // Refresh product list to show updated stock
    } else {
        QMessageBox::critical(this, "Error", "Failed to save sale.");
    }
}

void SalesmanPanel::onPrintInvoice()
{
    if (m_currentSale.getId() == 0) {
        QMessageBox::warning(this, "Warning", "No completed sale to print.");
        return;
    }

    m_slipPrinter->previewSalesInvoice(m_currentSale, this);
}
