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
 * Returns id to dictionary mapping (created once only)
 * @return id to dictionary mapping
 */
unordered_map<size_t, dict_t> &dictionaries() {
  static unordered_map<size_t, dict_t> dictionaries =
    unordered_map<size_t, dict_t>();
  return dictionaries;
}

/** Denotes currently greatest dictionary id number */
size_t max_id = -1;

/**
 * Returns vector with available ids smaller then max_id
 * @return vector with ids
 */
vector<size_t> &free_ids() {
  static vector<size_t> free_ids = vector<size_t>();
  return free_ids;
}

/**
 * Checks if given telephone number is valid
 * @param tel_num: telephone number
 * @return whether given telephone number is valid
 */
bool is_correct_tel_num(const char *tel_num) {
  static regex tel_num_regex = regex("[0-9]{1,22}");
  if (tel_num == NULL)
    return false;
  return regex_match(tel_num, tel_num_regex);
}

/**
 * Helper function for maptel_transform - finds final transform and 
 * detects cycle.
 * @param id: id of dictionary
 * @param tel_src: source telephone number
 * @param loop_detected: placeholder for setting flag
 * @return transformed telephone number
 */
string find_final_transformation(const dict_t &dict, const string &tel_src,
                                 bool &cycle_detected) {
  string tel_current = tel_src;
  unordered_set<string> visited = unordered_set<string>();
  auto next_tel_it = dict.find(tel_src);

  while (next_tel_it != dict.end() &&
         visited.find(next_tel_it->second) == visited.end()) {
    tel_current = next_tel_it->second;
    visited.insert(tel_current);
    next_tel_it = dict.find(tel_current);
  }

  cycle_detected = next_tel_it != dict.end();
  return tel_current;
}

/**
 * Helper function for maptel_transform - saves final transform into tel_dst. 
 * @param tel_dst: points to save destination
 * @param len: size of dedicated memory
 * @param tel_src_str: source telephone number
 * @param tel_final: final telephone number
 * @param cycle_detected: whether cycle was detected 
 */
void save_final_transformation(char *tel_dst, size_t len,
                               const string &tel_src_str,
                               const string &tel_final, bool cycle_detected) {
  string str_to_save = cycle_detected ? tel_src_str : tel_final;
  if (debug) {
    if (cycle_detected)
      cerr << "maptel: maptel_transform: cycle detected\n";
    assert(tel_dst != NULL && len >= str_to_save.size() + 1);
  }

  strncpy(tel_dst, str_to_save.c_str(), len);
  
  if (debug)
    cerr << "maptel: maptel_transform: " << tel_src_str << " -> " << tel_dst<< '\n';
}

}

namespace jnp1 {

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
  auto iter_id = dictionaries().find(id);
  auto iter_end = dictionaries().end();
  if (debug) {
    cerr << "maptel: maptel_delete(" << id << ")\n";
    assert(iter_id != iter_end);
  }

  if (id == max_id)
    max_id--;
  else
    free_ids().push_back(id);

  dictionaries().erase(id);

  if (debug)
    cerr << "maptel: maptel_delete: map " << id << " deleted\n";
}

void maptel_insert(unsigned long id, char const *tel_src,
                   char const *tel_dst) {
  auto dict_it = dictionaries().find(id);
  if (debug) {
    cerr << "maptel: maptel_insert(" 
         << id << ", " << tel_src << ", " << tel_dst << ")\n";
    assert(is_correct_tel_num(tel_src));
    assert(is_correct_tel_num(tel_dst));
    assert(dict_it != dictionaries().end());
  }

  dict_it->second[tel_src] = tel_dst;

  if (debug)
    cerr << "maptel: maptel_insert: inserted\n";
}

void maptel_erase(unsigned long id, char const *tel_src) {
  auto iter_id = dictionaries().find(id);
  auto iter_end = dictionaries().end();
  if (debug) {
    cerr << "maptel: maptel_erase(" << id << ", " << tel_src << ")\n";
    assert(iter_id != iter_end);
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
  auto iter_id = dictionaries().find(id);
  auto iter_end = dictionaries().end();
  if (debug) {
    cerr << "maptel: maptel_transform(" << id << ", " << tel_src << ", "
         << (void *)tel_dst << ", " << len << ")\n";
    assert(tel_dst != NULL);
    assert(iter_id != iter_end);
    assert(is_correct_tel_num(tel_src));
  }

  const dict_t &dict = iter_id->second;
  const string tel_src_str = tel_src;
  bool cycle_detected;
  string tel_final = 
    find_final_transformation(dict, tel_src_str, cycle_detected);
  save_final_transformation(tel_dst, len, tel_src_str, tel_final,
                            cycle_detected);
}
}
