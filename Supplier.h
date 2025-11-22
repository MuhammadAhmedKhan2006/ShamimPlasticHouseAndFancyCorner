#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <QString>

class Supplier {
public:
    Supplier();
    Supplier(int id, const QString& name, const QString& contact);

    // Getters
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getContact() const { return m_contact; }

    // Setters
    void setId(int id) { m_id = id; }
    void setName(const QString& name) { m_name = name; }
    void setContact(const QString& contact) { m_contact = contact; }

private:
    int m_id;
    QString m_name;
    QString m_contact;
};

#endif // SUPPLIER_H
