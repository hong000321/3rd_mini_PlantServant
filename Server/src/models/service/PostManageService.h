#ifndef POSTMANAGESERVICE_H
#define POSTMANAGESERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include "models/entity/Post.h"
#include "models/repository/PostJsonRepo.h"
#include "utils/CustomErrorCodes.h"

enum class PostSortType {
    CreatedDate_Desc,    // 작성일 내림차순 (최신순)
    CreatedDate_Asc,     // 작성일 오름차순 (오래된순)
    UpdatedDate_Desc,    // 수정일 내림차순
    UpdatedDate_Asc,     // 수정일 오름차순
    Title_Asc,           // 제목 오름차순
    Title_Desc           // 제목 내림차순
};

class PostManageService : public QObject
{
    Q_OBJECT

private:
    static PostManageService* instance_;
    PostJsonRepo* postRepo_;

    PostManageService(QObject *parent = nullptr);
    PostManageService(const PostManageService&) = delete;
    PostManageService& operator=(const PostManageService&) = delete;

public:
    static PostManageService* getInstance();
    static void destroyInstance();
    ~PostManageService() = default;

    // 게시글 관리 메서드들
    RaErrorCode createPost(const Post& post);
    RaErrorCode updatePost(const Post& post);
    RaErrorCode deletePost(id_t postId);
    const Post* getPostById(id_t postId);
    QVector<Post> getPostsByAuthorId(id_t authorId);
    QVector<Post> getAllPosts();

    // 게시글 정렬 조회
    QVector<Post> getAllPostsSorted(PostSortType sortType = PostSortType::CreatedDate_Desc);
    QVector<Post> getPostsByAuthorIdSorted(id_t authorId, PostSortType sortType = PostSortType::CreatedDate_Desc);

    // 검색 기능
    QVector<Post> searchPostsByTitle(const QString& keyword);
    QVector<Post> searchPostsByContent(const QString& keyword);
    QVector<Post> searchPostsByTitleOrContent(const QString& keyword);

    // 날짜 범위 검색
    QVector<Post> getPostsInDateRange(const QDateTime& startDate, const QDateTime& endDate);
    QVector<Post> getRecentPosts(int days = 7); // 최근 n일 게시글

    // 페이징 지원
    QVector<Post> getPostsPaginated(int page, int pageSize, PostSortType sortType = PostSortType::CreatedDate_Desc);
    int getTotalPostCount();
    int getPostCountByAuthor(id_t authorId);

    // 통계
    QVector<Post> getPopularPosts(int limit = 10); // 향후 조회수 기능 추가 시 사용
    QVector<Post> getLatestPosts(int limit = 10);

    // 데이터 관리
    bool loadPosts(const QString& filePath);
    bool savePosts();
    int getPostCount();

    // 이미지 전송용
    QString getPostImageAsBase64(id_t postId);
    qint64 getImageFileSize(const QString& imagePath);

private:
    // 정렬 헬퍼 함수
    void sortPosts(QVector<Post>& posts, PostSortType sortType);

signals:
    void postCreated(id_t postId);
    void postUpdated(id_t postId);
    void postDeleted(id_t postId);
};

#endif // POSTMANAGESERVICE_H
