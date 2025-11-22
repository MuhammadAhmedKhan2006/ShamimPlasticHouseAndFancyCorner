#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool initialize();

    // Product operations
    QSqlQuery getAllProducts();
    QSqlQuery getAvailableProducts(); // Only products with stock > 0
    QSqlQuery getProductsByCategory(const QString& category);
    QSqlQuery getAvailableProductsByCategory(const QString& category); // Only products with stock > 0
    QSqlQuery searchProducts(const QString& searchTerm);
    QSqlQuery searchAvailableProducts(const QString& searchTerm); // Only products with stock > 0
    bool addProduct(const QString& name, const QString& category, const QString& subcategory, double price, int stock);
    bool updateProduct(int id, const QString& name, const QString& category, const QString& subcategory, double price, int stock);
    bool deleteProduct(int id);
    bool updateStock(int productId, int newQuantity);

    // Supplier operations
    QSqlQuery getAllSuppliers();
    bool addSupplier(const QString& name, const QString& contact);
    bool updateSupplier(int id, const QString& name, const QString& contact);
    bool deleteSupplier(int id);

    // Transaction operations
    QSqlQuery getAllTransactions();
    QSqlQuery getTransactionsBySupplier(int supplierId);
    bool addTransaction(int supplierId, const QString& date, const QString& description, double purchases, double payments);
    double getSupplierBalance(int supplierId);

    // Sales operations
    bool addSale(const QString& date, double total, const QList<QPair<int, QPair<int, double>>>& items);
    QSqlQuery getAllSales();
    QSqlQuery getSalesByDateRange(const QString& startDate, const QString& endDate);
    QSqlQuery getDailySales(const QString& date);
    QSqlQuery getSaleItems(int saleId);

    // User operations
    bool authenticateUser(const QString& username, const QString& password, QString& role);
    bool addUser(const QString& username, const QString& password, const QString& role);

    // Utility
    QSqlQuery getCategories();
    QSqlQuery getSubcategories(const QString& category);
    QSqlQuery getLowStockProducts(int threshold = 10);

private:
    DatabaseManager() = default;
    ~DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    bool seedInitialData();

    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H
