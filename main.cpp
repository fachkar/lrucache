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
    CacheObj(unsigned int keyo, int valueo = 0, unsigned int usageo = 0): key(keyo), value(valueo), mru_usage(usageo) {}
    CacheObj(const CacheObj &cctor): key(cctor.key), value(cctor.value), mru_usage(cctor.mru_usage) {}
    CacheObj &operator=(const CacheObj &assigno) {
        if (this == & assigno)
            return *this;
        key = assigno.key;
        value = assigno.value;
        mru_usage = assigno.mru_usage;
        return *this;
    }

    bool operator > (const CacheObj &othrObj) {
        return mru_usage > othrObj.mru_usage;
    }
    bool operator >= (const CacheObj &othrObj) {
        return mru_usage >= othrObj.mru_usage;
    }
    bool operator < (const CacheObj &othrObj) {
        return mru_usage < othrObj.mru_usage;
    }
    bool operator <= (const CacheObj &othrObj) {
        return mru_usage <= othrObj.mru_usage;
    }
    bool operator == (const CacheObj &othrObj) {
        return (mru_usage == othrObj.mru_usage);
    }
    bool operator != (const CacheObj &othrObj) {
        return (mru_usage != othrObj.mru_usage);
    }

    friend std::ostream &operator << (std::ostream &out, const CacheObj &cacheObje) {
        return out << "key: " << cacheObje.key << ", value: " << cacheObje.value << ", mru_usage: " << cacheObje.mru_usage << std::endl;
    }

    //for simplicity we keep all variables public for direct access
    unsigned int key;
    int value;
    unsigned int mru_usage;
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

    void printLRUCacheObjList() {
        std::cout <<  "Start of lru CacheObjs: " << std::endl;
        std::map<unsigned int, CacheObj>::reverse_iterator rit = m_cacheMap.rbegin();
        std::pair<mltimp_t::iterator, mltimp_t::iterator> keyItr = m_cacheMap.equal_range(rit->first /* a m_cacheMap key */);
        for(mltimp_t::const_iterator cItr = keyItr.first; cItr != keyItr.second; ++cItr) {
            CacheObj tmpObj = cItr->second;
            std::cout << "\t" <<  tmpObj;
        }

        std::cout <<  "End of lru CacheObjs. " << std::endl;
    }

    /* this reads CacheObj if exists, or other wise adds it as new (removing old less used CacheObj if needed) */
    CacheObj &getInsertCacheObj(CacheObj &cacheObje) {
        /* check if CacheObj already member of cache */
        std::unordered_map<unsigned int, unsigned int>::iterator fndIt = m_cacheUnOrderMap.find(cacheObje.key);
        if (fndIt != m_cacheUnOrderMap.end()) {
            std::pair<mltimp_t::iterator, mltimp_t::iterator> keyItr = m_cacheMap.equal_range(fndIt->second /* a m_cacheMap key */);

            for(mltimp_t::const_iterator cItr = keyItr.first; cItr != keyItr.second; ++cItr) {
                CacheObj tmpObj = cItr->second;
                if(tmpObj.key ==  cacheObje.key) {
                    m_cacheMap.erase(cItr);
                    /* update lru status */
                    tmpObj.mru_usage++;
                    m_cacheMap.insert(std::pair<unsigned int, CacheObj>(tmpObj.mru_usage, tmpObj));
                    /* update this CacheObj in m_cacheUnOrderMap */
                    m_cacheUnOrderMap.at(tmpObj.key) = tmpObj.mru_usage;
                    cacheObje = tmpObj;
                    break;
                }
            }

            /* should not reach here,  nevertheless,  return this same passed CacheObj */
            return cacheObje;
        } else {
            /* insert this new CacheObj */

            /* check available capacity */
            if (m_cacheUnOrderMap.size() <= m_maximum_cache_size) {
                m_cacheMap.insert(std::pair<unsigned int, CacheObj>(cacheObje.mru_usage, cacheObje));
                m_cacheUnOrderMap.insert(std::pair<unsigned int, unsigned int>(cacheObje.key, cacheObje.mru_usage));
            } else {
                std::map<unsigned int, CacheObj>::reverse_iterator rit = m_cacheMap.rbegin();
                CacheObj tmpobj = rit->second;
                m_cacheMap.erase(tmpobj.mru_usage);
                m_cacheUnOrderMap.erase(tmpobj.key);

                m_cacheMap.insert(std::pair<unsigned int, CacheObj>(cacheObje.mru_usage, cacheObje));
                m_cacheUnOrderMap.insert(std::pair<unsigned int, unsigned int>(cacheObje.key, cacheObje.mru_usage));
            }

            /* return this same passed CacheObj */
            return cacheObje;
        }
    }


    bool setUpdateCacheObj(const CacheObj &cacheObje) {
        /* check if CacheObj already member of cache */
        std::unordered_map<unsigned int, unsigned int>::iterator fndIt = m_cacheUnOrderMap.find(cacheObje.key);
        if (fndIt != m_cacheUnOrderMap.end()) {

            std::pair<mltimp_t::iterator, mltimp_t::iterator> keyItr = m_cacheMap.equal_range(fndIt->second /* a m_cacheMap key */);

            for(mltimp_t::const_iterator cItr = keyItr.first; cItr != keyItr.second; ++cItr) {
                CacheObj tmpObj = cItr->second;
                if(tmpObj.key ==  cacheObje.key) {
                    m_cacheMap.erase(cItr);

                    /* copy to passed CacheObj */
                    tmpObj = cacheObje;
                    /* update lru status */
                    tmpObj.mru_usage++;
                    m_cacheMap.insert(std::pair<unsigned int, CacheObj>(tmpObj.mru_usage, tmpObj));
                    /* update this CacheObj in m_cacheUnOrderMap */
                    m_cacheUnOrderMap.at(tmpObj.key) = tmpObj.mru_usage;
                    return true;
                }
            }

            return false;
        }

        return false;
    }

    friend std::ostream &operator << (std::ostream &out, const LRUCache &lrucacheObje) {

        for (std::multimap<unsigned int, CacheObj >::const_iterator cacheObjIt = lrucacheObje.m_cacheMap.begin();
                cacheObjIt != lrucacheObje.m_cacheMap.end(); ++cacheObjIt) {
            CacheObj tmpObj = cacheObjIt->second;
            out << tmpObj;
        }
        return out;
    }


private:
    unsigned int m_maximum_cache_size;
    std::unordered_map < unsigned int, unsigned int /* key to m_cacheMap */ > m_cacheUnOrderMap;
    typedef std::multimap<unsigned int, CacheObj, std::greater<unsigned int>> mltimp_t;
    mltimp_t m_cacheMap;
};

int main(int argc, char *argv[])
{
    std::cout << ">> ---------" << ", thread: 0x" << std::hex << std::this_thread::get_id() << std::dec << std::endl;

    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t (today);
    std::cout << "today is: " << ctime(&tt) << std::endl;

    std::chrono::high_resolution_clock::time_point tstart = std::chrono::high_resolution_clock::now();

    try {

    /* note the the key should be unique not checking in ctor for simplicity */
        CacheObj obj1(1, 22, 1);
        CacheObj obj2(2, 23, 5);
        CacheObj obj21(3, 23, 5);
        CacheObj obj3(4, 24, 2);
        CacheObj obj4(5, 25, 4);
        CacheObj obj11(6, 22, 1);

        LRUCache lruCache(5);
        lruCache.getInsertCacheObj(obj1);
        lruCache.getInsertCacheObj(obj2);
        lruCache.getInsertCacheObj(obj3);
        lruCache.getInsertCacheObj(obj4);
        lruCache.getInsertCacheObj(obj21);
        lruCache.getInsertCacheObj(obj11);

        std::cout << lruCache << std::endl;

        //lets increase lru of obj4 to 6
        obj4.mru_usage = 7;
        lruCache.setUpdateCacheObj(obj4);
        // let read obj21 this should increase its mru
        lruCache.getInsertCacheObj(obj21);

        std::cout << lruCache << std::endl;

        /* print list of lru cache objects */
        lruCache.printLRUCacheObjList();



    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    std::chrono::high_resolution_clock::time_point at_some_point = std::chrono::high_resolution_clock::now();
    std::chrono::duration < double >time_span = std::chrono::duration_cast < std::chrono::duration <double> > (at_some_point - tstart);

    std::cout << " << --------- " << time_span.count() << ", thread: 0x" << std::hex << std::this_thread::get_id() << std::dec << std::endl;

    return 0;
}

