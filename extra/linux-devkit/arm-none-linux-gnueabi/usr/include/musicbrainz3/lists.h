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
 * $Id: lists.h 9218 2007-06-25 20:11:09Z luks $
 */
 
#ifndef __MUSICBRAINZ3_LISTS_H__
#define __MUSICBRAINZ3_LISTS_H__

#include <vector>

namespace MusicBrainz
{
	
	class Artist;
	class ArtistAlias;
	class ArtistResult;
	class Disc;
	class Relation;
	class Release;
	class ReleaseEvent;
	class ReleaseResult;
    class Result;
	class Track;
	class TrackResult;
	class User;
	class Tag;
	class Label;
	class LabelAlias;
	
	//! A vector of pointers to Artist objects
	typedef std::vector<Artist *> ArtistList;
	//! A vector of pointers to ArtistAlias objects
	typedef std::vector<ArtistAlias *> ArtistAliasList;
	//! A vector of pointers to ArtistResult objects
	typedef std::vector<ArtistResult *> ArtistResultList;
	//! A vector of pointers to Disc objects
	typedef std::vector<Disc *> DiscList;
	//! A vector of pointers to Relation objects
	typedef std::vector<Relation *> RelationList;
	//! A vector of pointers to Release objects
	typedef std::vector<Release *> ReleaseList;
	//! A vector of pointers to ReleaseEvent objects
	typedef std::vector<ReleaseEvent *> ReleaseEventList;
	//! A vector of pointers to ReleaseResult objects
	typedef std::vector<ReleaseResult *> ReleaseResultList;
	//! A vector of pointers to Result objects
	typedef std::vector<Result *> ResultList;
	//! A vector of pointers to Track objects
	typedef std::vector<Track *> TrackList;
	//! A vector of pointers to TrackResult objects
	typedef std::vector<TrackResult *> TrackResultList;
	//! A vector of pointers to User objects
	typedef std::vector<User *> UserList;
	//! A vector of pointers to Tag objects
	typedef std::vector<Tag *> TagList;
	//! A vector of pointers to Label objects
	typedef std::vector<Label *> LabelList;
	//! A vector of pointers to LabelAlias objects
	typedef std::vector<LabelAlias *> LabelAliasList;
	
}

#endif

