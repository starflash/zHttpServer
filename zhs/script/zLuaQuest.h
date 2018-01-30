//
// Created by zhangweiwen on 17-3-19.
//

#ifndef ZHS_ZLUAQUEST_H
#define ZHS_ZLUAQUEST_H

#include <ext/hash_set>

#include <zhs/base/zSingleton.h>

// 类声明
namespace luabind
{
    namespace detail
    {
        template <class T>
        struct delete_s;

        template <class T>
        struct destruct_only_s;
    }
}

/**
 * global index, used to decide executing a script or not.
 * it's for speed purpose and avoid wasting cpu time
 */
class zLuaQuest : public zSingleton<zLuaQuest>
{
public:

    // 友元类
    friend class zSingleton<zLuaQuest>;
    friend class zSingletonFactory<zLuaQuest>;
    friend class luabind::detail::delete_s<zLuaQuest>;
    friend class luabind::detail::destruct_only_s<zLuaQuest>;

    void add(int, int);
    bool has(int, int) const;
    void sort();

private:

    //
    __gnu_cxx::hash_set<int> lqs;

    // 构造函数
    zLuaQuest() = default;

    // 析构函数
    ~zLuaQuest() override = default;

    int hash(int, int) const;
};


#endif //ZHS_ZLUAQUEST_H
