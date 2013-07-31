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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * $Id: artistalias.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_ARTISTALIAS_H__
#define __MUSICBRAINZ3_ARTISTALIAS_H__

#include <string>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{
	
	/**
	 * Represents an artist alias.
	 *
	 * An alias (the \a alias \a value) is a different representation of an
	 * artist's name. This may be a common misspelling or a transliteration
	 * (the \a alias \a type).
	 *
	 * The \a alias \a script is interesting mostly for transliterations and
	 * indicates which script is used for the alias value. To represent the
	 * script, ISO-15924 script codes like 'Latn', 'Cyrl', or 'Hebr' are used. 
	 */
	class MB_API ArtistAlias
	{
	public:
	
		/**
		 * Constructor.
		 *
		 * @param value a string containing the alias 
		 * @param type a string containing an absolute URI 
		 * @param script a string containing an ISO-15924 script code 
		 */
		ArtistAlias(const std::string &value = std::string(),
					const std::string &type = std::string(),
					const std::string &script = std::string());
		
		/**
		 * Destructor.
		 */
		virtual ~ArtistAlias(); 
		
		/**
		 * Returns the alias.	
		 *
		 * @return a string containing the alias 
		 */
		std::string getValue() const;
		
		/**
		 * Sets the alias. 
		 *
		 * @param value a string containing the alias 
		 */
		void setValue(const std::string &value);
		
		/**
		 * Returns the alias type. 
		 *
		 * @return a string containing an absolute URI
		 */
		std::string getType() const;
		
		/**
		 * Sets the alias type. 
		 *
		 * @param type a string containing an absolute URI	
		 */
		void setType(const std::string &type);
		
		/**
		 * Returns the alias script.  
		 *
		 * @return a string containing an ISO-15924 script code
		 */
		std::string getScript() const;
		
		/**
		 * Sets the alias script. 
		 *
		 * @param type a string containing an ISO-15924 script code	  
		 */
		void setScript(const std::string &type);
		
	private:
		
		class ArtistAliasPrivate;
		ArtistAliasPrivate *d;
	};
	
}

#endif
