/*
 * Lexer.h
 *
 *  Created on: 28 Oct 2013
 *      Author: akampmann
 */

#ifndef LEXER_H_
#define LEXER_H_

#include <cstring>
#include <list>

#include "pos.h"

using namespace std;

enum TokenType {
	KEYWORD = 0,
	IDENTIFIER = 1,
	CONSTANT = 2,
	STRING = 3,
	PUNCTUATOR = 4,
};

static char const* const types [5] = {
		"keyword", "identifier", "constant",
		"string", "punctuator"
};

class Token {
public:
	Token(Pos p, char const* const t, TokenType ty)
		:text(t), pos(p), type(ty) {};
	void print(FILE *stream);
private:
	char const* const text;
	Pos const pos;
	TokenType const type;
};

class Lexer {
public:
	Lexer(FILE *in):input(in) {};
	~Lexer();
	list<Token> lex();
private:
	FILE *input;
};

#endif /* LEXER_H_ */
