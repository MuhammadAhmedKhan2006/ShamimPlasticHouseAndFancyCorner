#include "Supplier.h"

Supplier::Supplier()
    : m_id(0)
{
}

Supplier::Supplier(int id, const QString& name, const QString& contact)
    : m_id(id), m_name(name), m_contact(contact)
{
}
