#include <jni.h>
#include "KernelWrapper.h"

#include <stdio.h>
#include <stdlib.h> // calloc
#include <unistd.h> // _exit, fork, lseek, read, write, close, dup2
#include <fcntl.h> // open
#include <sys/types.h> // waitpid, opendir, closedir, open, lseek
#include <sys/wait.h> // waitpid
#include <sys/stat.h> // open
#include <dirent.h> // opendir, readdir, closedir
#include <errno.h>


JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_exit(JNIEnv *env, jclass clazz, jint rc) {
  _exit(rc);
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_fork(JNIEnv *env, jclass clazz) {
  pid_t pid;
  if ( (pid = fork()) == -1) { perror("KernelWrapper fork"); }
  return pid;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_waitpid(JNIEnv *env, jclass clazz, jint pid, jintArray status, jint options) {
  int *stat = (*env)->GetIntArrayElements(env, status, 0);
  pid_t p = waitpid(pid, stat, options);
  if (p==-1) perror("KernelWrapper waitpid");
  (*env)->ReleaseIntArrayElements(env, status, stat, 0);
  return p;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_execv( JNIEnv *env, jclass clazz, jstring path, jobjectArray argv ) {
  const char *c_path = (*env)->GetStringUTFChars(env, path, 0);
  char **c_argv = calloc(sizeof(char*), (*env)->GetArrayLength(env, argv)+2);
  for (int i = 0; i < (*env)->GetArrayLength(env, argv); i++) 
    c_argv[i] = (char*)(*env)->GetStringUTFChars(env, (*env)->GetObjectArrayElement(env, argv, i), 0);
  if (execv(c_path, (char *const*)c_argv) == -1) { perror("KernelWrapper execv"); return -1; }
  fprintf(stderr, "\nHow did you get here?!?\n"); exit(1);
}

JNIEXPORT jobjectArray JNICALL Java_cTools_KernelWrapper_readdir(JNIEnv *env, jclass clazz, jstring path) {
  DIR *dp;
  struct dirent *ep;
  const char *c_path = (*env)->GetStringUTFChars(env, path, 0);

  if ( ! (dp = opendir(c_path)) ) { perror("KernelWrapper opendir"); return 0; }

  int size = -1;
  do {
    size++;
    errno = 0;
    ep = readdir(dp);
    if (errno) { perror("KernelWrapper readdir"); return 0; }
  } while (ep);

  rewinddir(dp);
  jobjectArray array = (*env)->NewObjectArray(env, size, (*env)->FindClass(env, "java/lang/String"), 0);
  for (int i = 0; ep = readdir(dp); i++)
    (*env)->SetObjectArrayElement(env, array, i, (*env)->NewStringUTF(env, ep->d_name));
  closedir(dp);

  (*env)->ReleaseStringUTFChars(env, path, c_path);
  return array;
}

JNIEXPORT jstring JNICALL Java_cTools_KernelWrapper_get_1current_1dir_1name(JNIEnv *env, jclass clazz) {
  char *xxx=get_current_dir_name();
  return (*env)->NewStringUTF(env, xxx);
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_open( JNIEnv *env, jclass clazz, jstring path, jint flags ) {
  const char *c_path = (*env)->GetStringUTFChars(env, path, 0);
  int file;
  if ( (file = open(c_path, flags, 0666)) == -1) { perror("KernelWrapper open"); }
  (*env)->ReleaseStringUTFChars(env, path, c_path);
  return file;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_close( JNIEnv *env, jclass clazz, jint fd ) {
  int err;
  if ( (err = close(fd)) == -1) { perror("KernelWrapper close"); }
  return err;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_lseek( JNIEnv *env, jclass clazz, jint fd, jint offset, jint whence ) {
  int bytes;
  if ( (bytes = lseek(fd, offset, whence)) == -1) { perror("KernelWrapper lseek"); exit(1); }
  return bytes;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_read( JNIEnv *env, jclass clazz, jint fd, jbyteArray buf, jint count ) {
  unsigned char *c_buf = (*env)->GetByteArrayElements(env, buf, 0);
  int bytes;
  if ( (bytes = read(fd, c_buf, count)) == -1) { perror("KernelWrapper read"); exit(1); }
  (*env)->ReleaseByteArrayElements(env, buf, c_buf, 0);
  return bytes;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_readOffset( JNIEnv *env, jclass clazz, jint fd, jbyteArray buf, jint offset, jint count ) {
  unsigned char *c_buf = (*env)->GetByteArrayElements(env, buf, 0);
  int bytes;
  if ( (bytes = read(fd, c_buf+offset, count)) == -1) { perror("KernelWrapper read"); }
  (*env)->ReleaseByteArrayElements(env, buf, c_buf, 0);
  return bytes;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_write( JNIEnv *env, jclass clazz, jint fd, jbyteArray buf, jint count ) {
  unsigned char *c_buf = (*env)->GetByteArrayElements(env, buf, 0);
  int bytes;
  if ( (bytes = write(fd, c_buf, count)) == -1) { perror("KernelWrapper write"); }
  if (bytes != count) { fprintf(stderr, "Could not write everything!\nThis should not happen!\n"); }
  (*env)->ReleaseByteArrayElements(env, buf, c_buf, 0);
  return bytes;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_writeOffset( JNIEnv *env, jclass clazz, jint fd, jbyteArray buf, jint offset, jint count ) {
  unsigned char *c_buf = (*env)->GetByteArrayElements(env, buf, 0);
  int bytes;
  if ( (bytes = write(fd, c_buf+offset, count)) == -1) { perror("KernelWrapper write"); }
  if (bytes != count) { fprintf(stderr, "Could not write everything!\nThis should not happen!\n"); }
  (*env)->ReleaseByteArrayElements(env, buf, c_buf, 0);
  return bytes;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_dup2( JNIEnv *env, jclass clazz, jint oldfd, jint newfd ) {
  int fd;
  if ( (fd = dup2(oldfd, newfd)) == -1) perror("KernelWrapper dup2");
  return fd;
}

JNIEXPORT jint JNICALL Java_cTools_KernelWrapper_pipe( JNIEnv *env, jclass clazz, jintArray pipefd ) {
  int *ary = (*env)->GetIntArrayElements(env, pipefd, 0);
  int rc = pipe(ary);
  if (rc==-1) { perror("KernelWrapper pipe"); }
  (*env)->ReleaseIntArrayElements(env, pipefd, ary, 0);
  return rc;
}
