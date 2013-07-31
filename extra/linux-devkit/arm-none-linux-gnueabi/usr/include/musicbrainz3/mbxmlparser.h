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
 * $Id: mbxmlparser.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_MBXMLPARSER_H__
#define __MUSICBRAINZ3_MBXMLPARSER_H__

#include <exception>
#include <musicbrainz3/musicbrainz.h>
#include <musicbrainz3/factory.h>
#include <musicbrainz3/metadata.h>

namespace MusicBrainz
{

	/**
	 * Exception to be thrown if a parse error occurs. 
	 */
	class MB_API ParseError : public Exception
	{
	public:
		ParseError(const std::string &msg = std::string()) : Exception(msg) {}
	};
   
	
	/**
	 * A parser for the Music Metadata XML format.
	 *
	 * @see <a href="http://musicbrainz.org/development/mmd/">The Music
	 *      Metadata XML Format</a>
	 */
	class MB_API MbXmlParser
	{
	public:

		/**
		 * Constructor. 
		 */
		MbXmlParser(/*IFactory factory = DefaultFactory()*/);

		/**
		 * Destructor. 
		 */
		virtual ~MbXmlParser();

		/**
		 * Parses the MusicBrainz web service XML.
		 *
		 * Returns a Metadata object representing the parsed XML or
		 * raises a ParseError exception if the data was malformed.
		 * The parser tries to be liberal and skips invalid content if
		 * possible.
		 *
		 * @param data a string containing the XML data
		 * @return a Metadata object (never NULL)
		 * @throw ParseError if the document is not valid
		 */
		Metadata *parse(const std::string &data);
       
	private:
	
		class MbXmlParserPrivate;
		MbXmlParserPrivate *d;
	};
    
}

#endif
