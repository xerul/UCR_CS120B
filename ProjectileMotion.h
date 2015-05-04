
/* This is the Header File for the LED_Matrix7x10.c */

#ifndef _ProjectileMotion_H
#define _ProjectileMotion_H


#define MAX_COLUMN 10
#define MAX_ROW 7

unsigned char ComputeBallistic(unsigned char cannon_height, unsigned char cannon_power);
void Initialize_Projectile(unsigned char cannon_height, unsigned char cannon_power);
void Update_Trajectory();
void Fire_Projectile();

#endif
