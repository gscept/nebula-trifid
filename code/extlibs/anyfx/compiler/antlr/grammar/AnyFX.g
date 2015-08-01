grammar AnyFX;

options 
{
	language=C;
	backtrack = true;
}


// Lexer API hooks
@lexer::apifuncs
{
	// include own error handler for lexer error recognition
	#include "lexererrorhandler.h"
	RECOGNIZER->displayRecognitionError = LexerError;
	LEXER->emit = EmitPreprocessedToken;
}


@lexer::members
{

int preprocessorRowLexer = 0;
std::string includeFileNameLexer = "";

// perform token-level editing, for this to work properly, the antlr3lexer.c needs to be edited at line 292 from
// emit(lexer);
// to
// lexer->emit(lexer);
// this seems to be a bug/mistake in the implementation
pANTLR3_COMMON_TOKEN
EmitPreprocessedToken(pANTLR3_LEXER lexer)
{
	pANTLR3_COMMON_TOKEN token;

	/* We could check pointers to token factories and so on, but
	* we are in code that we want to run as fast as possible
	* so we are not checking any errors. So make sure you have installed an input stream before
	* trying to emit a new token.
	*/
	token   = lexer->rec->state->tokFactory->newToken(lexer->rec->state->tokFactory);

	/* Install the supplied information, and some other bits we already know
	* get added automatically, such as the input stream it is associated with
	* (though it can all be overridden of course)
	*/
	token->type		    = lexer->rec->state->type;
	token->channel	    = lexer->rec->state->channel;
	token->start	    = lexer->rec->state->tokenStartCharIndex;
	token->stop		    = lexer->getCharIndex(lexer) - 1;
	token->line		    = lexer->rec->state->tokenStartLine;
	token->charPosition	= lexer->rec->state->tokenStartCharPositionInLine;

	if	(lexer->rec->state->text != NULL)
	{
	token->textState	    = ANTLR3_TEXT_STRING;
	token->tokText.text	    = lexer->rec->state->text;
	}
	else
	{
	token->textState	= ANTLR3_TEXT_NONE;
	}
	token->lineStart	= lexer->input->currentLine;
	token->user1	= lexer->rec->state->user1;
	token->user2	= lexer->rec->state->user2;
	token->user3	= lexer->rec->state->user3;
	std::string* file = new std::string;
	*file = includeFileNameLexer;
	token->custom	= (void*)file;

	lexer->rec->state->token	    = token;

	return  token;
}


}

@lexer::includes
{
#include <string>
struct LexerErrorPackage
{
	std::string file;
};
}

// parser API hooks
@parser::apifuncs
{
	// include own error handler for parser error recognition
	#include "parsererrorhandler.h"
	RECOGNIZER->displayRecognitionError = ParserError;	
}

// parser includes
@parser::includes
{	

// include std container
#include <vector>
#include <typeinfo>
#include <iostream>
#include <string>
#include <stack>

#include "../../code/effect.h"
#include "../../code/header.h"
#include "../../code/datatype.h"
#include "../../code/parameter.h"
#include "../../code/annotation.h"
#include "../../code/programrow.h"
#include "../../code/program.h"
#include "../../code/samplertexturelist.h"
#include "../../code/samplerrow.h"
#include "../../code/sampler.h"
#include "../../code/structure.h"
#include "../../code/constant.h"
#include "../../code/blendstaterow.h"
#include "../../code/renderstaterow.h"
#include "../../code/renderstate.h"
#include "../../code/function.h"
#include "../../code/valuelist.h"
#include "../../code/functionattribute.h"
#include "../../code/variable.h"
#include "../../code/varblock.h"
#include "../../code/varbuffer.h"
#include "../../code/subroutine.h"
#include "../../code/preprocessor.h"
#include "../../code/expressions/expression.h"
#include "../../code/expressions/binaryexpression.h"
#include "../../code/expressions/unaryexpression.h"
#include "../../code/expressions/intexpression.h"
#include "../../code/expressions/boolexpression.h"
#include "../../code/expressions/floatexpression.h"

using namespace AnyFX;

}

SC:				';';
CO: 			',';
COL: 			':';
LP: 			'(';
RP: 			')';
LB: 			'{';
RB: 			'}';
LL: 			'[';
RR: 			']';
DOT:			'.';
NOT:			'!';
EQ: 			'=';
QO: 			'"';
QU:				'?';
AND:			'&';
OR:				'|';
Q: 				'\'';
NU:				'#';
FORWARDSLASH: 	'\\';
LESS: 			'<';
LESSEQ:			'<=';
GREATER: 		'>';
GREATEREQ:		'>=';
LOGICEQ:		'==';
NOTEQ:			'!=';
LOGICAND:		'&&';
LOGICOR:		'||';
MOD:			'%';
	
ADD: 			'+';
SUB: 			'-';
DIV: 			'/';
MUL: 			'*';

fragment
INTEGER: ('0'..'9');

INTEGERLITERAL: INTEGER+;

// single line comment begins with // and ends with new line
COMMENT		: ('//' .* '\n') { $channel=HIDDEN; } ;

// multi line comment begins with /* and ends with */
ML_COMMENT	: '/*' .* '*/' { $channel=HIDDEN; } ;

FLOATLITERAL
	:	INTEGERLITERAL DOT INTEGER* EXPONENT? 'f'
	|	DOT INTEGER* EXPONENT? 'f'
	|	INTEGERLITERAL EXPONENT? 'f'
	;

EXPONENT: ('e'|'E') ('+'|'-')? INTEGERLITERAL;

DOUBLELITERAL 
	: 	INTEGERLITERAL DOT INTEGER* EXPONENT?
	|	DOT INTEGERLITERAL* EXPONENT?
	|	INTEGERLITERAL EXPONENT
	;

HEX
	: '0' 'x' ('0'..'9' | 'a'..'f')*
	;
	
// Any alphabetical character, both lower and upper case
fragment
ALPHABET	: ('A'..'Z'|'a'..'z');

// Identifier, must begin with alphabetical token, but can be followed by integer literal or underscore
IDENTIFIER			: ALPHABET (ALPHABET|INTEGERLITERAL|'_')*;
	
// since the lexer also needs to be able to handle preprocessor tokens, we define this rule which will do exactly the same as the 'preprocessor' parser equal, but for the lexer
PREPROCESSOR
	@init
	{
		std::string file;
	}
	: NU 'line' WS includeLine = INTEGERLITERAL WS QO (data = ~QO {file.push_back((char)$data); })* QO
	{
		int line = atoi((const char*)$includeLine.text->chars);
		LEXER->input->line = line - 1;
		if (LEXSTATE->userp) delete LEXSTATE->userp;
		LexerErrorPackage* package = new LexerErrorPackage;
		includeFileNameLexer = file;
		package->file = file;
		LEXSTATE->userp = (void*)package;
	}
	;
	
WS	: ( '\t' | ' ' | '\r' | '\n' | '\u000C' )+ { $channel = HIDDEN; } ;

string	returns [ std::string val ]
	:	QO (data = ~QO { $val.append((const char*)$data.text->chars); })* QO 
	|	Q (data = ~Q { $val.append((const char*)$data.text->chars); })* Q
	;
		
boolean returns [ bool val ]
	@init
	{
		val = false;
	}
	:	'true' { $val = true; }
	|	'false' { $val = false; }
	;
	
// main entry point
entry		returns [ Effect returnEffect ]
	:	effect { $returnEffect = $effect.effect; } EOF
	;
	
// entry point for effect, call this function to begin parsing
effect	returns [ Effect effect ]
	:  
		(
			variable { $effect.AddVariable($variable.variable); } 
			| constant { $effect.AddConstant($constant.constant);}
			| renderState { $effect.AddRenderState($renderState.state); }
			| function { $effect.AddFunction($function.function); }
			| program { $effect.AddProgram($program.program); }
			| structure { $effect.AddStructure($structure.structure); }
			| varblock { $effect.AddVarBlock($varblock.block); }
			| varbuffer { $effect.AddVarBuffer($varbuffer.buffer); }
			| subroutine { $effect.AddSubroutine($subroutine.subroutine); }
			| sampler { $effect.AddSampler($sampler.sampler); }
		)*
	;

// all types are declared in this expression
// here, we define all variable types from both HLSL and GLSL up to the latest release
type		returns [ DataType type ]
	: IDENTIFIER
	{
		std::string typeString((const char*)$IDENTIFIER.text->chars);
		if (typeString == "float") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::Float); }
		else if (typeString == "int") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::Integer); }
		else if (typeString == "bool") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::Bool); }
		else if (typeString == "uint") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::UInteger); }
		else if (typeString == "short") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::Short); }
		else if (typeString == "string") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::String); }
		else if (typeString == "void") { $type.SetStyle(DataType::Generic); $type.SetType(DataType::Void); }
		
		
		// HLSL types
		else if (typeString == "float2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Float2); }
		else if (typeString == "float3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Float3); }
		else if (typeString == "float4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Float4); }		
		else if (typeString == "int2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Integer2); }
		else if (typeString == "int3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Integer3); }
		else if (typeString == "int4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Integer4); }		
		else if (typeString == "uint2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::UInteger2); }
		else if (typeString == "uint3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::UInteger3); }
		else if (typeString == "uint4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::UInteger4); }
		else if (typeString == "short2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Short2); }
		else if (typeString == "short3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Short3); }
		else if (typeString == "short4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Short4); }
		else if (typeString == "bool2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Bool2); }
		else if (typeString == "bool3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Bool3); }
		else if (typeString == "bool4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Bool4); }
		else if (typeString == "float2x2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix2x2); }
		else if (typeString == "float2x3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix2x3); }
		else if (typeString == "float2x4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix2x4); }
		else if (typeString == "float3x2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix3x2); }
		else if (typeString == "float3x3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix3x3); }
		else if (typeString == "float3x4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix3x4); }
		else if (typeString == "float4x2") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix4x2); }
		else if (typeString == "float4x3") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix4x3); }
		else if (typeString == "float4x4") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Matrix4x4); }
		else if (typeString == "Texture1D") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler1D); }
		else if (typeString == "Texture1DArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler1DArray); }
		else if (typeString == "Texture2D") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler2D); }
		else if (typeString == "Texture2DArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler2DArray); }
		else if (typeString == "Texture2DMS") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler2DMS); }
		else if (typeString == "Texture2DMSArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler2DMSArray); }
		else if (typeString == "Texture3D") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Sampler3D); }
		else if (typeString == "TextureCube") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::SamplerCube); }
		else if (typeString == "TextureCubeArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::SamplerCubeArray); }
		else if (typeString == "RWTexture1D") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image1D); }
		else if (typeString == "RWTexture1DArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image1DArray); }
		else if (typeString == "RWTexture2D") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image2D); }
		else if (typeString == "RWTexture2DArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image2DArray); }
		else if (typeString == "RWTexture2DMS") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image2DMS); }
		else if (typeString == "RWTexture2DMSArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image2DMSArray); }
		else if (typeString == "RWTexture3D") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::Image3D); }
		else if (typeString == "RWTextureCube") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::ImageCube); }
		else if (typeString == "RWTextureCubeArray") { $type.SetStyle(DataType::HLSL); $type.SetType(DataType::ImageCubeArray); }
		
		// GLSL types
		else if (typeString == "vec2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Float2); }
		else if (typeString == "vec3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Float3); }
		else if (typeString == "vec4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Float4); }		
		else if (typeString == "ivec2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Integer2); }
		else if (typeString == "ivec3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Integer3); }
		else if (typeString == "ivec4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Integer4); }		
		else if (typeString == "uvec2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::UInteger2); }
		else if (typeString == "uvec3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::UInteger3); }
		else if (typeString == "uvec4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::UInteger4); }
		else if (typeString == "svec2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Short2); }
		else if (typeString == "svec3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Short3); }
		else if (typeString == "svec4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Short4); }
		else if (typeString == "bvec2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Bool2); }
		else if (typeString == "bvec3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Bool3); }
		else if (typeString == "bvec4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Bool4); }
		else if (typeString == "mat2x2" || typeString == "mat2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix2x2); }
		else if (typeString == "mat2x3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix2x3); }
		else if (typeString == "mat2x4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix2x4); }
		else if (typeString == "mat3x2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix3x2); }
		else if (typeString == "mat3x3" || typeString == "mat3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix3x3); }
		else if (typeString == "mat3x4") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix3x4); }
		else if (typeString == "mat4x2") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix4x2); }
		else if (typeString == "mat4x3") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix4x3); }
		else if (typeString == "mat4x4" || typeString == "mat4" ) { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Matrix4x4); }
		else if (typeString == "sampler1D") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler1D); }
		else if (typeString == "sampler1DArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler1DArray); }
		else if (typeString == "sampler2D") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler2D); }
		else if (typeString == "sampler2DArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler2DArray); }
		else if (typeString == "sampler2DMS") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler2DMS); }
		else if (typeString == "sampler2DMSArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler2DMSArray); }
		else if (typeString == "sampler3D") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Sampler3D); }
		else if (typeString == "samplerCube") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::SamplerCube); }
		else if (typeString == "samplerCubeArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::SamplerCubeArray); }
		else if (typeString == "image1D") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image1D); }
		else if (typeString == "image1DArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image1DArray); }
		else if (typeString == "image2D") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image2D); }
		else if (typeString == "image2DArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image2DArray); }
		else if (typeString == "image2DMS") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image2DMS); }
		else if (typeString == "image2DMSArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image2DMSArray); }
		else if (typeString == "image3D") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::Image3D); }
		else if (typeString == "imageCube") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::ImageCube); }
		else if (typeString == "imageCubeArray") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::ImageCubeArray); }
		else if (typeString == "atomic_uint") { $type.SetStyle(DataType::GLSL); $type.SetType(DataType::AtomicCounter); }
		
		// user defined type detected
		else { $type.SetStyle(DataType::Generic); $type.SetType(DataType::UserType); $type.SetName(typeString); }
	}
	;

// we can also define structs outside the function scopes
structure	returns [ Structure structure ]
	: 	'struct' IDENTIFIER { $structure.SetLine(LT(-2)->line ); $structure.SetPosition(LT(-2)->charPosition); $structure.SetFile(*(std::string*)LT(-2)->custom); } LB ( parameter SC { $structure.AddParameter($parameter.parameter); } )* RB SC { $structure.SetName((const char*)$IDENTIFIER.text->chars);  }
	;

// a varblock denotes a block within which we can find variables, using this structure, we can feed variables in chunks instead of individually, which may improve performance
// this is equal to DirectX constant buffers or OpenGL uniform block
// since this is just a special way of denoting a structure, we give it some extra attributes such as shared, which means this will structure will go in a shared dictionary, and be EQUAL to every shader utilizing this block
varblock	returns [ VarBlock block ]
	: 
	(qualifier = IDENTIFIER { $block.AddQualifier((const char*)$qualifier.text->chars); } )*		
	( 'buffers' EQ expression
	{
		{ $block.SetBufferExpression($expression.tree); }
	}
	)?
	'varblock' name = IDENTIFIER { $block.SetLine(LT(-2)->line ); $block.SetPosition(LT(-2)->charPosition); $block.SetFile(*(std::string*)LT(-2)->custom);} 
	(annotation { $block.SetAnnotation($annotation.annotation); })?
	LB ( variable { $block.AddVariable($variable.variable); } )* RB SC { $block.SetName((const char*)$name.text->chars); }
	;
	
// a varbuffer denotes a data type which has a dynamic size.
// varbuffers are much like varblocks, but they support for having its members in forms of unsized arrays.
// in OpenGL this is known as a shader storage block.
varbuffer	returns [ VarBuffer buffer ]
	:
	(qualifier = IDENTIFIER { $buffer.AddQualifier((const char*)$qualifier.text->chars); } )*		
	'varbuffer' name = IDENTIFIER { $buffer.SetLine(LT(-2)->line ); $buffer.SetPosition(LT(-2)->charPosition); $buffer.SetFile(*(std::string*)LT(-2)->custom); }
	(annotation { $buffer.SetAnnotation($annotation.annotation); })?
	LB ( variable {$buffer.AddVariable($variable.variable); } )* RB SC { $buffer.SetName((const char*)$name.text->chars); }
	;
	
// a subroutine denotes a function which can be dynamically switched without switching shader states.
// there are two implementations of a subroutine, the first being the subroutine variable which is a 'variable' which corresponds to a function.
// then there is the other which is an implementation based on an interface.
subroutine	returns [ Subroutine subroutine ]
	:
	'prototype' retval = type { subroutine.SetLine(LT(-2)->line ); $subroutine.SetPosition(LT(-2)->charPosition); $subroutine.SetFile(*(std::string*)LT(-2)->custom); } name = IDENTIFIER LP parameterList RP SC
	{
		$subroutine.SetSubroutineType(Subroutine::Signature); 
		$subroutine.SetName((const char*)$name.text->chars);
		$subroutine.SetParameters($parameterList.parameters);
		$subroutine.SetReturnType(retval);		
	}
	| 'subroutine' LP signature = IDENTIFIER RP { subroutine.SetLine(LT(-2)->line ); $subroutine.SetPosition(LT(-2)->charPosition); $subroutine.SetFile(*(std::string*)LT(-2)->custom); } function
	{
		$subroutine.SetName($function.function.GetName());
		$subroutine.SetSubroutineType(Subroutine::Implementation); 
		$subroutine.SetSignature((const char*)$signature.text->chars);
		$subroutine.SetFunction($function.function);
	}
	;

// values can be initialized with literal values by using the default constructor
valueList	returns [ ValueList valueList ]
	:
	first = expression { $valueList.AddValue($first.tree); } ( CO rest = expression { $valueList.AddValue($rest.tree); } )*
	;
		
// values can also be initialized with a single value, this is basically a valueList but with just a single value
valueSingleList	returns [ ValueList valueList ]
	:
	expression { $valueList.AddValue($expression.tree); }
	;
	

// variable is type, name and semicolon
variable	returns [ Variable variable ]
	:	(qualifier = IDENTIFIER { $variable.AddQualifier((const char*)$qualifier.text->chars); } )*
		declType = type name = IDENTIFIER { $variable.SetVarType($declType.type); $variable.SetName((const char*)$name.text->chars); $variable.SetLine(LT(-2)->line ); $variable.SetPosition(LT(-2)->charPosition); $variable.SetFile(*(std::string*)LT(-2)->custom); }
		( 	
			LL RR EQ 	{ $variable.SetArrayType(Variable::TypedArray); }		LB fstType = type LP fstValue = valueList RP { $variable.AddValue($fstType.type, $fstValue.valueList); }  // array initializer which assumes the size of the value list
														  ( CO cntType = type LP cntValue = valueList RP { $variable.AddValue($cntType.type, $cntValue.valueList); } )* RB 
			| LL RR EQ  { $variable.SetArrayType(Variable::SimpleArray); }		LB valList = valueList RB { $variable.AddValue($valList.valueList); }
			| LL asize0 = expression RR 					{ $variable.SetSizeExpression($asize0.tree); } 								// array variable
			| LL asize1 = expression RR 					{ $variable.SetSizeExpression($asize1.tree); $variable.SetArrayType(Variable::TypedArray); } EQ LB fstType = type LP fstValue = valueList RP { $variable.AddValue($fstType.type, $fstValue.valueList); } (CO cntType = type LP cntValue = valueList RP { $variable.AddValue($cntType.type, $cntValue.valueList); } )* RB				// array initializer for vector types
			| LL asize2 = expression RR 					{ $variable.SetSizeExpression($asize2.tree); $variable.SetArrayType(Variable::SimpleArray); } EQ LB valList = valueList RB { $variable.AddValue($valList.valueList); } 				
			| LL RR											{ $variable.SetArrayType(Variable::UnsizedArray); } 						// unsized array type, only usable for varbuffers
			| EQ defType = type LP list2 = valueList RP 	{ $variable.AddValue($defType.type, $list2.valueList); } 					// explicit variable initialization
			| EQ list3 = valueSingleList					{ $variable.AddValue($list3.valueList); }									// implicit variable initialization, only viable for generic values
		)?	
		
		(annotation { $variable.SetAnnotation($annotation.annotation); })?
		SC
	;
	
// constant value
constant	returns [ Constant constant ]
	: 'const' declType = type IDENTIFIER { $constant.SetDataType($declType.type); $constant.SetName((const char*)$IDENTIFIER.text->chars); $constant.SetLine(LT(-2)->line ); $constant.SetPosition(LT(-2)->charPosition); $constant.SetFile(*(std::string*)LT(-2)->custom);}
	(
		EQ defType = type LP sinVal1 = valueList RP SC		{ $constant.AddValue($defType.type, $sinVal1.valueList); }			// explicit variable initialization
		| EQ sinVal2 = valueSingleList SC					{ $constant.AddValue($sinVal2.valueList); }							// implicit constant initialization, only viable for generic values
		| LL RR EQ 											{ $constant.SetArrayType(Constant::TypedArray); }	LB fstType = type LP fstValue = valueList RP { $constant.AddValue($fstType.type, $fstValue.valueList); }  // array initializer which assumes the size of the value list
																						  ( CO cntType = type LP cntValue = valueList RP { $constant.AddValue($cntType.type, $cntValue.valueList); } )* RB SC
		| LL RR EQ  										{ $constant.SetArrayType(Constant::SimpleArray); }		LB valList = valueList RB { $constant.AddValue($valList.valueList); } SC
		| LL asize1 = expression RR 						{ $constant.SetSizeExpression($asize1.tree); $constant.SetArrayType(Constant::TypedArray); } EQ LB fstType = type LP fstValue = valueList RP { $constant.AddValue($fstType.type, $fstValue.valueList); } (CO cntType = type LP cntValue = valueList RP { $constant.AddValue($cntType.type, $cntValue.valueList); } )* RB SC				// array initializer for vector types
		| LL asize2 = expression RR 						{ $constant.SetSizeExpression($asize2.tree); $constant.SetArrayType(Constant::SimpleArray); } EQ LB valList = valueList RB { $constant.AddValue($valList.valueList); } SC 																																									// simple array initializer
	)
	;

// parameter modifiers denotes variables with special use, such as vertex position output, instance id input etc
parameterAttribute	returns [ Parameter::Attribute attribute ]
		@init
		{
			$attribute = Parameter::NoAttribute;
		}
		: LL IDENTIFIER RR
		{
			std::string identifierString((const char*)$IDENTIFIER.text->chars);
			
			if (identifierString == "drawinstanceID") 						{ $attribute = Parameter::DrawInstance; }
			else if (identifierString == "vertexID") 						{ $attribute = Parameter::Vertex; }
			else if (identifierString == "primitiveID") 					{ $attribute = Parameter::Primitive; }
			else if (identifierString == "invocationID") 					{ $attribute = Parameter::Invocation; }
			else if (identifierString == "viewportID") 						{ $attribute = Parameter::Viewport; }
			else if (identifierString == "rendertargetID") 					{ $attribute = Parameter::Rendertarget; }
			else if (identifierString == "innertessellation") 				{ $attribute = Parameter::InnerTessellation; }
			else if (identifierString == "outertessellation") 				{ $attribute = Parameter::OuterTessellation; }
			else if (identifierString == "position") 						{ $attribute = Parameter::Position; }
			else if (identifierString == "pointsize") 						{ $attribute = Parameter::PointSize; }
			else if (identifierString == "clipdistance") 					{ $attribute = Parameter::ClipDistance; }
			else if (identifierString == "frontface") 						{ $attribute = Parameter::FrontFace; }
			else if (identifierString == "coordinate") 						{ $attribute = Parameter::Coordinate; }
			else if (identifierString == "depth")							{ $attribute = Parameter::Depth; }
			else if (identifierString == "color0") 							{ $attribute = Parameter::Color0; }
			else if (identifierString == "color1") 							{ $attribute = Parameter::Color1; }
			else if (identifierString == "color2") 							{ $attribute = Parameter::Color2; }
			else if (identifierString == "color3") 							{ $attribute = Parameter::Color3; }
			else if (identifierString == "color4") 							{ $attribute = Parameter::Color4; }
			else if (identifierString == "color5") 							{ $attribute = Parameter::Color5; }
			else if (identifierString == "color6") 							{ $attribute = Parameter::Color6; }
			else if (identifierString == "color7") 							{ $attribute = Parameter::Color7; }
			else if (identifierString == "workgroupID")						{ $attribute = Parameter::WorkGroup; }
			else if (identifierString == "numgroups")						{ $attribute = Parameter::NumGroups; }
			else if (identifierString == "localID") 						{ $attribute = Parameter::LocalID; }
			else if (identifierString == "localindex") 						{ $attribute = Parameter::LocalIndex; }
			else if (identifierString == "globalID") 						{ $attribute = Parameter::GlobalID; }
			else 															{ $attribute = Parameter::InvalidAttribute; }
		}
		;

// defines a parameter, parameters have in/out/inout qualifiers which are used in shaders to denote how a variable should be handled
// default IO for a parameter is input (as a normal function parameter in C)
parameter	returns [ Parameter parameter ]
	: 
	(parameterAttribute 
		{ 
			$parameter.SetAttribute($parameterAttribute.attribute); 
		} 
	)?
	(LL 'feedback' EQ LP feedbackBuffer = expression CO feedbackOffset = expression RP RR
		{
			$parameter.SetFeedbackBufferExpression($feedbackBuffer.tree);
			$parameter.SetFeedbackOffsetExpression($feedbackOffset.tree);
		}
	)?
	(LL 'slot' EQ slotExpression = expression RR
		{
			$parameter.SetSlotExpression($slotExpression.tree);
		}
	)?
	(qualifier = IDENTIFIER 
		{ 
			$parameter.AddQualifier((const char*)$qualifier.text->chars); 
		} 
	)* 
	('const'
		{
			$parameter.AddQualifier("const"); 
		}
	)?
	type name = IDENTIFIER 
	{ $parameter.SetDataType($type.type); $parameter.SetName((const char*)$name.text->chars); $parameter.SetLine(LT(-2)->line ); $parameter.SetPosition(LT(-2)->charPosition); $parameter.SetFile(*(std::string*)LT(-2)->custom);}	
	( LL (size = expression { $parameter.SetSizeExpression($size.tree); } )? RR { $parameter.ForceArrayFlag(); } )?
	;
	
// parameters can either be in or out
parameterList	returns [ std::vector<Parameter> parameters ]
	: 	(firstParam = parameter { $parameters.push_back($firstParam.parameter); } (CO followParam = parameter { $parameters.push_back($followParam.parameter); } )* )? 
	;
	
// function attributes are used for tessellation, geometry and compute shading
functionAttribute	returns [ FunctionAttribute attribute ]
	: LL flag1 = IDENTIFIER RR EQ expression
	{
		std::string identifierString((const char*)$flag1.text->chars);
		
		$attribute.SetExpression(identifierString, $expression.tree);
	}
	| LL flag3 = IDENTIFIER RR EQ data = IDENTIFIER
	{
		std::string identifierString((const char*)$flag3.text->chars);
		std::string dataString((const char*)$data.text->chars);
		
		$attribute.SetString(identifierString, dataString);		
	}
	| LL flag3 = IDENTIFIER RR
	{
		std::string identifierString((const char*)$flag3.text->chars);
		
		$attribute.SetBool(identifierString, true);
	}
	;
	
// a code block denotes a series of scopes with contents.
// we don't need to get any data here, since we will fetch it from our surrounding function
codeBlock
	:	LB (codeBlock)* RB
	|	~(LB|RB)
	;
	
// function is denoted by shader type, function name, parameter list, then code block
function	returns [ Function function ]
		@init
		{
			pANTLR3_COMMON_TOKEN startToken;
			pANTLR3_COMMON_TOKEN endToken;
		}
		: (functionAttribute { $function.ConsumeAttribute($functionAttribute.attribute); } )* 
		('shader' { $function.SetShader(true); })?
		type IDENTIFIER  { $function.SetLine(LT(1)->line ); $function.SetPosition(LT(1)->charPosition); $function.SetFile(*(std::string*)LT(1)->custom); } LP parameterList RP
		{
			// save first token
			startToken = LT(2);
			
			$function.SetCodeLine(LT(2)->line );
		}
		codeBlock
		{ 
			// save last token
			endToken = LT(-2);
			pANTLR3_TOKEN_STREAM stream = PARSER->getTokenStream(PARSER);
			
			// get all text inbetween the two tokens
			pANTLR3_STRING code = stream->toStringSS(stream, startToken->index, endToken->index);
			if (code->size > 0) $function.SetCode((const char*)code->chars);
		}
		{ $function.SetName((const char*)$IDENTIFIER.text->chars); $function.SetReturnType($type.type); $function.SetParameters($parameterList.parameters); } 
		;

blendStateRow	returns [ BlendStateRow row ]
	: flag1 = IDENTIFIER LL index1 = expression RR EQ blendFlag = IDENTIFIER SC
	{
		std::string flag((const char*)$flag1.text->chars);		
		$row.SetString($index1.tree, flag, (const char*)$blendFlag.text->chars);
	}
	| flag2 = IDENTIFIER LL index2 = expression RR EQ value = expression SC
	{
		std::string flag((const char*)$flag2.text->chars);		
		$row.SetExpression($index2.tree, flag, $value.tree);
	}
	;

renderStateRow	returns [ RenderStateRow row ]
	: flag1 = IDENTIFIER EQ expression SC
	{
		std::string flag((const char*)$flag1.text->chars);
		$row.SetExpression(flag, $expression.tree);
	}
	| flag2 = IDENTIFIER EQ renderStateSetting = IDENTIFIER SC
	{
		std::string flag((const char*)$flag2.text->chars);
		$row.SetString(flag, (const char*)$renderStateSetting.text->chars);		
	}
	;

// draw state, contains a series of draw flags
renderState	returns [ RenderState state ]
	: 	'state' IDENTIFIER SC		{ $state.SetName((const char*)$IDENTIFIER.text->chars); $state.SetLine(LT(-2)->line); $state.SetPosition(LT(-2)->charPosition); $state.SetFile(*(std::string*)LT(-2)->custom); }
	| 	'state' IDENTIFIER LB 		{ $state.SetName((const char*)$IDENTIFIER.text->chars); $state.SetLine(LT(-2)->line); $state.SetPosition(LT(-2)->charPosition); $state.SetFile(*(std::string*)LT(-2)->custom); }
	( 
	  renderStateRow { $state.ConsumeRenderRow($renderStateRow.row); } 
	| blendStateRow  { $state.ConsumeBlendRow($blendStateRow.row); }	
	)* 
	RB SC
	;
		

// a sampler explains how to sample textures
sampler		returns [ Sampler sampler ]
	:	'samplerstate' IDENTIFIER SC					{ $sampler.SetName((const char*)$IDENTIFIER.text->chars); $sampler.SetLine(LT(-2)->line); $sampler.SetPosition(LT(-2)->charPosition); $sampler.SetFile(*(std::string*)LT(-2)->custom); }
	|	'samplerstate' IDENTIFIER 						{ $sampler.SetName((const char*)$IDENTIFIER.text->chars); $sampler.SetLine(LT(-1)->line); $sampler.SetPosition(LT(-1)->charPosition); $sampler.SetFile(*(std::string*)LT(-1)->custom); }
		LB ( samplerRow { $sampler.ConsumeRow($samplerRow.row); } )* RB SC
	;
	
// a sampler texture list defines a list of textures to which a sampler is to be attached
samplerTextureList	returns [ SamplerTextureList list ]
	:	firstItem = IDENTIFIER { $list.AddTexture((const char*)$firstItem.text->chars); } (CO nextItem = IDENTIFIER { $list.AddTexture((const char*)$nextItem.text->chars); })*
	;
	
samplerRow	returns [ SamplerRow row ]
	: flag1 = IDENTIFIER EQ samplerMode = IDENTIFIER SC
	{
		std::string flag((const char*)$flag1.text->chars);
		$row.SetString(flag, (const char*)$samplerMode.text->chars);
	}
	| flag2 = IDENTIFIER EQ expression SC
	{
		std::string flag((const char*)$flag2.text->chars);
		$row.SetExpression(flag, $expression.tree);
	}
	| flag4 = IDENTIFIER EQ LB samplerTextureList RB SC
	{
		std::string flag((const char*)$flag4.text->chars);
		
		if (flag == "Samplers") $row.SetTextures($samplerTextureList.list);
		else					$row.SetString(flag, "Incorrectly formatted texture list"); 
	}
	| flag5 = IDENTIFIER EQ LB red = expression CO green = expression CO blue = expression CO alpha = expression RB SC
	{
		std::string flag((const char*)$flag5.text->chars);
		$row.SetFloat4(flag, $red.tree, $green.tree, $blue.tree, $alpha.tree);
	}
	;
	
// program block, must have a vertex shader and pixel shader, the four other shaders are optional
programRow	returns [ ProgramRow row ]
	: shader = IDENTIFIER EQ name = IDENTIFIER 
	LP 
	(var = IDENTIFIER EQ implementation = IDENTIFIER 
	{ $row.SetSubroutineMapping((const char*)$var.text->chars, (const char*)$implementation.text->chars); } 
	)? 
	(CO var2 = IDENTIFIER EQ implementation2 = IDENTIFIER 
	{ $row.SetSubroutineMapping((const char*)$var2.text->chars, (const char*)$implementation2.text->chars); } 
	)*
	RP
	SC
	{
		std::string shaderString((const char*)$shader.text->chars);
		$row.SetString(shaderString, (const char*)$name.text->chars);
	}
	| 'RenderState' EQ IDENTIFIER SC
	{
		$row.SetString("RenderState", (const char*)$IDENTIFIER.text->chars);
	}
	| 'CompileFlags' EQ string SC
	{
		$row.SetString("CompileFlags", (const char*)$string.val.c_str());		
	}
	;

// annotations can be any user-specific data which can be read
annotation	returns [ Annotation annotation ]
	: 	
	LL	
	{ $annotation.SetLine(LT(-1)->line); $annotation.SetPosition(LT(-1)->charPosition); $annotation.SetFile(*(std::string*)LT(-1)->custom); }
	(type IDENTIFIER EQ value = 
		(string { $annotation.AddString($string.val); }
		| expression { $annotation.AddExpression($expression.tree); }
		) SC { $annotation.AddType($type.type); $annotation.AddName((const char*)$IDENTIFIER.text->chars);  }
	)* 
	RR
	;
	
// a program is a complete pipeline
program		returns [ Program program ]
	: 	'program' IDENTIFIER { $program.SetLine(LT(-1)->line ); $program.SetPosition(LT(-1)->charPosition); $program.SetFile(*(std::string*)LT(1)->custom); }
		(annotation { $program.SetAnnotation($annotation.annotation); })? LB 
		{ $program.SetName((const char*)$IDENTIFIER.text->chars);  }
		(programRow { $program.ConsumeRow($programRow.row); })* RB SC 
	;

// an expression in AnyFX is a constant time expression which can be evaluated during compile time
expression	returns [ Expression* tree ]
					@init
					{
						tree = 0;
					}
	:	binaryexp7 { $tree = $binaryexp7.tree; }
	;
	
// start of with ||
binaryexp7	returns [ Expression* tree ]
					@init
					{
						Expression* prev = 0;
						tree = 0;
					}
					:
					e1 = binaryexp6 { $tree = $e1.tree;	$tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); } ( ( LOGICOR ) e2 = binaryexp6
					{ 
						Expression* lhs = 0;
												
						if (prev)
						{
							lhs = new BinaryExpression("||", prev, $e2.tree); 
						}
						else
						{
							lhs = new BinaryExpression("||", $e1.tree, $e2.tree); 
						}
						
						lhs->SetLine(LT(1)->line);
						lhs->SetPosition(LT(1)->charPosition);
						lhs->SetFile(*(std::string*)LT(-1)->custom);
						
						prev = lhs;
						$tree = lhs;
					}
					)*
					;	
					
// then solve &&	
binaryexp6			returns [ Expression* tree ]
					@init
					{
						Expression* prev = 0;
						tree = 0;
					}
					:
					e1 = binaryexp5 { $tree = $e1.tree;	$tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); } ( ( LOGICAND ) e2 = binaryexp5
					{ 
						Expression* lhs = 0;
												
						if (prev)
						{
							lhs = new BinaryExpression("&&", prev, $e2.tree); 
						}
						else
						{
							lhs = new BinaryExpression("&&", $e1.tree, $e2.tree); 
						}
						
						lhs->SetLine(LT(1)->line);
						lhs->SetPosition(LT(1)->charPosition);
						lhs->SetFile(*(std::string*)LT(-1)->custom);
						
						prev = lhs;
						$tree = lhs;
					}
					)*
					;
					
//  == and !=										
binaryexp5			returns [ Expression* tree ]
					@init
					{
						Expression* prev = 0;
						tree = 0;
					}
					: 
					e1 = binaryexp4 { $tree = $e1.tree;	$tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); } ( op = ( LOGICEQ | NOTEQ ) e2 = binaryexp4
					{ 
						Expression* lhs = 0;
												
						if (prev)
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, prev, $e2.tree); 
						}
						else
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, $e1.tree, $e2.tree); 
						}
						
						lhs->SetLine(LT(1)->line);
						lhs->SetPosition(LT(1)->charPosition);
						lhs->SetFile(*(std::string*)LT(-1)->custom);
						
						prev = lhs;
						$tree = lhs;
					}
					)*
					;
					
// <, >, <= and >=			
binaryexp4			returns [ Expression* tree ]
					@init
					{
						Expression* prev = 0;
						tree = 0;
					}
					:
					e1 = binaryexp3 { $tree = $e1.tree;	$tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); } ( op = ( LESS | GREATER | LESSEQ | GREATEREQ ) e2 = binaryexp3
					{ 
						Expression* lhs = 0;
												
						if (prev)
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, prev, $e2.tree); 
						}
						else
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, $e1.tree, $e2.tree); 
						}
						
						lhs->SetLine(LT(1)->line);
						lhs->SetPosition(LT(1)->charPosition);
						lhs->SetFile(*(std::string*)LT(-1)->custom);
						
						prev = lhs;
						$tree = lhs;
					}
					)*
					;
					
// + and -	
binaryexp3			returns [ Expression* tree ]
					@init
					{
						Expression* prev = 0;
						tree = 0;
					}
					:					
					e1 = binaryexp2 { $tree = $e1.tree; $tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); } ( op = ( ADD | SUB ) e2 = binaryexp2
					{ 
						Expression* lhs = 0;
												
						if (prev)
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, prev, $e2.tree); 
						}
						else
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, $e1.tree, $e2.tree); 
						}
						
						lhs->SetLine(LT(1)->line);
						lhs->SetPosition(LT(1)->charPosition);
						lhs->SetFile(*(std::string*)LT(-1)->custom);
						
						prev = lhs;
						$tree = lhs;
					}
					)*
					;

// * and /
binaryexp2			returns [ Expression* tree ]
					@init
					{
						Expression* prev = 0;
						tree = 0;
					}
					:
					e1 = binaryexp1 { $tree = $e1.tree; $tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); } ( op = ( MUL | DIV ) e2 = binaryexp1
					{ 
						Expression* lhs = 0;
						
						if (prev)
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, prev, $e2.tree); 
						}
						else
						{
							lhs = new BinaryExpression((const char*)$op.text->chars, $e1.tree, $e2.tree); 
						}
						
						lhs->SetLine(LT(1)->line);
						lhs->SetPosition(LT(1)->charPosition);
						lhs->SetFile(*(std::string*)LT(-1)->custom);

						prev = lhs;
						$tree = lhs;
					}
					)*
					;
					
// unary expressions. Create chain of unary expressions by removing one token from the left and create new unary expressions
binaryexp1			returns [ Expression* tree ]
					@init
					{
						char operat = 0;
						tree = 0;
					}
					:
					( op = (SUB | NOT)  )? e1 = binaryexpatom
					{ 
						Expression* rhs = $e1.tree;
						
						if ($op != 0)
						{
							operat = $op.text->chars[0];
							rhs = new UnaryExpression(operat, rhs);
						}
						
						rhs->SetLine(LT(1)->line);
						rhs->SetPosition(LT(1)->charPosition);
						rhs->SetFile(*(std::string*)LT(-1)->custom);

						$tree = rhs;					                                                
					
					}
					;

// end of binary expansion, in the end, every expression can be expressed as either an ID or a new expression surrounded by paranthesis.
binaryexpatom		returns [ Expression* tree ]
					@init
					{
						tree = 0;
					}
					:
					INTEGERLITERAL  	{ $tree = new IntExpression(atoi((const char*)$INTEGERLITERAL.text->chars)); $tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); }
					| FLOATLITERAL  	{ $tree = new FloatExpression(atof((const char*)$FLOATLITERAL.text->chars)); $tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition); }
					| DOUBLELITERAL		{ $tree = new FloatExpression(atof((const char*)$DOUBLELITERAL.text->chars)); $tree->SetLine(LT(1)->line); $tree->SetPosition(LT(1)->charPosition);}
					| boolean  		
					{ 
						$tree = new BoolExpression($boolean.val);						
						$tree->SetLine(LT(1)->line);
						$tree->SetPosition(LT(1)->charPosition);
						$tree->SetFile(*(std::string*)LT(-1)->custom);
					}
					| parantexpression { $tree = $parantexpression.tree; }
					;
					
// expands an expression surrounded by paranthesis
parantexpression	returns [ Expression* tree ]
					@init
					{
						tree = 0;
					}
					:
					LP expression RP { $tree = $expression.tree; }
					;