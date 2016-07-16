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

//____________________________________________________________________________________________________________
TEST_CASE( "1. Default Color Texture / No Depth" ) {
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().disableDepth() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}

	
//____________________________________________________________________________________________________________
TEST_CASE( "2. Default Color Texture / Default Depth Buffer" ) {
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "3. Default Color Texture / Default Depth Texture" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture() );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "4. Default Color Texture / Custom Depth Buffer" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer( GL_DEPTH_COMPONENT24 ) );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "5. Default Color Texture / Custom Depth Texture" ) { 
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
TEST_CASE( "6. Default Color Texture / Default Depth Buffer / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer().stencilBuffer() );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "7. Default Color Texture / Custom Depth Buffer / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture()
								.depthBuffer( GL_DEPTH_COMPONENT24 ).stencilBuffer() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "8. Default Color Texture / Default Depth Texture / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture().stencilBuffer() );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "9. Custom Color Texture 2D / No Depth / No stencil" ) { 
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
TEST_CASE( "10. 3D Fbo: Custom Color Texture 3D / No Depth / No stencil" ) { 
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
TEST_CASE( "11. Layered Fbo: Custom Color Texture 2D Array / No Depth / No stencil" ) { 
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
	
//____________________________________________________________________________________________________________
TEST_CASE( "12. Custom Color Texture 2D / Default Depth Buffer / No stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "13. Custom Color Texture 2D / Default Depth Texture / No stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) )
								.depthTexture() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "14. 3D Fbo: Custom Color Texture 3D / Default Depth Buffer (Skip because Tex3D doesn't support Depth) / No stencil" ) { 
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
TEST_CASE( "15. Layered Fbo: Custom Color Texture 2D Array / Default Depth Buffer / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "16. Layered Fbo: Custom Color Texture 2D Array / Default Depth Texture / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) )
								.depthTexture() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_DEPTH_ATTACHMENT ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
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
