#include "EnrollAdminWindow.h"
#include "ui_enroll_admin_window.h"
#include <QRegularExpression>
#include <QDebug>

EnrollAdminWindow::EnrollAdminWindow(ProtocolController *protocolController, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EnrollAdminWindow)
    , m_protocolController(protocolController)
{
    ui->setupUi(this);

    // 창 제목 설정
    setWindowTitle("관리자 등록");

    // 등록 버튼 연결 (UI 파일의 pushButton_2를 사용)
    connect(ui->pushButton_register, &QPushButton::clicked,
            this, &EnrollAdminWindow::on_pushButton_register_clicked);

    // 입력 검증을 위한 연결
    connect(ui->lineEdit_id, &QLineEdit::textChanged,
            this, &EnrollAdminWindow::on_lineEdit_id_textChanged);
    connect(ui->lineEdit_email, &QLineEdit::textChanged,
            this, &EnrollAdminWindow::on_lineEdit_email_textChanged);

    // 초기 상태 설정
    ui->pushButton_register->setEnabled(false);
}

EnrollAdminWindow::~EnrollAdminWindow()
{
    delete ui;
}

void EnrollAdminWindow::on_pushButton_register_clicked()
{
    if (!validateInput()) {
        return;
    }

    // 입력값 수집
    QString strId = ui->lineEdit_id->text().trimmed();
    QString password = ui->lineEdit_password->text();
    QString name = ui->lineEdit_name->text().trimmed();
    QString email = ui->lineEdit_email->text().trimmed();
    QString address = ui->lineEdit_address->text().trimmed();

    try {
        // User 객체 생성
        User newAdmin;

        // JSON을 통해 User 설정 (User 클래스에 적절한 생성자가 없을 경우)
        QJsonObject userJson;
        userJson["strId"] = strId;
        userJson["password"] = password;
        userJson["name"] = name;
        userJson["email"] = email;
        userJson["address"] = address;
        userJson["level"] = static_cast<int>(PermissionLevel::ADMIN_LEVEL);
        userJson["boolConnected"] = false;

        RaErrorCode result = newAdmin.fromJson(userJson);
        if (result != Ra_Success) {
            showMessage("오류", "사용자 데이터 생성에 실패했습니다.", true);
            return;
        }

        // ProtocolController를 통해 관리자 등록 요청
        QJsonObject parameters = newAdmin.toJson();
        QJsonObject request = QJsonObject{
            {"header", QJsonObject{
                           {"messageId", QUuid::createUuid().toString(QUuid::WithoutBraces)},
                           {"messageType", "command"},
                           {"timestamp", QDateTime::currentSecsSinceEpoch()},
                           {"version", "1.0"}
                       }},
            {"body", QJsonObject{
                         {"action", "register"},
                         {"target", "user"},
                         {"parameters", parameters}
                     }}
        };

        // 직접 ProtocolController에서 사용자 서비스를 통해 등록
        if (m_protocolController) {
            // UserManageService를 통해 직접 등록
            UserManageService* userService = UserManageService::getInstance();
            RaErrorCode createResult = userService->createUser(newAdmin);

            if (createResult == Ra_Success) {
                showMessage("성공",
                            QString("관리자 '%1'이(가) 성공적으로 등록되었습니다.").arg(name));
                qDebug() << "Admin registered successfully:" << strId;

                // 폼 초기화
                clearForm();

                // 창 닫기 (선택사항)
                // this->close();
            } else {
                showMessage("오류", "관리자 등록에 실패했습니다.\n아이디가 이미 존재하거나 다른 문제가 발생했습니다.", true);
                qDebug() << "Admin registration failed for:" << strId;
            }
        } else {
            showMessage("오류", "서버 연결이 없습니다.", true);
        }

    } catch (const std::exception& e) {
        showMessage("오류", QString("예외 발생: %1").arg(e.what()), true);
    } catch (...) {
        showMessage("오류", "알 수 없는 오류가 발생했습니다.", true);
    }
}

void EnrollAdminWindow::on_lineEdit_id_textChanged(const QString &text)
{
    Q_UNUSED(text)
    // 입력 검증 후 등록 버튼 활성화/비활성화
    ui->pushButton_register->setEnabled(validateInput());
}

void EnrollAdminWindow::on_lineEdit_email_textChanged(const QString &text)
{
    Q_UNUSED(text)
    // 입력 검증 후 등록 버튼 활성화/비활성화
    ui->pushButton_register->setEnabled(validateInput());
}

bool EnrollAdminWindow::validateInput()
{
    // 아이디 검증
    QString strId = ui->lineEdit_id->text().trimmed();
    if (strId.length() < 3 || strId.length() > 20) {
        return false;
    }

    // 비밀번호 검증
    QString password = ui->lineEdit_password->text();
    if (password.length() < 4) {
        return false;
    }

    // 이름 검증
    QString name = ui->lineEdit_name->text().trimmed();
    if (name.isEmpty()) {
        return false;
    }

    // 이메일 검증
    QString email = ui->lineEdit_email->text().trimmed();
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        return false;
    }

    return true;
}

void EnrollAdminWindow::clearForm()
{
    ui->lineEdit_id->clear();        // 아이디
    ui->lineEdit_password->clear();      // 비밀번호
    ui->lineEdit_name->clear();      // 이름
    ui->lineEdit_email->clear();      // 이메일
    ui->lineEdit_address->clear();      // 주소

    ui->lineEdit_id->setFocus();
    ui->pushButton_register->setEnabled(false);
}

void EnrollAdminWindow::showMessage(const QString &title, const QString &message, bool isError)
{
    if (isError) {
        QMessageBox::critical(this, title, message);
    } else {
        QMessageBox::information(this, title, message);
    }
}
