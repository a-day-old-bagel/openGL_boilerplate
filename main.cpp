/*
 * In main.cpp, all openGL boilerplate stuff is done and the main loop is handled.
 */

#include <iostream>
#include <SDL.h>
#include <GL/glew.h>

#include "SimpleSquare.h"

#define RESOLUTION_X 800
#define RESOLUTION_Y 600
#define GLVERSION_MAJOR 3
#define GLVERSION_MINOR 0
#define SCREENOPTIONS SDL_WINDOW_OPENGL // | SDL_WINDOW_FULLSCREEN_DESKTOP
#define USE_VSYNC
#define CLEARCOLOR 0.0f,0.0f,0.0f,1.0f
#define USE_TRANSPARENCY
#define USE_DEPTHBUFFER
#define USE_BACKFACE_CULLING
#define DEBUG_DRIVERS
#define OUTSTREAM std::cout

SDL_Window* pWindow;    // The SDL window
SDL_GLContext context;  // The openGL context
SimpleSquare square;

void mainLoop();
bool initGL ();
void checkGlError(int line = -1);
void checkSDLError(int line = -1);
void cleanup();

int main() {
	if (!initGL()) { // if init fails, exit
		return 1;
	}
    square.init();
    mainLoop();
    cleanup();
    return 0;
}

void mainLoop() {
    bool loop = true;
    while (loop) {
        /**************************** HANDLE EVENTS *********************************/
        SDL_Event event;
        while (SDL_PollEvent(&event)) { // process all accumulated events
            if (event.type == SDL_QUIT) { // quit events (like if you press the x)
                loop = false;
            } else if (event.type == SDL_KEYDOWN) { // keydown events (generated only once per key press)
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        loop = false;
                        break;
                    case SDLK_LEFT:
                        square.translate(-0.01f, 0.0f, 0.0f);
                        break;
                    case SDLK_RIGHT:
                        square.translate(0.01f, 0.0f, 0.0f);
                        break;
                    case SDLK_UP:
                        square.translate(0.0f, 0.01f, 0.0f);
                        break;
                    case SDLK_DOWN:
                        square.translate(0.0f, -0.01f, 0.0f);
                        break;
                    default:
                        break;
                }
            }
        }
        /**************************** DO THE DRAWING *********************************/
        // clear the buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // draw the square
        square.draw();
        // swap frame buffers (back buffer was drawn, will now be shown)
        SDL_GL_SwapWindow(pWindow);
    }
}

bool initGL() {
	#ifdef DEBUG_DRIVERS /************ DEBUG DRIVERS ********************************/
	// Identify video drivers (this is for debug)
    OUTSTREAM << "\nSDL found the following video drivers:\n";
    int numdrivers = SDL_GetNumVideoDrivers();
    const char* drivername;
    for (int i = 0; i < numdrivers; ++i) {
        drivername = SDL_GetVideoDriver(i);
        if (SDL_VideoInit(drivername) == 0) {
            SDL_VideoQuit();
            OUTSTREAM << "\t\t   Driver " << drivername << " works.\n";
        }
        else {
            OUTSTREAM << "\t<!>\tDriver " << drivername << " DOES NOT WORK!\n";
        }
    }
    // Identify render drivers (this is for debug)
    OUTSTREAM << "SDL found the following render drivers: ";
    numdrivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < numdrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        OUTSTREAM << info.name << ((i < numdrivers - 1)? ", " : "\n");
    }
    #endif /************************** DEBUG DRIVERS ********************************/

    // Initialize SDL overall.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        OUTSTREAM << "<!>    SDL did not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Specify OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLVERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLVERSION_MINOR);

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create the SDL window
    pWindow = SDL_CreateWindow("OpenGL Window",	// name of window
                    SDL_WINDOWPOS_CENTERED,		// x position of window
                    SDL_WINDOWPOS_CENTERED,		// y position of window
                    RESOLUTION_X, RESOLUTION_Y,	// x and y width of window
                    SCREENOPTIONS);    			// options (fullscreen, etc)

    // If the window couldn't be created for whatever reason
    if (pWindow == NULL) {
        OUTSTREAM << "<!>    SDL window was not created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
	}
	else {
		OUTSTREAM << "SDL window created.\n";
	}

    //Create context
    context = SDL_GL_CreateContext(pWindow);
    if(context == NULL) {
        OUTSTREAM << "<!>    OpenGL context was not created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
	}
	else {
		OUTSTREAM << "GL context created.\n";
	}

	checkGlError(__LINE__);

    //Initialize GLEW (openGL Extensions Wrangler)
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();  // GL enumerator error is thrown here when using openGL versions 3.2+ It's fine.
                                    // see https://www.opengl.org/wiki/OpenGL_Loading_Library
    if(glewError != GLEW_OK) {
        OUTSTREAM << "<!>    Could not initialize GLEW! " << glewGetErrorString(glewError) << std::endl;
        return false;
	}
	else {
		OUTSTREAM << "GLEW initialized.\n";
	}

	checkGlError(__LINE__);


    #ifdef USE_VSYNC /************ USE VSYNC ********************************/
	if (SDL_GL_SetSwapInterval(1) < 0) {
		OUTSTREAM << "<!>    Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
		// Do not return
	}
	else {
		OUTSTREAM << "VSync enabled.\n";
	}
	#endif /********************** USE VSYNC ********************************/
    
    // set openGL clear color
    glClearColor(CLEARCOLOR);

    #ifdef USE_DEPTHBUFFER
    // enable the occlusion of objects hidden behind other objects (using the depth buffer)
    glEnable(GL_DEPTH_TEST);				
    // set the test for that occlusion to: "draw if distance of new fragment is less than or equal to old fragment"
    glDepthFunc(GL_LEQUAL);
    #endif

    #ifdef USE_BACKFACE_CULLING
    // enable the culling of back faces (back faces will not be drawn)
    glEnable(GL_CULL_FACE);
    #endif

    #ifdef USE_TRANSPARENCY
    // enable transparency
    glEnable (GL_BLEND);
    // set transparnecy function (this is standard transparency)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    #endif

	checkGlError(__LINE__);
    checkSDLError(__LINE__);

    return true;
}

/*
 * DO NOT call this function before the gl context has initialized.  It will make an infinite loop.
 * This is because glGetError always returns GL_INVALID_OPERATION before there exists a context. There's
 * code in here to catch the loop and stop, but it will still produce unnecessary scary output.
 */
void checkGlError(int line /*= -1*/) {
    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR) {
        OUTSTREAM << "<!>    GL error(s) detected";
        if (line > 0) {
            OUTSTREAM << " at line " << line;
        }
        OUTSTREAM << ":\n";
        int loopGuard = 0;
        while (glErr != GL_NO_ERROR) {
            if (++loopGuard <= 10) {
                OUTSTREAM << "\t\t\t" << gluErrorString(glErr) << std::endl;
                glErr = glGetError();
            }
            else {
                OUTSTREAM << "\t\t\t<!> Suppressing further errors...\n";
                break;
            }
        }
    }
}

void checkSDLError(int line /*= -1*/) {
    std::string error = SDL_GetError();
    if (error != "") {
        OUTSTREAM << "<!>    SLD Error";
        if (line > 0) {
            OUTSTREAM << " at line " << line;
        }
        OUTSTREAM << ": " << error;
        SDL_ClearError();
    }
}

void cleanup() {
    // Delete our OpengL context
    SDL_GL_DeleteContext(context);

    // Destroy our window
    SDL_DestroyWindow(pWindow);

    // Shutdown SDL 2
    SDL_Quit();
}