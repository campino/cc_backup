
#include "pos.h"

Pos *clone(Pos *pos) {
	return new Pos(pos->name, pos->line, pos->column);
}
