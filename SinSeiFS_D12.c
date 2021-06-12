#define FUSE_USE_VERSION 28
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static char *path_dir = "/home/bagus/Downloads"; //direktori yang digunakan
static char *path_log = "/home/bagus/SinSeiFS.log"; //direktori log
static char *ATOZ = "AtoZ_"; // code AtoZ
const int INFO = 1; //flag info
const int WARNING = 2; //flag warning

void log_v1(char *from, char *to) { //membuat log nomor 1
    int i;
    for (i = strlen(to); i >= 0; i--) {
        if (to[i] == '/') break; //mengabaikan tanda /
    }
    if (strstr(to + i, ATOZ) == NULL) return; // kalau hasil rename tanpa AtoZ, diabaikan

    FILE *log_file = fopen(path_log, "a"); //buka file log
    fprintf(log_file, "%s -> %s\n", from, to); //tulis isi file log
}

void log_v2(char *str, int type) { // membuat log nomor 4
    FILE *log_file = fopen(path_log, "a"); //membuka file log

    time_t current_time;
    time(&current_time);
    struct tm *time_info;
    time_info = localtime(&current_time); //mengambil current time komputer

    if (type == INFO) { //tipe info
        if(time_info->tm_mon+1 < 10){ //bulan 1 sampai 9
            fprintf(log_file, "INFO::%d0%d%d-%d:%d:%d:%s\n", time_info->tm_mday,    
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str); // tulis isi log
        }
        else{ // bulan 10 sampai 12
            fprintf(log_file, "INFO::%d%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str); //tulis isi log
        }
    } 
    else if (type == WARNING) { // tipe warning
        if(time_info->tm_mon+1 < 10){ //bulan 1 sampai 9
            fprintf(log_file, "WARNING::%d0%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str); //tulis isi log
        }
        else{ //bulan 10 sampai 12
            fprintf(log_file, "WARNING::%d%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str);
        }
    }
}

/*
    Ini fungsi metode atbash cipher 
*/
void atbash(char *str, int start, int end) {
    for (int i = start; i < end; i++) {
        if (str[i] == '/') continue; // jika ada substring '/' diabaikan
        if (i != start && str[i] == '.') break; // jika telah menemukan format ekstensi file yang diawali titik maka diabaikan

        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] = 'Z' + 'A' - str[i]; //atbash huruf kapital
        else if (str[i] >= 'a' && str[i] <= 'z')
            str[i] = 'z' + 'a' - str[i]; //atbash huruf kecil
    }
}

/*
    Ini fungsi encode nama file dan folder sesuai metode atbash chiper
*/
void encode_atbash(char *str) {
    if (!strcmp(str, ".") || !strcmp(str, "..")) return; // Untuk current dan parent directory tidak perlu di encode
    atbash(str, 0, strlen(str)); //memanggil fungsi atbash

    printf("==== enc:atb:%s\n", str);
}

void decode_atbash(char *str) {
    if (!strcmp(str, ".") || !strcmp(str, "..")) return; // Untuk current dan parent directory tidak perlu di encode
    if (strstr(str, "/") == NULL) return; // untuk root directory tidak perlu di encode

    printf("==== before:%s\n", str);
    int str_length = strlen(str), s = 0, i;
    /*
        Pengulangan ini mengecek batas panjang file atau folder hingga sebelum '/' atau ekstensi file
    */
    for (i = str_length; i >= 0; i--) {
        if (str[i] == '/') break; // untuk folder

        if (str[i] == '.') {
            str_length = i; // untuk file. 
            break;
        }
    }
    /*
        untuk mencari titik awal nama file atau folder
    */
    for (i = 0; i < str_length; i++) {
        if (str[i] == '/') {
            s = i + 1;
            break;
        }
    }

    atbash(str, s, str_length);
    printf("==== after:atb:%s\n", str);
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    char *enc1 = strstr(path, ATOZ);
    printf("getattr:%s=enc:%s\n", path, enc1);
    if (enc1 != NULL) {
        printf("YEY::getattr:%s\n", path);
        decode_atbash(enc1);
    }

    int res;
    /*
        Ini hanya untuk debug
    */
    char fpath[1000];
    sprintf(fpath, "%s%s", path_dir, path);
    printf("== getattr::fpath:%s\n", fpath);
    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    char *enc1 = strstr(path, ATOZ);
    printf("readdir:%s=enc:%s\n", path, enc1);
    if (enc1 != NULL) {
        printf("YEY::readdir:%s\n", path);
        decode_atbash(enc1);
    }

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = path_dir;
        sprintf(fpath, "%s", path);
    } else
        sprintf(fpath, "%s%s", path_dir, path);

    int res = 0;
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    dp = opendir(fpath);
    if (dp == NULL) return -errno;

    /*
        Membaca seluruh isi directory 
    */
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        /*
            Pemanggilan proses encode_atbash
        */
        if (enc1 != NULL) { // kalau path folder ada AtoZ_
            encode_atbash(de->d_name); //encode
            printf("== readdir::encode:%s\n", de->d_name);
        }

        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0) break;
    }

    closedir(dp);
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = path_dir;
        sprintf(fpath, "%s", path);
    } else
        sprintf(fpath, "%s%s", path_dir, path);

    log_v1(path_dir, fpath); //membuat log jika ada folder baru

    int res = mkdir(fpath, mode);

    char str[100];
    sprintf(str, "MKDIR::%s", path); //merangkai isi log MKDIR
    log_v2(str, INFO); //memanggil fungsi membuat log nomor 4

    if (res == -1) return -errno;
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = path_dir;
        sprintf(fpath, "%s", path);
    } else
        sprintf(fpath, "%s%s", path_dir, path);

    int res;
    if (S_ISREG(mode)) {
        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0) res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(fpath, mode);
    else
        res = mknod(fpath, mode, rdev);

    char str[100];
    sprintf(str, "CREATE::%s", path); //merangkai isi log CREATE
    log_v2(str, INFO); //memanggil fungsi membuat log nomor 4

    if (res == -1) return -errno;
    return 0;
}

static int xmp_unlink(const char *path) {
    char *enc1 = strstr(path, ATOZ);
    printf("unlink:%s=enc:%s\n", path, enc1);
    if (enc1 != NULL) {
        printf("YEY::unlink\n");
        decode_atbash(enc1);
    }

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = path_dir;
        sprintf(fpath, "%s", path);
    } else
        sprintf(fpath, "%s%s", path_dir, path);

    int res = unlink(fpath);

    char str[100];
    sprintf(str, "REMOVE::%s", path); //merangkai isi log REMOVE
    log_v2(str, WARNING); //memanggil fungsi log nomor 4

    if (res == -1) return -errno;

    return 0;
}

static int xmp_rmdir(const char *path) {
    char *enc1 = strstr(path, ATOZ);
    printf("rmdir:%s=enc:%s\n", path, enc1);
    if (enc1 != NULL) {
        printf("YEY::rmdir:%s\n", path);
        decode_atbash(enc1);
    }

    char fpath[1000];
    sprintf(fpath, "%s%s", path_dir, path);

    int res = rmdir(fpath);

    char str[100];
    sprintf(str, "RMDIR::%s", path);
    log_v2(str, WARNING);

    if (res == -1) return -errno;
    return 0;
}

static int xmp_rename(const char *from, const char *to) {
    printf("rename\n");
    char fileFrom[1000], fileTo[1000];
    sprintf(fileFrom, "%s%s", path_dir, from);
    sprintf(fileTo, "%s%s", path_dir, to);
    printf("%s || %s")
    log_v1(fileFrom, fileTo);

    int res = rename(fileFrom, fileTo);

    char str[100];
    sprintf(str, "RENAME::%s::%s", from, to);
    log_v2(str, INFO);

    if (res == -1) return -errno;
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {  // open file
    char fpath[1000];
    sprintf(fpath, "%s%s", path_dir, path);

    int res;
    res = open(fpath, fi->flags);
    if (res == -1) return -errno;
    
    char str[100];
    sprintf(str, "OPEN::%s", path);
    log_v2(str, INFO);
    
    close(res);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = path_dir;

        sprintf(fpath, "%s", path);
    } else
        sprintf(fpath, "%s%s", path_dir, path);

    int res = 0;
    int fd = 0;

    (void)fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    
    char str[100];
    sprintf(str, "READ::%s", path);
    log_v2(str, INFO);

    res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;

    close(fd);
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi) {  // write file

    char newPath[1000];
    sprintf(newPath, "%s%s", path_dir, path);

    int res = 0;
    int fd = 0;

    (void)fi;
    fd = open(newPath, O_WRONLY);
    if (fd == -1) return -errno;

    char str[100];
    sprintf(str, "WRITE::%s", path);
    log_v2(str, INFO);

    res = pwrite(fd, buf, size, offset);
    if (res == -1) res = -errno;

    close(fd);
    return res;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .mknod = xmp_mknod,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .open = xmp_open,
    .write = xmp_write,
};

int main(int argc, char *argv[]) {
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
