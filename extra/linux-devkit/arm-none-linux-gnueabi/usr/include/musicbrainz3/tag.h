/*
 * MusicBrainz -- The Internet music metadatabase
 *
 * Copyright (C) 2007 Lukas Lalinsky
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
 * $Id: tag.h 9216 2007-06-25 19:42:20Z luks $
 */
 
#ifndef __MUSICBRAINZ3_TAG_H__
#define __MUSICBRAINZ3_TAG_H__

#include <string>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{
	
	/**
	 * 
	 */
	class MB_API Tag
	{
	public:
	
		/**
		 * Constructor.
		 *
		 * @param name
		 * @param count
		 */
		Tag(const std::string &name = std::string(), int count = 0);

		/**
		 * Destructor.
		 */
		virtual ~Tag();
		
		std::string getName() const;
		void setName(const std::string &name);
		
		int getCount() const;
		void setCount(const int count);
		
	private:
		
		class TagPrivate;
		TagPrivate *d;
	};
	
}

#endif

