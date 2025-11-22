#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

class Transaction
{
public:
    Transaction();
    Transaction(int id, int supplierId, const QDate& date, const QString& description,
                double purchases, double payments, double balance);

    // Getters
    int getId() const { return m_id; }
    int getSupplierId() const { return m_supplierId; }
    QDate getDate() const { return m_date; }
    QString getDescription() const { return m_description; }
    double getPurchases() const { return m_purchases; }
    double getPayments() const { return m_payments; }
    double getBalance() const { return m_balance; }

    // Setters
    void setId(int id) { m_id = id; }
    void setSupplierId(int supplierId) { m_supplierId = supplierId; }
    void setDate(const QDate& date) { m_date = date; }
    void setDescription(const QString& description) { m_description = description; }
    void setPurchases(double purchases) { m_purchases = purchases; }
    void setPayments(double payments) { m_payments = payments; }
    void setBalance(double balance) { m_balance = balance; }

private:
    int m_id;
    int m_supplierId;
    QDate m_date;
    QString m_description;
    double m_purchases;
    double m_payments;
    double m_balance;
};

#endif // TRANSACTION_H
