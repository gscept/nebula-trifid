//------------------------------------------------------------------------------
/**
    AnyFX compiler functions
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "cmdlineargs.h"
#include "afxcompiler.h"
#include "antlr3.h"
#include "parser/AnyFXLexer.h"
#include "parser/AnyFXParser.h"
#include "lexererrorhandler.h"
#include "parsererrorhandler.h"
#include "GL/glew.h"
#include "typechecker.h"
#include "generator.h"
#include "header.h"
#include <fstream>
#include <algorithm>

#if __linux__
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#undef Success
#elif APPLE
#include <libproc.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/OpenGL.h>
#endif

extern bool lexerError;
extern std::string lexerErrorBuffer;
extern bool parserError;
extern std::string parserErrorBuffer;
static GLenum glewInitialized = -1;

#include "mcpp_lib.h"

#if WIN32
//------------------------------------------------------------------------------
/**
*/
LRESULT CALLBACK
AnyFXWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

    HDC hDc;
    HGLRC hRc;      
    HACCEL hAccel;
    HINSTANCE hInst;
    HWND hWnd;
#elif __linux__
    Display* dsp;
    Window root;
    GLint attrs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    XVisualInfo* vi;
    Colormap cmap;
    XSetWindowAttributes swa;
    Window win;
    GLXContext glc;
    XWindowAttributes gwa;
    XEvent xev;
#elif APPLE
    CGLContextObj ctx; 
    CGLPixelFormatObj pix; 
    GLint npix; 
    CGLPixelFormatAttribute attribs[] = { 
        (CGLPixelFormatAttribute) 0
    }; 
#endif

//------------------------------------------------------------------------------
/**
*/
bool
AnyFXPreprocess(const std::string& file, const std::vector<std::string>& defines, const std::string& vendor, std::string& output)
{
    std::string fileName = file.substr(file.rfind("/")+1, file.length()-1);
    std::string curDir(file);
    unsigned lastSlash = curDir.rfind("/");
    curDir = curDir.substr(0, lastSlash);

	std::string vend = "-DVENDOR=" + vendor;

    const unsigned numargs = 4 + defines.size();
    std::string* args = new std::string[numargs];
	args[0] = std::string("-C");
	args[1] = std::string("-W 0");
	args[2] = vend.c_str();
    //args[2] = "-I" + curDir;
    unsigned i;
    for (i = 0; i < defines.size(); i++)
    {
        args[3 + i] = defines[i].c_str();
    }
    args[numargs-1] = file;

    const char** arguments = new const char*[numargs];
    
    for (i = 0; i < numargs; i++)
    {
        arguments[i] = args[i].c_str();
    }

    // run preprocessing
    mcpp_use_mem_buffers(1);
    int result = mcpp_lib_main(numargs, (char**)arguments);
    if (result != 0)
    {
		char* preprocessed = mcpp_get_mem_buffer(ERR); // get error
		output.append(preprocessed);
		mcpp_use_mem_buffers(1);	// clear memory
        delete[] args;
		delete[] arguments;
        return false;
    }
    else
    {
        char* preprocessed = mcpp_get_mem_buffer(OUT); // get output
        output.append(preprocessed);
		mcpp_use_mem_buffers(1);	// clear memory
		delete[] args;
		delete[] arguments;
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Compiles AnyFX effect.

    @param file			Input file to compile
    @param output		Output destination file
    @param target		Target language
	@param vendor		GPU vendor name
    @param defines		List of preprocessor definitions
    @param errorBuffer	Buffer containing errors, created in function but must be deleted manually
*/
bool
AnyFXCompile(const std::string& file, const std::string& output, const std::string& target, const std::string& vendor, const std::vector<std::string>& defines, const std::vector<std::string>& flags, AnyFXErrorBlob** errorBuffer)
{
    std::string preprocessed;
    (*errorBuffer) = NULL;

    // if preprocessor is successful, continue parsing the actual code
	if (AnyFXPreprocess(file, defines, vendor, preprocessed))
    {
        // when we preprocess, we save it 
        pANTLR3_INPUT_STREAM input;
        pAnyFXLexer lex;
        pAnyFXParser parser;
        pANTLR3_COMMON_TOKEN_STREAM tokens;

        input = antlr3StringStreamNew((pANTLR3_UINT8)preprocessed.c_str(), ANTLR3_ENC_UTF8, preprocessed.size(), (pANTLR3_UINT8)file.c_str());
        lex = AnyFXLexerNew(input);
        tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
        parser = AnyFXParserNew(tokens);

        // create new effect
        Effect effect = parser->entry(parser);

        // stop the process if lexing or parsing fails
        if (!lexerError && !parserError)
        {
            // create header
            Header header;
            header.SetProfile(target);
			header.SetFlags(flags);

            // set effect header and setup effect
            effect.SetHeader(header);
            effect.Setup();

            if (header.GetType() == Header::GLSL)
            {
                std::string ext = Format("GL_VERSION_%d_%d", header.GetMajor(), header.GetMinor());
                if (!glewIsSupported(ext.c_str()))
                {
                    printf("OpenGL version %d.%d is not supported by the hardware.\n", header.GetMajor(), header.GetMinor());

					// destroy compiler state and return
					parser->free(parser);
					tokens->free(tokens);
					lex->free(lex);
					input->free(input);
                    return false;
                }
            }

            // create type checker
            TypeChecker typeChecker;

            // type check effect
            typeChecker.SetHeader(header);
            effect.TypeCheck(typeChecker);

            // compile effect
            int typeCheckerStatus = typeChecker.GetStatus();
            if (typeCheckerStatus == TypeChecker::Success || typeCheckerStatus == TypeChecker::Warnings)
            {
                // create code generator
                Generator generator;

                // generate code for effect
                generator.SetHeader(header);
                effect.Generate(generator);

                // set warnings as 'error' buffer
                if (typeCheckerStatus == TypeChecker::Warnings)
                {
                    unsigned warnings = typeChecker.GetWarningCount();
                    std::string errorMessage;
                    errorMessage = typeChecker.GetErrorBuffer();
                    errorMessage = errorMessage + Format("Type checking returned with %d warnings\n", warnings);

                    *errorBuffer = new AnyFXErrorBlob;
                    (*errorBuffer)->buffer = new char[errorMessage.size()];
                    (*errorBuffer)->size = errorMessage.size();
                    errorMessage.copy((*errorBuffer)->buffer, (*errorBuffer)->size);
                    (*errorBuffer)->buffer[(*errorBuffer)->size-1] = '\0';
                }

                if (generator.GetStatus() == Generator::Success)
                {
                    // create binary writer
                    BinWriter writer;
                    writer.SetPath(output);
                    if (writer.Open())
                    {
                        // compile and write to binary writer
                        effect.Compile(writer);

                        // close writer and finish file
                        writer.Close();

						// destroy compiler state and return
						parser->free(parser);
						tokens->free(tokens);
						lex->free(lex);
						input->free(input);						
                        return true;
                    }
                    else
                    {
                        std::string errorMessage = Format("File '%s' could not be opened for writing\n", output.c_str());
                        *errorBuffer = new AnyFXErrorBlob;
                        (*errorBuffer)->buffer = new char[errorMessage.size()];
                        (*errorBuffer)->size = errorMessage.size();
                        errorMessage.copy((*errorBuffer)->buffer, (*errorBuffer)->size);
                        (*errorBuffer)->buffer[(*errorBuffer)->size-1] = '\0';

						// destroy compiler state and return
						parser->free(parser);
						tokens->free(tokens);
						lex->free(lex);
						input->free(input);
                        return false;
                    }
                }
                else
                {
                    unsigned errors = generator.GetErrorCount();
                    unsigned warnings = generator.GetWarningCount();
                    std::string errorMessage;
                    errorMessage = generator.GetErrorBuffer();
                    errorMessage = errorMessage + Format("Code generation failed with %d errors and %d warnings\n", errors, warnings);

                    *errorBuffer = new AnyFXErrorBlob;
                    (*errorBuffer)->buffer = new char[errorMessage.size()];
                    (*errorBuffer)->size = errorMessage.size();
                    errorMessage.copy((*errorBuffer)->buffer, (*errorBuffer)->size);
                    (*errorBuffer)->buffer[(*errorBuffer)->size-1] = '\0';

					// destroy compiler state and return
					parser->free(parser);
					tokens->free(tokens);
					lex->free(lex);
					input->free(input);
                    return false;
                }
            }
            else
            {
                unsigned errors = typeChecker.GetErrorCount();
                unsigned warnings = typeChecker.GetWarningCount();
                std::string errorMessage;
                errorMessage = typeChecker.GetErrorBuffer();
                errorMessage = errorMessage + Format("Type checking failed with %d errors and %d warnings\n", errors, warnings);

                *errorBuffer = new AnyFXErrorBlob;
                (*errorBuffer)->buffer = new char[errorMessage.size()];
                (*errorBuffer)->size = errorMessage.size();
                errorMessage.copy((*errorBuffer)->buffer, (*errorBuffer)->size);
                (*errorBuffer)->buffer[(*errorBuffer)->size-1] = '\0';

				// destroy compiler state and return
				parser->free(parser);
				tokens->free(tokens);
				lex->free(lex);
				input->free(input);
                return false;
            }
        }
        else
        {
            std::string errorMessage;
            errorMessage.append(lexerErrorBuffer);
            errorMessage.append(parserErrorBuffer);
            lexerError = false;
            parserError = false;
            lexerErrorBuffer.clear();
            parserErrorBuffer.clear();

            *errorBuffer = new AnyFXErrorBlob;
            (*errorBuffer)->buffer = new char[errorMessage.size()];
            (*errorBuffer)->size = errorMessage.size();
            errorMessage.copy((*errorBuffer)->buffer, (*errorBuffer)->size);
            (*errorBuffer)->buffer[(*errorBuffer)->size-1] = '\0';

			// destroy compiler state and return
			parser->free(parser);
			tokens->free(tokens);
			lex->free(lex);
			input->free(input);
            return false;
        }
    }
    else
    {
		if (output.length() > 0)
		{
			size_t size = output.size();
			*errorBuffer = new AnyFXErrorBlob;
			(*errorBuffer)->buffer = new char[size];
			(*errorBuffer)->size = size;
			memcpy((void*)(*errorBuffer)->buffer, (void*)output.c_str(), size);
			(*errorBuffer)->buffer[size - 1] = '\0';
		}
        return false;
    }	
}

//------------------------------------------------------------------------------
/**
    Run before compilation
*/
void
AnyFXBeginCompile()
{
#if WIN32
    HDC hDc;
    HGLRC hRc;      
    HINSTANCE hInst;
    HWND hWnd;

    hInst = GetModuleHandle(0);

    HICON icon = LoadIcon(NULL, IDI_APPLICATION);
    // register window class
    WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.cbSize        = sizeof(wndClass);
    wndClass.style         = CS_DBLCLKS | CS_OWNDC;
    wndClass.lpfnWndProc   = AnyFXWinProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(void*);   // used to hold 'this' pointer
    wndClass.hInstance     = hInst;
    wndClass.hIcon         = icon;
    wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wndClass.lpszMenuName  = NULL;
    wndClass.lpszClassName = "AnyFX::Compiler";
    wndClass.hIconSm       = NULL;
    RegisterClassEx(&wndClass);

    DWORD windowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
    DWORD extendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

    RECT		windowRect;				// Grabs Rectangle Upper Left / Lower Right Values
    windowRect.left=(long)0;			// Set Left Value To 0
    windowRect.right=(long)0;		// Set Right Value To Requested Width
    windowRect.top=(long)0;				// Set Top Value To 0
    windowRect.bottom=(long)0;		// Set Bottom Value To Requested Height
    AdjustWindowRectEx(&windowRect, windowStyle, FALSE, extendedStyle);		// Adjust Window To True Requested Size

    // open window
    hWnd = CreateWindow("AnyFX::Compiler",
        "AnyFX Compiler",					
        windowStyle,					
        0,								
        0,								
        windowRect.right-windowRect.left,						
        windowRect.bottom-windowRect.top,						
        NULL,							
        NULL,                             
        hInst,                      
        NULL);          


    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                       //Number of bits for the depthbuffer
        8,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    hDc = GetDC(hWnd);
    int pixelFormat = ChoosePixelFormat(hDc, &pfd);
    SetPixelFormat(hDc, pixelFormat, &pfd);
    hRc = wglCreateContext(hDc);
    wglMakeCurrent(hDc, hRc);
#elif __linux__
    dsp = XOpenDisplay(NULL);
    if (dsp == NULL)
    {
        Emit("Could not connect to X.\n");
    }
    root = DefaultRootWindow(dsp);
    vi = glXChooseVisual(dsp, 0, attrs);
    if (vi == NULL)
    {
        Emit("Could not create visual.\n");
    }
    cmap = XCreateColormap(dsp, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;
    win = XCreateWindow(dsp, root, 0, 0, 1024, 768, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    XStoreName(dsp, win, "AnyFX Compiler");
    XMapWindow(dsp, win);
    glc = glXCreateContext(dsp, vi, NULL, GL_TRUE);
    glXMakeCurrent(dsp, win, glc);

    XNextEvent(dsp, &xev);

    if (xev.type == Expose)
    {
        XGetWindowAttributes(dsp, win, &gwa);
        glXSwapBuffers(dsp, win);
    }
#elif APPLE
    CGLChoosePixelFormat(attribs, &pix, &npix); 
    CGLCreateContext(pix, NULL, &ctx); 
    CGLSetCurrentContext(ctx);
#endif

	if (glewInitialized != GLEW_OK)
	{
		glewInitialized = glewInit();
	}

#ifndef __ANYFX_COMPILER_LIBRARY__
	if (glewInitialized != GLEW_OK)
    {
        Emit("Glew failed to initialize!\n");
    }

    printf("AnyFX OpenGL capability report:\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR)); 
    printf("Renderer: %s\n", glGetString(GL_RENDERER)); 
    printf("Version:  %s\n", glGetString(GL_VERSION)); 
    printf("GLSL:     %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

//------------------------------------------------------------------------------
/**
    Run after compilation
*/
void
AnyFXEndCompile()
{
#if (WIN32)
    DestroyWindow(hWnd);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRc);
#elif (__linux__)
    glXMakeCurrent(dsp, None, NULL);
    glXDestroyContext(dsp, glc);
    XDestroyWindow(dsp, win);
    XCloseDisplay(dsp);
#elif (APPLE)
#endif
}
