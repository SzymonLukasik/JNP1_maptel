#include"maptel.h"
#include<stdlib.h>
#include<string>
#include<cassert>
#include<cstring>
#include<iostream>
#include<unordered_map>
#include<regex>

#ifndef NDEBUG
    const bool debug = true;
#else
    const bool debug = false;
#endif

using namespace std;

namespace jnp1 {

size_t TEL_NUM_MAX_LEN = 22;

using dict_t = unordered_map<string, string>;

static unordered_map<size_t, dict_t>& dictionaries() {
    static unordered_map<size_t, dict_t> dictionaries = (
    unordered_map<size_t, dict_t>()
    );
    return dictionaries;
}

static unordered_map<string, unsigned long long>& colors() {
    static unordered_map<string, unsigned long long> colors = (
        unordered_map<string, unsigned long long>()
    );
    return colors;
}

static unsigned long long COLOR = 0;

static bool tel_visited(string tel) {
    unordered_map<string, unsigned long long>::iterator color_it = colors().find(tel);
    return  color_it != colors().end() && color_it->second == COLOR;
}


static size_t max_id = -1;

static vector<size_t>& free_ids() {
    static vector<size_t> free_ids = vector<size_t>();
    return free_ids;
}

static bool is_correct_id(unsigned long id) {
    return dictionaries().find(id) != dictionaries().end();
}

static bool is_correct_tel_num(const char * tel_num) {
    static regex tel_num_regex = regex("[0-9]{0,22}");
    if (tel_num == NULL)
        return false;
    return regex_match(tel_num, tel_num_regex);
}

unsigned long maptel_create(void) {
    if (debug) 
        cerr << "maptel: maptel_create()\n";
    
    size_t new_id;
    if (free_ids().empty()) {
        max_id++;
        new_id = max_id;
    }
    else {
        new_id = free_ids().back();
        free_ids().pop_back();
    }

    dictionaries()[new_id] = unordered_map<string, string>();

    if (debug)
        cerr << "maptel: maptel_create: new map id = " 
             << new_id << '\n';

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

void maptel_insert(unsigned long id, char const *tel_src, 
                   char const *tel_dst) {
    if (debug) {
        cerr << "maptel: maptel_insert("
             << id << ", " << tel_src << ", " << tel_dst << ")\n";
        assert(is_correct_tel_num(tel_src));
        assert(is_correct_tel_num(tel_dst));
        assert(is_correct_id(id));
    }

    dictionaries()[id][tel_src] = string(tel_dst);

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
        cerr << "maptel: maptel_transform(" 
             << id << ", " << tel_src << ", " 
             << (void *) tel_dst << ", " << len << ")\n";
        assert(is_correct_id(id));
        assert(is_correct_tel_num(tel_src));
        assert(tel_dst != NULL);
    }

    dict_t &dict = dictionaries().find(id)->second;
    string tel_current = tel_src;
    unordered_map<string, string>::iterator next_tel_it = dict.find(tel_src);
    colors()[tel_current] = COLOR;
    while (next_tel_it != dict.end() && !tel_visited(next_tel_it->second)) {
        tel_current = next_tel_it->second;
        colors()[tel_current] = COLOR;
        next_tel_it = dict.find(tel_current);
    }

    bool cycle_detected = next_tel_it != dict.end();
    if (cycle_detected)
        strncpy(tel_dst, tel_src, len);
    else
        strncpy(tel_dst, tel_current.c_str(), len);
    COLOR++;

    if (debug) {
        if(cycle_detected)
            cerr << "maptel: maptel_transform: cycle detected\n";
        cerr << "maptel: maptel_transform: " << tel_src << " -> " << tel_dst << '\n';
        assert(tel_dst != NULL 
               && len >= strlen(cycle_detected ? tel_src : tel_current.c_str()) + 1);
    }
}

}