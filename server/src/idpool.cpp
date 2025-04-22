#include "idpool.hpp"
#include "SQLiteCpp/Transaction.h"

IdPool::IdPool(std::filesystem::path const& pathToDb):db(pathToDb,SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE){
    db.exec("CREATE TABLE IF NOT EXISTS idpool ("
            "id   INTEGER PRIMARY KEY, "
            "timesec   INTEGER NOT NULL"
            ")");
    delete30doIds();
}
IdPool::~IdPool(){
    loadFromCacheToDb();
}

bool IdPool::isIdValid(lhc::unique_id const& id){
    auto iter_map = cache.map.find(id);
    if(iter_map not_eq cache.map.end()
    and iter_map->second + std::chrono::days(30) > std::chrono::system_clock::now()){
        iter_map->second = std::chrono::system_clock::now();
        return true;
    }
    auto opt = getIdFromDb(id);
    if(opt){
        if(opt->second + std::chrono::days(30) > std::chrono::system_clock::now()){
            opt->second = std::chrono::system_clock::now();
            addToCache(*opt);
            return true;
        }
    }
    return false;
}
lhc::unique_id IdPool::getFreeId(){
    lhc::unique_id newId;
    std::uint16_t limit = 0;
    do{
        newId = generateRandomId();
        if(limit++ > 4000){
            throw std::out_of_range("tried gen new id 4000 times but failed");
        }
    }while(isIdValid(newId));
    addToCache(std::make_pair(newId,std::chrono::system_clock::now()));
    return newId;
}

std::optional<std::pair<lhc::unique_id, std::chrono::system_clock::time_point>>
IdPool::getIdFromDb(const lhc::unique_id& id) {
    using clock = std::chrono::system_clock;
    SQLite::Statement query(db, "SELECT id, ts FROM idpool WHERE id = ?");
    query.bind(1, id);

    if (query.executeStep()) {
        auto id = static_cast<lhc::unique_id>(query.getColumn(0).getUInt());
        auto timesec = query.getColumn(1).getInt64();
        auto tp = clock::time_point{} + std::chrono::seconds(timesec);
        return std::make_optional(std::make_pair(id, tp));
    }else{
        //todo log
        (void)query.getErrorMsg();
    }
    return std::nullopt;
}
void IdPool::addToCache(std::pair<lhc::unique_id,std::chrono::system_clock::time_point>const& toAdd){
    if(cache.map.size() > cache.maxCacheSize){
        loadFromCacheToDb();
        delete30doIds();
    }
    cache.map.clear();
    cache.map.insert(toAdd);
}

void IdPool::loadFromCacheToDb() {
    SQLite::Transaction transaction(db);
    SQLite::Statement insert(db, "INSERT OR REPLACE INTO idpool (id, timesec) VALUES (?, ?)");
    for (auto const& [id, tp] : cache.map) {
        const std::int64_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        insert.bind(1, id);
        insert.bind(2, secs);
        insert.exec();
        insert.reset();
    }
    transaction.commit();
}

void IdPool::delete30doIds() {
    const auto _30do = std::chrono::system_clock::now() - std::chrono::days(30);
    const std::int64_t secs = std::chrono::duration_cast<std::chrono::seconds>(
        _30do.time_since_epoch()).count();

    SQLite::Statement deleteStmt(db, "DELETE FROM idpool WHERE timesec < ?");
    deleteStmt.bind(1, secs);
    deleteStmt.exec();
}

lhc::unique_id IdPool::generateRandomId() {
    if(not random_.made){
        random_.gen = std::mt19937(random_.rd.operator()()); // Mersenne Twister
        random_.dist = std::uniform_int_distribution<lhc::unique_id>(/*0 == not or invalid*/1, std::numeric_limits<lhc::unique_id>::max());
        static_assert(std::is_same<lhc::unique_id,uint32_t>::value); // may be not required, maybe everything ok on its own
    }
    return random_.dist(random_.gen);
}