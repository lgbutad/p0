#include "stdafx.h"
#include "conio.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"

#include <list>

#define LEVEL_WIDTH        30
#define ENEMY_CHANCE       5
#define BONUS_CHANCE       100
#define BONUS_SCORE        100
#define PLAYER_LIFES       3

#define SYMBOL_FLOOR       '_'
#define SYMBOL_PLAYER      '@'
#define SYMBOL_ENEMY       'g'
#define SYMBOL_BONUS       '$'
#define SYMBOL_SHOOT_LEFT  '<'
#define SYMBOL_SHOOT_RIGHT '>'

#define KEY_ESC            27
#define KEY_LEFT_1         'a'
#define KEY_LEFT_2         'A'
#define KEY_RIGHT_1        'd'
#define KEY_RIGHT_2        'D'
#define KEY_SHOOT_LEFT_1   'k'
#define KEY_SHOOT_LEFT_2   'K'
#define KEY_SHOOT_RIGHT_1  'l'
#define KEY_SHOOT_RIGHT_2  'L'

struct Bullet {
	int  pos    = -1;
	char symbol = 0;
};

std::list<int> enemy_positions;
std::list<Bullet> bullets;

int player_pos   = LEVEL_WIDTH / 2;
int player_score = 0;
int player_lifes = PLAYER_LIFES;
int bonus_pos    = -1;
int key          = 0;

// Input manager.
void manageInput() {
	switch (key) {
	case KEY_LEFT_1:
	case KEY_LEFT_2:
		if (player_pos != 0) {
			player_pos--;
		}
		break;
	case KEY_RIGHT_1:
	case KEY_RIGHT_2:
		if (player_pos != LEVEL_WIDTH - 1) {
			player_pos++;
		}
		break;
	case KEY_SHOOT_LEFT_1:
	case KEY_SHOOT_LEFT_2:
		if (player_pos != 0) {
			Bullet bullet{ player_pos - 1, SYMBOL_SHOOT_LEFT };
			bullets.push_back(bullet);
		}
		break;
	case KEY_SHOOT_RIGHT_1:
	case KEY_SHOOT_RIGHT_2:
		if (player_pos != LEVEL_WIDTH - 1) {
			Bullet bullet{ player_pos + 1, SYMBOL_SHOOT_RIGHT };
			bullets.push_back(bullet);
		}
		break;
	default:
		break;
	}
}

// Destroy functions.
void destroyBonus() {
	bonus_pos = -1;
}

void destroyPlayer() {
	player_pos = LEVEL_WIDTH / 2;
	player_lifes--;	
	Sleep(400);
}

// Generators.
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

void generateBonus() {
	if (bonus_pos == -1) {
		int roll = rand() % 100 + 1;
		if (roll <= BONUS_CHANCE) {
			bonus_pos = rand() % LEVEL_WIDTH;			
		}
	}
}

// Update functions.
void enemyUpdate(int &enemy_pos) {
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
		if (bullet.symbol == SYMBOL_SHOOT_LEFT) {			
			if (bullet.pos == 0) {
				bullet.pos = -1;
			}
			else {
				bullet.pos--;
			}
		}
		else if (bullet.symbol == SYMBOL_SHOOT_RIGHT) {
			if (bullet.pos == LEVEL_WIDTH - 1) {
				bullet.pos = -1;
			}
			else {
				bullet.pos++;
			}
		}
	}
}

// Collision functions.
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

void checkPlayerBonusCollision() {
	if (player_pos == bonus_pos) {
		destroyBonus();
		player_score += BONUS_SCORE;
	}
}

// Print functions.
void printGame() {	
	for (int i = 0; i < LEVEL_WIDTH; ++i) {
		if (i == player_pos) {
			printf("%c", SYMBOL_PLAYER);
		}				
		else if (i == bonus_pos) {
			printf("%c", SYMBOL_BONUS);
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
					printf("%c", SYMBOL_ENEMY);
					print = true;
				}
			}
			if (!print) {
				printf("%c", SYMBOL_FLOOR);
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
		key = 0;		
		if (_kbhit()) {
			key = _getch();
		}

		// Update game state.
		checkBulletEnemyCollision();
		checkPlayerEnemyCollision();

		for (auto it = bullets.begin(); it != bullets.end(); ++it)
			bulletUpdate(*it);

		manageInput();
		checkBulletEnemyCollision();
		checkPlayerEnemyCollision();
		checkPlayerBonusCollision();

		for (auto it = enemy_positions.begin(); it != enemy_positions.end(); ++it)
			enemyUpdate(*it);		

		generateEnemy();
		generateBonus();
		
		// Print Game.
		printGame();

		Sleep(200);
	}

	return 0;
}
