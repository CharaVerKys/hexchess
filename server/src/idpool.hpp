#pragma once

#include <SQLiteCpp/Database.h>
#include "types.hpp"
#include <map>
#include <random>

class IdPool{
    struct cache{
        constexpr static std::size_t maxCacheSize = 1000; // if reach this, load to db and clear map
        std::map<lhc::unique_id,std::chrono::system_clock::time_point> map;
    } cache;
    SQLite::Database db;
    struct {
        bool made = false;
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_int_distribution<lhc::unique_id> dist;
    } random_;

public:
    IdPool(std::filesystem::path const& pathToDb);
    ~IdPool();
    bool isIdValid(lhc::unique_id const&);
    lhc::unique_id getFreeId();

private:
    std::optional<std::pair<lhc::unique_id,std::chrono::system_clock::time_point>> getIdFromDb(lhc::unique_id const&);
    void addToCache(std::pair<lhc::unique_id,std::chrono::system_clock::time_point>const&);
    void loadFromCacheToDb();
    void delete30doIds();
    lhc::unique_id generateRandomId(); //
};