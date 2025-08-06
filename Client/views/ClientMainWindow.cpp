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
    plantService_ = new PlantService(socket_, this);

    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);

    ui->input_chatting->setPlaceholderText("채팅을 입력하세요");
    ui->input_title->setPlaceholderText("제목을 입력하세요");
    ui->input_post->setPlaceholderText("내용을 입력하세요");

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

    connect(postService_, &PostService::postListReceived,
            this, &ClientMainWindow::displayPostList);

    connect(postService_, &PostService::postReceived,
            this, &ClientMainWindow::showPost);

    //plantInfoReady 시그널 받으면 -> updatePlantInfo
    connect(plantService_, &PlantService::plantInfoReady,
            this, &ClientMainWindow::updatePlantInfo);
}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

void ClientMainWindow::on_button_myplant_clicked()
{
    plantService_->requestPlantInfo(currentUser_.getId());
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
    postService_->requestPostList();
    //plantService_->requestPlantInfo(currentUser_.getId());
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
    post.setUserName(currentUser_.getName());

    postService_->createPost(post);
    qDebug() << "📝 글 작성 요청 전송됨";

    // UI 초기화 및 이동
    ui->input_title->clear();
    ui->input_post->clear();
    ui->image_plant->clear();
    imageBase64_.clear(); //다음작성을 위해 초기화
    postService_->requestPostList();
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

void ClientMainWindow::displayPostList(const QJsonArray& posts)
{
    QGridLayout* layout = qobject_cast<QGridLayout*>(ui->gridLayout->layout());

    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    int row = 0, col = 0;
    int maxCols = 4; // 열 개수: 한 행에 2개씩 배치

    for (const QJsonValue& val : posts) {
        Post post;
        post.fromJson(val.toObject());

        PostWidget* widget = new PostWidget(post);

        connect(widget, &PostWidget::postClicked, this, &ClientMainWindow::showPost);

        layout->addWidget(widget, row, col);  // ✅ (행, 열) 위치에 배치

        if (++col >= maxCols) {
            col = 0;
            ++row;
        }
    }
}

void ClientMainWindow::showPost(const Post& post)
{
    ui->post_title->setText(post.getTitle());
    ui->post_content->setText(post.getContent());
    ui->post_author_id->setText(post.getUserName());

    if (!post.getImageBase64().isEmpty()){
        QPixmap pixmap;

        bool loaded = pixmap.loadFromData(QByteArray::fromBase64(post.getImageBase64().toLatin1()));
        if (!loaded){
            qDebug() << "이미지 로드 실패";
        } else {
            qDebug() << "이미지 로드 성공";
        }
        ui->post_image->setPixmap(pixmap.scaled(ui->post_image->size(), Qt::KeepAspectRatio));
    } else {
        ui->post_image->clear();
    }

    ui->stackedWidget->setCurrentWidget(ui->page_show_post);

}

void ClientMainWindow::updatePlantInfo(const Plant& plant)
{
    // 🌿 온도
    ui->bar_temp->setMinimum(0);
    ui->bar_temp->setMaximum(80);
    ui->bar_temp->setValue(static_cast<int>(plant.getTemperature()));
    ui->bar_temp->setFormat(QString("%1 °C").arg(plant.getTemperature(), 0, 'f', 1));

    // 💧 습도
    ui->bar_humi->setMinimum(0);
    ui->bar_humi->setMaximum(100);
    ui->bar_humi->setValue(plant.getHumidity());
    ui->bar_humi->setFormat(QString("%1 %").arg(plant.getHumidity()));

    // 🌼 별명 등 다른 정보 있으면 여기에
    ui->label_myplantname->setText(plant.getNickname());
    QString nickname = plant.getNickname();
    qDebug() << "닉네임 출력" << nickname;

    if (nickname == "초록이"){
        // QString imagePath = QCoreApplication::applicationDirPath() + "/tomato.jpg";
        QString imagePath = QCoreApplication::applicationDirPath() + "/../../../tomato.jpg";

        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            ui->image_myplant->setPixmap(
                pixmap.scaled(ui->image_myplant->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
                );
        }
    }

    if (nickname == "고무나무"){
        QString imagePath = QCoreApplication::applicationDirPath() + "/../../../gomu.jpg";
        QPixmap pixmap(imagePath);
        qDebug() << "경로?" << imagePath;
        qDebug() << "닉네임?" << nickname;

        if (!pixmap.isNull()) {
            ui->image_myplant->setPixmap(
                pixmap.scaled(ui->image_myplant->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
                );
        }
    }
}
