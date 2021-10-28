/**
 * @authors Szymon Łukasik, Olaf Placha
 */

#include "maptel.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <regex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

using namespace std;

namespace {

/** Holds telephone numbers mappings */
using dict_t = unordered_map<string, string>;

/**
 * Return id to dictionary mapping (created once only)
 * @return id to dictionary mapping
 */
unordered_map<size_t, dict_t> &dictionaries() {
  static unordered_map<size_t, dict_t> dictionaries =
      (unordered_map<size_t, dict_t>());
  return dictionaries;
}

/** Denotes currently greatest dictionary id number */
size_t max_id = -1;

/**
 * Return vector with available ids smaller then max_id
 * @return vector with ids
 */
vector<size_t> &free_ids() {
  static vector<size_t> free_ids = vector<size_t>();
  return free_ids;
}

/**
 * Checks if given id is a valid dictionary id
 * @param id: id to be validated
 * @return true iff given id is a valid dictionary id
 */
bool is_correct_id(unsigned long id) {
  return dictionaries().find(id) != dictionaries().end();
}

/**
 * Checks if given telephone number is valid
 * @param tel_num: telephone number
 * @return true iff given telephone number is valid
 */
bool is_correct_tel_num(const char *tel_num) {
  static regex tel_num_regex = regex("[0-9]{1,22}");
  if (tel_num == NULL)
    return false;
  return regex_match(tel_num, tel_num_regex);
}
} // namespace

namespace jnp1 {

/** Specifies maximum length of telephone number */
size_t TEL_NUM_MAX_LEN = 22;

unsigned long maptel_create(void) {
  if (debug)
    cerr << "maptel: maptel_create()\n";

  size_t new_id;
  if (free_ids().empty()) {
    max_id++;
    new_id = max_id;
  } else {
    new_id = free_ids().back();
    free_ids().pop_back();
  }

  dictionaries()[new_id] = unordered_map<string, string>();

  if (debug)
    cerr << "maptel: maptel_create: new map id = " << new_id << '\n';

  return new_id;
}

void maptel_delete(unsigned long id) {
  if (debug) {
    cerr << "maptel: maptel_delete(" << id << ")\n";
    assert(is_correct_id(id));
  }

  if (id == max_id)
    max_id--;
  else
    free_ids().push_back(id);

  dictionaries().erase(id);

  if (debug)
    cerr << "maptel: maptel_delete: map " << id << " deleted\n";
}

void maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst) {
  auto dict_it = dictionaries().find(id);
  if (debug) {
    cerr << "maptel: maptel_insert(" << id << ", " << tel_src << ", " << tel_dst
         << ")\n";
    assert(is_correct_tel_num(tel_src));
    assert(is_correct_tel_num(tel_dst));
    assert(dict_it != dictionaries().end());
  }

  dict_it->second[tel_src] = tel_dst;

  if (debug)
    cerr << "maptel: maptel_insert: inserted\n";
}

void maptel_erase(unsigned long id, char const *tel_src) {
  if (debug) {
    cerr << "maptel: maptel_erase(" << id << ", " << tel_src << ")\n";
    assert(is_correct_id(id));
    assert(is_correct_tel_num(tel_src));
  }

  dict_t &dict = dictionaries()[id];
  if (dict.find(tel_src) == dict.end()) {
    if (debug)
      cerr << "maptel: maptel_erase: nothing to erase\n";
    return;
  }

  dict.erase(tel_src);

  if (debug)
    cerr << "maptel: maptel_erase: erased\n";
}

void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst,
                      size_t len) {
  if (debug) {
    cerr << "maptel: maptel_transform(" << id << ", " << tel_src << ", "
         << (void *)tel_dst << ", " << len << ")\n";
    assert(tel_dst != NULL);
    assert(is_correct_id(id));
    assert(is_correct_tel_num(tel_src));
  }

  dict_t &dict = dictionaries().find(id)->second;
  string tel_current = tel_src;
  size_t src_len = tel_current.size();
  unordered_set<string> visited = unordered_set<string>();
  unordered_map<string, string>::iterator next_tel_it = dict.find(tel_src);
  while (next_tel_it != dict.end() &&
         visited.find(next_tel_it->second) == visited.end()) {
    tel_current = next_tel_it->second;
    visited.insert(tel_current);
    next_tel_it = dict.find(tel_current);
  }

  bool cycle_detected = next_tel_it != dict.end();
  if (cycle_detected)
    strncpy(tel_dst, tel_src, len);
  else
    strncpy(tel_dst, tel_current.c_str(), len);

  if (debug) {
    if (cycle_detected)
      cerr << "maptel: maptel_transform: cycle detected\n";
    cerr << "maptel: maptel_transform: " << tel_src << " -> " << tel_dst
         << '\n';
    assert(tel_dst != NULL &&
           len >= (cycle_detected ? src_len : tel_current.size()) + 1);
  }
}
} // namespace jnp1
