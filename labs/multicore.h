#include <atomic>
#pragma once

//
// INVARIANT: w_deleted_count <= w_deleting_count <= w_cached_read_count <= shared_read_count <= r_reading_count <= r_cached_write_count <= shared_write_count <= w_writing_count <= w_deleted_count + MAX_SIZE
//
// INVARIANT:      w_writing_count      - w_deleted_count     <= MAX_SIZE
// =========>      w_writing_count      - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - shared_read_count   <= MAX_SIZE
//
//
// INVARIANT: 0 <= r_cached_write_count - r_reading_count
// =========> 0 <= r_cached_write_count - shared_read_count
// =========> 0 <= shared_write_count   - shared_read_count
//
//
// THEOREM: =========> 0 <= shared_write_count   - shared_read_count   <= MAX_SIZE
//

//
//
// Channel/Queue:
//
// Shared between Producer and Consumer
//
struct channel_t{
  
  public:
    std::atomic<int> shared_read_count;
    std::atomic<int> shared_write_count;
    std::atomic<bool> full;
    std::atomic<bool> empty;

  public:
    //
    // Intialize
    //
    channel_t(){
      shared_read_count = 0;
      shared_write_count = 0;
      full = false;
      empty = true;
    }
};


//
// Producer's (not shared)
//
struct writeport_t{
public:
    int max;
    int w_deleting_count;
    int w_cached_read_count;
    int w_writing_count;
    bool w_full;
    bool w_empty;

public:

  //
  // Intialize
  //
  writeport_t(size_t tsize)
  {
    max = tsize;
    w_deleting_count = 0;
    w_cached_read_count = 0;
    w_writing_count = 0;
    w_empty = true;
    w_full = false;
  }

public:

  //
  // no of entries available to write
  //
  // helper function for write_canreserve
  //
  size_t write_reservesize(){
    if(w_full)
      return 0;
    else if(w_empty)
      return max;
    else if(w_deleting_count < w_writing_count)
      return (max - (w_writing_count - w_deleting_count));
    else
      return (w_deleting_count - w_writing_count);
  }

  //
  // Can write 'n' entries?
  //
  bool write_canreserve(size_t n){
    if(write_reservesize() >= n)
      return true;
    else
      return false;
  }

  //
  // Reserve 'n' entries for write
  //
  size_t write_reserve(size_t n){
    if(w_empty)
      w_empty = false; 
    int tmp = w_writing_count;
    w_writing_count = (w_writing_count + n) % max;
    if(w_writing_count == w_deleting_count)
      w_full = true;
    return tmp;
  }

  //
  // Commit
  //
  // Read/Write shared memory data structure
  //
  void write_release(channel_t& ch){
    ch.shared_write_count = w_writing_count;
    ch.full = w_full;
    ch.empty = w_empty;

  }

public:
  //
  //
  // Read/Write shared memory data structure
  //
  void read_acquire(channel_t& ch){
    w_cached_read_count = ch.shared_read_count;
  }

  //
  // No of entires available to delete
  //
  size_t delete_reservesize(){
    if(w_empty)
      return 0;
    else if(w_full && (w_deleting_count == w_cached_read_count))
      return max;
    if(w_deleting_count <= w_cached_read_count)
      return (w_cached_read_count - w_deleting_count);
    else
      return (max - (w_deleting_count - w_cached_read_count));
  }

  //
  // Can delete 'n' entires?
  //
  bool delete_canreserve(size_t n){
    if(delete_reservesize() >= n)
      return true;
    else
      return false;
  }

  //
  // Reserve 'n' entires for deletion
  //
  size_t delete_reserve(size_t n){
    if(w_full)
      w_full = false;
    int tmp = w_deleting_count;
    w_deleting_count = (w_deleting_count + n) % max;
    if(w_deleting_count == w_writing_count)
      w_empty = true;
    return tmp;
  }


  //
  // Update the state, if any.
  //
  void delete_release(){
  }


};


//
// Consumer's (not shared)
//
//
struct readport_t{

public:
  int max;
  int r_reading_count;
  int r_cached_write_count;
  bool r_full;
  bool r_empty;

public:
  //
  // Initialize
  //
  readport_t(size_t tsize)
  {
    max = tsize;
    r_reading_count = 0;
    r_cached_write_count = 0;
    r_full = true;
    r_empty = false;
  }
  public:

  //
  // Read/Write shared memory data structure
  //
  void write_acquire(channel_t& ch){
    r_cached_write_count = ch.shared_write_count;
    r_full = ch.full;
    r_empty = ch.empty;

  }

  //
  // no of entries available to read
  //
  size_t read_reservesize(){
    if(r_empty)
      return 0;
    else if(r_full && (r_reading_count == r_cached_write_count))
      return max;
    if(r_reading_count <= r_cached_write_count)
      return (r_cached_write_count - r_reading_count);
    else
      return (max - (r_reading_count - r_cached_write_count));
  }

  //
  // Can Read 'n' entires?
  //
  bool read_canreserve(size_t n){
    if(read_reservesize() >= n)
      return true;
    else
      return false;
  }

  //
  // Reserve 'n' entires to be read
  //
  size_t read_reserve(size_t n){
    int tmp = r_reading_count;
    r_reading_count = (r_reading_count + n) % max;
    return tmp;
  }

  //
  // Read/write shared memory data structure
  //
  void read_release(channel_t& ch){
    ch.shared_read_count = r_reading_count;
  }

};