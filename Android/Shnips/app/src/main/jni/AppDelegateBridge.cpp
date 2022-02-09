// Copyright © 2018 Igor Pener. All rights reserved.

#include "AppDelegateBridge.hpp"

#include <android/bitmap.h>
#include <GLES3/gl3.h>
#include "AL/al.h"
#include "AL/alc.h"

#include "GameController.hpp"
#include "Logger.hpp"

AppDelegateBridge::JNI AppDelegateBridge::jni;
GameController *AppDelegateBridge::game_controller = nullptr;

AppDelegateBridge::AppDelegateBridge(float width, float height) : IAppDelegate(width, height) {
}

AppDelegateBridge::JNI::~JNI() {
    if (auto env = getEnv()) {
        env->DeleteGlobalRef(obj);
    }
}

jint AppDelegateBridge::JNI::init(JavaVM *vm) {
    this->vm = vm;

    if (auto env = getEnv()) {
        obj = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("shnips/MainActivity")));

#define GET_METHOD_ID(M, S) M = env->GetStaticMethodID(obj, #M, S)
        GET_METHOD_ID(reset, "()V");
        GET_METHOD_ID(save, "(Ljava/lang/String;Ljava/lang/String;)V");
        GET_METHOD_ID(load, "(Ljava/lang/String;)Ljava/lang/String;");
        GET_METHOD_ID(localizedString, "(Ljava/lang/String;)Ljava/lang/String;");
        GET_METHOD_ID(loadImage, "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        GET_METHOD_ID(loadAudio, "(Ljava/lang/String;)[B");
        GET_METHOD_ID(setPaused, "(Z)V");
        GET_METHOD_ID(setAchievementsProgress, "([F)V");
        GET_METHOD_ID(setLeaderboardData, "(J)V");
        GET_METHOD_ID(getLeaderboardData, "()V");
        GET_METHOD_ID(showAppRatingView, "()V");
        GET_METHOD_ID(showTwitterWebsite, "()V");
        GET_METHOD_ID(showAchievements, "()V");
        GET_METHOD_ID(showLeaderboard, "()V");
        GET_METHOD_ID(authenticatedForLeaderboard, "()Z");
        GET_METHOD_ID(startPurchase, "(S)V");
        GET_METHOD_ID(share, "(Ljava/lang/String;)V");
        GET_METHOD_ID(scheduleNotifications, "([Ljava/lang/String;)V");
        GET_METHOD_ID(cancelNotifications, "()V");
        GET_METHOD_ID(showQuitAppAlert, "()V");
        GET_METHOD_ID(setUpInAppBilling, "()V");
#undef GET_METHOD_ID

        return JNI_VERSION_1_6;
    } else {
        return -1;
    }
}

JNIEnv *AppDelegateBridge::JNI::getEnv() {
    JNIEnv *env;
    return vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK ? env : nullptr;
}

void AppDelegateBridge::reset() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.reset) {
        env->CallStaticVoidMethod(jni.obj, jni.reset);
    }
}

void AppDelegateBridge::save(const std::string &data, const std::string &key) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.save) {
        const auto jkey = env->NewStringUTF(key.c_str());
        const auto jdata = env->NewStringUTF(data.c_str());

        env->CallStaticVoidMethod(jni.obj, jni.save, jdata, jkey);
        env->DeleteLocalRef(jdata);
        env->DeleteLocalRef(jkey);
    }
}

bool AppDelegateBridge::load(std::string &data, const std::string &key) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.load) {
        const auto jkey = env->NewStringUTF(key.c_str());
        const auto jdata = static_cast<jstring>(env->CallStaticObjectMethod(jni.obj, jni.load, jkey));
        env->DeleteLocalRef(jkey);

        if (jdata) {
            const char *state = env->GetStringUTFChars(jdata, 0);
            data = std::string(state);
            env->ReleaseStringUTFChars(jdata, state);
            env->DeleteLocalRef(jdata);
            return true;
        }
    }
    return false;
}

std::wstring AppDelegateBridge::localizedString(const char *key) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.localizedString) {
        const auto jkey = env->NewStringUTF(key);
        const auto jdata = static_cast<jstring>(env->CallStaticObjectMethod(jni.obj, jni.localizedString, jkey));
        env->DeleteLocalRef(jkey);

        if (jdata) {
            const auto str = env->GetStringUTFChars(jdata, 0);
            const auto wstr = converter.from_bytes(str);
            env->ReleaseStringUTFChars(jdata, str);
            env->DeleteLocalRef(jdata);
            return wstr;
        }
    }
    return L"";
}

void AppDelegateBridge::loadImage(void **data, int *width, int *height, const char *filename) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.loadImage) {
        const auto jstr = env->NewStringUTF((std::string(filename) + ".png").c_str());
        const auto bitmap = env->CallStaticObjectMethod(jni.obj, jni.loadImage, jstr);
        env->DeleteLocalRef(jstr);
        if (bitmap == nullptr) {
            loge << "Could no load texture: " << jstr << std::endl;
            return;
        }
        AndroidBitmapInfo info;
        if (AndroidBitmap_getInfo(env, bitmap, &info) >= 0) {
            const auto bitmap_size = info.width * info.height * 4;
            *width = static_cast<GLsizei>(info.width);
            *height = static_cast<GLsizei>(info.height);
            *data = malloc(bitmap_size);
            void *bitmap_data;
            if (AndroidBitmap_lockPixels(env, bitmap, &bitmap_data) >= 0) {
                memcpy(*data, bitmap_data, bitmap_size);
                AndroidBitmap_unlockPixels(env, bitmap);
            }
        }
    }
}

void AppDelegateBridge::loadAudio(void **data, int *bytes, const char *filename) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.loadAudio) {
        const auto jstr = env->NewStringUTF(("sounds/" + std::string(filename) + ".sound").c_str());
        auto audio = static_cast<jbyteArray>(env->CallStaticObjectMethod(jni.obj, jni.loadAudio, jstr));
        env->DeleteLocalRef(jstr);

        if (audio == nullptr) {
            loge << "Could not load audio: " << std::string(filename) << std::endl;
            return;
        }
        const auto size = env->GetArrayLength(audio);
        *data = malloc(static_cast<size_t>(size));
        *bytes = static_cast<ALsizei>(size);
        env->GetByteArrayRegion(audio, 0, size, static_cast<jbyte *>(*data));
    }
}

void AppDelegateBridge::setPaused(bool paused) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.setPaused) {
        env->CallStaticVoidMethod(jni.obj, jni.setPaused, static_cast<jboolean>(paused));
    }
}

void AppDelegateBridge::setAchievementsProgress(const float progress[achievement_type_range]) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.setAchievementsProgress) {
        if (auto jprogress = env->NewFloatArray(achievement_type_range)) {
            env->SetFloatArrayRegion(jprogress, 0, achievement_type_range, progress);
            env->CallStaticVoidMethod(jni.obj, jni.setAchievementsProgress, jprogress);
            env->DeleteLocalRef(jprogress);
        }
    }
}

void AppDelegateBridge::setLeaderboardData(unsigned long long score) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.setLeaderboardData) {
        env->CallStaticVoidMethod(jni.obj, jni.setLeaderboardData, static_cast<jlong>(score));
    }
}

void AppDelegateBridge::getLeaderboardData() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.getLeaderboardData) {
        env->CallStaticVoidMethod(jni.obj, jni.getLeaderboardData);
    }
}

void AppDelegateBridge::showAppRatingView() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.showAppRatingView) {
        env->CallStaticVoidMethod(jni.obj, jni.showAppRatingView);
    }
}

void AppDelegateBridge::showTwitterWebsite() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.showTwitterWebsite) {
        env->CallStaticVoidMethod(jni.obj, jni.showTwitterWebsite);
    }
}

void AppDelegateBridge::showAchievements() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.showAchievements) {
        env->CallStaticVoidMethod(jni.obj, jni.showAchievements);
    }
}

void AppDelegateBridge::showLeaderboard() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.showLeaderboard) {
        env->CallStaticVoidMethod(jni.obj, jni.showLeaderboard);
    }
}

bool AppDelegateBridge::authenticatedForLeaderboard() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.authenticatedForLeaderboard) {
        return static_cast<bool>(env->CallStaticBooleanMethod(jni.obj, jni.authenticatedForLeaderboard));
    }
    return false;
}

void AppDelegateBridge::startPurchase(in_app_purchase_type type) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.startPurchase) {
        env->CallStaticVoidMethod(jni.obj, jni.startPurchase, static_cast<jshort>(type));
    }
}

void AppDelegateBridge::share(const char *str) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.share && str) {
        const auto jstr = env->NewStringUTF(str);
        env->CallStaticVoidMethod(jni.obj, jni.share, jstr);
        env->DeleteLocalRef(jstr);
    }
}

void AppDelegateBridge::scheduleNotifications(const char *titles[notification_type_range]) {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.scheduleNotifications) {
        const auto element_class = env->FindClass("java/lang/String");
        auto jtitles = env->NewObjectArray(notification_type_range, element_class, NULL);

        for (unsigned short i = 0; i < notification_type_range; ++i) {
            jstring jstr = env->NewStringUTF(titles[i]);
            env->SetObjectArrayElement(jtitles, i, jstr);
        }
        env->CallStaticVoidMethod(jni.obj, jni.scheduleNotifications, jtitles);

        for (unsigned short i = 0; i < notification_type_range; ++i) {
            env->DeleteLocalRef(env->GetObjectArrayElement(jtitles, i));
        }
        env->DeleteLocalRef(jtitles);
    }
}

void AppDelegateBridge::cancelNotifications() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.cancelNotifications) {
        env->CallStaticVoidMethod(jni.obj, jni.cancelNotifications);
    }
}

void AppDelegateBridge::showQuitAppAlert() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.showQuitAppAlert) {
        env->CallStaticVoidMethod(jni.obj, jni.showQuitAppAlert);
    }
}

void AppDelegateBridge::setUpInAppBilling() {
    auto env = AppDelegateBridge::jni.getEnv();
    if (env && jni.setUpInAppBilling) {
        env->CallStaticVoidMethod(jni.obj, jni.setUpInAppBilling);
    }
}
