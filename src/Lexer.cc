/*
 * Lexer.cpp
 *
 *  Created on: 28 Oct 2013
 *      Author: akampmann
 */

#include "Lexer.h"

void Token::print(FILE *stream) {
	fprintf(stream, "%s:%d:%d %s %s\n",
			pos.name, pos.line, pos.column,
			types[type], text);
};

Lexer::~Lexer() {
}
