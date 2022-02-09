// Copyright © 2018 Igor Pener. All rights reserved.

#include "json.hpp"

#include <iostream>
#include <stack>
#include "Logger.hpp"

namespace json {
    key::key(const std::string &str) : std::string(str) {
    }

    bool expect(char c, const char **s) {
        if (**s == c) {
            (*s)++;
            return true;
        }
        loge << "unexpected symbol found: " << *s << " (expected: " << c << ")" << std::endl;
        return false;
    }

    bool skip(const char **s) {
        if (**s == 0)
            return false;

        std::stack<char> stack;
        while (**s) {
            if (**s == '\'' || **s == '\"') {
                if (!stack.empty() && stack.top() == **s) {
                    stack.pop();
                } else {
                    stack.push(**s);
                }
            } else if (**s == ']') {
                if (!stack.empty() && stack.top() == '[') {
                    stack.pop();
                } else {
                    return false;
                }
            } else if (**s == '}') {
                if (!stack.empty() && stack.top() == '{') {
                    stack.pop();
                } else {
                    return stack.empty();
                }
            } else if (**s == '[' || **s == '{') {
                stack.push(**s);
            } else if (**s == ',' && stack.empty()) {
                return true;
            }
            (*s)++;
        }
        return stack.empty();
    }

    bool get(json::key &v, const char **s) {
        std::string str;
        if (!get(str, s))
            return false;

        v = json::key(str);
        return expect(':', s);
    }

    template<typename T>
    bool get(const char *key, T &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        auto it = key_value_tokens.find(key);
        return it != key_value_tokens.end() && get(v, &it->second);
    }

    bool get(bool &v, const char **s) {
        v = strtoul(*s, (char **)s, 10);
        return true;
    }

    bool get(unsigned char &v, const char **s) {
        v = static_cast<unsigned char>(**s);
        (*s)++;
        return true;
    }

    bool get(unsigned short &v, const char **s) {
        v = strtoul(*s, (char **)s, 10);
        return true;
    }

    bool get(unsigned long &v, const char **s) {
        v = strtoul(*s, (char **)s, 10);
        return true;
    }

    bool get(unsigned long long &v, const char **s) {
        v = strtoull(*s, (char **)s, 10);
        return true;
    }

    bool get(long long &v, const char **s) {
        v = strtoll(*s, (char **)s, 10);
        return true;
    }

    bool get(double &v, const char **s) {
        v = strtod(*s, (char **)s);
        return true;
    }

    bool get(float &v, const char **s) {
        v = strtof(*s, (char **)s);
        return true;
    }

    bool get(::game_type &v, const char **s) {
        v = static_cast<::game_type>(strtoul(*s, (char **)s, 10));
        return true;
    }

    bool get(::chip_type &v, const char **s) {
        v = static_cast<::chip_type>(strtoul(*s, (char **)s, 10));
        return true;
    }

    template<typename T>
    bool get(vec2<T> &v, const char **s) {
        return expect('[', s) && get(v.x, s) && expect(',', s) && get(v.y, s) && expect(']', s);
    }

    bool get(Chip &v, const char **s) {
        return expect('[', s) && get(v.p, s) && expect(',', s) && get(v.v, s) && expect(']', s);
    }

    bool get(std::string &v, const char **s) {
        if (!expect('\"', s))
            return false;

        while (**s && **s != '\"') {
            v += (**s);
            (*s)++;
        }
        return expect('\"', s);
    }

    bool get(base_state::shot &v, const char **s) {
        return decode(v, *s);
    }

    bool get(base_state &v, const char **s) {
        return decode(v, *s);
    }

    bool get(base_state::shot &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return (
            get(base_state::shot::key_passes, v.passes, key_value_tokens) &&
            get(base_state::shot::key_cushions_after_last_pass, v.cushions_after_last_pass, key_value_tokens) &&
            get(base_state::shot::key_max_cushions_before_pass, v.max_cushions_before_pass, key_value_tokens)
        );
    }

    bool get(base_state &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return (
            get(base_state::key_chips_data, v.chips_data, key_value_tokens) &&
            get(base_state::key_shot_data, v.shot_data, key_value_tokens) &&
            get(base_state::key_chip_type, v.chip_type, key_value_tokens) &&
            get(base_state::key_ability_time_left, v.ability_time_left, key_value_tokens) &&
            get(base_state::key_charge_level, v.charge_level, key_value_tokens) &&
            get(base_state::key_max_continued_game_count, v.max_continued_game_count, key_value_tokens) &&
            get(base_state::key_max_supercharge_count, v.max_supercharge_count, key_value_tokens) &&
            get(base_state::key_last_index, v.last_index, key_value_tokens) &&
            get(base_state::key_waiting_for_user, v.waiting_for_user, key_value_tokens)
        );
    }

    bool get(app_state::singleplayer &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return (
            get(app_state::singleplayer::key_score, v.score, key_value_tokens) &&
            get(app_state::singleplayer::key_highscore, v.highscore, key_value_tokens) &&
            get(app_state::singleplayer::key_progress, v.progress, key_value_tokens) &&
            get(app_state::singleplayer::key_supercharge_count, v.supercharge_count, key_value_tokens) &&
            get(app_state::singleplayer::key_game_type, v.game_type, key_value_tokens) &&
            get(app_state::singleplayer::key_goal, v.goal, key_value_tokens) &&
            get(app_state::singleplayer::key_continued_game_count, v.continued_game_count, key_value_tokens) &&
            get(app_state::singleplayer::key_charging_intro_seen, v.charging_intro_seen, key_value_tokens) &&
            get(app_state::singleplayer::key_swap_intro_seen, v.swap_intro_seen, key_value_tokens)
        );
    }

    bool get(app_state::store &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return get(app_state::store::key_progress, v.progress, key_value_tokens);
    }

    bool get(app_state::tutorial &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return get(app_state::tutorial::key_completed, v.completed, key_value_tokens);
    }

    bool get(app_state::achievements &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return get(app_state::achievements::key_progress, v.progress, key_value_tokens);
    }

    bool get(app_state &v, std::unordered_map<std::string, const char *> &key_value_tokens) {
        return (
            get(app_state::key_base_state, v.base_state, key_value_tokens) &&
            get(app_state::key_rate_app_prompts_count, v.rate_app_prompts_count, key_value_tokens) &&
            get(app_state::key_muted, v.muted, key_value_tokens) &&
            get(app_state::key_discard_base_state, v.discard_base_state, key_value_tokens) &&
            get(app_state::key_followed_on_twitter, v.followed_on_twitter, key_value_tokens)
        );
    }

    template<typename T>
    bool get(std::vector<T> &v, const char **s) {
        if (!expect('[', s))
            return false;

        while (**s != ']') {
            T data;
            if (!get(data, s))
                return false;

            v.push_back(data);
            if (**s != ']' && !expect(',', s))
                return false;
        }
        return expect(']', s);
    }

    template<typename T>
    bool get(T *arr, int len, const char **s) {
        if (!expect('[', s))
            return false;

        int pos = 0;
        while (**s != ']' && pos < len) {
            T data;
            if (!get(data, s))
                return false;

            arr[pos++] = data;
            if (**s != ']' && !expect(',', s))
                return false;
        }

        if (!expect(']', s))
            return false;

        return pos == len;
    }
}

std::ostream &operator<<(std::ostream &os, const json::key &v) {
    os << '\"' << v.c_str() << "\":";
    return os;
}

std::ostream &operator<<(std::ostream &os, game_type v) {
    os << static_cast<unsigned short>(v);
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::string &v) {
    os << '\"' << v.c_str() << '\"';
    return os;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const vec2<T> &v) {
    os << '[' << v.x << ',' << v.y << ']';
    return os;
}

std::ostream &operator<<(std::ostream &os, const Chip &v) {
    os << '[' << v.p << ',' << v.v << ']';
    return os;
}

std::ostream &operator<<(std::ostream &os, const base_state::shot &v) {
    os << '{';
    os << json::key(base_state::shot::key_passes) << v.passes << ',';
    os << json::key(base_state::shot::key_cushions_after_last_pass) << v.cushions_after_last_pass << ',';
    os << json::key(base_state::shot::key_max_cushions_before_pass) << v.max_cushions_before_pass << '}';
    return os;
}

std::ostream &operator<<(std::ostream &os, const base_state &v) {
    os << '{';
    os << json::key(base_state::key_chips_data) << v.chips_data << ',';
    os << json::key(base_state::key_shot_data) << v.shot_data << ',';
    os << json::key(base_state::key_chip_type) << v.chip_type << ',';
    os << json::key(base_state::key_ability_time_left) << v.ability_time_left << ',';
    os << json::key(base_state::key_charge_level) << v.charge_level << ',';
    os << json::key(base_state::key_max_continued_game_count) << v.max_continued_game_count << ',';
    os << json::key(base_state::key_max_supercharge_count) << v.max_supercharge_count << ',';
    os << json::key(base_state::key_last_index) << v.last_index << ',';
    os << json::key(base_state::key_waiting_for_user) << v.waiting_for_user << '}';
    return os;
}

std::ostream &operator<<(std::ostream &os, const app_state::singleplayer &v) {
    os << '{';
    os << json::key(app_state::singleplayer::key_score) << v.score << ',';
    os << json::key(app_state::singleplayer::key_highscore) << v.highscore << ',';
    os << json::key(app_state::singleplayer::key_progress) << v.progress << ',';
    os << json::key(app_state::singleplayer::key_supercharge_count) << v.supercharge_count << ',';
    os << json::key(app_state::singleplayer::key_game_type) << v.game_type << ',';
    os << json::key(app_state::singleplayer::key_goal) << v.goal << ',';
    os << json::key(app_state::singleplayer::key_continued_game_count) << v.continued_game_count << ',';
    os << json::key(app_state::singleplayer::key_charging_intro_seen) << v.charging_intro_seen << ',';
    os << json::key(app_state::singleplayer::key_swap_intro_seen) << v.swap_intro_seen << '}';
    return os;
}

std::ostream &operator<<(std::ostream &os, const app_state::store &v) {
    os << '{' << json::key(app_state::store::key_progress) << v.progress << '}';
    return os;
}

std::ostream &operator<<(std::ostream &os, const app_state::tutorial &v) {
    os << '{';
    os << json::key(app_state::tutorial::key_completed) << v.completed << '}';
    return os;
}

std::ostream &operator<<(std::ostream &os, const app_state::achievements &v) {
    os << '{';
    os << json::key(app_state::achievements::key_progress) << v.progress << '}';
    return os;
}

std::ostream &operator<<(std::ostream &os, const app_state &v) {
    os << '{';
    os << json::key(app_state::key_base_state) << v.base_state << ',';
    os << json::key(app_state::key_rate_app_prompts_count) << v.rate_app_prompts_count << ',';
    os << json::key(app_state::key_muted) << v.muted << ',';
    os << json::key(app_state::key_discard_base_state) << v.discard_base_state << ',';
    os << json::key(app_state::key_followed_on_twitter) << v.followed_on_twitter << '}';
    return os;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    os << '[';
    if (v.size()) {
        os << v[0];
        for (size_t i = 1; i < v.size(); ++i) {
            os << ',' << v[i];
        }
    }
    os << ']';
    return os;
}

template std::ostream &operator<<(std::ostream &os, const std::vector<Chip> &);
template std::ostream &operator<<(std::ostream &os, const std::vector<unsigned long> &);
template std::ostream &operator<<(std::ostream &os, const std::vector<unsigned short> &);
