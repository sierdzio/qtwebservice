/********************************************************************************
** Form generated from reading UI file 'qdnmain.ui'
**
** Created: Wed Nov 2 11:50:12 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QDNMAIN_H
#define UI_QDNMAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QdnMain
{
public:
    QAction *actionQuit;
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QVBoxLayout *layoutMain;
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout;
    QLabel *labelLogin;
    QLineEdit *lineEditLogin;
    QLabel *labelPassword;
    QLineEdit *lineEditPassword;
    QPushButton *buttonLogin;
    QTabWidget *tabWidget;
    QWidget *tabProfile;
    QWidget *layoutWidget1;
    QVBoxLayout *layoutProfile;
    QFormLayout *formLayout_2;
    QLabel *labelUserId;
    QLabel *labelUserIdResult;
    QLabel *labelPoints;
    QLabel *labelPointsResult;
    QLabel *labelLevel;
    QLabel *labelLevelResult;
    QLabel *labelTitle;
    QLabel *labelTitleResult;
    QLabel *labelGravatar;
    QLabel *labelGravatarResult;
    QTextBrowser *labelUsernameResult;
    QWidget *tabPosts;
    QWidget *layoutWidget2;
    QVBoxLayout *layoutPosts;
    QLabel *labelPosts;
    QTextBrowser *labelPostsResult;
    QMenuBar *menuBar;
    QMenu *menuOptions;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QdnMain)
    {
        if (QdnMain->objectName().isEmpty())
            QdnMain->setObjectName(QString::fromUtf8("QdnMain"));
        QdnMain->resize(644, 605);
        actionQuit = new QAction(QdnMain);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        centralWidget = new QWidget(QdnMain);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 20, 571, 481));
        layoutMain = new QVBoxLayout(layoutWidget);
        layoutMain->setSpacing(6);
        layoutMain->setContentsMargins(11, 11, 11, 11);
        layoutMain->setObjectName(QString::fromUtf8("layoutMain"));
        layoutMain->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        labelLogin = new QLabel(layoutWidget);
        labelLogin->setObjectName(QString::fromUtf8("labelLogin"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelLogin);

        lineEditLogin = new QLineEdit(layoutWidget);
        lineEditLogin->setObjectName(QString::fromUtf8("lineEditLogin"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEditLogin);

        labelPassword = new QLabel(layoutWidget);
        labelPassword->setObjectName(QString::fromUtf8("labelPassword"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelPassword);

        lineEditPassword = new QLineEdit(layoutWidget);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setContextMenuPolicy(Qt::NoContextMenu);
        lineEditPassword->setInputMethodHints(Qt::ImhNone);

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEditPassword);


        horizontalLayout->addLayout(formLayout);

        buttonLogin = new QPushButton(layoutWidget);
        buttonLogin->setObjectName(QString::fromUtf8("buttonLogin"));

        horizontalLayout->addWidget(buttonLogin);


        layoutMain->addLayout(horizontalLayout);

        tabWidget = new QTabWidget(layoutWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabProfile = new QWidget();
        tabProfile->setObjectName(QString::fromUtf8("tabProfile"));
        layoutWidget1 = new QWidget(tabProfile);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 20, 258, 217));
        layoutProfile = new QVBoxLayout(layoutWidget1);
        layoutProfile->setSpacing(6);
        layoutProfile->setContentsMargins(11, 11, 11, 11);
        layoutProfile->setObjectName(QString::fromUtf8("layoutProfile"));
        layoutProfile->setContentsMargins(0, 0, 0, 0);
        formLayout_2 = new QFormLayout();
        formLayout_2->setSpacing(6);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        labelUserId = new QLabel(layoutWidget1);
        labelUserId->setObjectName(QString::fromUtf8("labelUserId"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, labelUserId);

        labelUserIdResult = new QLabel(layoutWidget1);
        labelUserIdResult->setObjectName(QString::fromUtf8("labelUserIdResult"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, labelUserIdResult);

        labelPoints = new QLabel(layoutWidget1);
        labelPoints->setObjectName(QString::fromUtf8("labelPoints"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, labelPoints);

        labelPointsResult = new QLabel(layoutWidget1);
        labelPointsResult->setObjectName(QString::fromUtf8("labelPointsResult"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, labelPointsResult);

        labelLevel = new QLabel(layoutWidget1);
        labelLevel->setObjectName(QString::fromUtf8("labelLevel"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, labelLevel);

        labelLevelResult = new QLabel(layoutWidget1);
        labelLevelResult->setObjectName(QString::fromUtf8("labelLevelResult"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, labelLevelResult);

        labelTitle = new QLabel(layoutWidget1);
        labelTitle->setObjectName(QString::fromUtf8("labelTitle"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, labelTitle);

        labelTitleResult = new QLabel(layoutWidget1);
        labelTitleResult->setObjectName(QString::fromUtf8("labelTitleResult"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, labelTitleResult);

        labelGravatar = new QLabel(layoutWidget1);
        labelGravatar->setObjectName(QString::fromUtf8("labelGravatar"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, labelGravatar);

        labelGravatarResult = new QLabel(layoutWidget1);
        labelGravatarResult->setObjectName(QString::fromUtf8("labelGravatarResult"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, labelGravatarResult);


        layoutProfile->addLayout(formLayout_2);

        labelUsernameResult = new QTextBrowser(layoutWidget1);
        labelUsernameResult->setObjectName(QString::fromUtf8("labelUsernameResult"));

        layoutProfile->addWidget(labelUsernameResult);

        tabWidget->addTab(tabProfile, QString());
        tabPosts = new QWidget();
        tabPosts->setObjectName(QString::fromUtf8("tabPosts"));
        layoutWidget2 = new QWidget(tabPosts);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(10, 10, 258, 213));
        layoutPosts = new QVBoxLayout(layoutWidget2);
        layoutPosts->setSpacing(6);
        layoutPosts->setContentsMargins(11, 11, 11, 11);
        layoutPosts->setObjectName(QString::fromUtf8("layoutPosts"));
        layoutPosts->setContentsMargins(0, 0, 0, 0);
        labelPosts = new QLabel(layoutWidget2);
        labelPosts->setObjectName(QString::fromUtf8("labelPosts"));

        layoutPosts->addWidget(labelPosts);

        labelPostsResult = new QTextBrowser(layoutWidget2);
        labelPostsResult->setObjectName(QString::fromUtf8("labelPostsResult"));

        layoutPosts->addWidget(labelPostsResult);

        tabWidget->addTab(tabPosts, QString());

        layoutMain->addWidget(tabWidget);

        QdnMain->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QdnMain);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setEnabled(true);
        menuBar->setGeometry(QRect(0, 0, 644, 21));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        QdnMain->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QdnMain);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QdnMain->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QdnMain);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QdnMain->setStatusBar(statusBar);
#ifndef QT_NO_SHORTCUT
        labelLogin->setBuddy(lineEditLogin);
        labelPassword->setBuddy(lineEditPassword);
#endif // QT_NO_SHORTCUT

        menuBar->addAction(menuOptions->menuAction());
        menuOptions->addAction(actionQuit);

        retranslateUi(QdnMain);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(QdnMain);
    } // setupUi

    void retranslateUi(QMainWindow *QdnMain)
    {
        QdnMain->setWindowTitle(QApplication::translate("QdnMain", "QdnMain", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("QdnMain", "Quit", 0, QApplication::UnicodeUTF8));
        labelLogin->setText(QApplication::translate("QdnMain", "Login:", 0, QApplication::UnicodeUTF8));
        labelPassword->setText(QApplication::translate("QdnMain", "Password:", 0, QApplication::UnicodeUTF8));
        buttonLogin->setText(QApplication::translate("QdnMain", "Login", 0, QApplication::UnicodeUTF8));
        labelUserId->setText(QApplication::translate("QdnMain", "User id:", 0, QApplication::UnicodeUTF8));
        labelUserIdResult->setText(QString());
        labelPoints->setText(QApplication::translate("QdnMain", "Points:", 0, QApplication::UnicodeUTF8));
        labelPointsResult->setText(QString());
        labelLevel->setText(QApplication::translate("QdnMain", "Level:", 0, QApplication::UnicodeUTF8));
        labelLevelResult->setText(QString());
        labelTitle->setText(QApplication::translate("QdnMain", "Title:", 0, QApplication::UnicodeUTF8));
        labelTitleResult->setText(QString());
        labelGravatar->setText(QApplication::translate("QdnMain", "Gravatar:", 0, QApplication::UnicodeUTF8));
        labelGravatarResult->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabProfile), QApplication::translate("QdnMain", "Profile", 0, QApplication::UnicodeUTF8));
        labelPosts->setText(QApplication::translate("QdnMain", "Reply:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabPosts), QApplication::translate("QdnMain", "Unread posts", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("QdnMain", "Options", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QdnMain: public Ui_QdnMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDNMAIN_H
