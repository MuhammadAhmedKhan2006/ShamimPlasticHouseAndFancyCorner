#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QMessageBox>
#include "DatabaseManager.h"
#include "adminpanel.h"
#include "salesmanpanel.h"
#include "logindialog.h"

QString createModernStylesheet() {
    return R"(
        QMainWindow {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1,
                                      stop: 0 #2c3e50, stop: 1 #34495e);
            color: white;
        }

        QWidget {
            background-color: #34495e;
            color: #ecf0f1;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 11px;
        }

        QPushButton {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            border: none;
            padding: 12px 24px;
            border-radius: 8px;
            color: white;
            font-weight: bold;
            font-size: 12px;
        }

        QPushButton:hover {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3cb4f0, stop: 1 #3498db);
        }

        QPushButton:pressed {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #2980b9, stop: 1 #21618c);
        }

        QLineEdit, QTextEdit, QSpinBox, QDoubleSpinBox {
            background-color: #2c3e50;
            border: 2px solid #34495e;
            border-radius: 6px;
            padding: 8px;
            color: #ecf0f1;
            selection-background-color: #3498db;
        }

        QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #3498db;
        }

        QTableWidget {
            background-color: #2c3e50;
            alternate-background-color: #34495e;
            border: 1px solid #34495e;
            border-radius: 8px;
            gridline-color: #34495e;
        }

        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #34495e;
        }

        QTableWidget::item:selected {
            background-color: #3498db;
        }

        QHeaderView::section {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
            padding: 12px;
            border: none;
            font-weight: bold;
        }

        QComboBox {
            background-color: #2c3e50;
            border: 2px solid #34495e;
            border-radius: 6px;
            padding: 8px;
            color: #ecf0f1;
        }

        QComboBox:hover {
            border-color: #3498db;
        }

        QComboBox::drop-down {
            border: none;
            background-color: #3498db;
            border-radius: 4px;
            margin: 2px;
        }

        QComboBox::down-arrow {
            image: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAwAAAAGCAYAAAD37n+BAAAAGklEQVQYV2P8//8/AzYwOjGq);
            width: 12px;
            height: 6px;
        }

        QLabel {
            color: #ecf0f1;
            font-size: 12px;
        }

        QGroupBox {
            background-color: #2c3e50;
            border: 2px solid #34495e;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 8px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px 0 8px;
            color: #3498db;
            font-weight: bold;
        }

        QTabWidget::pane {
            border: 2px solid #34495e;
            border-radius: 8px;
            background-color: #2c3e50;
        }

        QTabBar::tab {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #34495e, stop: 1 #2c3e50);
            border: 1px solid #34495e;
            padding: 12px 24px;
            margin-right: 2px;
        }

        QTabBar::tab:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                      stop: 0 #3498db, stop: 1 #2980b9);
            color: white;
        }

        QScrollBar:vertical {
            background: #2c3e50;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background: #3498db;
            border-radius: 6px;
            min-height: 20px;
        }

        QScrollBar::handle:vertical:hover {
            background: #5dade2;
        }
    )";
}

QPixmap createSplashPixmap() {
    QPixmap splash(600, 400);
    splash.fill(QColor(44, 62, 80));

    QPainter painter(&splash);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background gradient
    QLinearGradient gradient(0, 0, 600, 400);
    gradient.setColorAt(0, QColor(52, 152, 219));
    gradient.setColorAt(1, QColor(41, 128, 185));
    painter.fillRect(splash.rect(), gradient);

    // Company name
    painter.setPen(Qt::white);
    QFont titleFont("Arial", 24, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(QRect(0, 150, 600, 50), Qt::AlignCenter, "Shamim Plastic House");

    QFont subtitleFont("Arial", 18);
    painter.setFont(subtitleFont);
    painter.drawText(QRect(0, 200, 600, 30), Qt::AlignCenter, "& Fancy Corner");

    // Contact info
    QFont contactFont("Arial", 12);
    painter.setFont(contactFont);
    painter.drawText(QRect(0, 280, 600, 20), Qt::AlignCenter, "Online Orders: 03333152019, 03333152018");

    // Loading text
    painter.drawText(QRect(0, 350, 600, 20), Qt::AlignCenter, "Loading...");

    return splash;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Shamim Plastic House Management System");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Shamim Plastic House");

    // Apply modern stylesheet
    app.setStyleSheet(createModernStylesheet());

    // Show splash screen
    QSplashScreen splash(createSplashPixmap());
    splash.show();
    app.processEvents();

    // Initialize database
    QTimer::singleShot(2000, [&]() {
        splash.showMessage("Initializing database...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    });

    app.processEvents();

    if (!DatabaseManager::instance().initialize()) {
        splash.finish(nullptr);
        return -1;
    }

    QTimer::singleShot(3000, [&]() {
        splash.showMessage("Loading interface...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    });

    app.processEvents();

    // Show login dialog after splash
    QTimer::singleShot(4000, [&]() {
        splash.finish(nullptr);

        LoginDialog loginDialog;
        if (loginDialog.exec() == QDialog::Accepted) {
            QString role = loginDialog.getUserRole();

            if (role == "Admin") {
                AdminPanel *adminPanel = new AdminPanel();
                adminPanel->show();
            } else {
                SalesmanPanel *salesmanPanel = new SalesmanPanel();
                salesmanPanel->show();
            }
        } else {
            app.quit();
        }
    });

    return app.exec();
}
