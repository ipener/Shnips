// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef JSON_HPP
#define JSON_HPP

#include <sstream>
#include <string>
#include <unordered_map>

#include "app_state.hpp"

namespace json {
    struct key : std::string {
        key() = default;

        key(const std::string &str);
    };

    bool expect(char c, const char **s);

    bool skip(const char **s);

    bool get(json::key &v, const char **s);

    template<typename T>
    bool get(const char *key, T &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(bool &v, const char **s);

    bool get(unsigned char &v, const char **s);

    bool get(unsigned short &v, const char **s);

    bool get(unsigned long &v, const char **s);

    bool get(unsigned long long &v, const char **s);

    bool get(long long &v, const char **s);

    bool get(double &v, const char **s);

    bool get(float &v, const char **s);

    bool get(::game_type &v, const char **s);

    bool get(::chip_type &v, const char **s);

    template<typename T>
    bool get(vec2<T> &v, const char **s);

    bool get(Chip &v, const char **s);

    bool get(std::string &v, const char **s);

    bool get(base_state::shot &v, const char **s);

    bool get(base_state &v, const char **s);

    bool get(base_state::shot &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(base_state &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(app_state::singleplayer &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(app_state::store &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(app_state::tutorial &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(app_state::achievements &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    bool get(app_state &v, std::unordered_map<std::string, const char *> &key_value_tokens);

    template<typename T>
    bool get(std::vector<T> &v, const char **s);

    template<typename T>
    bool get(T *arr, int len, const char **s);
}

std::ostream &operator<<(std::ostream &os, const json::key &v);

std::ostream &operator<<(std::ostream &os, game_type v);

template<typename T>
std::ostream &operator<<(std::ostream &os, const vec2<T> &v);

std::ostream &operator<<(std::ostream &os, const Chip &v);

std::ostream &operator<<(std::ostream &os, const base_state::shot &v);

std::ostream &operator<<(std::ostream &os, const base_state &v);

std::ostream &operator<<(std::ostream &os, const app_state::singleplayer &v);

std::ostream &operator<<(std::ostream &os, const app_state::store &v);

std::ostream &operator<<(std::ostream &os, const app_state::tutorial &v);

std::ostream &operator<<(std::ostream &os, const app_state::achievements &v);

std::ostream &operator<<(std::ostream &os, const app_state &v);

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v);

namespace json {
    template<typename T>
    std::string encode(const T &data) {
        std::ostringstream os;
        os << data;
        return os.str();
    }

    template<typename T>
    bool decode(T &v, const char *s) {
        std::unordered_map<std::string, const char *> key_value_tokens;
        json::key k;
        if (*s != '{')
            return false;

        s++;
        while(*s != '\0') {
            if (!get(k, &s))
                return false;

            key_value_tokens[k] = s;
            if (!skip(&s))
                return false;

            if (*s == ',') {
                s++;
            } else if (*s == '}') {
                break;
            } else if (*s == '\0') {
                return false;
            }
        }
        return get(v, key_value_tokens);
    }
}

#endif
