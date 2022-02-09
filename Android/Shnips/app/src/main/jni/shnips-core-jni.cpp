// Copyright © 2018 Igor Pener. All rights reserved.

#include <jni.h>
#include <memory>
#include <cctype>

#include "AppDelegateBridge.hpp"
#include "GameController.hpp"

#include "alMain.h"
#include "Alc/apportable_openal_funcs.h"

ALC_API void ALC_APIENTRY alcSuspend(void) {
    if (apportableOpenALFuncs.alc_android_suspend) {
        apportableOpenALFuncs.alc_android_suspend();
    }
}

ALC_API void ALC_APIENTRY alcResume(void) {
    if (apportableOpenALFuncs.alc_android_resume) {
        apportableOpenALFuncs.alc_android_resume();
    }
}

extern "C" {
    JNIEXPORT JavaVM *alcGetJavaVM() {
        return AppDelegateBridge::jni.vm;
    }

    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
        if (apportableOpenALFuncs.alc_android_set_java_vm) {
            apportableOpenALFuncs.alc_android_set_java_vm(vm);
        }
        return AppDelegateBridge::jni.init(vm);
    }

    JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *) {
        AppDelegateBridge::jni = {};
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_init(JNIEnv *env, jclass instance, float width, float height, float scale_factor) {
        if (!AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller = new GameController(new AppDelegateBridge(width, height), scale_factor);
            AppDelegateBridge::game_controller->init();
            AppDelegateBridge::game_controller->appDidBecomeActive();
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_dealloc(JNIEnv *, jclass) {
        if (AppDelegateBridge::game_controller) {
            delete AppDelegateBridge::game_controller;
            AppDelegateBridge::game_controller = nullptr;
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_appDidBecomeActive(JNIEnv *, jclass) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->appDidBecomeActive();
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_appWillResignActive(JNIEnv *, jclass) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->appWillResignActive();
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_update(JNIEnv *, jclass, float t) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->update(t);
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_touchesBegan(JNIEnv *, jclass, double x, double y, long t_ms) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->touchesBegan(x, y, static_cast<long long>(t_ms));
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_touchesMoved(JNIEnv *, jclass, double x, double y, long t_ms) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->touchesMoved(x, y, static_cast<long long>(t_ms));
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_touchesEnded(JNIEnv *, jclass, double x, double y, long t_ms) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->touchesEnded(x, y, static_cast<long long>(t_ms));
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_didRetrieveRank(JNIEnv *, jclass, long long rank, short index) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->didRetrieveRank(static_cast<unsigned long long>(rank), static_cast<unsigned short>(index));
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_didRetrievePurchase(JNIEnv *env, jclass, jstring price, short type) {
        if (AppDelegateBridge::game_controller) {
            const char *str = env->GetStringUTFChars(price, false);
            AppDelegateBridge::game_controller->didRetrievePurchase(str, static_cast<in_app_purchase_type>(type));
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_didBuyInAppPurchase(JNIEnv *, jclass, short type) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->didBuyInAppPurchase(static_cast<in_app_purchase_type>(type));
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_didCancelInAppPurchase(JNIEnv *, jclass) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->didCancelInAppPurchase();
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_setAuthenticated(JNIEnv *, jclass, bool authenticated) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->setAuthenticated(authenticated);
        }
    }

    JNIEXPORT void JNICALL Java_shnips_MainActivity_didTapBackButton(JNIEnv *, jclass) {
        if (AppDelegateBridge::game_controller) {
            AppDelegateBridge::game_controller->didTapBackButton();
        }
    }
}
