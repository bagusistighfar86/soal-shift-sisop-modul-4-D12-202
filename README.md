# soal-shift-sisop-modul-4-D12-2021
## Anggota Kelompok
| Nama | NRP |
|------|-----|
|Muhammad Bagus Istighfar|05111940000049|
|Rizqi Rifaldi|05111940000068|

## Soal 1
Di suatu jurusan,terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia 
bertemu orang - orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat file system dengan metode encode yang mutakhir. Berikut adalah filesystem rancangan Sin dan Sei :
```
Semua file yang berada pada direktori harus ter-encode menggunakan Atbash
cipher(mirror).
Misalkan terdapat file bernama kucinglucu123.jpg pada direktori DATA_PENTING
“AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” →
“AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”
Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root)
filesystem adalah directory /home/[USER]/Downloads, dalam penamaan file ‘/’
diabaikan, dan ekstensi tidak perlu di-encode.
```
### Sub Soal 1a
Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
Maka dari itu, pada pengerjaan nomor 1a ini dibutuhkan fungsi untuk melakukan encode ```Atbash Cipher``` yaitu mirroring ('A' menjadi 'Z'). Kami disini menggunakan 2 fungsi yaitu ```atbash()``` yang berfungsi sebagai dasar encode atbash dan ```encode_atbash()``` yang berfungsi memanggil fungsi ```atbash()```. Potongan kode dari masing - masing fungsinya adalah sebagai berikut :
```C
void atbash(char *str, int start, int end) {
    for (int i = start; i < end; i++) {
        if (str[i] == '/') continue; // jika ada substring '/' diabaikan
        if (i != start && str[i] == '.') break; // jika telah menemukan format ekstensi file yang diawali titik maka diabaikan

        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] = 'Z' + 'A' - str[i];
        else if (str[i] >= 'a' && str[i] <= 'z')
            str[i] = 'z' + 'a' - str[i];
    }
}

void encode_atbash(char *str) {
    if (!strcmp(str, ".") || !strcmp(str, "..")) return; // Untuk current dan parent directory tidak perlu di encode
    atbash(str, 0, strlen(str));
}
```

### Sub Soal 1b
Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
Sama seperti pada sub soal 1a, di sub soal 1b juga membutuhkan 2 fungsi yang sama yaitu ```atbash()``` yang berfungsi sebagai dasar encode atbash dan ```encode_atbash()``` yang berfungsi memanggil fungsi ```atbash()```.

### Sub Soal 1c
Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
Pada sub soal 1c ini, jika ada direktori yang terenkripsi (awalan nama direktori 'AtoZ_'), ketika di rename akan menjadi tidak ter-encode lagi isinya (kembali normal), maka membutuhkan sebuah fungsi untuk melakukan decode terhadap file - file yang ter-encode. Fungsi yang kami gunakan adalah ```decode_atbash``` yang isinya adalah sebagai berikut :
```C
void decode_atbash(char *str) {
    if (!strcmp(str, ".") || !strcmp(str, "..")) return; // Untuk current dan parent directory tidak perlu di encode
    if (strstr(str, "/") == NULL) return; // untuk root directory tidak perlu di encode

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
}
```

### Sub Soal 1d
Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : ```/home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]```
Untuk membuat log yang mencatat adanya perubahan nama dari sebuah folder atau direktori, maka bisa menggunakan sebuah fungsi untuk mencatat perubahan nama dari suatu direktori, dan kelompok kami membuat fungsi ```log_v1()``` untuk membuat log tersebut yang isi fungsinya adalah sebagai berikut :
```C
void log_v1(char *from, char *to) {
    int i;
    for (i = strlen(to); i >= 0; i--) {
        if (to[i] == '/') break;
    }
    if (strstr(to + i, ATOZ) == NULL) return;

    FILE *log_file = fopen(path_log, "a");
    fprintf(log_file, "%s -> %s\n", from, to);
}
```

### Sub Soal 1e
Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)
Semua metode encode menggunakan fungsi - fungsi diatas sudah berlaku terhadap dirktori yang ada didalamnya, jadi sub soal 1e sudah di-cover oleh nomor - nomor sebelumnya.


## Soal 2
Selain itu Sei mengusulkan untuk membuat metode enkripsi tambahan agar data pada komputer mereka semakin aman. Berikut rancangan metode enkripsi tambahan yang
dirancang oleh Sei.
### Sub Soal 2a
Jika sebuah direktori dibuat dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1 dengan algoritma tambahan ROT13 (Atbash + ROT13).
### Sub Soal 2b
Jika sebuah direktori di-rename dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key “SISOP” (Case-sensitive, Atbash + Vigenere).
### Sub Soal 2c 
Apabila direktori yang terencode di-rename (Dihilangkan “RX_” nya), maka folder menjadi tidak terencode dan isi direktori tersebut akan terdecode berdasar nama
aslinya.
### Sub Soal 2d 
Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat ke sebuah log file beserta methodnya (apakah itu mkdir atau rename).
### Sub Soal 2e 
Pada metode enkripsi ini, file-file pada direktori asli akan menjadi terpecah menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. Sebagai contoh, Suatu_File.txt berukuran 3 kiloBytes pada directory asli akan menjadi 3 file kecil yakni:
```
Suatu_File.txt.0000
Suatu_File.txt.0001
Suatu_File.txt.0002
```
Ketika diakses melalui filesystem hanya akan muncul Suatu_File.txt

## Soal 3
Karena Sin masih super duper gabut akhirnya dia menambahkan sebuah fitur lagi pada filesystem mereka.
### Sub Soal 3a 
Jika sebuah direktori dibuat dengan awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
### Sub Soal 3b 
Jika sebuah direktori di-rename dengan memberi awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
### Sub Soal 3c 
Apabila direktori yang terenkripsi di-rename dengan menghapus “A_is_a_” pada bagian awal nama folder maka direktori tersebut menjadi direktori normal.
### Sub Soal 3d 
Direktori spesial adalah direktori yang mengembalikan enkripsi/encoding pada direktori “AtoZ_” maupun “RX_” namun masing-masing aturan mereka tetap berjalan pada direktori di dalamnya (sifat recursive “AtoZ_” dan “RX_” tetap berjalan pada subdirektori).
### Sub Soal 3e 
Pada direktori spesial semua nama file (tidak termasuk ekstensi) pada fuse akan berubah menjadi lowercase insensitive dan diberi ekstensi baru berupa nilai desimal dari binner perbedaan namanya.

Contohnya jika pada direktori asli nama filenya adalah “FiLe_CoNtoH.txt” maka pada fuse akan menjadi “file_contoh.txt.1321”. 1321 berasal dari biner 10100101001.

## Soal 4
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.
### Sub Soal 4a 
Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
### Sub Soal 4b 
Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
### Sub Soal 4c 
Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
### Sub Soal 4d 
Sisanya, akan dicatat pada level INFO.
### Sub Soal 4e 
Format untuk logging yaitu:
```
[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]
Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun,
HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD :
System Call yang terpanggil, DESC : informasi dan parameter tambahan
INFO::28052021-10:00:00:CREATE::/test.txt
INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt
```

Untuk menjawab soal nomor 4, kami membuat fungsi yaitu ```log_v2``` untuk membuat sebuah log pada direktori pengguna, yaitu ```/home/bagus/``` dan menulis isi log sesuai dengan struktur pada soal, yang isi fungsinya adalah sebagai berikut :
```C
void log_v2(char *str, int type) {
    FILE *log_file = fopen(path_log, "a");

    time_t current_time;
    time(&current_time);
    struct tm *time_info;
    time_info = localtime(&current_time);

    if (type == INFO) {
        if(time_info->tm_mon+1 < 10){
            fprintf(log_file, "INFO::%d0%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str);
        }
        else{
            fprintf(log_file, "INFO::%d%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str);
        }
    } 
    else if (type == WARNING) {
        if(time_info->tm_mon+1 < 10){
            fprintf(log_file, "WARNING::%d0%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str);
        }
        else{
            fprintf(log_file, "WARNING::%d%d%d-%d:%d:%d:%s\n", time_info->tm_mday,
                    time_info->tm_mon+1, time_info->tm_year+1900, time_info->tm_hour,
                    time_info->tm_min, time_info->tm_sec, str);
        }
    }
}
```
Fungsi log_v2 ini akan dipanggil pada semua fungsi yang lain agar semua aktivitas pada FUSE dapat ditulis pada log tersebut. Pertama-tama fungsi ini akan membuka path dari log yaitu direktori pengguna (home), kemudian fungsi akan mengambil waktu yang terdapat pada komputer kita menggunakan ```localtime(&current_time)```. Setelah itu fungsi akan memilah yang mana yang INFO dan WARNING (ada di parameter fungsi), dan menuliskan hasilnya sesuai struktur log pada soal.
