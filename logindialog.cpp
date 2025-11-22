#include "LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("Login - Shamim Plastic House Management System");
    setFixedSize(400, 500);
    setModal(true);
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Logo and title
    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(createLogoPixmap());
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("background: transparent;");

    QLabel *titleLabel = new QLabel("Shop Management System");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 18px;
            font-weight: bold;
            color: #3498db;
            background: transparent;
        }
    )");

    QLabel *subtitleLabel = new QLabel("Shamim Plastic House & Fancy Corner");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 14px;
            color: #7f8c8d;
            background: transparent;
        }
    )");

    // Manual login group
    QGroupBox *manualLoginGroup = new QGroupBox("Manual Login");
    QFormLayout *formLayout = new QFormLayout(manualLoginGroup);

    m_usernameEdit = new QLineEdit;
    m_usernameEdit->setPlaceholderText("Enter username");
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Username:", m_usernameEdit);
    formLayout->addRow("Password:", m_passwordEdit);

    m_loginButton = new QPushButton("Login");
    m_loginButton->setDefault(true);

    // Quick login group
    QGroupBox *quickLoginGroup = new QGroupBox("Quick Login");
    QVBoxLayout *quickLayout = new QVBoxLayout(quickLoginGroup);

    m_quickLoginCombo = new QComboBox;
    m_quickLoginCombo->addItem("Admin (admin/admin123)", "Admin");
    m_quickLoginCombo->addItem("Salesman (salesman/sales123)", "Salesman");

    m_quickLoginButton = new QPushButton("Quick Login");

    quickLayout->addWidget(m_quickLoginCombo);
    quickLayout->addWidget(m_quickLoginButton);

    // Cancel button
    m_cancelButton = new QPushButton("Cancel");
    m_cancelButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ec7063, stop: 1 #e74c3c);
        }
    )");

    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_cancelButton);

    // Add to main layout
    mainLayout->addWidget(logoLabel);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(manualLoginGroup);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(quickLoginGroup);
    mainLayout->addStretch();

    // Connect signals
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_quickLoginButton, &QPushButton::clicked, this, &LoginDialog::onQuickLoginClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

QPixmap LoginDialog::createLogoPixmap()
{
    QPixmap logo(120, 120);
    logo.fill(Qt::transparent);

    QPainter painter(&logo);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw circular background
    QLinearGradient gradient(0, 0, 120, 120);
    gradient.setColorAt(0, QColor(52, 152, 219));
    gradient.setColorAt(1, QColor(41, 128, 185));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(10, 10, 100, 100);

    // Draw shop icon
    painter.setPen(QPen(Qt::white, 3));
    painter.setBrush(Qt::NoBrush);

    // Simple shop/house icon
    QPolygon house;
    house << QPoint(60, 35) << QPoint(45, 50) << QPoint(75, 50);
    painter.drawPolygon(house);

    painter.drawRect(45, 50, 30, 25);
    painter.drawRect(52, 60, 8, 15);
    painter.drawRect(65, 60, 6, 8);

    return logo;
}

void LoginDialog::onLoginClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter both username and password.");
        return;
    }

    QString role;
    if (DatabaseManager::instance().authenticateUser(username, password, role)) {
        m_userRole = role;
        accept();
    } else {
        QMessageBox::critical(this, "Login Failed", "Invalid username or password.");
        m_passwordEdit->clear();
        m_usernameEdit->setFocus();
    }
}

void LoginDialog::onQuickLoginClicked()
{
    m_userRole = m_quickLoginCombo->currentData().toString();
    accept();
}
