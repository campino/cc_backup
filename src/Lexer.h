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
	KEYWORD = 0,
	IDENTIFIER = 1,
	CONSTANT = 2,
	STRING = 3,
	PUNCTUATOR = 4,
	END = 5,
};

static char const* const types[6] = { "keyword", "identifier", "constant",
		"string", "punctuator", "EOF" };

static string keywords[44] = {"auto", "break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"register",
	"restrict",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
	"_Alignas",
	"_Alignof",
	"_Atomic",
	"_Bool",
	"_Complex",
	"_Generic",
	"_Imaginary",
	"_Noreturn",
	"_Static_assert",
	"_Thread_local"};

class Token {
public:
	Token(Pos *p, const char* t, TokenType ty) :
			text(t), pos(p), type(ty) {
	}
	;
	void print(FILE *stream);
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
	Token *identifier(char c);
	Token *exponentPart(string, char);
	Token *punctuator(char);
	Token *string_literal(char);
	char get_char();
	void unget_char(char c);

	inline Token *makePunc(char c) {
		return new Token(lastToken, new char[2] { c, 0 }, TokenType::PUNCTUATOR);
	}

	inline Token *makePunc(char c, char c1) {
		return new Token(lastToken, new char[3] { c, c1, 0 },
				TokenType::PUNCTUATOR);
	}

	inline Token *makePunc(char c, char c1, char c2) {
		return new Token(lastToken, new char[4] { c, c1, c2, 0 },
				TokenType::PUNCTUATOR);
	}

	inline Token *makePunc(char c, char c1, char c2, char c3) {
		return new Token(lastToken, new char[5] { c, c1, c2, c3, 0 },
				TokenType::PUNCTUATOR);
	}
};

#endif /* LEXER_H_ */
