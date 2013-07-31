/*
 * MusicBrainz -- The Internet music metadatabase
 *
 * Copyright (C) 2006 Lukas Lalinsky
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * $Id: musicbrainz.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_MUSICBRAINZ_H__
#define __MUSICBRAINZ3_MUSICBRAINZ_H__

#include <musicbrainz3/defines.h>

#include <exception>
#include <string>

namespace MusicBrainz
{ 
	
	class MB_API Exception : public std::exception
	{
	public:
		Exception(const std::string &msg = std::string()) : msg(msg) {}
		virtual ~Exception() throw() {}
		const char *what() const throw() { return msg.c_str(); }
	private:
		std::string msg;
	};
	
	class MB_API ValueError : public Exception
	{
	public:
		ValueError(const std::string &msg = std::string()) : Exception(msg) {}
	};
	
	/**
	 * The ID of the special 'Various Artists' artist. 
	 */	   
	static const std::string VARIOUS_ARTISTS_ID = "http://musicbrainz.org/artist/89ad4ac3-39f7-470e-963a-56509c546377";		

	/**
	 * Default namespace prefix for all MusicBrainz metadata. 
	 */	   
	static const std::string NS_MMD_1 = "http://musicbrainz.org/ns/mmd-1.0#";
	
	/**
	 * Namespace prefix for relations. 
	 */	   
	static const std::string NS_REL_1 = "http://musicbrainz.org/ns/rel-1.0#";
	
	/**
	 * Namespace prefix for MusicBrainz extensions. 
	 */	   
	static const std::string NS_EXT_1 = "http://musicbrainz.org/ns/ext-1.0#";	  
	
}

#endif
