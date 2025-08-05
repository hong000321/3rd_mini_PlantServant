#include "PostWidget.h"
#include <QPixmap>
#include <QByteArray>
#include <QMouseEvent>

PostWidget::PostWidget(const Post& post, QWidget* parent)
    : QWidget(parent), post_(post)
{
    imageLabel_ = new QLabel(this);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setFixedSize(150, 150);

    // 이미지 base64 → QPixmap
    QPixmap pixmap;
    QByteArray imageData = QByteArray::fromBase64(post_.getImageBase64().toLatin1());

    if (pixmap.loadFromData(imageData)) {
        imageLabel_->setPixmap(pixmap.scaled(imageLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        qDebug() << "❌ 이미지 로드 실패: base64 길이 = " << imageData.length();
        imageLabel_->setText("No Image");
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel_);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void PostWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit postClicked(post_);  // ✅ 클릭 시 Post 전달
}
