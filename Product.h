#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class Product
{
public:
    Product();
    Product(int id, const QString& name, const QString& category, const QString& subcategory,
            double price, int stockQuantity, const QString& description = "");

    // Getters
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getCategory() const { return m_category; }
    QString getSubcategory() const { return m_subcategory; }
    double getPrice() const { return m_price; }
    int getStockQuantity() const { return m_stockQuantity; }
    QString getDescription() const { return m_description; }

    // Setters
    void setId(int id) { m_id = id; }
    void setName(const QString& name) { m_name = name; }
    void setCategory(const QString& category) { m_category = category; }
    void setSubcategory(const QString& subcategory) { m_subcategory = subcategory; }
    void setPrice(double price) { m_price = price; }
    void setStockQuantity(int quantity) { m_stockQuantity = quantity; }
    void setDescription(const QString& description) { m_description = description; }

    // Utility methods
    bool isLowStock(int threshold = 10) const { return m_stockQuantity <= threshold; }
    QString getFullCategoryName() const;

private:
    int m_id;
    QString m_name;
    QString m_category;
    QString m_subcategory;
    double m_price;
    int m_stockQuantity;
    QString m_description;
};

#endif // PRODUCT_H
