#include "Product.h"

Product::Product()
    : m_id(0), m_price(0.0), m_stockQuantity(0)
{
}

Product::Product(int id, const QString& name, const QString& category, const QString& subcategory,
                 double price, int stockQuantity, const QString& description)
    : m_id(id), m_name(name), m_category(category), m_subcategory(subcategory),
    m_price(price), m_stockQuantity(stockQuantity), m_description(description)
{
}

QString Product::getFullCategoryName() const
{
    if (m_subcategory.isEmpty()) {
        return m_category;
    }
    return m_category + " > " + m_subcategory;
}
