/*
 * Lexer.cpp
 *
 *  Created on: 28 Oct 2013
 *      Author: akampmann
 */

#include "Lexer.h"

inline bool isNonDigit(char c) {
	return ('a'<=c && c<='z') || ('A'<=c && c<='Z') || '_'==c;
}

inline bool isDigit(char c) {
	return ('0'<=c && c<='9');
}

inline bool isKeyword(string str) {
	for(int i=0;i<44;i++) {
		if(keywords[i]==str) {
			return true;
		}
	}
	return false;
}

void Token::print(FILE *stream) {
	fprintf(stream, "%s:%d:%d %s %s\n",
			pos->name, pos->line, pos->column,
			types[type], text);
};

Lexer::~Lexer() {
}

void Lexer::unget_char(char ch) {
	ungetc(ch, input);
	current->column--;
	if('\n'==ch) {
		current->line--;
		current->column = last_line_length;
	}
}

char Lexer::get_char() {
	if(feof(input)) {
		return '\n';
	}
	char c = fgetc(input);

	if('\n'==c) {
		last_line_length = current->column;
		current->line++;
		current->column=0;
	} else {
		current->column++;
	}
	return c;
}

Token *Lexer::exponentPart(string all, char e) {
	char c = get_char();
	if(!('+'==c||'-'==c || ('0'<=c && c <='9'))) {
		unget_char(c);
		unget_char(e);
		char *text = new char[all.length()+1];
		strncpy(text, all.c_str(), all.length()+1);
		return new Token(lastPos, text, TokenType::CONSTANT);
	}
	all.push_back(e);
	all.push_back(c);

	bool decimal = false;
	bool isConstant = true;
	while(!feof(input) && isConstant) {
		char c = get_char();
		if('0'<=c && c <='9') {
			all.push_back(c);
		} else if(!decimal && '.'==c) {
			decimal = true;
			all.push_back(c);
		} else {
			unget_char(c);
			isConstant = false;
		}
	}

	char *text = new char[all.length()+1];
	strncpy(text, all.c_str(), all.length()+1);
	return new Token(lastPos, text, TokenType::CONSTANT);
}

Token *Lexer::digit(char c) {
	string all;

	if('.'==c) {
		char n = get_char();
		if(isDigit(n)) {
			all.push_back(c);
			all.push_back(n);
		} else {
			unget_char(n);
			return punctuator(c);
		}
	} else {
		all.push_back(c);
	}

	bool decimal = false;
	bool isConstant = true;
	do {
		c = get_char();
		if(isDigit(c)) {
			all.push_back(c);
		} else if(!decimal && '.'==c) {
			decimal = true;
			all.push_back(c);
		} else if('e'==c || 'E'==c) {
			return exponentPart(all, c);
		} else {
			unget_char(c);
			isConstant = false;
		}
	} while(!feof(input) && isConstant);

	char *text = new char[all.length()+1];
	strncpy(text, all.c_str(), all.length()+1);
	return new Token(lastPos, text, TokenType::CONSTANT);
};

Token *Lexer::next() {

	char c = get_char();
	while(' '==c||'\n'==c||'\t'==c) {
		c = get_char();
	}
	lastPos = clone(current);
	

	// EOF
	if(feof(input)){
		return new Token(current, "EOF", TokenType::END);
	}


	// character constant
	if('\''==c) {
		return char_constant(c);	
	}

	// digit
	if(isDigit(c) || c=='.') {
		return digit(c);
	}
	// identifier/keyword
	if(isNonDigit(c)) {
		return identifier(c);
	}

	// string literal
	if ('"'==c) {
		return string_literal(c);
	}

	// punctuator and unknown char
	return punctuator(c);

}

Token *Lexer::string_literal(char c) {
	string all;
	all.push_back(c);

	bool goOn = true;
	bool escape = false;
	do {
		char c=get_char();
		if('\\'==c) {
			escape = true;
		} else if('"'==c && !escape) {
			goOn=false;
		} else {
			escape = false;
		}
		all.push_back(c);
	} while(goOn && !feof(input));

	if(goOn && feof(input)) {
		errorf(*current, "Non-terminated string!");
	}

	char *text = new char[all.length()+1];
	strncpy(text, all.c_str(), all.length()+1);
	return new Token(lastPos, text, TokenType::STRING);
}

Token *Lexer::char_constant(char c) {
	string all;
	all.push_back(c);
	
	bool goOn = true;
	bool escape = false;
	do {
		char c=get_char();
		if (c=='\\'){
			escape = true;
		} else if(c=='\'' && !escape && (c!='\n')){
			goOn = false;
		} else {
			escape = false;
		}
		all.push_back(c);
	} while(goOn && !feof(input));

	if(goOn && feof(input)) {
		errorf(*current, "Non-terminated char consatant!");
	}

	char *text = new char[all.length()+1];
	strncpy(text, all.c_str(), all.length()+1);
	return new Token(lastPos, text, TokenType::CONSTANT);
}

Token *Lexer::identifier(char c) {
	string all;
	all.push_back(c);

	bool goOn = true;
	do {
		char c=get_char();

		if(isDigit(c)||isNonDigit(c)) {
			all.push_back(c);
		} else {
			goOn = false;
			unget_char(c);
		}
	} while(!feof(input) && goOn);

	TokenType tt = TokenType::IDENTIFIER;
	if(isKeyword(all)) {
		tt = TokenType::KEYWORD;
	}

	char *text = new char[all.length()+1];
	strncpy(text, all.c_str(), all.length()+1);
	return new Token(lastPos, text, tt);
}

Token *Lexer::punctuator(char c) {
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

		//unknown char
		errorf(*current, "Unknown Char in the input!");
		return new Token(current, new char[2] {c,0} , TokenType::UNKNOWN);
}

list<Token*> *Lexer::lex() {
	list<Token*> *result = new list<Token*>();

	while(!feof(input)) {
		Token *tok = next();
		result->push_back(tok);
	}

	return result;
}
