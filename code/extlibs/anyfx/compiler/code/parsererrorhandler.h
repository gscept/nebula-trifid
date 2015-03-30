#ifndef PARSERERRORHANDLER_H
#define PARSERERRORHANDLER_H
void ParserError(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8* tokenNames);
void PrintParserException(pANTLR3_EXCEPTION ex, pANTLR3_UINT8* tokenNames);
#endif
