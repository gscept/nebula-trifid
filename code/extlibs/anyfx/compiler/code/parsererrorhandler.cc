//------------------------------------------------------------------------------
//	parsererrorhandler.cc
//	2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#pragma warning(disable: 4800)
#include "antlr3.h"
#include "parsererrorhandler.h"
#include "util.h"
#include "parser/AnyFXParser.h"
#include <map>

bool parserError = false;
std::string parserErrorBuffer;

using namespace AnyFX;
//------------------------------------------------------------------------------
/**
	Handle parser error, abort upon printing
*/
void 
ParserError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8* tokenNames)
{
	// set parser error flag
	parserError = true;

	pANTLR3_RECOGNIZER_SHARED_STATE state = recognizer->state;

	// get exception
	pANTLR3_EXCEPTION ex = state->exception;

	// print exception while exception exists
	while (ex != 0)
	{
		// perform printing
		PrintParserException(ex, tokenNames);

		// go to next exception
		ex = ex->nextException;
	}
}

//------------------------------------------------------------------------------
/**
	Format text string using given exception and output it
*/
void 
PrintParserException(pANTLR3_EXCEPTION ex, pANTLR3_UINT8* tokenNames)
{
	// create static token name conversion map
	// all tokens must be defined here
	std::map<std::string, std::string> tokenMap;
	tokenMap["SC"] = ";";
	tokenMap["CO"] = ",";
	tokenMap["COL"] = ":";
	tokenMap["LP"] = "(";
	tokenMap["RP"] = ")";
	tokenMap["RB"] = "{";
	tokenMap["LB"] = "}";
	tokenMap["LL"] = "[";
	tokenMap["RR"] = "]";
	tokenMap["DOT"] = ".";
	tokenMap["NOT"] = "!";
	tokenMap["EQ"] = "=";
	tokenMap["QO"] = "\"";
	tokenMap["QU"] = "?";	
	tokenMap["Q"] = "'";
	tokenMap["NU"] = "#";
	tokenMap["NL"] = "newline";
	tokenMap["FORWARDSLASH"] = "\\";
	tokenMap["LESS"] = "<";
	tokenMap["LESSEQ"] = "<=";
	tokenMap["GREATER"] = ">";
	tokenMap["GREATEREQ"] = ">=";
	tokenMap["LOGICEQ"] = "==";
	tokenMap["NOTEQ"] = "!=";
	tokenMap["LOGICAND"] = "&&";
	tokenMap["LOGICOR"] = "||";
	tokenMap["ADD"] = "+";
	tokenMap["SUB"] = "-";
	tokenMap["DIV"] = "/";
	tokenMap["MUL"] = "*";
	tokenMap["INTEGERLITERAL"] = "integer literal";
	tokenMap["FLOATLITERAL"] = "float literal";
	tokenMap["DOUBLELITERAL"] = "double literal";
	tokenMap["IDENTIFIER"] = "identifier";

	// retrieve message and cast
	const char* message = (const char*)ex->message;

	// get the token where this happened
	pANTLR3_COMMON_TOKEN token = (pANTLR3_COMMON_TOKEN)ex->token;
	
	// get position of token in row (column)
	int row = ex->charPositionInLine;

	// get line where the exception occurred
	int line = token->line;
	
	// get the token as a string
	std::string tokenString = tokenMap[(const char*)tokenNames[token->type]];

	// explanation string, will be formatted depending on what type of exception we got
	std::string exceptionAsString;

	// select exception string based on exception type
	switch (ex->type)
	{
		case ANTLR3_RECOGNITION_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				exceptionAsString = Format("Unexpected %s '%s'", tokenString.c_str(), str->chars);
				break;
			}			
		case ANTLR3_MISMATCHED_TOKEN_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				exceptionAsString = Format("Mismatched token '%s', expected '%s'", str->chars, tokenMap[(const char*)tokenNames[ex->expecting]].c_str());
				break;
			}			
		case ANTLR3_MISMATCHED_SET_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				pANTLR3_BITSET bitset = antlr3BitsetLoad(ex->expectingSet);
				pANTLR3_INT32 expecting = bitset->toIntList(bitset);
				ANTLR3_UINT32 count = bitset->size(bitset);
				std::string alternatives;
				unsigned i;
				for (i = 0; i < count; i++)
				{
					alternatives.append(tokenMap[(const char*)tokenNames[expecting[i]]]);
					if (i < count)
					{
						alternatives.append(",\n");
					}
				}
				exceptionAsString = Format("Mismatched token '%s', expected any of the following:\n'%s'", str->chars, alternatives.c_str());
				break;
			}			
		case ANTLR3_NO_VIABLE_ALT_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				exceptionAsString = Format("No viable alternative for '%s'", str->chars);
				break;
			}			
		case ANTLR3_FAILED_PREDICATE_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				exceptionAsString = Format("Prediction failed for '%s'", str->chars);
				break;
			}			
		case ANTLR3_MISSING_TOKEN_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				exceptionAsString = Format("Missing token '%s'", tokenString.c_str());
				break;
			}			
		case ANTLR3_UNWANTED_TOKEN_EXCEPTION:
			{
				pANTLR3_STRING str = token->getText(token);
				exceptionAsString = Format("Unwanted token '%s', expecting '%s'", str->chars, tokenMap[(const char*)tokenNames[ex->expecting]].c_str());
				break;
			}			
	}

	// remove the quotes surrounding the file name
	std::string fileString(*(std::string*)token->custom);

	// format error and save to buffer
	std::string errorMessage = Format("Syntax error: %s at %d:%d in file '%s'.\n", exceptionAsString.c_str(), line, row, fileString.c_str());
	parserErrorBuffer.append(errorMessage);
}

