//------------------------------------------------------------------------------
//	lexererrorhandler.cc
//	2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "antlr3.h"
#include "lexererrorhandler.h"
#include "parser/AnyFXLexer.h"
#include "util.h"

bool lexerError = false;
std::string lexerErrorBuffer;

using namespace AnyFX;
//------------------------------------------------------------------------------
/**
	Handles lexer errors and exits application whenever an error is found
*/
void LexerError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8* tokens)
{
	// set lexer error flag
	lexerError = true;

	pANTLR3_RECOGNIZER_SHARED_STATE state = recognizer->state;

	// retrieve exception
	pANTLR3_EXCEPTION ex = state->exception;

	// get error package
	LexerErrorPackage* package = (LexerErrorPackage*)state->userp;

	// while we still have exceptions
	while (ex != 0)
	{
		// print exception
		PrintLexerException(ex, package->file.c_str());

		// go to next exception
		ex = ex->nextException;
	}
}

//------------------------------------------------------------------------------
/**
	Print lexer error using exception
*/
void PrintLexerException(pANTLR3_EXCEPTION ex, const char* file)
{
	// get position in row (column)
	int row = ex->charPositionInLine;

	// get line where this exception occurred
	int line = ex->line;

	// get the character we didn't want
	unsigned char character = ex->c;

	// explanation string, will be formatted depending on what type of exception we got
	const char* exceptionAsString;

	// select exception string based on exception type
	switch (ex->type)
	{
	case ANTLR3_RECOGNITION_EXCEPTION:
		exceptionAsString = (const char*)ex->message;
		break;
	case ANTLR3_MISMATCHED_TOKEN_EXCEPTION:
		exceptionAsString = "Mismatched token";
		break;
	case ANTLR3_NO_VIABLE_ALT_EXCEPTION:
		exceptionAsString = "No viable alternative";
		break;
	case ANTLR3_MISMATCHED_SET_EXCEPTION:
		exceptionAsString = "Mismatched set";
		break;
	case ANTLR3_EARLY_EXIT_EXCEPTION:
		exceptionAsString = "Early exit";
		break;
	case ANTLR3_FAILED_PREDICATE_EXCEPTION:
		exceptionAsString = "Failed predicate";
		break;	
	case ANTLR3_REWRITE_EARLY_EXCEPTION:
		exceptionAsString = "Early rewrite";
		break;
	}

	// remove the quotes surrounding the file name
	std::string fileString(file);
	fileString = fileString.substr(1, fileString.size()-2);

	// format error message and add to list
	std::string errorMessage;
	if (character == 10)
	{
		errorMessage = Format("Unexpected newline at: %d:%d in file %s\n", line, row, fileString.c_str());
	}
	else
	{
		errorMessage = Format("Lexer error: %s '%c' at %d:%d in file %s\n", exceptionAsString, character, line, row, fileString.c_str());
	}
	lexerErrorBuffer.append(errorMessage);
}

