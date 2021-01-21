#include<graphics.h>
#include<conio.h>
#include<cmath>

int Mp[230][230], cnt[230][230];

struct coordinate {
    int x;
    int y;
};

void draw_map(const int *map_size, coordinate *first_element);                                   //绘制网格
void change_size(int *map_size, coordinate *first_element, coordinate mouse, int wheel);   //放大缩小
void inoculation(int *map_size, coordinate *first_element, coordinate mouse);              //细胞接种
void reproduction();                                                                       //细胞繁衍

coordinate location(const int *map_size, coordinate mouse) {                          //鼠标在当前显示的网格中的位置
    int i, j;
    for (i = 0; 1 + i * *map_size < 900; i++)
        if (mouse.x >= 1 + i * *map_size && mouse.x < 1 + (i + 1) * *map_size) break;
    for (j = 0; 1 + j * *map_size < 900; j++)
        if (mouse.y >= 1 + j * *map_size && mouse.y < 1 + (j + 1) * *map_size) break;
    coordinate ans{};
    ans.x = i;
    ans.y = j;
    return ans;
}

int main() {
    initgraph(900, 900);
    memset(Mp, 0, sizeof(Mp));
    int map_size = 64;
    coordinate first_element{};
    first_element.x = 107;
    first_element.y = 107;
    draw_map(&map_size, &first_element);
    MOUSEMSG m{};
    while (!_kbhit()) {
        m = GetMouseMsg();
        coordinate mouse{};
        mouse.x = m.x;
        mouse.y = m.y;
        switch (m.uMsg) {
            case WM_MOUSEWHEEL:
                change_size(&map_size, &first_element, mouse, m.wheel);
                break;
            case WM_LBUTTONDOWN:
                inoculation(&map_size, &first_element, mouse);
                break;
        }
    }
    int step = 1024;
    while (true) {
        reproduction();
        draw_map(&map_size, &first_element);
        while (MouseHit()) {
            m = GetMouseMsg();
            coordinate mouse{};
            mouse.x = m.x;
            mouse.y = m.y;
            switch (m.uMsg) {
                case WM_MOUSEWHEEL:
                    change_size(&map_size, &first_element, mouse, m.wheel);
                    break;
                case WM_LBUTTONDOWN:
                    if (step <= 1024) step *= 2;
                    break;
                case WM_RBUTTONDOWN:
                    if (step >= 2) step /= 2;
                    break;
            }
        }
        Sleep(step);
    }
}

void draw_map(const int *map_size, coordinate *first_element) {
    int i, j;
    cleardevice();
    setlinecolor(0x00696969);
    if (*map_size >= 16) {
        for (i = 1; i < 900; i += *map_size)
            line(i, 0, i, 899);
        for (j = 1; j < 900; j += *map_size)
            line(0, j, 899, j);
    }
    for (i = 0; i < (int) (900 / *map_size); i++)
        for (j = 0; j < (int) (900 / *map_size); j++)
            if (Mp[first_element->x + i][first_element->y + j])
                solidrectangle(2 + i * *map_size, 2 + j * *map_size, (i + 1) * *map_size, (j + 1) * *map_size);
}

void change_size(int *map_size, coordinate *first_element, coordinate mouse, int wheel) {
    if ((*map_size == 128 && wheel > 0) || (*map_size == 4 && wheel < 0)) return;
    int i, j;
    cleardevice();
    if (wheel < 0) {
        first_element->x -= location(map_size, mouse).x;
        first_element->y -= location(map_size, mouse).y;
    } else if (wheel > 0) {
        first_element->x += location(map_size, mouse).x / 2;
        first_element->y += location(map_size, mouse).y / 2;
    }
    setlinecolor(0x00696969);
    *map_size *= pow(2, wheel / 120);
    if (*map_size >= 16) {
        for (i = 1; i < 900; i += *map_size)
            line(i, 0, i, 899);
        for (j = 1; j < 900; j += *map_size)
            line(0, j, 899, j);
    }
    if (wheel < 0) {
        if (first_element->x < 1) first_element->x = 1;
        if (first_element->y < 1) first_element->y = 1;
        if (*map_size == 4) {
            first_element->x = 1;
            first_element->y = 1;
        }
    } else if (wheel > 0) {
        if (first_element->x > 225 - (int) (900 / *map_size) + 1) first_element->x = 225 - (int) (900 / *map_size) + 1;
        if (first_element->y > 225 - (int) (900 / *map_size) + 1) first_element->y = 225 - (int) (900 / *map_size) + 1;
    }
    for (i = 0; i < (int) (900 / *map_size); i++)
        for (j = 0; j < (int) (900 / *map_size); j++)
            if (Mp[first_element->x + i][first_element->y + j])
                solidrectangle(2 + i * *map_size, 2 + j * *map_size, (i + 1) * *map_size, (j + 1) * *map_size);
}

void inoculation(int *map_size, coordinate *first_element, coordinate mouse) {
    int i, j;
    i = location(map_size, mouse).x;
    j = location(map_size, mouse).y;
    if (!Mp[first_element->x + i][first_element->y + j]) {
        solidrectangle(2 + i * *map_size, 2 + j * *map_size, (i + 1) * *map_size, (j + 1) * *map_size);
        Mp[first_element->x + i][first_element->y + j] = 1;
    } else {
        clearrectangle(2 + i * *map_size, 2 + j * *map_size, (i + 1) * *map_size, (j + 1) * *map_size);
        Mp[first_element->x + i][first_element->y + j] = 0;
    }
}

void reproduction() {
    int i, j;
    memset(cnt, 0, sizeof(cnt));
    for (i = 1; i <= 225; i++)
        for (j = 1; j <= 225; j++)
            cnt[i][j] += Mp[i - 1][j - 1] + Mp[i - 1][j] + Mp[i - 1][j + 1] + Mp[i][j - 1] + Mp[i][j + 1] +
                         Mp[i + 1][j - 1] + Mp[i + 1][j] + Mp[i + 1][j + 1];
    for (i = 1; i <= 225; i++) {
        for (j = 1; j <= 225; j++) {
            if (Mp[i][j] == 1 && (cnt[i][j] < 2 || cnt[i][j] > 3)) Mp[i][j] = 0;
            if (Mp[i][j] == 0 && cnt[i][j] == 3) Mp[i][j] = 1;
        }
    }
}