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
 * $Id: user.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_USER_H__
#define __MUSICBRAINZ3_USER_H__

#include <string>
#include <vector>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{
	
	/**
	 * Represents a MusicBrainz user.
	 */
	class MB_API User
	{
	public:

		/**
		 * Constructor.
		 */
		User();
		
		/**
		 * Destructor.
		 */
		virtual ~User();
		
		/**
		 * Returns the user name.
		 *
		 * @return a string containing the user name   
		 */
		std::string getName() const;
		
		/**
		 * Sets the user name. 
		 *
		 * @param name a string containing the user name   
		 */
		void setName(const std::string &name);

		/**
		 * Returns the types of this user.
		 *
		 * Most users' type list is empty. Currently, the following types
		 * are defined:
		 * 
		 *	- \a "http://musicbrainz.org/ns/ext-1.0#AutoEditor"
		 *	- \a "http://musicbrainz.org/ns/ext-1.0#RelationshipEditor"
		 *	- \a "http://musicbrainz.org/ns/ext-1.0#Bot"
		 *	- \a "http://musicbrainz.org/ns/ext-1.0#NotNaggable"
		 *
		 * @return a vector of strings containing absolute URIs 
		 */
		std::vector<std::string> &getTypes();

		/**
		 * Returns number of types.
		 *
		 * This is equivalent to \c getTypes().size()
		 *
		 * @return an int containing number of types
		 *
		 * @see getTypes		 
		 */
		int getNumTypes() const;
		
		/**
		 * Returns an type specified by index.
		 *
		 * This is equivalent to \c getTypes()[index]
		 *
		 * @return a string containing the type
		 *
		 * @see getTypes		 
		 */
		std::string getType(int index) const;
		
		/**
		 * Add a type to the list of types.
		 *
		 * @param type a string containing absolute URIs
		 *
		 * @see getTypes 
		 */
		void addType(const std::string &type);

		/**
		 * Returns true if a nag screen should be displayed to the user.
		 *
		 * @return \a true or \a false 
		 */
		bool getShowNag() const;
		
		/**
		 * Sets the value of the nag screen flag.
		 *
		 * @param value \a true or \a false
		 *
		 * @see getShowNag 
		 */
		void setShowNag(bool value);
		
	private:
		
		class UserPrivate;
		UserPrivate *d;
	};
	
}

#endif
