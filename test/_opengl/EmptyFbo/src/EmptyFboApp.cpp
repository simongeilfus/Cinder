#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EmptyFboApp : public App {
  public:
	EmptyFboApp();
	void draw() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;

	gl::Texture2dRef mTextureRed, mTextureGreen, mTextureBlue;
};

EmptyFboApp::EmptyFboApp()
{
	// sanity checks
	console() << "gl::getMaxSamples()\t\t\t\t\t" << gl::getMaxSamples() << endl;
	console() << "gl::getMaxColorTextureSamples()\t\t" << gl::getMaxColorTextureSamples() << endl;
	console() << "gl::getMaxDepthTextureSamples()\t\t" << gl::getMaxDepthTextureSamples() << endl;
	
	// create 3 test textures
	mTextureRed		= gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );
	mTextureGreen	= gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );
	mTextureBlue	= gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );

	// create an empty fbo and bind it
	auto fbo = gl::Fbo::create();
	gl::ScopedFramebuffer scopedFbo( fbo );
	// attach the first texture and clear it with red
	fbo->attach( mTextureRed, GL_COLOR_ATTACHMENT0, 0 );
	gl::clear( Color( 1.0f, 0.0f, 0.0f ) );
	
	// replace the first attachement by the second texture and clear it with green
	fbo->attach( mTextureGreen, GL_COLOR_ATTACHMENT0, 0 );
	gl::clear( Color( 0.0f, 1.0f, 0.0f ) );
	
	// replace the first attachement by the third texture and clear it with blue
	fbo->attach( mTextureBlue, GL_COLOR_ATTACHMENT0, 0 );
	gl::clear( Color( 0.0f, 0.0f, 1.0f ) );

	// just to make sure let's check if there's an issue with our fbo	
	gl::Fbo::checkStatus(); // now check inside gl::Fbo::attach

	// check for erros
	CI_CHECK_GL();

	// need more work here:	
	console() << "AspectRatio\t\t\t" << fbo->getAspectRatio() << endl;
	console() << "Bounds\t\t\t\t" << fbo->getBounds() << endl;
	console() << "ColorTexture\t\t" << fbo->getColorTexture() << endl;
	console() << "DepthTexture\t\t" << fbo->getDepthTexture() << endl;
	console() << "Format\t\t" << fbo->getFormat().getLabel() << endl;
	console() << "Height\t\t\t\t" << fbo->getHeight() << endl;
	console() << "Label\t\t\t\t" << fbo->getLabel() << endl;
	console() << "MultisampleId\t\t" << fbo->getMultisampleId() << endl;
	console() << "ResolveId\t\t\t" << fbo->getResolveId() << endl;
	console() << "Size\t\t\t\t" << fbo->getSize() << endl;
	console() << "Texture2d\t\t\t" << fbo->getTexture2d( GL_COLOR_ATTACHMENT0 ) << endl;
	console() << "TextureBase\t\t\t" << fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) << endl;
	console() << "Width\t\t\t\t" << fbo->getWidth() << endl;
}

void EmptyFboApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
	gl::ScopedViewport viewport( getWindowSize() );
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mTextureRed );
	gl::draw( mTextureGreen, vec2( getWindowWidth() / 3, 0.0f ) );
	gl::draw( mTextureBlue, vec2( getWindowWidth() / 3 * 2, 0.0f ) );
}

void EmptyFboApp::mouseDown( MouseEvent event )
{
	// pick the texture under the mouse
	gl::Texture2dRef tex;
	if( event.getPos().x < getWindowWidth() / 3 ) tex = mTextureRed;
	else if( event.getPos().x > getWindowWidth() / 3 * 2 ) tex = mTextureBlue;
	else tex = mTextureGreen;
	
	// create an empty fbo and bind it
	auto fbo = gl::Fbo::create();
	gl::ScopedFramebuffer scopedFbo( fbo );
	if( event.isLeft() ) {
		// attach the current texture and clear it with a random color
		fbo->attach( tex, GL_COLOR_ATTACHMENT0, 0 );
		gl::Fbo::checkStatus();

		gl::clear( Color( randFloat(), randFloat(), randFloat() ) );
	}
	else if( event.isRight() && event.isShiftDown() ) {
		// attach the current texture and draw a teapot without depth testing
		fbo->attach( tex, GL_COLOR_ATTACHMENT0, 0 );
		gl::Fbo::checkStatus();
		
		gl::ScopedDepth scopedDepth( true ); // has no effect because we don't have a depth buffer
		gl::ScopedViewport viewport( tex->getSize() );
		gl::setMatrices( CameraPersp( tex->getWidth(), tex->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
		auto teapot = gl::Batch::create( geom::Teapot(), gl::getStockShader( gl::ShaderDef().lambert() ) );
		teapot->draw();
	}
	else if( event.isRight() && event.isAltDown() ) {
		// create a depth texture
		auto depthTexture = gl::Texture2d::create( tex->getWidth(), tex->getHeight(), gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ) );

		// attach the current texture and depth texture and draw a teapot with proper depth testing
		fbo->attach( tex, GL_COLOR_ATTACHMENT0, 0 );
		fbo->attach( depthTexture, GL_DEPTH_ATTACHMENT, 0 );
		gl::Fbo::checkStatus();
		
		gl::clear( GL_DEPTH_BUFFER_BIT );

		gl::ScopedDepth scopedDepth( true );
		gl::ScopedViewport viewport( tex->getSize() );
		gl::setMatrices( CameraPersp( tex->getWidth(), tex->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
		auto teapot = gl::Batch::create( geom::Teapot(), gl::getStockShader( gl::ShaderDef().lambert() ) );
		teapot->draw();
	}
	else if( event.isRight() ) {
		// create a depth buffer
		auto depthBuffer = gl::Renderbuffer::create( tex->getWidth(), tex->getHeight(), GL_DEPTH_COMPONENT24 );

		// attach the current texture and depth buffer and draw a teapot with proper depth testing
		fbo->attach( tex, GL_COLOR_ATTACHMENT0, 0 );
		fbo->attach( depthBuffer, GL_DEPTH_ATTACHMENT );
		gl::Fbo::checkStatus();
		
		gl::clear( GL_DEPTH_BUFFER_BIT );

		gl::ScopedDepth scopedDepth( true );
		gl::ScopedViewport viewport( tex->getSize() );
		gl::setMatrices( CameraPersp( tex->getWidth(), tex->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
		auto teapot = gl::Batch::create( geom::Teapot(), gl::getStockShader( gl::ShaderDef().lambert() ) );
		teapot->draw();
	}

	// check for erros
	CI_CHECK_GL();

	// need more work here:	
	console() << "AspectRatio\t\t\t" << fbo->getAspectRatio() << endl;
	console() << "Bounds\t\t\t\t" << fbo->getBounds() << endl;
	console() << "ColorTexture\t\t" << fbo->getColorTexture() << endl;
	console() << "DepthTexture\t\t" << fbo->getDepthTexture() << endl;
	console() << "Format\t\t" << fbo->getFormat().getLabel() << endl;
	console() << "Height\t\t\t\t" << fbo->getHeight() << endl;
	console() << "Label\t\t\t\t" << fbo->getLabel() << endl;
	console() << "MultisampleId\t\t" << fbo->getMultisampleId() << endl;
	console() << "ResolveId\t\t\t" << fbo->getResolveId() << endl;
	console() << "Size\t\t\t\t" << fbo->getSize() << endl;
	console() << "Texture2d\t\t\t" << fbo->getTexture2d( GL_COLOR_ATTACHMENT0 ) << endl;
	console() << "TextureBase\t\t\t" << fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) << endl;
	console() << "Width\t\t\t\t" << fbo->getWidth() << endl;
}
void EmptyFboApp::keyDown( KeyEvent event ) 
{
	if( event.getCode() == KeyEvent::KEY_1 ) {
		// create an empty fbo, attach the 3 textures and clear them with black
		auto fbo = gl::Fbo::create();
		gl::ScopedFramebuffer scopedFbo( fbo );
		gl::framebufferTexture( mTextureRed, GL_COLOR_ATTACHMENT0, 0 );
		gl::framebufferTexture( mTextureGreen, GL_COLOR_ATTACHMENT1, 0 );
		gl::framebufferTexture( mTextureBlue, GL_COLOR_ATTACHMENT2, 0 );
		gl::Fbo::checkStatus();
		
		//gl::ScopedDrawBuffers drawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );
		GLenum buffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		gl::drawBuffers( 3, &buffers[0] );

		gl::clear( Color::black() );

		//gl::drawBuffer( GL_COLOR_ATTACHMENT0 ); // If this line is commented there's an issue with how
	}
	else if( event.getCode() == KeyEvent::KEY_0 ) {
		// create an empty fbo, attach the 3 textures and clear them with black
		auto fbo = gl::Fbo::create();
		gl::ScopedFramebuffer scopedFbo( fbo );
		gl::framebufferTexture( mTextureRed, GL_COLOR_ATTACHMENT0, 0 );
		gl::framebufferTexture( mTextureGreen, GL_COLOR_ATTACHMENT1, 0 );
		gl::framebufferTexture( mTextureBlue, GL_COLOR_ATTACHMENT2, 0 );
		gl::Fbo::checkStatus();
		
		gl::ScopedDrawBuffers drawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );
		//GLenum buffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		//gl::drawBuffers( 3, &buffers[0] );

		gl::clear( Color::black() );
	}
	else if( event.getCode() == KeyEvent::KEY_2 ) {
		// create two empty fbo2, attach renderbuffers to the first and the red texture to the other.
		// render cubes in the first and then blit it to the second
		auto colorBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureRed->getHeight() );
		auto depthBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureRed->getHeight(), GL_DEPTH_COMPONENT24 );
		auto fbo0 = gl::Fbo::create();
		{
			gl::ScopedFramebuffer scopedFbo( fbo0 );
			gl::framebufferRenderbuffer( colorBuffer, GL_COLOR_ATTACHMENT0 );
			gl::framebufferRenderbuffer( depthBuffer, GL_DEPTH_ATTACHMENT );
			gl::Fbo::checkStatus();
		
			gl::clear();

			gl::ScopedDepth scopedDepth( true );
			gl::ScopedViewport viewport( mTextureRed->getSize() );
			gl::setMatrices( CameraPersp( mTextureRed->getWidth(), mTextureRed->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
			Rand rnd( 12345 );
			auto colorCube = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
			for( int i = 0; i < 150; ++i ) {
				gl::ScopedModelMatrix scopedMat;
				gl::multModelMatrix( glm::scale( vec3( rnd.nextFloat() ) ) * glm::rotate( rnd.nextFloat(), rnd.nextVec3() ) * glm::translate( rnd.nextVec3() * rnd.nextFloat( 0.0f, 10.0f ) ) );
				colorCube->draw();
			}
		}
	
		// create the second fbo and attach its texture
		auto fbo1 = gl::Fbo::create();
		gl::ScopedFramebuffer scopedFbo( fbo1 );
		gl::framebufferTexture( mTextureRed, GL_COLOR_ATTACHMENT0, 0 );

		// blit fbo0 to fbo1
		fbo0->blitTo( fbo1, mTextureRed->getBounds(), mTextureRed->getBounds() );
	}
	else if( event.getCode() == KeyEvent::KEY_3 ) {
		// Multisampling test
		// create two empty fbo2, attach renderbuffers to the first and the red texture to the other.
		// render cubes in the first and then blit it to the second
		auto colorBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), GL_RGBA8, gl::getMaxSamples() );
		auto depthBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), GL_DEPTH_COMPONENT24, gl::getMaxSamples() );
		auto fbo0 = gl::Fbo::create();
		{
			gl::ScopedFramebuffer scopedFbo( fbo0 );
			gl::framebufferRenderbuffer( colorBuffer, GL_COLOR_ATTACHMENT0 );
			gl::framebufferRenderbuffer( depthBuffer, GL_DEPTH_ATTACHMENT );
			gl::Fbo::checkStatus();
		
			gl::clear();

			gl::ScopedDepth scopedDepth( true );
			gl::ScopedViewport viewport( mTextureGreen->getSize() );
			gl::setMatrices( CameraPersp( mTextureGreen->getWidth(), mTextureGreen->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
			Rand rnd( 12345 );
			auto colorCube = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
			for( int i = 0; i < 150; ++i ) {
				gl::ScopedModelMatrix scopedMat;
				gl::multModelMatrix( glm::scale( vec3( rnd.nextFloat() ) ) * glm::rotate( rnd.nextFloat(), rnd.nextVec3() ) * glm::translate( rnd.nextVec3() * rnd.nextFloat( 0.0f, 10.0f ) ) );
				colorCube->draw();
			}
		}
	
		// create the second fbo and attach its texture
		auto fbo1 = gl::Fbo::create();
		gl::ScopedFramebuffer scopedFbo( fbo1 );
		gl::framebufferTexture( mTextureGreen, GL_COLOR_ATTACHMENT0, 0 );

		// resolve the multisample anti-aliasing by blitting the multisample fbo to the regular one
		fbo0->blitTo( fbo1, mTextureGreen->getBounds(), mTextureGreen->getBounds() ); 
	}
	else if( event.getCode() == KeyEvent::KEY_4 ) {
		// Multisampling test
		// create two empty fbo2, attach renderbuffers to the first and the red texture to the other.
		// render cubes in the first and then blit it to the second
		auto immutable = true;
		auto colorTexture = gl::Texture2d::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), gl::Texture2d::Format().internalFormat( GL_RGBA8 ).samples( gl::getMaxSamples() ).immutableStorage( immutable ) );
		auto depthTexture = gl::Texture2d::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ).samples( gl::getMaxSamples() ).immutableStorage( immutable ) );
		auto fbo0 = gl::Fbo::create();
		{
			gl::ScopedFramebuffer scopedFbo( fbo0 );
			gl::framebufferTexture( colorTexture, GL_COLOR_ATTACHMENT0, 0 );
			gl::framebufferTexture( depthTexture, GL_DEPTH_ATTACHMENT, 0 );
			gl::Fbo::checkStatus();
			
			gl::clear();

			gl::ScopedDepth scopedDepth( true );
			gl::ScopedViewport viewport( mTextureGreen->getSize() );
			gl::setMatrices( CameraPersp( mTextureGreen->getWidth(), mTextureGreen->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
			//glEnable(GL_MULTISAMPLE);  
			Rand rnd( 12345 );
			auto colorCube = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
			for( int i = 0; i < 150; ++i ) {
				gl::ScopedModelMatrix scopedMat;
				gl::multModelMatrix( glm::scale( vec3( rnd.nextFloat() ) ) * glm::rotate( rnd.nextFloat(), rnd.nextVec3() ) * glm::translate( rnd.nextVec3() * rnd.nextFloat( 0.0f, 10.0f ) ) );
				colorCube->draw();
			}
			//glDisable(GL_MULTISAMPLE);  
		}
	
		// create the second fbo and attach its texture
		auto fbo1 = gl::Fbo::create();
		gl::ScopedFramebuffer scopedFbo( fbo1 );
		gl::framebufferTexture( mTextureBlue, GL_COLOR_ATTACHMENT0, 0 );

		// resolve the multisample anti-aliasing by blitting the multisample fbo to the regular one
		fbo0->blitTo( fbo1, mTextureGreen->getBounds(), mTextureGreen->getBounds() ); 
	}
	else if( event.getCode() == KeyEvent::KEY_5 ) {
		// Same as first test but using the usual interface
		// create an empty fbo, attach the 3 textures and clear them with black
	CI_LOG_I( "_________ Fbo creation _________ " );
		auto fbo = gl::Fbo::create( gl::Fbo::Format()
									.attachment( GL_COLOR_ATTACHMENT0, mTextureRed )
									.attachment( GL_COLOR_ATTACHMENT1, mTextureGreen )
									.attachment( GL_COLOR_ATTACHMENT2, mTextureBlue ) );
		CI_LOG_I( "_________ Fbo creation End _________ " );

		gl::ScopedFramebuffer scopedFbo( fbo );
		gl::Fbo::checkStatus();
		CI_LOG_I( "_________ ScopedDrawBuffers _________ " );
		gl::ScopedDrawBuffers drawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );

		gl::clear( Color( 1.0f, 0.0f, 0.0f ) );
	}

	// check for errors
	CI_CHECK_GL();
}

CINDER_APP( EmptyFboApp, RendererGl() )/* RendererGl::Options()
							.debugBreak( GL_DEBUG_SEVERITY_LOW )
							.debugLog( GL_DEBUG_SEVERITY_LOW ) ) )*/
