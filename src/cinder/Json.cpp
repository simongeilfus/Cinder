/*
 Copyright (c) 2019, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/Json.h"
#include "cinder/Utilities.h"

using namespace std;
using namespace ci;

namespace cinder { namespace json {

Json parseJson( const std::string &str, bool stripComments, bool whitespace )
{
	if( stripComments ) {
		return Json::parse( ci::json::stripComments( str, whitespace ) );		
	}
	else {
		return Json::parse( str );
	}
}

Json loadJson( const ci::fs::path &path, bool stripComments, bool whitespace )
{
	return parseJson( loadString( loadFile( path ) ), stripComments, whitespace );
}

Json loadJson( const ci::DataSourceRef &source, bool stripComments, bool whitespace )
{
	return parseJson( loadString( source ), stripComments, whitespace );
}

void writeJson( const ci::fs::path &path, const ci::json::Json &json, int indent, char indentChar, bool packArrays, bool ensureAscii )
{
	writeJson( writeFile( path ), json, indent, indentChar, packArrays, ensureAscii );
}

namespace detail {
	std::string packArrays( const std::string &str )
	{
		stringstream input, output;
		input << str;
		std::string line; 
		bool insideArray = false;
		while( std::getline( input, line ) ) {
			bool arrayStart = line.find( "[" ) != string::npos;
			bool arrayEnd = line.find( "]" ) != string::npos;
			bool addSpace = false;
			if( arrayStart ) {
				insideArray = true;
			}
			bool endLine = ! insideArray;
			if( insideArray ) {
				if( ! arrayStart ) {
					line.erase( std::remove( line.begin(), line.end(), '\t' ), line.end() );
					line = " " + line;
				}
				endLine = arrayEnd;
			}

			output << line;
			if( endLine ) {
				output << '\n';
			}

			if( insideArray && arrayEnd ) {
				insideArray = false;
			}
		}

		return output.str();
	}
}

void writeJson( const ci::DataTargetRef &target, const ci::json::Json &json, int indent, char indentChar, bool packArrays, bool ensureAscii )
{
	std::string str =  json.dump( indent, indentChar, ensureAscii );
	if( packArrays ) {
		str = detail::packArrays( str );
	}
	target->getStream()->writeData( str.data(), str.size() );
}

namespace detail {
inline void updateLeavesImpl( Json *base, const Json &leaves )
{
	for( auto leafIt = leaves.cbegin(); leafIt != leaves.cend(); ++leafIt ) {
		if( leafIt->is_object() ) { // recurse on dictionaries / objects
			auto child = base->find( leafIt.key() );
			if( child == base->end() ) // no child with this name; add it
				(*base)[leafIt.key()] = *leafIt;
			else
				updateLeavesImpl( &*child, *leafIt );
		}
		else
			(*base)[leafIt.key()] = *leafIt;
	}
}
}

//! Replaces or appends leaf keys
Json updateLeaves( const Json &base, const Json &leaves )
{
	Json result = base;
	detail::updateLeavesImpl( &result, leaves );        
	return result;
}

// strip comment impl. from https://github.com/andrew-d/json-strip/
namespace detail {
enum class CommentType { NONE, SINGLE, MULTI };

typedef void (*stripFunction)(const std::string& str, size_t start, size_t end, std::string& out);

inline void stripWithoutWhitespace(const std::string& str, size_t start, size_t end, std::string& out) {
	// Do nothing.
	((void)str);
	((void)start);
	((void)end);
	((void)out);
	return;
}

inline void stripWithWhitespace(const std::string& str, size_t start, size_t end, std::string& out) {
	for (size_t i = start; i < end; i++) {
		char ch = str[i];

		if (isspace(ch)) {
			out.push_back(ch);
		} else {
			out.push_back(' ');
		}
	}
}


template <typename StripFunc>
inline std::string stripCommentsImpl(const std::string& str, StripFunc strip) {
	std::string ret;
	ret.reserve(str.length());

	char currentChar, nextChar;
	bool insideString = false;
	CommentType commentType = CommentType::NONE;

	size_t offset = 0;
	for (size_t i = 0; i < str.length(); i++) {
		currentChar = str[i];

		if (i < str.length() - 1) {
			nextChar = str[i + 1];
		} 
		else {
			nextChar = '\0';
		}

		// If we're not in a comment, check for a quote.
		if (commentType == CommentType::NONE && currentChar == '"') {
			bool escaped = false;

			// If the previous character was a single slash, and the one before
			// that was not (i.e. the previous character is escaping this quote
			// and is not itself escaped), then the quote is escaped.
			if (i >= 2 && str[i - 1] == '\\' && str[i - 2] != '\\') {
				escaped = true;
			}

			if (!escaped) {
				insideString = !insideString;
			}
		}

		if (insideString) {
			continue;
		}

		if (commentType == CommentType::NONE && currentChar == '/' && nextChar == '/') {
			ret.append(str, offset, i - offset);
			offset = i;
			commentType = CommentType::SINGLE;

			// Skip second '/'
			i++;
		} 
		else if (commentType == CommentType::SINGLE && currentChar == '\r' && nextChar == '\n') {
			// Skip '\r'
			i++;

			commentType = CommentType::NONE;
			strip(str, offset, i, ret);
			offset = i;

			continue;
		} 
		else if (commentType == CommentType::SINGLE && currentChar == '\n') {
			commentType = CommentType::NONE;
			strip(str, offset, i, ret);
			offset = i;
		} 
		else if (commentType == CommentType::NONE && currentChar == '/' && nextChar == '*') {
			ret.append(str, offset, i - offset);
			offset = i;
			commentType = CommentType::MULTI;

			// Skip the '*'
			i++;
			continue;
		} 
		else if (commentType == CommentType::MULTI && currentChar == '*' && nextChar == '/') {
			// Skip '*'
			i++;

			commentType = CommentType::NONE;
			strip(str, offset, i + 1, ret);
			offset = i + 1;
			continue;
		}
	}

	ret.append(str, offset, str.length() - offset);
	ret.shrink_to_fit();
	return ret;
}

}


//! Strips comments from string
std::string stripComments( const std::string &string, bool whitespace )
{
	if( whitespace ) {
		return detail::stripCommentsImpl( string, detail::stripWithWhitespace );
	} 
	else {
		return detail::stripCommentsImpl( string, detail::stripWithoutWhitespace );
	}
}

} } // namespace cinder::json