#include "Transaction.h"

Transaction::Transaction()
    : m_id(0), m_supplierId(0), m_purchases(0.0), m_payments(0.0), m_balance(0.0)
{
}

Transaction::Transaction(int id, int supplierId, const QDate& date, const QString& description,
                         double purchases, double payments, double balance)
    : m_id(id), m_supplierId(supplierId), m_date(date), m_description(description),
    m_purchases(purchases), m_payments(payments), m_balance(balance)
{
}
