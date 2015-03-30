#ifndef LEXERERRORHANDLER_H
#define LEXERERRORHANDLER_H
void LexerError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8* tokens);
void PrintLexerException(pANTLR3_EXCEPTION ex, const char* file);
#endif


