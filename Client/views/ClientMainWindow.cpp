#include "ClientMainWindow.h"
#include "ui_ClientMainWindow.h"
#include "LoginMainWindow.h"
#include "ClientSocket.h"
#include <QFile>
#include <QFileDialog>

ClientMainWindow::ClientMainWindow(ClientSocket* socket, UserService* userService, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
    , socket_(socket)
    , userService_(userService)
{
    chatService_ = new ChatService(socket_, this);
    postService_ = new PostService(socket_, this);

    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);

    //채팅 입력창 연결
    connect(ui->input_chatting, &QLineEdit::returnPressed, this, [=]() {
        QString message = ui->input_chatting->text().trimmed();
        if (!message.isEmpty()) {
            chatService_->sendMessage(chatRoomId_, message);
            ui->input_chatting->clear();
        }
    });

    // 서버에서 메시지 도착 시 채팅창에 append
    connect(chatService_, &ChatService::chatMessageReceived, this, [=](const QJsonObject& msg) {
        QString userName = msg.value("userName").toString();
        QString chatStr = msg.value("chatStr").toString();
        QString chatLog = QString("[%1]: %2").arg(userName, chatStr);
        ui->chatting_room->appendPlainText(chatLog);
    });

    //히스토리 수신 시 채팅창에 기록
    connect(chatService_, &ChatService::chatHistoryReceived, this, [=](const QJsonArray& messages) {
        ui->chatting_room->clear();
        for (const QJsonValue& val : messages) {
            QJsonObject msg = val.toObject();
            QString userName = msg.value("userName").toString();
            QString chatStr = msg.value("chatStr").toString();
            ui->chatting_room->appendPlainText(QString("[%1]: %2").arg(userName, chatStr));
        }
    });
}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

void ClientMainWindow::on_button_myplant_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_plant);
}


void ClientMainWindow::on_button_writepost_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_write_post);
}

// 뒤로 가기 버튼은 다 page_gallery로 이동한다
void ClientMainWindow::on_button_back_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}

void ClientMainWindow::on_button_back2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}

void ClientMainWindow::on_button_back3_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}


void ClientMainWindow::setUser(const QJsonObject& userData){
    currentUser_.fromJson(userData);
    qDebug() << "👤 사용자 정보:" << currentUser_.getName() << currentUser_.getEmail();

    chatService_->joinChatRoom(chatRoomId_, currentUser_.getId());
    chatService_->requestChatHistory(chatRoomId_);

}



//post 저장버튼 눌렀을때
void ClientMainWindow::on_button_save_clicked()
{
    QString title = ui->input_title->text().trimmed();
    QString content = ui->input_post->toPlainText().trimmed();

    if (title.isEmpty() || content.isEmpty()) {
        qDebug() << "❗ 제목이나 내용이 비어 있습니다.";
        return;
    }

    Post post;
    post.setTitle(title);
    post.setContent(content);
    post.setUserId(currentUser_.getId());
    post.setImageBase64(imageBase64_);

    postService_->createPost(post);
    qDebug() << "📝 글 작성 요청 전송됨";

    // UI 초기화 및 이동
    ui->input_title->clear();
    ui->input_post->clear();
    ui->image_plant->clear();
    imageBase64_.clear(); //다음작성을 위해 초기화
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}


//post file버튼 눌렀을때
void ClientMainWindow::on_button_file_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "이미지 선택", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty()) {
        QFile file(imagePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray imageData = file.readAll();
            imageBase64_ = QString::fromLatin1(imageData.toBase64());  // ✅ 멤버 변수에 저장
            file.close();

            QPixmap pixmap;
            pixmap.loadFromData(imageData);
            ui->image_plant->setPixmap(pixmap.scaled(ui->image_plant->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            qDebug() << "📎 이미지 선택 완료, base64 크기:" << imageBase64_.length();
        }
    }
}

