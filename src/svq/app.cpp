#include "svq/app.h"

namespace svq{

	
App::App(bool retrocore) {
	m_Retrocore = retrocore;
	m_Renderer = nullptr;
}

App::~App() {
	if (m_Renderer != nullptr)
		delete m_Renderer;
	if (!m_Retrocore) {
		// SDL cleanup
		SDL_GL_DeleteContext( m_Context );
    SDL_DestroyWindow( m_Window );
    SDL_Quit();
	}
}

bool App::init() {
	static const int width = 480;
  static const int height = 272;

	if (!m_Retrocore) {
		// Standalone SDL version
		if (SDL_Init( SDL_INIT_VIDEO ) != 0) {
			std::cerr << "Error: " << SDL_GetError() << std::endl;
			return false;
  	}

  	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    m_Window = SDL_CreateWindow( "soVoq", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    m_Context = SDL_GL_CreateContext( m_Window );

    glDisable( GL_DEPTH_TEST );
    glClearColor( 0.5, 0.5, 0.0, 0.0 );
    glViewport( 0, 0, width, height );

	}

	// testing renderer
	m_Renderer = new gfx::Renderer_2D( width, height);

	std::cout << "app init ok" << std::endl;
	return true;
}

int App::run() {
	for( ;; ) {
		SDL_Event event;
	  while( SDL_PollEvent( &event ) ) {
	    switch( event.type ) {
	    	case SDL_QUIT:  return 0; break; // exit app
	      case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE: return 0; break; //exit app
						case SDLK_F11: // toggle fullscreen
							if(!isFullscreen){
								SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
								isFullscreen = true;
							} else {
							    SDL_SetWindowFullscreen(m_Window, 0);
							    isFullscreen = false;
							}
							break;
	          }
	          break;
	     }
	  }
		onRender();
		SDL_GL_SwapWindow( m_Window );
		SDL_Delay( 1 );
  }
}

void App::onRender() {
	 glClear( GL_COLOR_BUFFER_BIT );

  //glBindVertexArray( vao );
  //glDrawArrays( GL_TRIANGLES, 0, 6 );


}

}