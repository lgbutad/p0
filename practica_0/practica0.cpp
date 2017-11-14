#include "stdafx.h"
#include "conio.h"
#include "stdio.h"
#include <stdlib.h>
#include "windows.h"

const int  LEVEL_WIDTH       = 30;
const int  ENEMY_CHANCE      = 100;
const int  BONUS_CHANCE      = 100;
const int  BONUS_SCORE       = 100;
const int  INIT_PLAYER_LIFES = 3;
const char CHAR_FLOOR        = '_';
const char CHAR_PLAYER       = '@';
const char CHAR_ENEMY        = 'g';
const char CHAR_BONUS        = '$';
const char CHAR_SHOOT_LEFT   = '<';
const char CHAR_SHOOT_RIGHT  = '>';
const char KEY_ESC           = '27';
const char KEY_LEFT          = 'a';
const char KEY_RIGHT         = 'd';
const char KEY_SHOOT_LEFT    = 'k';
const char KEY_SHOOT_RIGHT   = 'l';

void destroyEnemy(int &enemy_pos) {
	enemy_pos = -1;
}

void destroyBullet(int &bullet_pos, char &bullet_char) {
	bullet_pos = -1;
	bullet_char = 0;
}

void destroyBonus(int &bonus_pos) {
	bonus_pos = -1;
}

void destroyPlayer(int &player_pos, int &player_lifes) {
	player_pos = LEVEL_WIDTH / 2;
	player_lifes--;	
	Sleep(400);
}

void generateEnemy(int &enemy_pos) {	
	if (enemy_pos == -1) {
		int roll = rand() % 100 + 1;
		if (roll <= ENEMY_CHANCE) {
			int s = rand() % 2;
			if (s == 0) {
				enemy_pos = 0;
			}
			else {
				enemy_pos = LEVEL_WIDTH - 1;
			}
		}
	}
}

void generateBonus(int &bonus_pos) {
	if (bonus_pos == -1) {
		int roll = rand() % 100 + 1;
		if (roll <= BONUS_CHANCE) {
			bonus_pos = rand() % LEVEL_WIDTH;			
		}
	}
}

void enemyUpdate(int &enemy_pos, int player_pos) {
	if (enemy_pos != -1) {
		if (player_pos > enemy_pos) {
			enemy_pos++;
		}
		else if (player_pos < enemy_pos) {
			enemy_pos--;
		}
	}
}

void bulletUpdate(int &bullet_pos, char bullet_char) {
	if (bullet_pos != -1) {
		if (bullet_char == CHAR_SHOOT_LEFT) {
			if (bullet_pos == 0) {
				bullet_pos = -1;
			}
			else {
				bullet_pos--;
			}
		}
		else if (bullet_char == CHAR_SHOOT_RIGHT) {
			if (bullet_pos == LEVEL_WIDTH - 1) {
				bullet_pos = -1;
			}
			else {
				bullet_pos++;
			}
		}
	}
}

void manageInput(char &key, int &player_pos, int &bullet_pos, char &bullet_char) {
	switch (key) {
	case KEY_LEFT:
		if (player_pos != 0) {
			player_pos--;
		}
		break;
	case KEY_RIGHT:
		if (player_pos != LEVEL_WIDTH - 1) {
			player_pos++;
		}
		break;
	case KEY_SHOOT_LEFT:
		if (bullet_pos == -1 && player_pos != 0) {
			bullet_pos = player_pos - 1;
			bullet_char = CHAR_SHOOT_LEFT;
		}
		break;
	case KEY_SHOOT_RIGHT:
		if (bullet_pos == -1 && player_pos != LEVEL_WIDTH - 1) {
			bullet_pos = player_pos + 1;
			bullet_char = CHAR_SHOOT_RIGHT;
		}
		break;
	default:
		break;
	}
	key = 0;
}

void checkBulletEnemyCollision(int &bullet_pos, char &bullet_char, int &enemy_pos) {
	if (bullet_pos == enemy_pos) {
		destroyEnemy(enemy_pos);
		destroyBullet(bullet_pos, bullet_char);
	}
}

void checkPlayerEnemyCollision(int &player_pos, int &player_lifes, int &enemy_pos) {
	if (player_pos == enemy_pos) {
		destroyPlayer(player_pos, player_lifes);
		destroyEnemy(enemy_pos);
	}
}

void checkPlayerBonusCollision(int &player_pos, int &bonus_pos, int &player_score) {
	if (player_pos == bonus_pos) {
		destroyBonus(bonus_pos);
		player_score += BONUS_SCORE;
	}
}

void printGame(int player_pos, int enemy_pos, int bullet_pos, int bonus_pos, int bullet_char, int player_score, int player_lifes) {
	for (int i = 0; i < LEVEL_WIDTH; ++i) {
		if (i == player_pos) {
			printf("%c", CHAR_PLAYER);
		}
		else if (i == enemy_pos) {
			printf("%c", CHAR_ENEMY);
		}
		else if (i == bullet_pos) {
			printf("%c", bullet_char);
		}
		else if (i == bonus_pos) {
			printf("%c", CHAR_BONUS);
		}
		else {
			printf("%c", CHAR_FLOOR);
		}
	}

	printf("\t%s%d\t%s%d", "SCORE: ", player_score, "LIVES: ", player_lifes);

	printf("\r");
}

int main() {
	int player_pos   = LEVEL_WIDTH / 2;
	int bullet_pos   = -1;
	int enemy_pos    = -1;
	int bonus_pos    = -1;
	int player_score = 0;
	int player_lifes = INIT_PLAYER_LIFES;	
	char bullet_char = 0;
	char key         = 0;	

	printf("\n\n\n");

	while (key != KEY_ESC && player_lifes > 0) {

		// Get input.		
		if (_kbhit()) {
			key = _getch();
		}

		// Update game state.

		checkBulletEnemyCollision(bullet_pos, bullet_char, enemy_pos);

		checkPlayerEnemyCollision(player_pos, player_lifes, enemy_pos);

		bulletUpdate(bullet_pos, bullet_char);

		manageInput(key, player_pos, bullet_pos, bullet_char);		

		checkBulletEnemyCollision(bullet_pos, bullet_char, enemy_pos);

		checkPlayerEnemyCollision(player_pos, player_lifes, enemy_pos);

		checkPlayerBonusCollision(player_pos, bonus_pos, player_score);

		enemyUpdate(enemy_pos, player_pos);		

		generateEnemy(enemy_pos);

		generateBonus(bonus_pos);		

		// Print Game.
		printGame(player_pos, enemy_pos, bullet_pos, bonus_pos, bullet_char, player_score, player_lifes);

		Sleep(200);
	}

	return 0;
}

