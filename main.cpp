#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <map>
#include <unordered_map>
#include <algorithm>

/* this is a concept implementation that can be further generalized either by run time polymorphism or compile time polymorphism (aka templates) */
class CacheObj
{
public:
    CacheObj(unsigned int keyo, int valueo = 0, unsigned int usageo = 0): key(keyo), value(valueo), lru_usage(usageo) {}
    CacheObj(const CacheObj &cctor): key(cctor.key), value(cctor.value), lru_usage(cctor.lru_usage) {}
    CacheObj &operator=(const CacheObj &assigno) {
        if (this == & assigno)
            return *this;
        key = assigno.key;
        value = assigno.value;
        lru_usage = assigno.lru_usage;
        return *this;
    }

    bool operator > (const CacheObj &othrObj) {
        return lru_usage > othrObj.lru_usage;
    }
    bool operator >= (const CacheObj &othrObj) {
        return lru_usage >= othrObj.lru_usage;
    }
    bool operator < (const CacheObj &othrObj) {
        return lru_usage < othrObj.lru_usage;
    }
    bool operator <= (const CacheObj &othrObj) {
        return lru_usage <= othrObj.lru_usage;
    }
    bool operator == (const CacheObj &othrObj) {
        return (lru_usage == othrObj.lru_usage);
    }
    bool operator != (const CacheObj &othrObj) {
        return (lru_usage != othrObj.lru_usage);
    }

    friend std::ostream &operator << (std::ostream &out, const CacheObj &cacheObje) {
        return out << "key: " << cacheObje.key << ", value: " << cacheObje.value << ", lru_usage: " << cacheObje.lru_usage << std::endl;
    }

    //for simplicity we keep all variables public for direct access
    unsigned int key;
    int value;
    unsigned int lru_usage;
};

class LRUCache
{
public:
    LRUCache(unsigned int cachesize = 100): m_maximum_cache_size(cachesize) {
        m_cacheUnOrderMap.reserve(m_maximum_cache_size);
    }
    ~LRUCache() {
        m_cacheUnOrderMap.clear();
        m_cacheMap.clear();
    }

    /* this reads CacheObj if exists, or other wise adds it as new (removing old less used CacheObj if needed) */
    CacheObj &getInsertCacheObj(CacheObj &cacheObje) {
        /* check if CacheObj already member of cache */
        std::unordered_map<unsigned int, unsigned int>::iterator fndIt = m_cacheUnOrderMap.find(cacheObje.key);
        if (fndIt != m_cacheUnOrderMap.end()) {
            CacheObj tmpObj = m_cacheMap.at(fndIt->second /* a m_cacheMap key */);
            m_cacheMap.erase(fndIt->second /* a m_cacheMap key */);

            /* update lru status */
            tmpObj.lru_usage++;
            m_cacheMap.insert(std::pair<unsigned int, CacheObj>(tmpObj.lru_usage, tmpObj));

            /* update this CacheObj in m_cacheUnOrderMap */
            m_cacheUnOrderMap.at(tmpObj.key) = tmpObj.lru_usage;

            return m_cacheMap.at(tmpObj.lru_usage);
        } else {
            /* insert this new CacheObj */

            /* check available capacity */
            if (m_cacheUnOrderMap.size() <= m_maximum_cache_size) {
                m_cacheMap.insert(std::pair<unsigned int, CacheObj>(cacheObje.lru_usage, cacheObje));
                m_cacheUnOrderMap.insert(std::pair<unsigned int, unsigned int>(cacheObje.key, cacheObje.lru_usage));
            } else {
                std::map<unsigned int, CacheObj>::reverse_iterator rit = m_cacheMap.rbegin();
                CacheObj tmpobj = rit->second;
                m_cacheMap.erase(tmpobj.lru_usage);
                m_cacheUnOrderMap.erase(tmpobj.key);

                m_cacheMap.insert(std::pair<unsigned int, CacheObj>(cacheObje.lru_usage, cacheObje));
                m_cacheUnOrderMap.insert(std::pair<unsigned int, unsigned int>(cacheObje.key, cacheObje.lru_usage));

            }

            /* return this same passed CacheObj */
            return cacheObje;
        }
    }


    bool setUpdateCacheObj(const CacheObj &cacheObje) {
        /* check if CacheObj already member of cache */
        std::unordered_map<unsigned int, unsigned int>::iterator fndIt = m_cacheUnOrderMap.find(cacheObje.key);
        if (fndIt != m_cacheUnOrderMap.end()) {
            CacheObj tmpObj = m_cacheMap.at(fndIt->second /* a m_cacheMap key */);
            m_cacheMap.erase(fndIt->second /* a m_cacheMap key */);

            //update CacheObj
            tmpObj = cacheObje;

            m_cacheMap.insert(std::pair<unsigned int, CacheObj>(tmpObj.lru_usage, tmpObj));

            /* update this CacheObj in m_cacheUnOrderMap */
            m_cacheUnOrderMap.at(tmpObj.key) = tmpObj.lru_usage;

            return true;
        }

        return false;
    }

    friend std::ostream &operator << (std::ostream &out, const LRUCache &lrucacheObje) {

        for (std::map<unsigned int, CacheObj >::const_iterator cacheObjIt = lrucacheObje.m_cacheMap.begin();
                cacheObjIt != lrucacheObje.m_cacheMap.end(); ++cacheObjIt) {
				CacheObj tmpObj = cacheObjIt->second;
				out << tmpObj;
        }
        return out;
    }


private:
    unsigned int m_maximum_cache_size;
    std::unordered_map < unsigned int, unsigned int /* key to m_cacheMap */ > m_cacheUnOrderMap;
    std::map<unsigned int, CacheObj, std::greater<unsigned int>> m_cacheMap;	/* flawed by design!*/
};

int main(int argc, char *argv[])
{
    std::cout << ">> ---------" << ", thread: 0x" << std::hex << std::this_thread::get_id() << std::dec << std::endl;

    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t (today);
    std::cout << "today is: " << ctime(&tt) << std::endl;

    std::chrono::high_resolution_clock::time_point tstart = std::chrono::high_resolution_clock::now();

    try {
        unsigned int tmpkeyo = 1;
        CacheObj obj1(tmpkeyo++, 22, 1);
        CacheObj obj2(tmpkeyo++, 23, 5);
        CacheObj obj3(tmpkeyo++, 24, 2);
        CacheObj obj4(tmpkeyo++, 25, 4);

		LRUCache lruCache(5);
		lruCache.getInsertCacheObj(obj1);
		lruCache.getInsertCacheObj(obj2);
		lruCache.getInsertCacheObj(obj3);
		lruCache.getInsertCacheObj(obj4);

		std::cout << lruCache << std::endl;

		//lets increase lru of obj4 to 6
		obj4.lru_usage = 7;
		lruCache.setUpdateCacheObj(obj4);

		std::cout << lruCache << std::endl;



    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    std::chrono::high_resolution_clock::time_point at_some_point = std::chrono::high_resolution_clock::now();
    std::chrono::duration < double >time_span = std::chrono::duration_cast < std::chrono::duration <double> > (at_some_point - tstart);

    std::cout << " << --------- " << time_span.count() << ", thread: 0x" << std::hex << std::this_thread::get_id() << std::dec << std::endl;

    return 0;
}
