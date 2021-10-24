#include"maptel.h"
#include<stdlib.h>
#include<string>
#include<cassert>
#include<cstring>
#include<iostream>
#include<unordered_map>
#include<set>

#ifndef NDEBUG
    const bool debug = true;
#else
    const bool debug = false;
#endif

using namespace std;

size_t TEL_NUM_MAX_LEN = 22;

using dict_t = unordered_map<string, string>;

static unordered_map<size_t, dict_t> dictionaries = (
    unordered_map<size_t, dict_t>()
);

static size_t max_id = -1;

set<size_t> free_ids = set<size_t>();

static bool is_correct_id(unsigned long id) {
    return dictionaries.find(id) != dictionaries.end();
}

static bool is_correct_tel_num(const char * tel_num) {
    size_t length = strlen(tel_num);
    return tel_num != NULL &&
           length <= TEL_NUM_MAX_LEN &&
           tel_num[length] == '\0';
}

unsigned long maptel_create(void) {
    if (debug) 
        cerr << "maptel: maptel_create()\n";
    
    size_t new_id;
    if (free_ids.empty()) {
        max_id++;
        new_id = max_id;
    }
    else {
        new_id = *free_ids.begin();
        free_ids.erase(new_id);
    }

    dictionaries[new_id] = unordered_map<string, string>();

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
        free_ids.insert(id);

    dictionaries.erase(id);

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

    dictionaries[id][tel_src] = string(tel_dst);

    if (debug)
        cerr << "maptel: maptel_insert: inserted\n";
}

void maptel_erase(unsigned long id, char const *tel_src) {
    if (debug) {
        cerr << "maptel: maptel_erase(" << id << ", " << tel_src << ")\n";
        assert(is_correct_id(id));
        assert(is_correct_tel_num(tel_src));
    }
    
    dict_t &dict = dictionaries[id];
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
        assert(tel_dst != NULL && len >= strlen(tel_src));
    }

    char * tel_current = (char *) tel_src;
    dict_t &dict = dictionaries.find(id)->second;
    unordered_map<string, string>::iterator tel_it = dict.find(tel_src);
    bool tel_changed = tel_it != dict.end();
    while (tel_it != dict.end() && 
        strcmp(tel_it->second.c_str(), tel_src) != 0) {
        tel_current = (char *) tel_it->second.c_str();
        tel_it = dict.find(tel_it->second);
    }

    strncpy(tel_dst, tel_current, len);
    if (debug) {
        if(strcmp(tel_dst, tel_src) == 0 && tel_changed)
            cerr << "maptel: maptel_transform: cycle detected\n";
        cerr << "maptel: maptel_transform: " << tel_src << " -> " << tel_dst << '\n';
    }
}

