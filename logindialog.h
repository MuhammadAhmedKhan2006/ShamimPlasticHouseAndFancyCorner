#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include "DatabaseManager.h"

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getUserRole() const { return m_userRole; }

private slots:
    void onLoginClicked();
    void onQuickLoginClicked();

private:
    void setupUI();
    QPixmap createLogoPixmap();

    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QComboBox *m_quickLoginCombo;
    QPushButton *m_loginButton;
    QPushButton *m_quickLoginButton;
    QPushButton *m_cancelButton;

    QString m_userRole;
};

#endif // LOGINDIALOG_H
