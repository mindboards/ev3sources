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
 * $Id: labelalias.h 9218 2007-06-25 20:11:09Z luks $
 */
 
#ifndef __MUSICBRAINZ3_LABELALIAS_H__
#define __MUSICBRAINZ3_LABELALIAS_H__

#include <string>
#include <musicbrainz3/musicbrainz.h>
#include <musicbrainz3/artistalias.h>

namespace MusicBrainz
{
	
	/**
	 * Represents a label alias.
	 *
	 * BIC: make a generic class Alias with subclasses ArtistAlias, LabelAlias.
	 */
	class MB_API LabelAlias : public ArtistAlias
	{
	public:
	};
	
}

#endif
