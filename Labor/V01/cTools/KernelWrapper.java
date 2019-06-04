package cTools;

public class KernelWrapper {
  static {
    //System.out.println(System.getProperty("java.library.path"));
    System.loadLibrary("KernelWrapper"); // actually: libKernelWrapper.so
  }

  public static int O_RDONLY = 0;
  public static int O_WRONLY = 1;
  public static int O_RDWR = 2;
  public static int O_CREAT = 64;
  public static int O_TRUNC = 512;
  public static int SEEK_SET = 0;
  public static int SEEK_CUR = 1;
  public static int SEEK_END = 2;
  public static int STDIN_FILENO = 0;
  public static int STDOUT_FILENO = 1;
  public static int STDERR_FILENO = 2;

  public static native int exit(int rc);
  public static native int fork();
  public static native int waitpid(int pid, int[] status, int options);
  public static native int execv(String path, String[] argv);
  public static native String get_current_dir_name();

  public static native int open(String path, int flags);
  public static native int close(int fd);
  public static native int lseek(int fd, int offset, int whence);
  public static native int read(int fd, byte[] buf, int count);
  public static native int readOffset(int fd, byte[] buf, int offset, int count);
  public static native int write(int fd, byte[] buf, int count);
  public static native int writeOffset(int fd, byte[] buf, int offset, int count);

  public static native int pipe(int[] pipefd);
  public static native String[] readdir(String path);
  public static native int dup2(int oldfd, int newfd);
}
