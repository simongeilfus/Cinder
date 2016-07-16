#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#define CATCH_CONFIG_NOSTDOUT

namespace Catch {
	std::ostream &cout() { return ci::app::console(); }
	std::ostream &cerr() { return ci::app::console(); }
}

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;


TEST_CASE( "Fbo Test Suite" ) {

	//____________________________________________________________________________________________________________
	SECTION( "1. Default Color Texture / No Depth" ) {
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().disableDepth() );
		
		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( ! fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}

	
	//____________________________________________________________________________________________________________
	SECTION( "2. Default Color Texture / Default Depth Buffer" ) {
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer() );
		
		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "3. Default Color Texture / Default Depth Texture" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture() );

		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->getDepthTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "4. Default Color Texture / Custom Depth Buffer" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer( GL_DEPTH_COMPONENT24 ) );

		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->getDepthTexture() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "5. Default Color Texture / Custom Depth Texture" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture( gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ) ) );

		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( fbo->getDepthTexture() );
		REQUIRE( fbo->getDepthTexture()->getInternalFormat() == GL_DEPTH_COMPONENT24 );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "6. Default Color Texture / Default Depth Buffer / Default stencil" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer().stencilBuffer() );

		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "7. Default Color Texture / Custom Depth Buffer / Default stencil" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture()
									.depthBuffer( GL_DEPTH_COMPONENT24 ).stencilBuffer() );
		
		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "8. Default Color Texture / Default Depth Texture / Default stencil" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture().stencilBuffer() );

		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( fbo->getDepthTexture() );
		REQUIRE( fbo->hasDepthAttachment() );
		REQUIRE( fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "9. Custom Color Texture 2D / No Depth / No stencil" ) { 
		auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
									.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) )
									.disableDepth() );
		
		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( fbo->getColorTexture() );
		REQUIRE( ! fbo->getDepthTexture() );
		REQUIRE( ! fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "10. Custom Color Texture 3D / No Depth / No stencil" ) { 
		auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
									.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 256 ) )
									.disableDepth() );
				
		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
		REQUIRE( ! fbo->getColorTexture() );
		REQUIRE( ! fbo->getDepthTexture() );
		REQUIRE( ! fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
	
	//____________________________________________________________________________________________________________
	SECTION( "11. Custom Color Texture 2D Array / No Depth / No stencil" ) { 
		auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
									.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) )
									.disableDepth() );
		
		REQUIRE( gl::getError() == GL_NO_ERROR );
		REQUIRE( gl::Fbo::checkStatus() );
		REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
		REQUIRE( ! fbo->getColorTexture() );
		REQUIRE( ! fbo->getDepthTexture() );
		REQUIRE( ! fbo->hasDepthAttachment() );
		REQUIRE( ! fbo->hasStencilAttachment() );
	}
}

class FboTestSuite : public App {
public:
	FboTestSuite()
	{
		Catch::Session session;
		session.run();
	}
};


CINDER_APP( FboTestSuite, RendererGl( RendererGl::Options().debugBreak( GL_DEBUG_SEVERITY_NOTIFICATION ).debugLog( GL_DEBUG_SEVERITY_NOTIFICATION ) ) )
