#include "gba1.h"

//色の定義
#define RED BGR(0x1F, 0x00, 0x00)
#define GREEN BGR(0x00, 0x1F, 0x00)
#define BLUE BGR(0x00, 0x00, 0x1F)
#define YELLOW BGR(0x1F, 0x1F, 0x00)
#define MAGENTA BGR(0x1F, 0x00, 0x1F)
#define CYAN BGR(0x00, 0x1F, 0x1F)
#define BLACK BGR(0x00, 0x00, 0x00)
#define WHITE BGR(0x1F, 0x1F, 0x1F)
//ゲームフィールドの情報
#define FIELD_LEFT 8
#define FIELD_RIGHT 167
#define FIELD_TOP 8
//円の半径
#define CIRCLE_RADIUS 2
//バーの高さ(厚さ)
#define BAR_HEIGHT 4
//バーのy座標
#define BY 150
//四角形の高さ(厚さ)
#define SQUARE_HEIGHT 4
//四角形の横幅
#define SQUARE_WIDTH 20
//四角形のy座標
#define SY 100
//ブロックの高さ(厚さ)
#define BLOCK_HEIGHT 7
//ブロックの横幅
#define BLOCK_WIDTH 15
//時間
#define CURRENT_TIME *((unsigned short *)0x0400010C)
//キーの情報
#define KEY_IN (~(*(hword *) KEY_STATUS) & KEY_ALL)

/* 関数プロトタイプ宣言 */
//円の表示
void circle(hword color);
//バーの表示
void bar(hword x, hword y, hword color);
//四角形1の表示
void square1(hword x, hword y, hword color);
//四角形2の表示
void square2(hword x, hword y, hword color);
//指定した番地の色を計算
hword content(hword x, hword y);
//画面を黒で塗りつぶす
void displayReset();
//ゲームの説明画面を表示する
void displayExplain();
//ゲーム画面を表示する
void displayGame();
//ゲームオーバー画面を表示する
void displayGameover();
//クリア画面を表示する
void displayGameclear();
//バーの移動
void moveRight();
void moveLeft();
//バーの長さ変更
void barLong();
void barShort();
//円の移動
void moveCircle();
//四角形1の移動
void moveSquare1();
//四角形2の移動
void moveSquare2();
//円・四角1・四角2の移動
void move();
//1つのブロックの作成
void block(hword x, hword y, hword color);
//全てのブロックの作成
void blockAll();
//クリア・ゲームオーバーの判定
void check();
//円が当たったブロックを消す
void blockCheck();
//時間を表示
void display_time(hword val);
//タイマスタート
void timerStart();
//タイマストップ
void timerStop();
//スコアの表示
void displayScore();
//スコアの計算
void calcScore();

/* グローバル変数 */
point p;
//バーの長さ
hword barWidth;
//バーの位置情報
hword bx, by;
//円の位置情報
hword cx, cy;
//円の移動の変数
short dx, dy;
//四角形1のx座標
hword s1x;
//四角形2のx座標
hword s2x;
//四角形1の移動変数
short t1x;
//四角形2の移動変数
short t2x;
//ブロックの数
hword blockNum;
//ゲームの判定フラグ
hword gameFlag = 0;
//スコア変数
hword score;

int main(void){
	//時間変数
	volatile unsigned short time;

	/* 画面初期化ルーチン */
	*((unsigned short *)IOBASE) = 0xF03;
	/* タイマ初期化ルーチン */
	*((unsigned short *)0x04000100) = (0xFFFF - 1780);
	*((unsigned short *)0x04000104) = (0xFFFF - 100);
	*((unsigned short *)0x04000108) = (0xFFFF - 10);

	while (1) {
		//ゲームの初期画面
		while(gameFlag == 0){

			displayExplain();

			if(KEY_IN == KEY_START){
				//ゲームスタート
				gameFlag = 1;
				//画面を黒で塗りつぶす
				displayReset();
				//ゲームの基盤を表示する
				displayGame();
				//全てのブロックの作成
				blockAll();
				//スコア
				score = 0;
				//バーの長さ
				barWidth = 20;
				//バーの位置情報
				bx = 80;
				//球体の位置情報
				cx = 135, cy = 100;
				//円の移動の変数
				dx = -1, dy = 1;
				//四角形1の位置情報
				s1x = FIELD_LEFT;
				//四角形1の移動変数
				t1x = 1;
				//四角形2の位置情報
				s2x = FIELD_RIGHT - SQUARE_WIDTH + 1;
				//四角形2の移動変数
				t2x = -1;

				bar(barWidth, BAR_HEIGHT, WHITE);
				square1(SQUARE_WIDTH, SQUARE_HEIGHT, WHITE);
				square2(SQUARE_WIDTH, SQUARE_HEIGHT, WHITE);
				circle(WHITE);

				//タイマスタート
				*((unsigned short *)0x0400010C) = 0x0000;
				timerStart();
			}
		}

		//ゲームスタート
		while(gameFlag == 1){
			//時間を表示
			display_time(CURRENT_TIME);
			displayScore();

			switch (KEY_IN) {
				case KEY_DOWN:
					barShort();
					int j;
					for(j = 0; j < 10000; j++);
					break;

				case KEY_UP:
					barLong();
					int k;
					for(k = 0; k < 10000; k++);
					break;

				case KEY_RIGHT:
					moveRight();
					break;

				case KEY_LEFT:
					moveLeft();
					break;

				case KEY_SELECT:
					gameFlag = 4;
					break;
			}

			//円・四角1・四角2の移動
			move();
			//クリア・ゲームオーバーの判定
			check();

			int i;
			for(i = 0; i < 16500; i++);
		}

		//ゲームオーバー
		while(gameFlag == 2){
			//タイマストップ
			timerStop();
				
			displayGameover();

			if(KEY_IN == KEY_START){
				gameFlag = 0;
				//画面を黒で塗りつぶす
				displayReset();
			}	
		}

		//クリア
		while(gameFlag == 3){
			//タイマストップ
			time = 	*((unsigned short *)0x0400010C);
			timerStop();

			displayGameclear();

			if(KEY_IN == KEY_START){
				gameFlag = 0;
				//画面を黒で塗りつぶす
				displayReset();
			}	
		}

		//中断
		while(gameFlag == 4){
			//タイマストップ
			time = 	*((unsigned short *)0x0400010C);
			timerStop();

			locate(23, 15);
			prints("PAUSE");
			locate(23, 17);
			prints("tap");
			locate(24, 18);
			prints("enter");

			if(KEY_IN == KEY_START){
				gameFlag = 1;
				locate(23, 15);
				prints("     ");
				locate(23, 17);
				prints("   ");
				locate(24, 18);
				prints("     ");
				//タイマスタート
				*((unsigned short *)0x0400010C) = time;
				timerStart();
			}	
		}
	}
}

//ゲームの説明画面を表示する
void displayExplain(){
	locate(10, 2);
	prints("BREAKOUT");
	locate(3, 5);
	prints("ENTER : game start");
	locate(3, 7);
	prints("BACKSPACE : pause");
	locate(3, 9);
	prints("UP : lengthen the bar");
	locate(3, 11);
	prints("DOWN : shorten the bar");
	locate(3, 13);
	prints("LEFT : move to the left");
	locate(3, 15);
	prints("RIGHT : move to the right");
}

//ゲーム画面を表示する
void displayGame(){
	hword x, y;

	//ゲームの枠の表示
	for(y = 1; y <= 159; y++){
		for(x = 1; x <= 174; x++){
			if((x >= 1 && x <= 7) || (x >= 168 && x <= 174) || (y >= 1 && y <= 7)){
				draw_point(x, y, WHITE);
			} 
		}
	}

	locate(22, 1);
	prints("BREAKOUT");
	locate(23, 4);
	prints("SCORE");
	locate(23, 10);
	prints("TIME");
}

//ゲームオーバー画面を表示する
void displayGameover(){
	locate(10, 4);
	prints("GAME OVER");
	locate(4, 8);
	prints("Your score :");
	locate(17, 8);
	printn(score);
	locate(4, 10);
	prints("maximum score : 10000");
	locate(3, 13);
	prints("press ENTER to try again");
}

//クリア画面を表示する
void displayGameclear(){
	locate(10, 4);
	prints("GAME CLEAR");
	locate(4, 8);
	prints("Your score :");
	locate(17, 8);
	printn(score);
	locate(4, 10);
	prints("maximum score : 10000");
	locate(3, 13);
	prints("press ENTER to try again");
}

//画面を黒で塗りつぶす
void displayReset(){
	hword x, y;
	for(y = 0; y < LCD_HEIGHT; y++){
		for(x = 0; x < LCD_WIDTH; x++){
			draw_point(x, y, BLACK);
		}
	}
}

//円を表示
void circle(hword color){
	hword x, y;
	for(y = cy - CIRCLE_RADIUS; y <= cy + CIRCLE_RADIUS; y++){
		for(x = cx - CIRCLE_RADIUS; x <= cx + CIRCLE_RADIUS; x++){
			if(((x - cx) * (x - cx) + (y - cy) * (y - cy)) <= CIRCLE_RADIUS * CIRCLE_RADIUS){
				draw_point(x, y, color);
			}
		}
	}
}

//バーを表示する
void bar(hword x, hword y, hword color){
	hword i,j;
	for(j = 0; j < y; j++){
		for(i = 0; i < x; i++){
			draw_point((bx + i), (BY + j), color);
		}
	}
}

//四角形1を表示する
void square1(hword x, hword y, hword color){
	hword i,j;
	for(j = 0; j < y; j++){
		for(i = 0; i < x; i++){
			draw_point((s1x + i), (SY + j), color);
		}
	}
}

//四角形2を表示する
void square2(hword x, hword y, hword color){
	hword i,j;
	for(j = 0; j < y; j++){
		for(i = 0; i < x; i++){
			draw_point((s2x + i), (SY + j), color);
		}
	}
}

//ブロックの作成
void block(hword x, hword y, hword color){
	hword i,j;
	for(j = 0; j < BLOCK_HEIGHT; j++){
		for(i = 0; i < BLOCK_WIDTH; i++){
			draw_point((x + i), (y + j), color);
		}
	}
}

//全てのブロックの作成
void blockAll(){
	hword x, y, color, tmp;
	for(y = FIELD_TOP + 8; y < 65; y+=8){
		tmp = div(y - (FIELD_TOP + 8), 8);
		if(tmp == 0){
			color = RED;
		} else if(tmp == 1){
			color = GREEN;
		} else if(tmp == 2){
			color = BLUE;
		} else if(tmp == 3){
			color = YELLOW;
		} else if(tmp == 4){
			color = MAGENTA;
		} else if(tmp == 5){
			color = CYAN;
		} else if(tmp == 6){
			color = YELLOW;
		}
		for(x = FIELD_LEFT + 16; x < FIELD_RIGHT - 16; x+=16){
			block(x, y, color);
		}
	}
	//ブロックの数
	blockNum = 56;
}

//バーを右に移動
void moveRight(){
	if(bx + barWidth <= FIELD_RIGHT){
		bar(barWidth, BAR_HEIGHT, BLACK);
		bx++;
		bar(barWidth, BAR_HEIGHT, WHITE);
	}
}

//バーを左に移動
void moveLeft(){
	if(bx > FIELD_LEFT){
		bar(barWidth, BAR_HEIGHT, BLACK);
		bx--;
		bar(barWidth, BAR_HEIGHT, WHITE);
	}
}

//バーを長くする
void barLong(){
	if(barWidth < 35 && bx + barWidth + 5 <= FIELD_RIGHT){
		bar(barWidth, BAR_HEIGHT, BLACK);
		barWidth += 5;
		bar(barWidth, BAR_HEIGHT, WHITE);
	}
}

//バーを短くする
void barShort(){
	if(barWidth > 5){
		bar(barWidth, BAR_HEIGHT, BLACK);
		barWidth -= 5;
		bar(barWidth, BAR_HEIGHT, WHITE);
	}
}

//指定した番地の色を計算
hword content(hword x, hword y){
	hword *ptr;
	ptr = (hword *)VRAM + x + y * LCD_WIDTH;
	return *ptr;
}

//円の移動
void moveCircle(){
	hword conRight = content(cx + 3, cy);
	hword conLeft = content(cx - 3, cy);
	hword conTop = content(cx , cy - 3);
	hword conBottom = content(cx, cy + 3);

	circle(BLACK);

	//円が右下に移動しているとき
	if(dx == 1 && dy == 1){
		//右の壁に当たった時
		if(conRight != BLACK){
			dx = -1;
		}
		//下の壁に当たった時
		if(conBottom != BLACK){
			dy = -1;
		}
		
	}
	//円が右上に移動しているとき
	if(dx == 1 && dy == -1){
		//右の壁に当たった時
		if(conRight != BLACK){
			dx = -1;
		}
		//上の壁に当たった時
		if(conTop != BLACK){
			dy = 1;
		}
		
	}
	//円が左下に移動しているとき
	if(dx == -1 && dy == 1){
		//左の壁に当たった時
		if(conLeft != BLACK){
			dx = 1;
		}
		//下の壁に当たった時
		if(conBottom != BLACK){
			dy = -1;
		}
	}
	//円が左上に移動しているとき
	if(dx == -1 && dy == -1){
		//左の壁に当たった時
		if(conLeft != BLACK){
			dx = 1;
		}
		//上の壁に当たった時
		if(conTop != BLACK){
			dy = 1;
		}
	}
	//円が当たったブロックを消す
	blockCheck();

	cx = cx + dx;
	cy = cy + dy;
	circle(WHITE);
}

//四角1の移動
void moveSquare1(){
	square1(SQUARE_WIDTH, SQUARE_HEIGHT, BLACK);
	//円が右に移動しているとき
	if(t1x == 1 && s1x + SQUARE_WIDTH >= 87){
		t1x = -1;
	}
	//円が左に移動しているとき
	if(t1x == -1 && s1x <= FIELD_LEFT){
		t1x = 1;
	}
	s1x = s1x + t1x;
	square1(SQUARE_WIDTH, SQUARE_HEIGHT, WHITE);
}

//四角2の移動
void moveSquare2(){
	square2(SQUARE_WIDTH, SQUARE_HEIGHT, BLACK);
	//円が右に移動しているとき
	if(t2x == 1 && s2x + SQUARE_WIDTH >= FIELD_RIGHT){
		t2x = -1;
	}
	//円が左に移動しているとき
	if(t2x == -1 && s2x <= 88){
		t2x = 1;
	}
	s2x = s2x + t2x;
	square2(SQUARE_WIDTH, SQUARE_HEIGHT, WHITE);
}

//円・四角1・四角2の移動
void move(){
	moveCircle();
	moveSquare1();
	moveSquare2();
}

//クリア・ゲームオーバーの判定
void check(){
	if(cy == 159){
		gameFlag = 2;
		//画面を黒で塗りつぶす
		displayReset();
	}
	if(blockNum == 0){
		gameFlag = 3;
		//画面を黒で塗りつぶす
		displayReset();
	}
}

//円が当たったブロックを消す
void blockCheck(){
	hword conRight = content(cx + 3, cy);
	hword conLeft = content(cx - 3, cy);
	hword conTop = content(cx , cy - 3);
	hword conBottom = content(cx, cy + 3);
	hword x, y, flag = 0;

	//円の右側がブロックに当たった時
	if(conRight != BLACK && conRight != WHITE){
		x = div((cx + 3 - 24), 16);
		y = div((cy - 16), 8);
		flag = 1;
	}

	//円の左側がブロックに当たった時
	if(conLeft != BLACK && conLeft != WHITE){
		x = div((cx - 3 - 24), 16);
		y = div((cy - 16), 8);
		flag = 1;
	}

	//円の上側がブロックに当たった時
	if(conTop != BLACK && conTop != WHITE){
		x = div((cx - 24), 16);
		y = div((cy - 3 - 16), 8);
		flag = 1;
	}

	//円の下側がブロックに当たった時
	if(conBottom != BLACK && conBottom != WHITE){
		x = div((cx - 24), 16);
		y = div((cy + 3 - 16), 8);
		flag = 1;
	}
	if(flag == 1){
		x = x * 16 + 24;
		y = y * 8 + 16;
		block(x, y, BLACK);
		blockNum--;
		calcScore();
	}
}

//時間を表示
void display_time(hword val) {
	
	/* 引数を10進数として表示する関数（最大値16ビット）*/
	byte char_data[] = "0123456789";
	byte buf[6]  = "0:00.0";
	hword tmp;
	int i;
	
	/* 入力値valの桁数設定 */
	i = 5;

	/* 文字列の最後にNULLコード挿入 */
	buf[i+1] = 0;

	/* 最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	buf[i] = char_data[mod(val, 10)];
	val = div(val, 10);
  
	i = 3;
	/* 秒の一の位の桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	buf[i] = char_data[mod(val, 10)];
	val = div(val, 10);

	/* 秒の十の位の桁の文字（6で割った余り）を挿入し，入力値を6で割る */
	i = 2;
	buf[i] = char_data[mod(val, 6)];
	val = div(val, 6);

	i = 0;
	/* 分の位の桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	buf[i] = char_data[mod(val, 10)];

	locate(23, 12);
	
	/* 文字列全体を表示 */
	prints(buf);

	return;
}

//タイマスタート
void timerStart(){
	*((unsigned short *)0x04000102) = 0x0080;
	*((unsigned short *)0x04000106) = 0x0084;
	*((unsigned short *)0x0400010A) = 0x0084;
	*((unsigned short *)0x0400010E) = 0x0084;
}

//タイマストップ
void timerStop(){
	*((unsigned short *)0x04000102) = 0x0000;
	*((unsigned short *)0x04000106) = 0x0004;
	*((unsigned short *)0x0400010A) = 0x0004;
	*((unsigned short *)0x0400010E) = 0x0004;
}

//スコアの計算
void calcScore(){
	if(barWidth == 5){
		score = score + 150;
	}else if(barWidth == 10){
		score = score + 130;
	}else if(barWidth == 15){
		score = score + 110;
	}else if(barWidth == 20){
		score = score + 90;
	}else if(barWidth == 25){
		score = score + 70;
	}else if(barWidth == 30){
		score = score + 50;
	}else if(barWidth == 35){
		score = score + 30;
	}
	if(blockNum == 0){
		score = score + 1600;
	}
}

//スコアの表示
void displayScore(){
	locate(23, 6);
	printn(score);
}
