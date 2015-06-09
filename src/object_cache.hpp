#pragma once

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

/*!
  Sometimes data read from one file is used in various processor (or in
  various instances of the same processor). In such cases data should
  read using ObjectCache singleton, so that a file is read only once,
  and its data is cached.
*/

/**
 * Singleton class to keep and re-use objects read from files
 */
class ObjectCache {

public:

    static ObjectCache& getInstance();

    template<typename T>
    boost::shared_ptr<T> getObject(const boost::filesystem::path& filePath) {
        if (mapping_.count(filePath))
            return boost::static_pointer_cast<T>(mapping_[filePath]);

        boost::shared_ptr<void> ptr(new T(filePath));

        mapping_[filePath] = ptr;

        return boost::static_pointer_cast<T>(ptr);
    }

    void clear();

private:
    std::map<boost::filesystem::path, boost::shared_ptr<void> > mapping_;

    ObjectCache() {
    }

};

