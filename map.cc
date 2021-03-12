extern "C"
{
#include <stdio.h>
#include "mylib.h"
#include "node.h"
#include "assert.h"
}

#include <unordered_map>
#include <iostream>
#include <string>
using namespace std;

template <typename T, typename U>
void cpp_map_set_real(unordered_map<U, T *> *map, U key, T *value)
{
    (*map)[key] = value;
}
void cpp_map_set(map_kind mkind, void *map_void, void *key, void *value)
{
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_v = (unordered_map<void *, void *> *)map_void;
        cpp_map_set_real(map_v, key, (void *)value);
        break;
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        cpp_map_set_real(map_v, (string)(char *)key, (void *)value);
        break;
    }
    default:
        throw string("unknown mkind at cpp_map_set: ") + to_string(mkind);
    }
}
template <typename T, typename U>
void *cpp_map_get_real(unordered_map<U, T *> *map, U key)
{
    auto it = map->find(key);
    if (it == map->end())
    {
        return (void *)0;
    }
    return (void *)it->second;
}
void *cpp_map_get(map_kind mkind, void *map_void, void *key)
{
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_v = (unordered_map<void *, void *> *)map_void;
        return cpp_map_get_real(map_v, key);
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        return cpp_map_get_real(map_v, (string)(char *)key);
    }
    default:
        throw string("unknown mkind at cpp_map_get: ") + to_string(mkind);
    }
}
template <typename T, typename U>
void cpp_map_erase_real(unordered_map<U, T *> *map, U key)
{
    int num = map->erase(key);
    assert(num == 1);
}
void cpp_map_erase(map_kind mkind, void *map_void, void *key)
{
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_v = (unordered_map<void *, void *> *)map_void;
        return cpp_map_erase_real(map_v, key);
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        return cpp_map_erase_real(map_v, (string)(char *)key);
    }
    default:
        throw string("unknown mkind at cpp_map_list_keys: ") + to_string(mkind);
    }
}
template <typename T, typename U>
void cpp_map_list_keys_real(unordered_map<U, T *> *map)
{
    cout << "cpp_map_list_keys: size = " << map->size() << endl;
    for (auto elem : *map)
    {
        cout << elem.first << ", " << elem.second << endl;
    }
}
void cpp_map_list_keys(map_kind mkind, void *map_void)
{
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_v = (unordered_map<void *, void *> *)map_void;
        return cpp_map_list_keys_real(map_v);
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        return cpp_map_list_keys_real(map_v);
    }
    default:
        throw string("unknown mkind at cpp_map_list_keys: ") + to_string(mkind);
    }
}
size_t cpp_map_size(map_kind mkind, void *map_void)
{
    size_t size;
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_v = (unordered_map<void *, void *> *)map_void;
        size = map_v->size();
        return size;
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        size = map_v->size();
        return size;
    }
    default:
        throw string("unknown mkind at cpp_map_list_keys: ") + to_string(mkind);
    }
}
void *cpp_map_new(map_kind mkind)
{
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_void = new unordered_map<void *, void *>;
        return (void *)map_void;
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_void = new unordered_map<string, void *>;
        return (void *)map_void;
    }
    default:
        throw string("unknown mkind at cpp_map_new: ") + to_string(mkind);
    }
}
void cpp_map_delete(map_kind mkind, void *map_void)
{
    switch (mkind)
    {
    case map_voidkey_kind:
    {
        unordered_map<void *, void *> *map_v = (unordered_map<void *, void *> *)map_void;
        delete map_v;
        break;
    }
    case map_stringkey_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        delete map_v;
        break;
    }
    case map_stringkey_node_kind:
    {
        unordered_map<string, void *> *map_v = (unordered_map<string, void *> *)map_void;
        for (auto elem : *map_v)
        {
            my_free_node(elem.second, 1);
        }
        delete map_v;
        break;
    }
    default:
    {
        throw string("unknown mkind at cpp_map_delete: ") + to_string(mkind);
    }
    }
}
