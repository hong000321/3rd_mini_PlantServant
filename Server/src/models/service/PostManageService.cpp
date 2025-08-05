#include "PostManageService.h"
#include <QDebug>
#include <algorithm>
#include <QFile>
#include <QFileInfo>

PostManageService* PostManageService::instance_ = nullptr;

PostManageService::PostManageService(QObject *parent)
    : QObject(parent)
{
    postRepo_ = PostJsonRepo::getInstance();
}

PostManageService* PostManageService::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new PostManageService();
    }
    return instance_;
}

void PostManageService::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

RaErrorCode PostManageService::createPost(const Post& post)
{
    id_t postId = postRepo_->insert(post);
    if (postId >= 0) {
        emit postCreated(postId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PostManageService::updatePost(const Post& post)
{
    const Post* existingPost = postRepo_->getObjPtrById(post.getId());
    if (existingPost == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    if (postRepo_->update(post)) {
        emit postUpdated(post.getId());
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PostManageService::deletePost(id_t postId)
{
    if (postRepo_->removeById(postId)) {
        emit postDeleted(postId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

const Post* PostManageService::getPostById(id_t postId)
{
    return postRepo_->getObjPtrById(postId);
}

QVector<Post> PostManageService::getPostsByAuthorId(id_t authorId)
{
    QVector<Post> result;
    QVector<Post> allPosts = postRepo_->getAllObjects();

    for (const Post& post : allPosts) {
        if (post.getUserId() == authorId) {
            result.append(post);
        }
    }

    return result;
}

QVector<Post> PostManageService::getAllPosts()
{
    return postRepo_->getAllObjects();
}

QVector<Post> PostManageService::getAllPostsSorted(PostSortType sortType)
{
    QVector<Post> posts = postRepo_->getAllObjects();
    sortPosts(posts, sortType);
    return posts;
}

QVector<Post> PostManageService::getPostsByAuthorIdSorted(id_t authorId, PostSortType sortType)
{
    QVector<Post> posts = getPostsByAuthorId(authorId);
    sortPosts(posts, sortType);
    return posts;
}

QVector<Post> PostManageService::searchPostsByTitle(const QString& keyword)
{
    QVector<Post> result;
    QVector<Post> allPosts = postRepo_->getAllObjects();

    for (const Post& post : allPosts) {
        if (post.getTitle().contains(keyword, Qt::CaseInsensitive)) {
            result.append(post);
        }
    }

    return result;
}

QVector<Post> PostManageService::searchPostsByContent(const QString& keyword)
{
    QVector<Post> result;
    QVector<Post> allPosts = postRepo_->getAllObjects();

    for (const Post& post : allPosts) {
        if (post.getContent().contains(keyword, Qt::CaseInsensitive)) {
            result.append(post);
        }
    }

    return result;
}

QVector<Post> PostManageService::searchPostsByTitleOrContent(const QString& keyword)
{
    QVector<Post> result;
    QVector<Post> allPosts = postRepo_->getAllObjects();

    for (const Post& post : allPosts) {
        if (post.getTitle().contains(keyword, Qt::CaseInsensitive) ||
            post.getContent().contains(keyword, Qt::CaseInsensitive)) {
            result.append(post);
        }
    }

    return result;
}

QVector<Post> PostManageService::getPostsInDateRange(const QDateTime& startDate, const QDateTime& endDate)
{
    QVector<Post> result;
    QVector<Post> allPosts = postRepo_->getAllObjects();

    for (const Post& post : allPosts) {
        QDateTime createdAt = post.getCreatedAt();
        if (createdAt >= startDate && createdAt <= endDate) {
            result.append(post);
        }
    }

    return result;
}

QVector<Post> PostManageService::getRecentPosts(int days)
{
    QDateTime startDate = QDateTime::currentDateTime().addDays(-days);
    QDateTime endDate = QDateTime::currentDateTime();
    return getPostsInDateRange(startDate, endDate);
}

QVector<Post> PostManageService::getPostsPaginated(int page, int pageSize, PostSortType sortType)
{
    QVector<Post> allPosts = getAllPostsSorted(sortType);

    int startIndex = (page - 1) * pageSize;
    int endIndex = qMin(startIndex + pageSize, allPosts.size());

    if (startIndex >= allPosts.size()) {
        return QVector<Post>(); // 빈 벡터 반환
    }

    QVector<Post> result;
    for (int i = startIndex; i < endIndex; ++i) {
        result.append(allPosts[i]);
    }

    return result;
}

int PostManageService::getTotalPostCount()
{
    return postRepo_->getSize();
}

int PostManageService::getPostCountByAuthor(id_t authorId)
{
    return getPostsByAuthorId(authorId).size();
}

QVector<Post> PostManageService::getPopularPosts(int limit)
{
    // 현재는 최신순으로 반환, 향후 조회수 기능 추가 시 수정
    QVector<Post> posts = getAllPostsSorted(PostSortType::CreatedDate_Desc);

    if (posts.size() > limit) {
        posts = posts.mid(0, limit);
    }

    return posts;
}

QVector<Post> PostManageService::getLatestPosts(int limit)
{
    QVector<Post> posts = getAllPostsSorted(PostSortType::CreatedDate_Desc);

    if (posts.size() > limit) {
        posts = posts.mid(0, limit);
    }

    return posts;
}

bool PostManageService::loadPosts(const QString& filePath)
{
    return postRepo_->loadDataFromFile(filePath);
}

bool PostManageService::savePosts()
{
    return postRepo_->saveToFile();
}

int PostManageService::getPostCount()
{
    return postRepo_->getSize();
}


QString PostManageService::getPostImageAsBase64(id_t postId)
{
    const Post* post = postRepo_->getObjPtrById(postId);
    if (!post) {
        return QString();
    }

    QString imagePath = post->getImagePath();
    if (imagePath.isEmpty()) {
        return QString();
    }

    QFile imageFile(imagePath);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open image file:" << imagePath;
        return QString();
    }

    QByteArray imageData = imageFile.readAll();
    imageFile.close();

    return imageData.toBase64();
}

qint64 PostManageService::getImageFileSize(const QString& imagePath)
{
    if (imagePath.isEmpty()) {
        return 0;
    }

    QFileInfo fileInfo(imagePath);
    return fileInfo.exists() ? fileInfo.size() : 0;
}


void PostManageService::sortPosts(QVector<Post>& posts, PostSortType sortType)
{
    switch (sortType) {
    case PostSortType::CreatedDate_Desc:
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
            return a.getCreatedAt() > b.getCreatedAt();
        });
        break;
    case PostSortType::CreatedDate_Asc:
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
            return a.getCreatedAt() < b.getCreatedAt();
        });
        break;
    case PostSortType::UpdatedDate_Desc:
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
            return a.getUpdatedAt() > b.getUpdatedAt();
        });
        break;
    case PostSortType::UpdatedDate_Asc:
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
            return a.getUpdatedAt() < b.getUpdatedAt();
        });
        break;
    case PostSortType::Title_Asc:
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
            return a.getTitle().compare(b.getTitle(), Qt::CaseInsensitive) < 0;
        });
        break;
    case PostSortType::Title_Desc:
        std::sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
            return a.getTitle().compare(b.getTitle(), Qt::CaseInsensitive) > 0;
        });
        break;
    }
}
