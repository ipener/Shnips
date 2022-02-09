// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IRESOURCELOADER_HPP
#define IRESOURCELOADER_HPP

#include <codecvt>
#include <locale>
#include <sstream>

#include "chip_type.h"
#include "intro_type.h"
#include "notification_type.h"
#include "skill_shot_type.h"
#include "tutorial_type.h"
#include "video_type.h"

#define SET_KEY(array, key) array[key] = #key

class IStringLoader {
public:
    IStringLoader() {
        SET_KEY(_chip_type_keys, chip_type_default);
        SET_KEY(_chip_type_keys, chip_type_fast);
        SET_KEY(_chip_type_keys, chip_type_extra_points);
        SET_KEY(_chip_type_keys, chip_type_swap);
        SET_KEY(_chip_type_keys, chip_type_extra_chip);

        SET_KEY(_intro_type_keys, intro_type_charging);
        SET_KEY(_intro_type_keys, intro_type_swap);

        SET_KEY(_notification_type_keys, notification_type_goals_left);
        SET_KEY(_notification_type_keys, notification_type_goal);
        SET_KEY(_notification_type_keys, notification_type_highscore);

        SET_KEY(_tutorial_type_keys, tutorial_type_simple_shot);
        SET_KEY(_tutorial_type_keys, tutorial_type_consecutive_shot);
        SET_KEY(_tutorial_type_keys, tutorial_type_cushion_shot);
        SET_KEY(_tutorial_type_keys, tutorial_type_corner_shot);

        SET_KEY(_video_type_keys, skill_shot_type_indirect_pass);
        SET_KEY(_video_type_keys, skill_shot_type_just_passed);
        SET_KEY(_video_type_keys, skill_shot_type_narrow_gap);
        SET_KEY(_video_type_keys, skill_shot_type_three_cushion_hit);
        SET_KEY(_video_type_keys, skill_shot_type_four_passes);
        SET_KEY(_video_type_keys, skill_shot_type_chips_close);
        SET_KEY(_video_type_keys, skill_shot_type_two_passes_no_cushion);
        SET_KEY(_video_type_keys, skill_shot_type_circumvent_chip);
        SET_KEY(_video_type_keys, skill_shot_type_hit_chips_before_pass);
        SET_KEY(_video_type_keys, video_type_no_cushion_shot);
        SET_KEY(_video_type_keys, video_type_double_cushion_shot);
        SET_KEY(_video_type_keys, video_type_double_pass);
        SET_KEY(_video_type_keys, video_type_triple_pass);
    }

    virtual std::wstring localizedString(const char *key) = 0;

    template<typename ... Args>
    std::wstring localizedString(const char *key, Args ... args) {
        const auto str = localizedString(key);
        std::wostringstream os;
        tprintf(os, str.c_str(), args...);
        return os.str();
    }

    std::string str(const std::wstring &wstr) {
        return converter.to_bytes(wstr);
    }

    std::wstring wstr(const char *str) {
        return converter.from_bytes(str);
    }

    std::string clean_str(const std::wstring &wstr) {
        std::stringstream ss;
        for (const auto c : str(wstr)) {
            if (c != '*') {
                ss << c;
            }
        }
        return ss.str();
    }

    const std::string &key(chip_type type) const {
        return type < chip_type_range ? _chip_type_keys[type] : _empty_string;
    }

    const std::string &key(intro_type type) const {
        return type < intro_type_range ? _intro_type_keys[type] : _empty_string;
    }

    const std::string &key(notification_type type) const {
        return type < notification_type_range ? _notification_type_keys[type] : _empty_string;
    }

    const std::string &key(skill_shot_type type) const {
        return type < skill_shot_type_range ? _video_type_keys[static_cast<skill_shot_type>(type)] : _empty_string;
    }

    const std::string &key(tutorial_type type) const {
        return type < tutorial_type_range ? _tutorial_type_keys[type] : _empty_string;
    }

    const std::string &key(video_type type) const {
        return type < video_type_range ? _video_type_keys[type] : _empty_string;
    }

protected:
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

private:
    void tprintf(std::wostringstream &os, const wchar_t *format) {
        os << format;
    }

    template<typename Arg, typename... Args>
    void tprintf(std::wostringstream &os, const wchar_t *format, Arg arg, Args... args) {
        for (; *format != L'\0'; ++format) {
            if (*format == L'%' && *++format == L'_') {
                os << arg;
                return tprintf(os, format + 1, args...);
            }
            os << *format;
        }
    }

    std::string _chip_type_keys[chip_type_range];
    std::string _intro_type_keys[intro_type_range];
    std::string _notification_type_keys[notification_type_range];
    std::string _tutorial_type_keys[tutorial_type_range];
    std::string _video_type_keys[video_type_range];
    std::string _empty_string = "";
};

class IResourceLoader : public IStringLoader {
public:
    virtual ~IResourceLoader() {
    }

    virtual void loadImage(void **data, int *width, int *height, const char *filename) = 0;

    virtual void loadAudio(void **data, int *size, const char *filename) = 0;
};

#undef SET_KEY

#endif
