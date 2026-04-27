# COP290 Assignment 3 — LevelDB Modifications

This repository contains a modified version of [Google LevelDB](https://github.com/google/leveldb)
with three new APIs

**GitHub:** https://github.com/OQHowler/COP290-A3.git

## Team

- **Shoubhit Wandile** — `cs5240164@iitd.ac.in`
- **Shivam Khandelwal** — `cs1241075@iitd.ac.in`

## What we did

Three APIs were added to the `leveldb::DB` base class:

| API | Purpose |
|-----|---------|
| `Status DB::Scan(const ReadOptions&, const Slice& start_key, const Slice& end_key, std::vector<std::pair<std::string,std::string>>* result)` | Returns all key-value pairs whose keys lie in the half-open interval `[start_key, end_key)`. |
| `Status DB::DeleteRange(const WriteOptions&, const Slice& start_key, const Slice& end_key)` | Logically deletes every key in `[start_key, end_key)`. Physical removal happens during compaction. |
| `Status DB::ForceFullCompaction()` | Synchronously compacts data across all LSM-tree levels and prints compaction statistics. Blocks foreground reads and writes for the duration. |

## Usage Examples

```cpp
// Range scan
std::vector<std::pair<std::string, std::string>> out;
db->Scan(leveldb::ReadOptions(), "key_010", "key_020", &out);

// Range delete
db->DeleteRange(leveldb::WriteOptions(), "key_030", "key_050");

// Force a full compaction; stats are printed to stdout
db->ForceFullCompaction();

delete db;
```

## Files Modified

| File | Changes |
|------|---------|
| `include/leveldb/db.h` | Added pure-virtual declarations for `Scan`, `DeleteRange`, `ForceFullCompaction`. |
| `db/db_impl.h` | Added the corresponding `override` declarations on `DBImpl`, plus the `ExplicitCompactionStats` struct and `explicit_stats_` member used for the manual-compaction reporting. |
| `db/db_impl.cc` | Implementations of all three new APIs. Also added concurrency guards in `Write`, `Get`, and `NewIterator`, and stats interception inside `DoCompactionWork`. |
| `db/db_test.cc` | Stub overrides on the `ModelDB` test fixture so the existing test suite still compiles. The stubs return `Status::NotSupported` — `ModelDB` is not a real database. |

The CMake build configuration is **unchanged**. No external libraries or
dependencies were introduced.

## Building

Standard LevelDB build procedure:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 ..
cmake --build . -j
```

This produces `libleveldb.a` along with the test binaries.

## Running the Existing LevelDB Tests

From the `build` directory:

```bash
./leveldb_tests
```

These are the upstream LevelDB tests, unmodified. They should all pass — our
changes do not alter any existing behaviour.

## Sample Output

A representative run (insert ~100 keys, delete a contiguous middle range,
then `ForceFullCompaction`) produces output like:

```
============================================
   EXPLICIT FULL COMPACTION STATISTICS
============================================
-> Compaction Cycles Executed : 1
-> SSTable Inputs Processed   : 1
-> SSTable Outputs Generated  : 1
-> Total Data Read (Bytes)    : 243
-> Total Data Written (Bytes) : 173
============================================
```