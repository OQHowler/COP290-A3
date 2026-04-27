// MY CODE-----------------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cassert>
#include <thread>
#include "leveldb/db.h"

// Quick helper to print our vector results clearly
void PrintScanResults(const std::string& test_name, 
                      const std::vector<std::pair<std::string, std::string>>& results) {
    std::cout << "\n[" << test_name << "] Results:\n";
    if (results.empty()) {
        std::cout << "  (Empty / No keys found in range)\n";
    }
    for (const auto& pair : results) {
        std::cout << "  Key: " << pair.first << " | Value: " << pair.second << "\n";
    }
}

int main() {
    std::cout << "======================================\n";
    std::cout << "   COP290 ADVANCED API TEST SUITE\n";
    std::cout << "======================================\n";

    // 1. Setup: Clean up any old databases and boot up a fresh one
    std::string db_path = "/tmp/cop290_test_db";
    system(("rm -rf " + db_path).c_str()); 

    leveldb::DB* my_database;
    leveldb::Options options;
    options.create_if_missing = true;
    
    leveldb::Status s = leveldb::DB::Open(options, db_path, &my_database);
    if (!s.ok()) {
        std::cerr << "CRITICAL ERROR: Failed to open database: " << s.ToString() << "\n";
        return 1;
    }

    leveldb::WriteOptions write_opts;
    leveldb::ReadOptions read_opts;

    // 2. Data Insertion
    std::cout << "\n[*] Populating database with baseline data...\n";
    my_database->Put(write_opts, "A_apple", "red");
    my_database->Put(write_opts, "B_banana", "yellow");
    my_database->Put(write_opts, "C_cherry", "red");
    my_database->Put(write_opts, "D_date", "brown");
    my_database->Put(write_opts, "E_elderberry", "purple");


    // ---------------------------------------------------------
    // TEST 1: STANDARD RANGE SCAN
    // ---------------------------------------------------------
    std::vector<std::pair<std::string, std::string>> scan_results;
    s = my_database->Scan(read_opts, "B_banana", "D_date", &scan_results);
    PrintScanResults("TEST 1: Standard Range Scan [B, D)", scan_results);
    
    assert(scan_results.size() == 2);
    assert(scan_results[0].first == "B_banana");
    assert(scan_results[1].first == "C_cherry");
    std::cout << "--> TEST 1 PASSED!\n";


    // ---------------------------------------------------------
    // TEST 2: REVERSED RANGE EDGE CASE (PIAZZA REQUIREMENT)
    // ---------------------------------------------------------
    std::cout << "\n[*] Testing Reversed Boundaries (Start > End)...\n";
    scan_results.clear();
    
    // Start is E, End is A. Should safely return empty without crashing.
    s = my_database->Scan(read_opts, "E_elderberry", "A_apple", &scan_results);
    assert(scan_results.empty());
    std::cout << "--> TEST 2 PASSED! (Safely handled reversed range)\n";


    // ---------------------------------------------------------
    // TEST 3: SNAPSHOT ISOLATION (PIAZZA REQUIREMENT)
    // ---------------------------------------------------------
    std::cout << "\n[*] Testing Snapshot Read Consistency...\n";
    
    // 3a. Take a snapshot of the current state
    const leveldb::Snapshot* historic_snapshot = my_database->GetSnapshot();
    
    // 3b. Mutate the database AFTER the snapshot
    my_database->Put(write_opts, "B_banana", "GREEN_NOW");
    my_database->Put(write_opts, "F_fig", "green");
    
    // 3c. Scan using the historic snapshot
    leveldb::ReadOptions snapshot_read_opts;
    snapshot_read_opts.snapshot = historic_snapshot;
    scan_results.clear();
    
    s = my_database->Scan(snapshot_read_opts, "A_apple", "Z_zebra", &scan_results);
    PrintScanResults("TEST 3: Snapshot Scan (Should ignore recent mutations)", scan_results);
    
    // Assert that "B" is still "yellow" and "F" does not exist in this read
    assert(scan_results[1].first == "B_banana" && scan_results[1].second == "yellow");
    assert(scan_results.size() == 5); // A, B, C, D, E
    std::cout << "--> TEST 3 PASSED! (Snapshot consistency maintained)\n";
    
    // Release snapshot
    my_database->ReleaseSnapshot(historic_snapshot);


    // ---------------------------------------------------------
    // TEST 4: STANDARD RANGE DELETE
    // ---------------------------------------------------------
    std::cout << "\n[*] Executing Range Delete for [B, D)...\n";
    s = my_database->DeleteRange(write_opts, "B_banana", "D_date");
    assert(s.ok());

    scan_results.clear();
    my_database->Scan(read_opts, "A", "Z", &scan_results);
    PrintScanResults("TEST 4: Database State Post-Deletion", scan_results);
    
    // A, D, E, F should be the only ones left
    assert(scan_results.size() == 4);
    assert(scan_results[0].first == "A_apple");
    assert(scan_results[1].first == "D_date");
    std::cout << "--> TEST 4 PASSED!\n";


    // ---------------------------------------------------------
    // TEST 5: GHOST RANGE DELETE EDGE CASE
    // ---------------------------------------------------------
    std::cout << "\n[*] Executing Range Delete for Non-Existent Keys [X, Z)...\n";
    // Deleting a range where no keys exist should succeed silently without mutating data
    s = my_database->DeleteRange(write_opts, "X_xylophone", "Z_zebra");
    assert(s.ok());
    std::cout << "--> TEST 5 PASSED!\n";


    // ---------------------------------------------------------
    // TEST 6: MANUAL FULL COMPACTION & BASIC THREADING
    // ---------------------------------------------------------
    std::cout << "\n[*] Triggering ForceFullCompaction with concurrent read attempt...\n";
    
    // We launch a background thread that tries to read while compaction is running
    // If your concurrency traps work, this thread will safely wait until compaction finishes!
    std::thread background_reader([&]() {
        std::string val;
        my_database->Get(read_opts, "A_apple", &val);
    });

    // Run the manual compaction
    s = my_database->ForceFullCompaction();
    assert(s.ok());
    
    // Rejoin the reader thread
    background_reader.join();

    std::cout << "--> TEST 6 PASSED! (Compaction finished and threads synchronized)\n";

    // Clean up memory
    delete my_database;
    std::cout << "\n======================================\n";
    std::cout << " ALL ADVANCED TESTS COMPLETED SUCCESSFULLY!\n";
    std::cout << "======================================\n";

    return 0;
}
// MY CODE-----------------------------------------------------------------------------------------------