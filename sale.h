#ifndef SALE_H
#define SALE_H

#include <QString>
#include <QDateTime>
#include <QList>

struct SaleItem {
    int productId;
    QString productName;
    int quantity;
    double unitPrice;
    double totalPrice;

    SaleItem() : productId(0), quantity(0), unitPrice(0.0), totalPrice(0.0) {}
    SaleItem(int id, const QString& name, int qty, double price)
        : productId(id), productName(name), quantity(qty), unitPrice(price)
    {
        totalPrice = qty * price;
    }
};

class Sale
{
public:
    Sale();
    Sale(int id, const QDateTime& date, double total, int salesmanId = 1);

    // Getters
    int getId() const { return m_id; }
    QDateTime getDate() const { return m_date; }
    double getTotal() const { return m_total; }
    int getSalesmanId() const { return m_salesmanId; }
    QString getCustomerNote() const { return m_customerNote; }
    QList<SaleItem> getItems() const { return m_items; }

    // Setters
    void setId(int id) { m_id = id; }
    void setDate(const QDateTime& date) { m_date = date; }
    void setTotal(double total) { m_total = total; }
    void setSalesmanId(int salesmanId) { m_salesmanId = salesmanId; }
    void setCustomerNote(const QString& note) { m_customerNote = note; }
    void setItems(const QList<SaleItem>& items) { m_items = items; }

    // Item management
    void addItem(const SaleItem& item);
    void removeItem(int index);
    void clearItems();

    // Calculate total from items
    void recalculateTotal();

private:
    int m_id;
    QDateTime m_date;
    double m_total;
    int m_salesmanId;
    QString m_customerNote;
    QList<SaleItem> m_items;
};

#endif // SALE_H
