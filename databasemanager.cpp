#include "DatabaseManager.h"
#include <QMessageBox>
#include <QApplication>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");

    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    m_database.setDatabaseName(dbPath + "/shop_management.db");

    if (!m_database.open()) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not open database: " + m_database.lastError().text());
        return false;
    }

    if (!createTables()) {
        return false;
    }

    if (!seedInitialData()) {
        return false;
    }

    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;

    // Create Products table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Products ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL,"
                    "category TEXT NOT NULL,"
                    "subcategory TEXT,"
                    "price REAL NOT NULL,"
                    "stock_quantity INTEGER NOT NULL DEFAULT 0,"
                    "description TEXT,"
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                    ")")) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create Products table: " + query.lastError().text());
        return false;
    }

    // Create Suppliers table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Suppliers ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL UNIQUE,"
                    "contact TEXT,"
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                    ")")) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create Suppliers table: " + query.lastError().text());
        return false;
    }

    // Create Transactions table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Transactions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "supplier_id INTEGER NOT NULL,"
                    "date DATE NOT NULL,"
                    "description TEXT,"
                    "purchases REAL DEFAULT 0,"
                    "payments REAL DEFAULT 0,"
                    "balance REAL DEFAULT 0,"
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "FOREIGN KEY (supplier_id) REFERENCES Suppliers(id)"
                    ")")) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create Transactions table: " + query.lastError().text());
        return false;
    }

    // Create Sales table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Sales ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "date DATETIME NOT NULL,"
                    "total REAL NOT NULL,"
                    "salesman_id INTEGER DEFAULT 1,"
                    "customer_note TEXT,"
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                    ")")) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create Sales table: " + query.lastError().text());
        return false;
    }

    // Create SalesItems table
    if (!query.exec("CREATE TABLE IF NOT EXISTS SalesItems ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "sale_id INTEGER NOT NULL,"
                    "product_id INTEGER NOT NULL,"
                    "quantity INTEGER NOT NULL,"
                    "unit_price REAL NOT NULL,"
                    "total_price REAL NOT NULL,"
                    "FOREIGN KEY (sale_id) REFERENCES Sales(id),"
                    "FOREIGN KEY (product_id) REFERENCES Products(id)"
                    ")")) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create SalesItems table: " + query.lastError().text());
        return false;
    }

    // Create Users table
    if (!query.exec("CREATE TABLE IF NOT EXISTS Users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "username TEXT NOT NULL UNIQUE,"
                    "password TEXT NOT NULL,"
                    "role TEXT NOT NULL,"
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                    ")")) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create Users table: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::seedInitialData()
{
    QSqlQuery query;

    // Check if data already exists
    query.exec("SELECT COUNT(*) FROM Users");
    if (query.next() && query.value(0).toInt() > 0) {
        return true; // Data already seeded
    }

    // Add default users
    query.prepare("INSERT OR IGNORE INTO Users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue("admin");
    query.addBindValue("admin123");
    query.addBindValue("Admin");
    if (!query.exec()) {
        qDebug() << "Error adding admin user:" << query.lastError().text();
    }

    query.prepare("INSERT OR IGNORE INTO Users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue("salesman");
    query.addBindValue("sales123");
    query.addBindValue("Salesman");
    if (!query.exec()) {
        qDebug() << "Error adding salesman user:" << query.lastError().text();
    }

    // Add default suppliers
    QStringList suppliers = {"Akhtar", "Sajid", "Anwar", "Mushtaq", "Fine Talha", "Chaudhary Plastic House"};
    for (const QString& supplier : suppliers) {
        query.prepare("INSERT OR IGNORE INTO Suppliers (name, contact) VALUES (?, ?)");
        query.addBindValue(supplier);
        query.addBindValue("Contact not provided");
        if (!query.exec()) {
            qDebug() << "Error adding supplier:" << query.lastError().text();
        }
    }

    // Add sample products
    struct ProductData {
        QString name;
        QString category;
        QString subcategory;
        double price;
        int stock;
    };

    QList<ProductData> products = {
        // Bedding and Comforters
        {"Fleece Blanket", "Bedding and Comforters", "Blankets", 1500.0, 25},
        {"Silk Bridal Comforter Set", "Bedding and Comforters", "Bridal Comforter Sets", 8500.0, 10},
        {"Belgium Bridal Comforter Set", "Bedding and Comforters", "Bridal Comforter Sets", 7200.0, 15},
        {"Cotton Bridal Comforter Set", "Bedding and Comforters", "Bridal Comforter Sets", 4500.0, 20},
        {"Silk Bridal Bed Cover", "Bedding and Comforters", "Bridal Bed Covers", 3200.0, 18},
        {"Cotton Bridal Bed Cover", "Bedding and Comforters", "Bridal Bed Covers", 2100.0, 25},
        {"Opal Bridal Bed Cover", "Bedding and Comforters", "Bridal Bed Covers", 2800.0, 12},

        // Bed Sheets
        {"Cotton Double Bed Sheet", "Bed Sheets", "Cotton Bed Sheets", 1800.0, 30},
        {"Cotton Single Bed Sheet", "Bed Sheets", "Cotton Bed Sheets", 1200.0, 40},
        {"3D Double Bed Sheet", "Bed Sheets", "3D Bed Sheets", 2200.0, 22},
        {"3D Single Bed Sheet", "Bed Sheets", "3D Bed Sheets", 1600.0, 28},
        {"Gull Tax Double Bed Sheet", "Bed Sheets", "Gull Tax Bed Sheets", 1900.0, 20},
        {"Gull Tax Single Bed Sheet", "Bed Sheets", "Gull Tax Bed Sheets", 1300.0, 35},

        // Jazim
        {"Jazim 7x11", "Jazim", "", 3200.0, 15},
        {"Jazim 9x12", "Jazim", "", 4500.0, 12},

        // Pillows
        {"Standard Pillow", "Pillows", "", 650.0, 50},
        {"Cushion Gaddi", "Pillows", "", 850.0, 30},
        {"Gow Filling Pillow", "Pillows", "", 750.0, 40},

        // Foot Mats and Carpets
        {"Small Foot Mat", "Foot Mats and Carpets", "Foot Mats", 350.0, 60},
        {"Large Foot Mat", "Foot Mats and Carpets", "Foot Mats", 550.0, 45},
        {"Anti-Slip Small Foot Mat", "Foot Mats and Carpets", "Foot Mats", 450.0, 35},
        {"Anti-Slip Large Foot Mat", "Foot Mats and Carpets", "Foot Mats", 650.0, 25}
    };

    for (const ProductData& product : products) {
        query.prepare("INSERT OR IGNORE INTO Products (name, category, subcategory, price, stock_quantity) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(product.name);
        query.addBindValue(product.category);
        query.addBindValue(product.subcategory);
        query.addBindValue(product.price);
        query.addBindValue(product.stock);
        if (!query.exec()) {
            qDebug() << "Error adding product:" << query.lastError().text();
        }
    }

    return true;
}

QSqlQuery DatabaseManager::getAllProducts()
{
    QSqlQuery query;
    query.exec("SELECT id, name, category, subcategory, price, stock_quantity FROM Products ORDER BY category, name");
    return query;
}

QSqlQuery DatabaseManager::getAvailableProducts()
{
    QSqlQuery query;
    query.exec("SELECT id, name, category, subcategory, price, stock_quantity FROM Products WHERE stock_quantity > 0 ORDER BY category, name");
    return query;
}

QSqlQuery DatabaseManager::getProductsByCategory(const QString& category)
{
    QSqlQuery query;
    query.prepare("SELECT id, name, category, subcategory, price, stock_quantity FROM Products WHERE category = ? ORDER BY name");
    query.addBindValue(category);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getAvailableProductsByCategory(const QString& category)
{
    QSqlQuery query;
    query.prepare("SELECT id, name, category, subcategory, price, stock_quantity FROM Products WHERE category = ? AND stock_quantity > 0 ORDER BY name");
    query.addBindValue(category);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::searchProducts(const QString& searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT id, name, category, subcategory, price, stock_quantity FROM Products WHERE name LIKE ? OR category LIKE ? ORDER BY name");
    QString searchPattern = "%" + searchTerm + "%";
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::searchAvailableProducts(const QString& searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT id, name, category, subcategory, price, stock_quantity FROM Products WHERE (name LIKE ? OR category LIKE ?) AND stock_quantity > 0 ORDER BY name");
    QString searchPattern = "%" + searchTerm + "%";
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);
    query.exec();
    return query;
}

bool DatabaseManager::addProduct(const QString& name, const QString& category, const QString& subcategory, double price, int stock)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Products (name, category, subcategory, price, stock_quantity) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(category);
    query.addBindValue(subcategory);
    query.addBindValue(price);
    query.addBindValue(stock);
    return query.exec();
}

bool DatabaseManager::updateProduct(int id, const QString& name, const QString& category, const QString& subcategory, double price, int stock)
{
    QSqlQuery query;
    query.prepare("UPDATE Products SET name = ?, category = ?, subcategory = ?, price = ?, stock_quantity = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(category);
    query.addBindValue(subcategory);
    query.addBindValue(price);
    query.addBindValue(stock);
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::deleteProduct(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Products WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::updateStock(int productId, int newQuantity)
{
    QSqlQuery query;
    query.prepare("UPDATE Products SET stock_quantity = ? WHERE id = ?");
    query.addBindValue(newQuantity);
    query.addBindValue(productId);
    return query.exec();
}

QSqlQuery DatabaseManager::getAllSuppliers()
{
    QSqlQuery query;
    query.exec("SELECT id, name, contact FROM Suppliers ORDER BY name");
    return query;
}

bool DatabaseManager::addSupplier(const QString& name, const QString& contact)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Suppliers (name, contact) VALUES (?, ?)");
    query.addBindValue(name);
    query.addBindValue(contact);
    return query.exec();
}

bool DatabaseManager::updateSupplier(int id, const QString& name, const QString& contact)
{
    QSqlQuery query;
    query.prepare("UPDATE Suppliers SET name = ?, contact = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(contact);
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::deleteSupplier(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Suppliers WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QSqlQuery DatabaseManager::getAllTransactions()
{
    QSqlQuery query;
    query.exec("SELECT t.id, s.name as supplier_name, t.date, t.description, t.purchases, t.payments, t.balance "
               "FROM Transactions t "
               "JOIN Suppliers s ON t.supplier_id = s.id "
               "ORDER BY t.date DESC");
    return query;
}

QSqlQuery DatabaseManager::getTransactionsBySupplier(int supplierId)
{
    QSqlQuery query;
    query.prepare("SELECT id, date, description, purchases, payments, balance FROM Transactions WHERE supplier_id = ? ORDER BY date DESC");
    query.addBindValue(supplierId);
    query.exec();
    return query;
}

bool DatabaseManager::addTransaction(int supplierId, const QString& date, const QString& description, double purchases, double payments)
{
    // Calculate current balance
    double currentBalance = getSupplierBalance(supplierId);
    double newBalance = currentBalance + purchases - payments;

    QSqlQuery query;
    query.prepare("INSERT INTO Transactions (supplier_id, date, description, purchases, payments, balance) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(supplierId);
    query.addBindValue(date);
    query.addBindValue(description);
    query.addBindValue(purchases);
    query.addBindValue(payments);
    query.addBindValue(newBalance);
    return query.exec();
}

double DatabaseManager::getSupplierBalance(int supplierId)
{
    QSqlQuery query;
    query.prepare("SELECT balance FROM Transactions WHERE supplier_id = ? ORDER BY id DESC LIMIT 1");
    query.addBindValue(supplierId);
    query.exec();

    if (query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

bool DatabaseManager::addSale(const QString& date, double total, const QList<QPair<int, QPair<int, double>>>& items)
{
    m_database.transaction();

    // Insert sale record
    QSqlQuery saleQuery;
    saleQuery.prepare("INSERT INTO Sales (date, total) VALUES (?, ?)");
    saleQuery.addBindValue(date);
    saleQuery.addBindValue(total);

    if (!saleQuery.exec()) {
        m_database.rollback();
        return false;
    }

    int saleId = saleQuery.lastInsertId().toInt();

    // Insert sale items and update stock
    for (const auto& item : items) {
        int productId = item.first;
        int quantity = item.second.first;
        double unitPrice = item.second.second;
        double totalPrice = quantity * unitPrice;

        // Insert sale item
        QSqlQuery itemQuery;
        itemQuery.prepare("INSERT INTO SalesItems (sale_id, product_id, quantity, unit_price, total_price) VALUES (?, ?, ?, ?, ?)");
        itemQuery.addBindValue(saleId);
        itemQuery.addBindValue(productId);
        itemQuery.addBindValue(quantity);
        itemQuery.addBindValue(unitPrice);
        itemQuery.addBindValue(totalPrice);

        if (!itemQuery.exec()) {
            m_database.rollback();
            return false;
        }

        // Update stock
        QSqlQuery stockQuery;
        stockQuery.prepare("UPDATE Products SET stock_quantity = stock_quantity - ? WHERE id = ?");
        stockQuery.addBindValue(quantity);
        stockQuery.addBindValue(productId);

        if (!stockQuery.exec()) {
            m_database.rollback();
            return false;
        }
    }

    m_database.commit();
    return true;
}

QSqlQuery DatabaseManager::getAllSales()
{
    QSqlQuery query;
    query.exec("SELECT id, date, total, salesman_id FROM Sales ORDER BY date DESC");
    return query;
}

QSqlQuery DatabaseManager::getSalesByDateRange(const QString& startDate, const QString& endDate)
{
    QSqlQuery query;
    query.prepare("SELECT id, date, total, salesman_id FROM Sales WHERE date BETWEEN ? AND ? ORDER BY date DESC");
    query.addBindValue(startDate);
    query.addBindValue(endDate);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getDailySales(const QString& date)
{
    QSqlQuery query;
    query.prepare("SELECT s.id, s.date, s.total, s.salesman_id, "
                  "COUNT(si.id) as items_count "
                  "FROM Sales s "
                  "LEFT JOIN SalesItems si ON s.id = si.sale_id "
                  "WHERE DATE(s.date) = DATE(?) "
                  "GROUP BY s.id "
                  "ORDER BY s.date DESC");
    query.addBindValue(date);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getSaleItems(int saleId)
{
    QSqlQuery query;
    query.prepare("SELECT si.product_id, p.name, si.quantity, si.unit_price, si.total_price "
                  "FROM SalesItems si "
                  "JOIN Products p ON si.product_id = p.id "
                  "WHERE si.sale_id = ?");
    query.addBindValue(saleId);
    query.exec();
    return query;
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password, QString& role)
{
    QSqlQuery query;
    query.prepare("SELECT role FROM Users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);
    query.exec();

    if (query.next()) {
        role = query.value(0).toString();
        return true;
    }
    return false;
}

bool DatabaseManager::addUser(const QString& username, const QString& password, const QString& role)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);
    return query.exec();
}

QSqlQuery DatabaseManager::getCategories()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT category FROM Products ORDER BY category");
    return query;
}

QSqlQuery DatabaseManager::getSubcategories(const QString& category)
{
    QSqlQuery query;
    query.prepare("SELECT DISTINCT subcategory FROM Products WHERE category = ? AND subcategory IS NOT NULL AND subcategory != '' ORDER BY subcategory");
    query.addBindValue(category);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getLowStockProducts(int threshold)
{
    QSqlQuery query;
    query.prepare("SELECT id, name, category, stock_quantity FROM Products WHERE stock_quantity <= ? ORDER BY stock_quantity ASC");
    query.addBindValue(threshold);
    query.exec();
    return query;
}
