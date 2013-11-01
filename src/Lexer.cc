/*
 * Lexer.cpp
 *
 *  Created on: 28 Oct 2013
 *      Author: akampmann
 */

#include "Lexer.h"

void Token::print(FILE *stream) {
	fprintf(stream, "%s:%d:%d %s %s\n",
			pos->name, pos->line, pos->column,
			types[type], text);
};

Lexer::~Lexer() {
}

void Lexer::unget_char(char ch) {
	if('\n'==ch) {
		return;
	}
	ungetc(ch, input);
	current->column--;
}

char Lexer::get_char() {
	if(feof(input)) {
		throw "end of file";
	}

	int c = fgetc(input);

	if('\n'==c) {
		current->line++;
		current->column=1;
	} else {
		current->column++;
	}
	return c;
}

Token *Lexer::exponentPart(string all) {
	bool decimal = false;
	bool isConstant = true;
	while(!feof(input) && isConstant) {
		char c = get_char();
		if('0'<=c || c <='9') {
			all.push_back(c);
		} else if(!decimal && '.'==c) {
			decimal = true;
			all.push_back(c);
		} else {
			unget_char(c);
			isConstant = false;
		}
	}

	return new Token(clone(current), all.c_str(), CONSTANT);
}

Token *Lexer::digit(char c) {
	string all;

	bool decimal = false;
	bool isConstant = true;
	for(;!feof(input) && isConstant;c = get_char()) {

		if('0'<=c && c <='9') {
			all.push_back(c);
		} else if(!decimal && '.'==c) {
			decimal = true;
			all.push_back(c);
		} else if('e'==c || 'E'==c) {
			all.push_back(c);
			return exponentPart(all);
		} else {
			unget_char(c);
			isConstant = false;
		}
	}

	char *text = new char[all.length()+1];
	strncpy(text, all.c_str(), all.length()+1);
	return new Token(lastToken, text, TokenType::CONSTANT);
};

Token *Lexer::next() {
	char c = get_char();
	lastToken = clone(current);

	// digit
	if('0'<=c && c<='9') {
		return digit(c);
	}
	// identifier/keyword

	// punctuator
	switch(c) {
	case '=':
	case '|':
	case '&':
	case '-':
	case '+':
	{
		char n = get_char();
		if('='==n||c==n || ('-'==c && '>'==n)) {
			return makePunc(c, n);
		} else {
			unget_char(n);
			return makePunc(c);
		}
	}
	case '!':
	case '^':
	case '/':
	case '*':
	{
		char n = get_char();
		if('='==n) {
			return makePunc(c, n);
		} else {
			unget_char(n);
		}
	}
	case '<':
	{
		char n1= get_char();
		if(':'==n1||'%'==n1) {
			return makePunc(c, n1);
		} else {
			unget_char(n1);
		}
	}
	case '>':
	{
		char n1 = get_char();
		if('='==n1) {
			return makePunc(c, n1);
		} else if(n1==c) {
			char n2 = get_char();
			if('='==n2) {
				return makePunc(c, n1, n2);
			} else {
				unget_char(n2);
				return makePunc(c, n1);
			}
		} else {
			unget_char(n1);
			// fall through
		}
	}
	case ',':
	case ';':
	case '?':
	case '~':
	case '(':
	case ')':
	case '{':
	case '}':
	case ']':
	case '[':
		return makePunc(c);
	case '#':
	{
		char n = get_char();
		if('#'==n) {
			return makePunc(c, n);
		} else {
			unget_char(n);
			return makePunc(c);
		}
	}
	case '%':
	{
		char n1=get_char();
		if('>'==n1 || '='==n1) {
			return makePunc(c, n1);
		} else if(':'==n1) {
			char n2 = get_char();
			char n3 = get_char();
			if('%'==n2 && ':'==n3) {
				return makePunc(c, n1, n2, n3);
			} else {
				unget_char(n3);
				unget_char(n2);
				return makePunc(c, n1);
			}
		} else {
			unget_char(n1);
			return makePunc(c);
		}
	}
	case '.':
	{
		char n1 = get_char();
		char n2 = get_char();
		if('.'==n1 && '.'==n2) {
			return makePunc(c, n1, n2);
		} else {
			unget_char(n2);
			unget_char(n1);
			return makePunc(c);
		}
	}
	case ':':
	{
		char n = get_char();
		if('>'==n) {
			return makePunc(c, n);
		} else {
			unget_char(n);
			return makePunc(c);
		}
	}
	}

	errorf(*current, "reached end without reading a token.");
	return new Token(current, "EOF", TokenType::END);
}

list<Token*> *Lexer::lex() {
	list<Token*> *result = new list<Token*>();

	while(!feof(input)) {
		Token *tok = next();
		result->push_back(tok);
	}

	return result;
}
