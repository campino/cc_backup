/*
 * Lexer.h
 *
 *  Created on: 28 Oct 2013
 *      Author: akampmann
 */

#ifndef LEXER_H_
#define LEXER_H_

#include <cstring>
#include <string>
#include <string.h>
#include <list>
#include <cstdarg>

#include "pos.h"
#include "diagnostic.h"

using namespace std;

enum TokenType {
	KEYWORD = 0, IDENTIFIER = 1, CONSTANT = 2, STRING = 3, PUNCTUATOR = 4, END = 5,
};

static char const* const types[6] = { "keyword", "identifier", "constant",
		"string", "punctuator", "EOF" };

class Token {
public:
	Token(Pos *p, const char* t, TokenType ty) :
			text(t), pos(p), type(ty) {
	}
	;
	void print(FILE *stream);
private:
	char const* const text;
	Pos * const pos;
	TokenType const type;
};

class Lexer {
public:
	Lexer(FILE *in, char const* const name) :
			input(in) {
		current = new Pos(name);
		current->line = 1;
		lastToken = current;
	}
	;
	~Lexer();
	list<Token*> *lex();
private:
	FILE *input;
	Pos *lastToken;
	Pos *current;
	Token *next();
	Token *digit(char c);
	Token *exponentPart(string);
	char get_char();
	void unget_char(char c);

	inline Token *makePunc(char c) {
		return new Token(lastToken, new char[1] { c }, TokenType::PUNCTUATOR);
	}

	inline Token *makePunc(char c, char c1) {
		return new Token(lastToken, new char[2] { c, c1 },
				TokenType::PUNCTUATOR);
	}

	inline Token *makePunc(char c, char c1, char c2) {
		return new Token(lastToken, new char[3] { c, c1, c2 },
				TokenType::PUNCTUATOR);
	}

	inline Token *makePunc(char c, char c1, char c2, char c3) {
		return new Token(lastToken, new char[4] { c, c1, c2, c3 },
				TokenType::PUNCTUATOR);
	}
};

#endif /* LEXER_H_ */
