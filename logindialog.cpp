#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("Login - Shamim Plastic House Management System");
    resize(600, 700);
    setModal(true);
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #f5f7fa, stop: 1 #c3cfe2);
        }
    )");

    // Logo
    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(createLogoPixmap());
    logoLabel->setAlignment(Qt::AlignCenter);

    // Title
    QLabel *titleLabel = new QLabel("Shop Management System");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 26px; font-weight: bold; color: #3498db; padding: 10px;");

    // Subtitle
    QLabel *subtitleLabel = new QLabel("Shamim Plastic House & Fancy Corner");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 16px; color: #7f8c8d; padding: 5px;");

    // Manual Login Group
    QGroupBox *manualLoginGroup = new QGroupBox("Manual Login");
    manualLoginGroup->setStyleSheet(R"(
        QGroupBox {
            font-size: 18px;
            font-weight: bold;
            border: 3px solid #3498db;
            border-radius: 12px;
            margin-top: 20px;
            padding: 25px;
            background: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 20px;
            padding: 0 10px;
            color: #2c3e50;
        }
    )");

    QVBoxLayout *manualLoginLayout = new QVBoxLayout(manualLoginGroup);
    manualLoginLayout->setSpacing(20);

    // Username
    QLabel *usernameLabel = new QLabel("Username:");
    usernameLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");

    m_usernameEdit = new QLineEdit;
    m_usernameEdit->setPlaceholderText("Enter username");
    m_usernameEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            background: white;
            color: #2c3e50;
            min-height: 25px;
        }
        QLineEdit:focus {
            border: 3px solid #3498db;
        }
    )");

    // Password
    QLabel *passwordLabel = new QLabel("Password:");
    passwordLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");

    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            background: white;
            color: #2c3e50;
            min-height: 25px;
        }
        QLineEdit:focus {
            border: 3px solid #3498db;
        }
    )");

    manualLoginLayout->addWidget(usernameLabel);
    manualLoginLayout->addWidget(m_usernameEdit);
    manualLoginLayout->addWidget(passwordLabel);
    manualLoginLayout->addWidget(m_passwordEdit);

    // Login and Cancel Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(20);

    m_loginButton = new QPushButton("Login");
    m_loginButton->setDefault(true);
    m_loginButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #27ae60, stop: 1 #229954);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 15px;
            font-size: 18px;
            font-weight: bold;
            min-height: 20px;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2ecc71, stop: 1 #27ae60);
        }
    )");

    m_cancelButton = new QPushButton("Cancel");
    m_cancelButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #e74c3c, stop: 1 #c0392b);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 15px;
            font-size: 18px;
            font-weight: bold;
            min-height: 20px;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #ec7063, stop: 1 #e74c3c);
        }
    )");

    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_cancelButton);

    // Quick Login Group
    QGroupBox *quickLoginGroup = new QGroupBox("Quick Login");
    quickLoginGroup->setStyleSheet(R"(
        QGroupBox {
            font-size: 18px;
            font-weight: bold;
            border: 3px solid #3498db;
            border-radius: 12px;
            margin-top: 20px;
            padding: 25px;
            background: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 20px;
            padding: 0 10px;
            color: #2c3e50;
        }
    )");

    QVBoxLayout *quickLayout = new QVBoxLayout(quickLoginGroup);
    quickLayout->setSpacing(15);

    m_quickLoginCombo = new QComboBox;
    m_quickLoginCombo->addItem("Admin (admin/admin123)", "Admin");
    m_quickLoginCombo->addItem("Salesman (salesman/sales123)", "Salesman");
    m_quickLoginCombo->setStyleSheet(R"(
        QComboBox {
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            background: white;
            color: #2c3e50;
            min-height: 25px;
        }
        QComboBox:hover {
            border: 3px solid #3498db;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
    )");

    m_quickLoginButton = new QPushButton("Quick Login");
    m_quickLoginButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 15px;
            font-size: 18px;
            font-weight: bold;
            min-height: 20px;
        }
        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }
    )");

    quickLayout->addWidget(m_quickLoginCombo);
    quickLayout->addWidget(m_quickLoginButton);

    // Add all to main layout
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

    QLinearGradient gradient(0, 0, 120, 120);
    gradient.setColorAt(0, QColor(52, 152, 219));
    gradient.setColorAt(1, QColor(41, 128, 185));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(10, 10, 100, 100);

    painter.setPen(QPen(Qt::white, 3));
    painter.setBrush(Qt::NoBrush);

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
