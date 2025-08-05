#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "models/entity/Post.h"

class PostWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PostWidget(const Post& post, QWidget* parent = nullptr);

signals:
    void postClicked(Post post);  // ✅ 클릭되었을 때 부모에게 Post 전달

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    Post post_;
    QLabel* imageLabel_;
};

#endif // POSTWIDGET_H
