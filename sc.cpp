#include <stdio.h>
#include <bangtal.h>

#define UP 3;
#define RIGHT 4;
#define DOWN 1;
#define LEFT 0;

SceneID scene1;
ObjectID tower;
ObjectID enemy;
ObjectID startButton;
TimerID timer1;

struct coolWindow
{
	short x;
	short y;
};

struct coolBoard {
	char x;
	char y;
};

// 수치 설정 영역
short boardX = 250, boardY = 100;	// 보드판의 왼쪽아래 모서리 좌표
char boardSize = 50;				// 보드판 한칸 크기. 가로 and 세로
char enemyStartX = 0, enemyStartY = 10;		// 적 대기 위치.
char enemyFinX = 0, enemyFinY = 0;			// 적 마지막 위치.
const char roadLength = 45;
coolBoard enemyRoad[60] = { {0, 9}, {0, 8}, {0, 7}, {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {9, 7},
	{9, 8}, {9, 9}, {8, 9}, {7, 9}, {6, 9}, {6, 8}, {6, 7}, {6, 6}, {6, 5}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {7, 0}, {8, 0}, {9, 0},
	{9, 1}, {9, 2}, {9, 3}, {8, 3}, {7, 3}, {6, 3}, {5, 3}, {4, 3}, {3, 3}, {2, 3}, {2, 2}, {2, 1}, {2, 0}, {1, 0}, {0, 0} };			// 적 이동 경로			
char roadDirection[60][2];

coolWindow semiLocation[5][6] = {};

unsigned int tick = 0;


int min(int a, int b) {
	return a > b ? b : a;
}


coolWindow coolBoard2Window(coolBoard coolboard) {
	coolWindow cW ;
	cW.x = boardX + boardSize * coolboard.x;
	cW.y = boardY + boardSize * coolboard.y;
	return cW;
}

void locateOnBoard(ObjectID object, SceneID scene, coolBoard coolboard) {
	coolWindow cW = coolBoard2Window(coolboard);
	locateObject(object, scene, cW.x, cW.y);
}

void calculateDirection() {
	char from = UP;
	char to = DOWN;
	for (int i = 0; i < roadLength; i++) {
		char fromX, fromY,  toX, toY, dx, dy, direction;
		if (i == 0) {
			fromX = enemyStartX, fromY = enemyStartY;
		}
		else {
			fromX = enemyRoad[i - 1].x;
			fromY = enemyRoad[i - 1].y;
		}
		if(i == (roadLength-1)){
			toX = enemyFinX, toY = enemyFinY;
		}
		else {
			toX = enemyRoad[i + 1].x;
			toY = enemyRoad[i + 1].y;			
		}
		dx = fromX - enemyRoad[i].x;
		dy = fromY - enemyRoad[i].y;
		direction = 2 * dx + dy + 2;
		if (direction > 4 || direction < 0 || direction == 2) {
			printf("Somthing wrong with enemy road : %d, from dir : %d\n", i, direction);
			return;

		}
		roadDirection[i][0] = direction;
		dx = toX - enemyRoad[i].x;
		dy = toY - enemyRoad[i].y;
		direction = 2 * dx + dy + 2;
		if (direction > 4 || direction < 0 || direction == 2) {
			printf("Somthing wrong with enemy road : %d, to dir : %d\n", i, direction);
			return;

		}
		roadDirection[i][1] = direction;
		
	}
}

void moveEnemy() {
	int distance = min(tick,450) / 10;
	char semitick = tick % 10;
	char from = roadDirection[distance][0];
	char to = roadDirection[distance][1];
	coolWindow cW = coolBoard2Window(enemyRoad[distance]);
	
	if(semitick<5){
		cW.x += semiLocation[from][semitick].x;
		cW.y += semiLocation[from][semitick].y;
	}
	else {
		cW.x += semiLocation[to][10-semitick].x;
		cW.y += semiLocation[to][10-semitick].y;
	}
	locateObject(enemy, scene1, cW.x, cW.y);
}

void init() {
	for(short i=0; i<6; i++){
		semiLocation[0][i] = { -25 + 5 * i, 0 };
		semiLocation[1][i] = { 0, -25 + 5 * i };
		semiLocation[3][i] = { 0, 25 - 5 * i };
		semiLocation[4][i] = { 25 - 5 * i, 0 };
	}
	for (short i = 0; i < 6; i++) {
		printf("%d : (%d, %d) \n", i, semiLocation[0][i].x, semiLocation[0][i].y);
	}
	for (short i = 0; i < 6; i++) {
		printf("%d : (%d, %d) \n", i, semiLocation[1][i].x, semiLocation[1][i].y);
	}
	for (short i = 0; i < 6; i++) {
		printf("%d : (%d, %d) \n", i, semiLocation[3][i].x, semiLocation[3][i].y);
	}
	for (short i = 0; i < 6; i++) {
		printf("%d : (%d, %d) \n", i, semiLocation[4][i].x, semiLocation[4][i].y);
	}


}

void timerCallback(TimerID timer) {
	if (timer == timer1) {
		setTimer(timer1, 0.1f);
		startTimer(timer1);
		tick++;
		moveEnemy();
	}
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	if (object == startButton) {
		tick = 0;
		setTimer(timer1, 0.1f);
		startTimer(timer1);
	}
}


int main() {

	setTimerCallback(timerCallback);
	setMouseCallback(mouseCallback);

	// Scene + Object 생성
	scene1 = createScene("테스트", "Images/배경.png");	

	tower = createObject("Images/tower1.png");
	locateOnBoard(tower, scene1, { 3, 7 });	
	showObject(tower);

	enemy = createObject("Images/enemy1.png");
	locateOnBoard(enemy, scene1, { enemyStartX, enemyStartY });
	showObject(enemy);

	startButton = createObject("Images/start.png");
	locateObject(startButton, scene1, 1000, 310);
	showObject(startButton);

	// 이동경로의 방향 계산
	calculateDirection();
	
	init();

	timer1 = createTimer(0.5f);
	//startTimer(timer1);

	startGame(scene1);
}