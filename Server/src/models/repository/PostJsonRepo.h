#ifndef POSTJSONREPO_H
#define POSTJSONREPO_H
#include "JsonRepo.h"
#include "models/Entity/Post.h"

class PostJsonRepo : public JsonRepo<Post> {
private:
    static PostJsonRepo* instance_;  // 싱글톤 인스턴스
    PostJsonRepo();
    PostJsonRepo(const PostJsonRepo&) = delete;
    PostJsonRepo& operator=(const PostJsonRepo&) = delete;
    PostJsonRepo(PostJsonRepo&&) = delete;
    PostJsonRepo& operator=(PostJsonRepo&&) = delete;

public:
    static PostJsonRepo* getInstance();
    ~PostJsonRepo() = default;
    static void destroyInstance();
};

#endif // POSTJSONREPO_H
