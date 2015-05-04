
#define _ENEMYS_
#ifndef _ENEMYS_

void Enemy_Init(unsigned short EnemyType);
void Move_Enemy(unsigned char Reset);
void Show_Dragon(unsigned attack_pos);
void Summon_Dragon();
void Move_Dragon();
void Dragon_Attack(unsigned char Reset);

unsigned char Clear_Enemy();
unsigned char Show_Enemy(unsigned char index);


#endif
