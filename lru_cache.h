

untested



template <typename Key_Type, typename Item_Type>
struct LRU_Cache {
    using List_Type    = std::list<Key_Type>;
    using Map_Type     = std::map<Key_Type, std::pair<Item_Type, typename std::list<Key_Type>::iterator>>;
    using Map_Iterator = typename Map_Type::iterator;

    i32       cache_size;
    Map_Type  cache_map;
    List_Type cache_list;

    LRU_Cache(i32 size) : cache_size(size) {
        cache_size = size;
    }

    void insert(const Key_Type &key, const Item_Type &item) {
        Map_Iterator cache_entry = cache_map.find(key);

        if (cache_entry == cache_map.end()) {
            cache_list.push_front(key);
            cache_map[key] = {item, cache_list.begin()};

            if (cache_list.size() > cache_size) {
                cache_map.erase(cache_list.back());
                cache_list.pop_back();
            }
        } else {
            cache_list.erase((*cache_entry).second.second);
            cache_list.push_front(key);
            cache_map[key] = {item, cache_list.begin()};
        }
    }

    bool get(const Key_Type &key, Item_Type &item) {
        Map_Iterator cache_entry = cache_map.find(key);

        if (cache_entry == cache_map.end()) {
            return false;
        }

        item = (*cache_entry).second.first;
        cache_list.erase((*cache_entry).second.second);
        cache_list.push_front(key);
        (*cache_entry).second.second = cache_list.begin();
        return true;
    }
};
