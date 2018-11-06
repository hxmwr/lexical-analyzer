#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define SIZE_OF_BLOCK 4194304
#define MAX_FILE_SIZE (1024 * 1024 * 1024 * 4)

enum error_codes {
    ERR_REG_COMPILE_FAILED,
    ERR_REG_MATCH_FAILED
};

long get_file_size(FILE *fp) {
    long pos = ftell(fp);
    if (pos == -1)
        return EOF;

    rewind(fp);

    if (fseek(fp, 0L, SEEK_END) == -1)
        return EOF;

    long sz = ftell(fp);
    if (sz == -1L)
        return EOF;

    if (fseek(fp, pos, SEEK_SET) == -1)
        return EOF;

    return sz;
}

int get_file_contents(FILE *fp, char *buf, long size_buf) {
    long sz = get_file_size(fp);
    if (sz) {
        if (sz > size_buf) {
            return EOF;
        } else {
            fread(buf, SIZE_OF_BLOCK, (sz % SIZE_OF_BLOCK) + 1UL, fp);
            return 0;
        }
    } else {
        return EOF;
    }
}

int preg_match(const char *pattern, const char *str, unsigned int nmatch, regmatch_t matches[], unsigned int compile_flag, unsigned int match_flag) {
    regex_t rgx;

    if (regcomp(&rgx, pattern, compile_flag) != 0) {
        perror("Regex compile failed");
        regfree(&rgx);
        return -1;
    }

    if (regexec(&rgx, str, nmatch, matches, match_flag) != 0) {
        perror("Regex match failed");
        regfree(&rgx);
        return -1;
    }

    regfree(&rgx);

    return EXIT_SUCCESS;
}

int main() {
    char *file = "test.c";
    FILE *fp;
    char buf[MAX_FILE_SIZE] = {0};

    if ((fp = fopen(file, "r+")) == NULL) {
        perror("打开文件失败");
        exit(EXIT_FAILURE);
    } else {
        long sz = get_file_contents(fp, buf, MAX_FILE_SIZE);
        if (sz != -1) {
            printf("file size is %u bytes long.\n", (uint32_t)sz);
            exit(EXIT_SUCCESS);
        } else {
            perror("空文件或者读取文件长度失败");
            exit(EXIT_FAILURE);
        }
    }
}