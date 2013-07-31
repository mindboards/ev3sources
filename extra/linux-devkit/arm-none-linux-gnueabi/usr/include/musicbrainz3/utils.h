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
 * $Id: utils.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_UTILS_H__
#define __MUSICBRAINZ3_UTILS_H__

#include <string>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{ 

	/**
	 * Extract the fragment part from a URI.
	 *
	 * If \a uriStr no absolute URI, then it is returned unchanged.
	 *
	 * @param uriStr a string containing an absolute URI
	 *
	 * @return a string containing the fragment
	 */
	MB_API std::string extractFragment(const std::string &uriStr);    

	/**
	 * Extract the UUID part from a MusicBrainz identifier.
	 *
	 * This function takes a MusicBrainz ID (an absolute URI) as the input
	 * and returns the UUID part of the URI, thus turning it into a relative
	 * URI. If \a uriStr is empty or a relative URI, then it is returned
	 * unchanged.
	 */
	MB_API std::string extractUuid(const std::string &uriStr);

	/**
	 * Returns a country's name based on an ISO-3166 country code.
	 *
	 * The country table this function is based on has been modified for
	 * MusicBrainz purposes by using the extension mechanism defined in
	 * ISO-3166. All IDs are still valid ISO-3166 country codes, but some
	 * IDs have been added to include historic countries and some of the
	 * country names have been modified to make them better suited for
	 * display purposes.
	 * 
	 * If the country ID is not found, an empty string is returned. This may
	 * happen for example, when new countries are added to the MusicBrainz web
	 * service which aren't known to this library yet.
	 *
	 * @param id a two-letter upper case string containing an ISO-3166 code
	 *
	 * @return: a string containing the country's name
	 */
	MB_API std::string getCountryName(const std::string &id);
	
	/**
	 * Returns a script name based on an ISO-15924 code.
	 *
	 * This function uses a subset of the ISO-15924 code table to map
	 * script IDs to names.
	 *
	 * @param id a four-letter string containing an ISO-15924 script code
	 *
	 * @return a string containing the script's name 
	 */ 
	MB_API std::string getScriptName(const std::string &id);
	
	/**
	 * Returns a language name based on an ISO-639-2/T code.
	 *
	 * This function uses a subset of the ISO-639-2/T code table to map
	 * language IDs (terminologic, not bibliographic ones!) to names.
	 *
	 * @param id a three-letter upper case string containing an ISO-639-2/T code
	 *
	 * @return a string containing the language's name
	 */ 
	MB_API std::string getLanguageName(const std::string &id);
	
	/** 
	 * Returns the name of a release type URI.
	 *
	 * @param releaseType a string containing a release type URI
	 *
	 * @return a string containing a printable name for the release type
	 *
	 * @see Release 
	 */
	MB_API std::string getReleaseTypeName(const std::string &releaseType);
    
}

#endif
