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

#pragma once

#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"

#include "nlohmann/json.hpp"

namespace cinder { namespace json {

using Json = nlohmann::json;

//! Parses Json from a string, optionally stripping comments
CI_API Json parseJson( const std::string &str, bool stripComments = false, bool whitespace = true );

//! Loads Json from a file path, optionally stripping comments
CI_API Json loadJson( const ci::fs::path &path, bool stripComments = false, bool whitespace = true );
//! Loads Json from a DataSource, optionally stripping comments
CI_API Json loadJson( const ci::DataSourceRef &source, bool stripComments = false, bool whitespace = true );
//! Saves Json to a file at \a path, optionally specifying indentation, array format or ascii options
CI_API void writeJson( const ci::fs::path &path, const ci::json::Json &json, int indent = -1, char indentChar = ' ', bool packArrays = false, bool ensureAscii = false );
//! Saves Json to a file at \a target, optionally specifying indentation, array format or ascii options
CI_API void writeJson( const ci::DataTargetRef &target, const ci::json::Json &json, int indent = -1, char indentChar = ' ', bool packArrays = false, bool ensureAscii = false );

//! Replaces or appends leaf keys; useful when merging two Json
CI_API Json updateLeaves( const Json &base, const Json &leaves );
//! Strips comments from string
CI_API std::string stripComments( const std::string &string, bool whitespace = true );

} } // namespace cinder::json

#if ! defined( CINDER_JSON_DISABLE_SERIALIZERS )

#include "cinder/Rect.h"
#include "cinder/Matrix.h"
#include "cinder/Color.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Plane.h"
#include "cinder/Ray.h"
#include "cinder/Url.h"
#include "glm/gtc/type_ptr.hpp"

// custom type support has to be done within nlohmann namespace using argument-dependent lookup
// see official documentation here: https://github.com/nlohmann/json#how-do-i-convert-third-party-types

namespace nlohmann {

//! Serializer for tvec2
template <typename T, glm::precision P>
struct adl_serializer<glm::tvec2<T,P>> {
	static void to_json( json& j, const glm::tvec2<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 2 ); }
	static void from_json( const json &j, glm::tvec2<T,P> &v ) { std::vector<T> vec = j; v = glm::make_vec2( vec.data() ); }
};

//! Serializer for tvec3
template <typename T, glm::precision P>
struct adl_serializer<glm::tvec3<T,P>> {
	static void to_json( json& j, const glm::tvec3<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 3 ); }
	static void from_json( const json &j, glm::tvec3<T,P> &v ) { std::vector<T> vec = j; v = glm::make_vec3( vec.data() ); }
};

//! Serializer for tvec4
template <typename T, glm::precision P>
struct adl_serializer<glm::tvec4<T,P>> {
	static void to_json( json& j, const glm::tvec4<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 4 ); }
	static void from_json( const json &j, glm::tvec4<T,P> &v ) { std::vector<T> vec = j; v = glm::make_vec4( vec.data() ); }
};

//! Serializer for tquat
template <typename T, glm::precision P>
struct adl_serializer<glm::tquat<T,P>> {
	static void to_json( json& j, const glm::tquat<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 4 ); }
	static void from_json( const json &j, glm::tquat<T,P> &v ) { std::vector<T> vec = j; v = glm::make_quat( vec.data() ); }
};

//! Serializer for tmat3x3
template <typename T, glm::precision P>
struct adl_serializer<glm::tmat3x3<T,P>> {
	static void to_json( json& j, const glm::tmat3x3<T,P> &m ) { j = std::vector<T>( &m[0][0], &m[0][0] + 3 * 3 ); }
	static void from_json( const json &j, glm::tmat3x3<T,P> &m ) { std::vector<T> vec = j; m = glm::make_mat3x3( vec.data() ); }
};

//! Serializer for tmat4x4
template <typename T, glm::precision P>
struct adl_serializer<glm::tmat4x4<T,P>> {
	static void to_json( json& j, const glm::tmat4x4<T,P> &m ) { j = std::vector<T>( &m[0][0], &m[0][0] + 4 * 4 ); }
	static void from_json( const json &j, glm::tmat4x4<T,P> &m ) { std::vector<T> vec = j; m = glm::make_mat4x4( vec.data() ); }
};

//! Serializer for RectT
template <typename T>
struct adl_serializer<ci::RectT<T>> {
	static void to_json( json& j, const ci::RectT<T> &r ) { j = glm::tvec4<T>( r.getUpperLeft(), r.getLowerRight() ); }
	static void from_json( const json &j, ci::RectT<T> &r ) { glm::tvec4<T> v = j; r = ci::RectT<T>( v.x, v.y, v.z, v.w ); }
};

//! Serializer for Area
template <>
struct adl_serializer<ci::Area> {
	static void to_json( json& j, const ci::Area &a ) { j = glm::ivec4( a.getUL(), a.getLR() );	}
	static void from_json( const json &j, ci::Area &a ) { glm::ivec4 v = j; a = ci::Area( v.x, v.y, v.z, v.w ); }
};

//! Serializer for ColorT
template <typename T>
struct adl_serializer<ci::ColorT<T>> {
	static void to_json( json& j, const ci::ColorT<T> &c ) { j = std::vector<T> { c.r, c.g, c.b }; }
	static void from_json( const json &j, ci::ColorT<T> &c ) { std::vector<T> vec = j; c = ci::ColorT<T>( vec[0], vec[1], vec[2] ); }
};

//! Serializer for ColorAT
template <typename T>
struct adl_serializer<ci::ColorAT<T>> {
	static void to_json( json& j, const ci::ColorAT<T> &c ) { j = std::vector<T> { c.r, c.g, c.b, c.a }; }
	static void from_json( const json &j, ci::ColorAT<T> &c ) { std::vector<T> vec = j; c = ci::ColorAT<T>( vec[0], vec[1], vec[2], vec[3] ); }
};

//! Serializer for fs::path
template <>
struct adl_serializer<ci::fs::path> {
	static void to_json( json& j, const ci::fs::path &p ) { j = p.string();	}
	static void from_json( const json &j, ci::fs::path &p ) { p = j.get<std::string>(); }
};

//! Serializer for Url
template <>
struct adl_serializer<ci::Url> {
	static void to_json( json& j, const ci::Url &u ) { j = u.str();	}
	static void from_json( const json &j, ci::Url &u ) { u = ci::Url( j.get<std::string>() ); }
};

//! Serializer for AxisAlignedBox
template <>
struct adl_serializer<ci::AxisAlignedBox> {
	static void to_json( json& j, const ci::AxisAlignedBox &b ) { j = { { "min", b.getMin() }, { "max", b.getMax() } }; }
	static void from_json( const json &j, ci::AxisAlignedBox &b ) { b = ci::AxisAlignedBox( j["min"], j["max"] ); }
};

//! Serializer for PlaneT
template <typename T>
struct adl_serializer<ci::PlaneT<T>> {
	static void to_json( json& j, const ci::PlaneT<T> &p ) { j = { { "point", p.getPoint() }, { "normal", p.getNormal() } }; }
	static void from_json( const json &j, ci::PlaneT<T> &p ) { p = ci::PlaneT<T>( j["point"], j["normal"] ); }
};

//! Serializer for PlaneT
template <>
struct adl_serializer<ci::Ray> {
	static void to_json( json& j, const ci::Ray &r ) { j = { { "origin", r.getOrigin() }, { "direction", r.getDirection() } }; }
	static void from_json( const json &j, ci::Ray &r ) { r = ci::Ray( j["origin"], j["direction"] ); }
};

//! Serializer for std::shared_ptr
template <typename T>
struct adl_serializer<std::shared_ptr<T>>
{
	static void to_json( json& j, const std::shared_ptr<T> &ptr ) {	ptr ? j = *ptr : j = nullptr; }
	static void from_json( const json& j, std::shared_ptr<T> &ptr ) { if( j.is_null() ) ptr = nullptr; else ptr.reset( new T( j.get<T>() ) ); }
};

//! Serializer for std::unique_ptr
template <typename T>
struct adl_serializer<std::unique_ptr<T>>
{
	static void to_json( json& j, const std::unique_ptr<T> &ptr ) {	ptr ? j = *ptr : j = nullptr; }
	static void from_json( const json& j, std::unique_ptr<T> &ptr ) { if( j.is_null() ) ptr = nullptr; else ptr.reset( new T( j.get<T>() ) ); }
};

} // namespace nlohmann

#endif