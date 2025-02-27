//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <cmath>

// ������ ������ ����  

void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false);

struct 
{
    HWND hWnd;//����� ����
    HDC device_context, context;// ��� ��������� ���������� (��� �����������)
    int width, height;//���� �������� ������� ���� ������� ������� ���������
} window;

struct Block 
{
    float x, y, width, height;
    HBITMAP hBitmap;
    bool status;

    float getLeftUpX() { return x; }
    float getLeftUpY() { return y; }
    float getRightUpX() { return x + width; }
    float getRightUpY() { return y; }
    float getLeftDownX() { return x; }
    float getLeftDownY() { return y + height; }
    float getRightDownX() { return x + width; }
    float getRightDownY() { return y + height; }
};

struct sprite
{
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//����� � ������� ������ 
    bool status;
    bool XCollision;
    bool YCollision;

    //  �������, ��� ���������� ���� ����� ����� ������� (� ��������)
    float getAngle(float x1, float y1, float x2, float y2) {
        return atan2(y2 - y1, x2 - x1);
    }

    void show()
    {
        ShowBitmap(window.context, x, y, width, height, hBitmap);
    }

    void checkCollision(Block& block) 
    {
        XCollision = false;
        YCollision = false;

        float blockLeftUpX = block.getLeftUpX();
        float blockLeftUpY = block.getLeftUpY();
        float blockRightUpX = block.getRightUpX();
        float blockRightUpY = block.getRightUpY();
        float blockLeftDownX = block.getLeftDownX();
        float blockLeftDownY = block.getLeftDownY();
        float blockRightDownX = block.getRightDownX();
        float blockRightDownY = block.getRightDownY();

        float angleLU = getAngle(x, y, blockLeftUpX, blockLeftUpY);
        float angleRU = getAngle(x, y, blockRightUpX, blockRightUpY);
        float angleLD = getAngle(x, y, blockLeftDownX, blockLeftDownY);
        float angleRD = getAngle(x, y, blockRightDownX, blockRightDownY);
        float angleMove = getAngle(x, y, x + dx, y + dy);

        if (x + dx > block.x && x + dx < block.x + block.width &&
            y + dy > block.y && y + dy < block.y + block.height)
        {
            //  ���������� ������� ������������ (���������� ������)
            if (x < block.x) { XCollision = true; }  // Left
            if (x > block.x + block.width) { XCollision = true; }  // Right
            if (y < block.y) { YCollision = true; } // Up
            if (y > block.y + block.height) { YCollision = true; } // Down
        }
    }
};

sprite racket;//������� ������
sprite enemy;//������� ����������
sprite ball;//�����

const int blocks_x = 20;
const int blocks_y = 5;
sprite blocks[blocks_x][blocks_y];

struct {
    int score, balls;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
} game;



HBITMAP hBack;// ����� ��� �������� �����������

//c����� ����

void InitGame()
{
    //� ���� ������ ��������� ������� � ������� ������� gdi
    //���� ������������� - ����� ������ ������ ����� � .exe 
    //��������� ������ LoadImageA ��������� � ������� ��������, ��������� �������� ����� ������������� � ������� ���� �������
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

    racket.width = 300;
    racket.height = 50;
    racket.speed = 30;//�������� ����������� �������
    racket.x = window.width / 2.;//������� ���������� ����
    racket.y = window.height - racket.height;//���� ���� ���� ������ - �� ������ �������

    enemy.x = racket.x;//� ���������� �������� ������ � �� �� ����� ��� � ������

    ball.dy = (rand() % 65 + 35) / 100.;//��������� ������ ������ ������
    ball.dx = -(1 - ball.dy);//��������� ������ ������ ������
    ball.speed = 11;
    ball.rad = 20;
    ball.x = racket.x;//x ���������� ������ - �� ������� �������
    ball.y = racket.y - ball.rad;//����� ����� ������ �������


    game.score = 0;
    game.balls = 9;

    for (int i = 0;i < blocks_x;i++)
    {
        for (int j = 0;j < blocks_y;j++)
        {
            
            blocks[i][j].hBitmap = enemy.hBitmap;
            blocks[i][j].width = window.width / blocks_x;
            blocks[i][j].height = window.height / 3 / blocks_y;
            blocks[i][j].x = i * blocks[i][j].width;
            blocks[i][j].y = j * blocks[i][j].height+ window.height / 3;
            blocks[i][j].status = true;
        }
    }
   
}

void ProcessSound(const char* name)//������������ ���������� � ������� .wav, ���� ������ ������ � ��� �� ����� ��� � ���������
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//���������� name �������� ��� �����. ���� ASYNC ��������� ����������� ���� ���������� � ����������� ���������
}

void ShowScore()
{
    //�������� �������� � �������
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//����� ��� ������
    _itoa_s(game.score, txt, 10);//�������������� �������� ���������� � �����. ����� �������� � ���������� txt
    TextOutA(window.context, 10, 10, "Score", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "Balls", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) racket.x -= racket.speed;
    if (GetAsyncKeyState(VK_RIGHT)) racket.x += racket.speed;

    if (!game.action && GetAsyncKeyState(VK_SPACE))
    {
        game.action = true;
        ProcessSound("bounce.wav");
    }
}

void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(hDC); // ������� �������� ������, ����������� � ���������� �����������
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// �������� ����������� bitmap � �������� ������

    if (hOldbm) // ���� �� ���� ������, ���������� ������
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // ���������� ������� �����������

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//��� ������� ������� ����� ����� ��������������� ��� ����������
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // ������ ����������� bitmap
        }

        SelectObject(hMemDC, hOldbm);// ��������������� �������� ������
    }

    DeleteDC(hMemDC); // ������� �������� ������
}

void ShowRacketAndBall()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//������ ���

    for (int i = 0; i < blocks_x; i++) {
        for (int j = 0; j < blocks_y; j++) {

            if (blocks[i][j].status)
            {
                blocks[i][j].show();
            }
        }
    };



    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ������� ������

    if (ball.dy < 0 && (enemy.x - racket.width / 4 > ball.x || ball.x > enemy.x + racket.width / 4))
    {
        //��������� ���������� ���������. �� ����� ����, ��������� ������� �� �����������, � �� �� ������� �������� �� ��� ������� �� ������
        //������ �����, �� ������ ������ ������ �� �������, � ������� ���������� ������� - ����������� ��� �����
        //�������� ����� ������ ���� ����� ����� �����, � ������ ���� ����� �� ��� X ������� �� ������� �������� ����� �������
        //� ���� ������, �� ��������� ���������� ������� � ������ � ��������� 9 � 1
        enemy.x = ball.x * .1 + enemy.x * .9;
    }

    ShowBitmap(window.context, enemy.x - racket.width / 2, 0, racket.width, racket.height, enemy.hBitmap);//������� ���������
    ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// �����
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//���� ��������� ������ ���� ������� ������ ����, �������� �� ����
    racket.x = min(racket.x, window.width - racket.width / 2.);//���������� ��� ������� ����
}

void CheckWalls()
{
    if (ball.x < ball.rad || ball.x > window.width - ball.rad)
    {
        ball.dx *= -1;
        ProcessSound("bounce.wav");
    }
}

void CheckRoof()
{
    if (ball.y < ball.rad + racket.height)
    {
        ball.dy *= -1;
        ProcessSound("bounce.wav");
    }
}

bool tail = false;

void CheckFloor()
{
    if (ball.y > window.height - ball.rad - racket.height)//����� ������� ����� ������� - ����������� �������
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//����� �����, � �� �� � ������ ��������� ������
        {
            game.score++;//�� ������ ������� ���� ���� ����
            ball.speed += 5. / game.score;//�� ����������� ��������� - ���������� �������� ������
            ball.dy *= -1;//������
            racket.width -= 10. / game.score;//������������� ��������� ������ ������� - ��� ���������
            ProcessSound("bounce.wav");//������ ���� �������
        }
        else
        {//����� �� �����

            tail = true;//����� ������ ������ ���� �������

            if (ball.y - ball.rad > window.height)//���� ����� ���� �� ������� ����
            {
                game.balls--;//��������� ���������� "������"

                ProcessSound("fail.wav");//������ ����

                if (game.balls < 0) { //�������� ������� ��������� "������"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//������� ��������� � ���������
                    InitGame();//������������������ ����
                }

                ball.dy = (rand() % 65 + 35) / 100.;//������ ����� ��������� ������ ��� ������
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//�������������� ���������� ������ - ������ ��� �� �������
                ball.y = racket.y - ball.rad;
                game.action = false;//���������������� ����, ���� ����� �� ������ ������
                tail = false;
            }
        }
    }
}

void CheckBlocks()
{
    for (int i = 0; i < blocks_x; i++)
    {
        for (int j = 0; j < blocks_y; j++)
        {
            if (blocks[i][j].status == true)
            {
                ball.checkCollision(blocks[i][j]);

                if (ball.XCollision == true)
                {
                    ball.dy *= -1;
                    blocks[i][j].status = false;
                }
                else if (ball.YCollision == true)

                {
                    ball.dx *= -1;
                    blocks[i][j].status = false;
                }

            }
        }
    }

}

void ProcessRoom()
{
    //������������ �����, ������� � ���. ������� - ���� ������� ����� ���� ���������, � ������, ��� ������� �� ����� ������ ������������� ����� ������� �������� ������
    CheckWalls();
    CheckRoof();
    CheckFloor();
    CheckBlocks();
}

void ProcessBall()
{
    if (game.action)
    {
        //���� ���� � �������� ������ - ���������� �����
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //����� - ����� "��������" � �������
        ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//�� ������ ���� ������� ����� ��������� ���������� ��� ���������
    window.width = r.right - r.left;//���������� ������� � ���������
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//������ �����
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//����������� ���� � ���������
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����
    InitGame();//����� �������������� ���������� ����

    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowRacketAndBall();//������ ���, ������� � �����
        ShowScore();//������ ���� � �����
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)

        ProcessInput();//����� ����������
        LimitRacket();//���������, ����� ������� �� ������� �� �����
        ProcessBall();//���������� �����
        ProcessRoom();//������������ ������� �� ���� � �������, ��������� ������ � ��������
    }

}
