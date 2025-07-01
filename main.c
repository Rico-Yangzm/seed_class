#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 2048
#define AREA 256 

void changemode(struct termios *cooked, struct termios *raw) {
    tcgetattr(STDIN_FILENO, cooked);
    *raw = *cooked;
    raw->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    raw->c_oflag &= ~OPOST;
    raw->c_lflag &= ~(ISIG | ICANON | ECHO | ECHONL | IEXTEN);
    raw->c_cflag &= ~(CSIZE | PARENB);
    raw->c_cflag |= CS8;
    raw->c_cc[VMIN] = (cc_t)1;
    raw->c_cc[VTIME] = (cc_t)0;
    tcsetattr(STDIN_FILENO, TCSANOW, raw);
}

void move_cursor(int r, int c) {
    printf("\033[%d;%dH", r + 1, c + 1);
    fflush(stdout);
}

void clear_and_display(char *content) {
    system("clear");
    printf("简易文本编辑器 (按Ctrl+S保存,q退出)\n");
    printf("bug\n");
    printf("%s", content);
    fflush(stdout);
}

void display(char *content){
    system("clear");
    printf("简易文本编辑器 (按Ctrl+S保存,q退出)\n");
    char output[256][256];
    printf("%s", content);
    fflush(stdout);
}

void save(char *filename, char *buffer, size_t length){
    FILE *file = fopen(filename, "w");
    if (file) {
        fwrite(buffer, 1, length, file);
        fclose(file);
        printf("\n文件已保存!\n");
    }
    else 
        printf("\n无法保存文件!\n");
}


void calculate_position(int pos, char *buffer, int *r, int *c) {
    *r = 2;
    *c = 0;
    for (int i = 0; i < pos; i++) {
        if (buffer[i] == '\n') {
            (*r)++;  
            *c = 0;  
        } else {
            (*c)++; 
        }
    }
}

int main() {
    char filename[] = "text.txt";
    char buffer[MAX_LENGTH + 1] = {0};
    size_t length = 0;
    FILE *file = fopen(filename, "r");
    if (file) {
        length = fread(buffer, 1, MAX_LENGTH, file);
        buffer[length] = '\0';
        fclose(file);
    }

    struct termios cooked, raw;
    changemode(&cooked, &raw);
    clear_and_display(buffer);
    
    int pos = 0;  
    int r, c;     // 当前屏幕行列位置(临时变量)
    calculate_position(pos, buffer, &r, &c);
    move_cursor(r, c);

    char ch;
    while (read(STDIN_FILENO, &ch, 1) > 0) {
        if (ch == 113) { 
            break;
        } else if (ch == 19) { //ctrl + s
            save(filename, buffer, length);
            display(buffer);
            calculate_position(pos, buffer, &r, &c);
            move_cursor(r, c);
        } else if (ch == 27) { 
            char seq[2];
            if (read(STDIN_FILENO, seq, 2) == 2) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'D': // 左
                            if (pos > 0) pos--;
                            break;
                        case 'C': // 右
                            if (pos < length) pos++;
                            break;
                        case 'A': // 上
                        case 'B': // 下
                        default:
                            break;
                    }
                    calculate_position(pos, buffer, &r, &c);
                    move_cursor(r, c);
                }
            }
        } else if (ch == 127) { 
            if (pos > 0) {
                memmove(&buffer[pos - 1], &buffer[pos], length - pos + 1);
                length--;
                pos--;
                buffer[length] = '\0';
                display(buffer);
                calculate_position(pos, buffer, &r, &c);
                move_cursor(r, c);
            }
        } else if (ch == 13) { 
            if (length < MAX_LENGTH - 1) {
                memmove(&buffer[pos + 1], &buffer[pos], length - pos + 1);
                buffer[pos] = '\n';
                length++;
                pos++;
                buffer[length] = '\0';
                display(buffer);
                calculate_position(pos, buffer, &r, &c);
                move_cursor(r, c);
            }
        } else if (ch >= 32 && ch <= 126) { 
            if (length < MAX_LENGTH - 1) {
                memmove(&buffer[pos + 1], &buffer[pos], length - pos + 1);
                buffer[pos] = ch;
                length++;
                pos++;
                buffer[length] = '\0';
                display(buffer);
                calculate_position(pos, buffer, &r, &c);
                move_cursor(r, c);
            }
        } 
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &cooked);
    system("clear");
    return 0;
}