#include "movement.h"
#include <stdio.h>
#include <string.h>
#include "collider.h"
#include "common.h"
#include "config.h"

extern const bool isDebug;
extern bool running;

bool moveBlocked(entity pl, entity box, enum Movement mov) {
	pos boxPos = box.position;
	pos plPos = pl.position;
	pos diff = pDiff(plPos, boxPos);
	if(isDebug) {
		printf("diff: x %d y %d\npl x %d y %d\n"
			   "box x %d y %d\n",
			   diff.x, diff.y, plPos.x, plPos.y,
			   boxPos.x, boxPos.y);
	}
	if(pSumU(diff) != 1)
		goto f;
	if(mov == UP || mov == DOWN) {
		if(diff.y == 0)
			goto f;
		if(mov == UP) {
			if(diff.y == 1)
				goto t;
		} else {
			if(diff.y == -1)
				goto t;
		}
	} else if(mov == LEFT || mov == RIGHT) {
		if (diff.x == 0)
			goto f;
		if (mov == RIGHT) {
			if (diff.x == -1)
				goto t;
		} else {
			if (diff.x == 1)
				goto t;
		}
	}
f:
	return false;
t:
	collHandler(box.collisionType, box, &pl, mov);
	return true;
}

void n_move(entity *pl, enum Movement mov) {
	for (int i = 0; i < ent_sz; i++) {
		entity *ent = entAtIndex(i);
		if (!ent->blockColl)
			if(!ent->canOverlap)
				continue;
		if (moveBlocked(*pl, *ent, mov) && !ent->canOverlap)
			return;
	}
	switch (mov) {
		case QUIT:
			if (isDebug)
				puts("Exiting...");
			running = false;
			return;
		case UP:
			if (isDebug)
				puts("U");
			if (pl->position.y > 0)
				pl->position.y--;
			break;
		case DOWN:
			if (isDebug)
				puts("D");
			if (pl->position.y + 1 < B_HEI)
				pl->position.y++;
			break;
		case LEFT:
			if (isDebug)
				puts("L");
			if (pl->position.x > 0)
				pl->position.x--;
			break;
		case RIGHT:
			if (isDebug)
				puts("R");
			if (pl->position.x+1 < B_WID)
				pl->position.x++;
			break;
		case SAVE: {
			printf("Name: ");
			char buff[16];
			scanf("%15s", buff);
			saveEntities(buff);
			afterscanf();
			break;
		}
		case LOAD:{
			printf("Name: ");
			char buff[16];
			scanf("%15s", buff);
			loadEntities(buff, pl);
			afterscanf();
			break;
		}
		case RESTART:
			pl->position = unusedPos();
			for(int i = 0; i < ent_sz; i++) {
				entAtIndex(i)->position = unusedPos();
				//entAtIndex(i)->position = (pos) { (int)random() % B_HEI, (int)random() % B_WID };
			}
			break;
		case NONE:
			fprintf(stderr, "Command not found\n");
			if (isDebug)
				puts("NONE");
			fflush(stderr);
			break;
		default:
			break;
	}
}

enum Movement strToMov(const char *in) {
	if(strnlen(in, 2) == 1) {
		for (int i = 0; i < MOV_SZ; i++) {
			for (int j = 0; j < strlen(movs[i].chnames); j++) {
				if (movs[i].chnames[j] == in[0])
					return movs[i].type;
			}
		}
		return NONE;
	}
	for (int i = 0; i < MOV_SZ; i++) {
		if(movs[i].multiple) {
			if(strncasecmp(movs[i].names, in, movs[i].lens[0]) == 0 ||
			   strncasecmp(movs[i].names+movs[i].lens[0], in, movs[i].lens[1]) == 0)
				return movs[i].type;
		} else if (strncasecmp(movs[i].names, in, 16) == 0) {
			return movs[i].type;
		}
	}
	return NONE;
}

pos unusedPos(void) {
	pos tmp = {(int) random() % B_HEI, (int) random() % B_WID};
	for (int j = -1; j < (int) ent_sz; j++) {
u:
		if (pSame(entAtIndex(j)->position, tmp))
			goto r;
		continue;
r:
		tmp = (pos) {(int) random() % B_HEI, (int) random() % B_WID};
		goto u;
	}
	return tmp;
}