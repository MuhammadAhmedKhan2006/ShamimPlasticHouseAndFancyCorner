#include "Sale.h"

Sale::Sale()
    : m_id(0), m_total(0.0), m_salesmanId(1)
{
    m_date = QDateTime::currentDateTime();
}

Sale::Sale(int id, const QDateTime& date, double total, int salesmanId)
    : m_id(id), m_date(date), m_total(total), m_salesmanId(salesmanId)
{
}

void Sale::addItem(const SaleItem& item)
{
    m_items.append(item);
    recalculateTotal();
}

void Sale::removeItem(int index)
{
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        recalculateTotal();
    }
}

void Sale::clearItems()
{
    m_items.clear();
    m_total = 0.0;
}

void Sale::recalculateTotal()
{
    m_total = 0.0;
    for (const SaleItem& item : m_items) {
        m_total += item.totalPrice;
    }
}
