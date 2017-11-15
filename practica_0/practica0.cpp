#include "stdafx.h"
#include "conio.h"
#include "stdio.h"
#include <stdlib.h>
#include "windows.h"
#include <list>

const int  LEVEL_WIDTH       = 30;
const int  ENEMY_CHANCE      = 5;
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

struct Bullet {
	int pos = -1;
	char symbol = 0;
};

std::list<int> enemy_positions;
std::list<Bullet> bullets;

int  player_pos   = LEVEL_WIDTH / 2;
int  player_score = 0;
int  player_lifes = INIT_PLAYER_LIFES;
int  bonus_pos    = -1;
char key          = 0;

void destroyBonus(int &bonus_pos) {
	bonus_pos = -1;
}

void destroyPlayer() {
	player_pos = LEVEL_WIDTH / 2;
	player_lifes--;	
	Sleep(400);
}

void generateEnemy() {		
	int roll = rand() % 100 + 1;
	if (roll <= ENEMY_CHANCE) {
		int s = rand() % 2;
		int enemy_pos = -1;
		if (s == 0) {
			enemy_pos = 0;
		}
		else {
			enemy_pos = LEVEL_WIDTH - 1;
		}
		enemy_positions.push_back(enemy_pos);
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

void bulletUpdate(Bullet &bullet) {	
	if (bullet.pos != -1) {
		if (bullet.symbol == CHAR_SHOOT_LEFT) {			
			if (bullet.pos == 0) {
				bullet.pos = -1;
			}
			else {
				bullet.pos--;
			}
		}
		else if (bullet.symbol == CHAR_SHOOT_RIGHT) {
			if (bullet.pos == LEVEL_WIDTH - 1) {
				bullet.pos = -1;
			}
			else {
				bullet.pos++;
			}
		}
	}
}

void manageInput(char &key, int &player_pos, std::list<Bullet> &bullets) {
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
		if (player_pos != 0) {
			Bullet bullet{ player_pos - 1, CHAR_SHOOT_LEFT };
			bullets.push_back(bullet);
		}
		break;
	case KEY_SHOOT_RIGHT:		
		if (player_pos != LEVEL_WIDTH - 1) {
			Bullet bullet{ player_pos + 1, CHAR_SHOOT_RIGHT };
			bullets.push_back(bullet);
		}
		break;
	default:
		break;
	}
	key = 0;
}

void checkBulletEnemyCollision() {
	for (auto it_b = bullets.begin(); it_b != bullets.end();) {
		bool destroy_bullet = false;
		for (auto it_e = enemy_positions.begin(); it_e != enemy_positions.end();) {
			if ((*it_b).pos == *it_e) {				
				it_e = enemy_positions.erase(it_e);				
				destroy_bullet = true;
			}
			else {
				++it_e;
			}
		}
		if (destroy_bullet) {
			it_b = bullets.erase(it_b);			
		}
		else {
			++it_b;
		}
	}	
}

void checkPlayerEnemyCollision() {
	for (auto it_e = enemy_positions.begin(); it_e != enemy_positions.end();) {
		if (player_pos == *it_e) {			
			destroyPlayer();			
			it_e = enemy_positions.erase(it_e);
		}
		else {
			++it_e;
		}
	}
}

void checkPlayerBonusCollision(int &player_pos, int &bonus_pos, int &player_score) {
	if (player_pos == bonus_pos) {
		destroyBonus(bonus_pos);
		player_score += BONUS_SCORE;
	}
}

void printGame() {
	
	for (int i = 0; i < LEVEL_WIDTH; ++i) {
		if (i == player_pos) {
			printf("%c", CHAR_PLAYER);
		}				
		else if (i == bonus_pos) {
			printf("%c", CHAR_BONUS);
		}
		else {
			bool print = false;
			for (auto it = bullets.begin(); it != bullets.end(); ++it) {
				if ((*it).pos == i) {
					printf("%c", (*it).symbol);
					print = true;
				}
			}
			for (auto it = enemy_positions.begin(); it != enemy_positions.end(); ++it) {
				if ((*it) == i) {
					printf("%c", CHAR_ENEMY);
					print = true;
				}
			}
			if (!print) {
				printf("%c", CHAR_FLOOR);
			}			
		}		
	}

	printf("\t%s%d\t%s%d", "SCORE: ", player_score, "LIVES: ", player_lifes);

	printf("\r");
}

int main() {

	printf("\n\n\n");

	while (key != KEY_ESC && player_lifes > 0) {

		// Get input.		
		if (_kbhit()) {
			key = _getch();
		}

		// Update game state.

		checkBulletEnemyCollision();

		checkPlayerEnemyCollision();

		//bulletUpdate(bullet_pos, bullet_char);
		for (auto it = bullets.begin(); it != bullets.end(); ++it) {
			bulletUpdate((*it));
		}

		//manageInput(key, player_pos, bullet_pos, bullet_char);		
		manageInput(key, player_pos, bullets);

		checkBulletEnemyCollision();

		checkPlayerEnemyCollision();

		checkPlayerBonusCollision(player_pos, bonus_pos, player_score);

		for (auto it = enemy_positions.begin(); it != enemy_positions.end(); ++it) {
			enemyUpdate((*it), player_pos);
		}

		generateEnemy();

		generateBonus(bonus_pos);		

		// Print Game.
		//printGame(player_pos, enemy_pos, bullet_pos, bonus_pos, bullet_char, player_score, player_lifes);
		printGame();

		Sleep(200);
	}

	return 0;
}

