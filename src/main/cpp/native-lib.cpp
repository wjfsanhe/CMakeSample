#include <jni.h>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/input.h>
#include <android/log.h>

struct DIR {
    int fd_;
    size_t available_bytes_;
    dirent* next_;
    pthread_mutex_t mutex_;
    dirent buff_[15];
    long current_pos_;
};
typedef struct DIR DIR;
#define LOG_TAG "KEY_JNI"
#define KEYPAD_DEVICE_1 "qpnp_pon"
#define KEYPAD_DEVICE_2 "gpio-keys"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
static int gKeyHomeFd;

extern "C"
JNIEXPORT jstring
JNICALL
Java_framework_com_keyboarddemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jint JNICALL
Java_framework_com_keyboarddemo_MainActivity_readKey(JNIEnv *env, jobject instance) {
    // TODO
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_framework_com_keyboarddemo_MainActivity_openDevice(JNIEnv *env, jobject instance) {

    //open device.
    struct DIR* inputDir;
    struct dirent* inputDev;
    char inodeName[80];

    gKeyHomeFd = -1;
    inputDir = opendir("/dev/input");
    if (NULL == inputDir) {
        LOGD("can't open input dir");
        return -1;
    }
    while ((inputDev = readdir(inputDir)) != NULL){
        sprintf(inodeName, "/dev/%s", inputDev->d_name);
        LOGD("open inode : %s \n", inodeName);
        int fd = open(inodeName, O_RDWR);
        if (fd < 0){
            LOGD("error open %s \n", inodeName);
            continue;
        }
        char name[80];
        if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
            name[0] = '\0';
            continue;
        }
        LOGD("open device name :%s \n",name);
        if (!strcmp(name, KEYPAD_DEVICE_1)) {
            LOGD("hit");
            gKeyHomeFd = fd;
            break;
        } else {
            close(fd);
            fd = -1;
        }
    }
    closedir(inputDir);
    return gKeyHomeFd;

}

