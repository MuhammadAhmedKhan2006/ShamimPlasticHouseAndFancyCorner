#ifndef SALESMANPANEL_H
#define SALESMANPANEL_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollArea>
#include <QFrame>
#include <QStatusBar>
#include <QSqlQuery>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include "DatabaseManager.h"
#include "slipprinter.h"
#include "sale.h"

class SalesmanPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit SalesmanPanel(QWidget *parent = nullptr);

private slots:
    void onCategoryChanged();
    void onSearchProducts();
    void onProductDoubleClicked();
    void onAddToCart();
    void onRemoveFromCart();
    void onUpdateCartQuantity();
    void onUpdateCartPrice();
    void onClearCart();
    void onFinalizeSale();
    void onPrintInvoice();
    void onQuickAddProduct();

private:
    void setupUI();
    void setupProductBrowser();
    void setupCartSection();
    void setupActionButtons();

    void loadCategories();
    void loadProducts();
    void loadProductsByCategory();
    void updateCartTotal();
    void clearCart();

    // UI Components
    QSplitter *m_mainSplitter;

    // Product browser
    QWidget *m_productBrowserWidget;
    QComboBox *m_categoryCombo;
    QLineEdit *m_productSearchEdit;
    QTableWidget *m_productTable;
    QPushButton *m_addToCartBtn;
    QPushButton *m_quickAddBtn;

    // Cart section
    QWidget *m_cartWidget;
    QTableWidget *m_cartTable;
    QLabel *m_totalLabel;
    QTextEdit *m_customerNoteEdit;
    QPushButton *m_removeFromCartBtn;
    QPushButton *m_clearCartBtn;
    QPushButton *m_finalizeSaleBtn;
    QPushButton *m_printInvoiceBtn;

    // Current sale
    Sale m_currentSale;
    SlipPrinter *m_slipPrinter;

    // Selected items
    int m_selectedProductId;
};

#endif // SALESMANPANEL_H
