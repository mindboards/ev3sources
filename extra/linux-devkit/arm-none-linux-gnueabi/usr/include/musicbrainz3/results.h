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
 * $Id: results.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_RESULTS_H__
#define __MUSICBRAINZ3_RESULTS_H__

#include <musicbrainz3/model.h>

namespace MusicBrainz
{
    
	/**
	 * Represents generic search result.
	 */
	class MB_API Result 
	{
	public:

		/**
		 * Constructor.
		 *
		 * @param score an int between 0 and 100 (both inclusive)
		 */
		Result(int score);
	
		/**
		 * Destructor.
		 */
		 virtual ~Result() {};
		
		/**
		 * Returns the result score.
		 *
		 * The score indicates how good this result matches the search
		 * parameters. The higher the value, the better the match.
		 * 
		 * @return an int between 0 and 100 (both inclusive)
		 */
		int getScore();
		
		/**
		 * Sets the result score.
		 *
		 * @param score an int between 0 and 100 (both inclusive)
		 */
		void setScore(int score);
		
	private:
	
		int score;
	};
	
	/**
	 * Represents an artist result.
	 *
	 * An \a ArtistResult consists of a \a score and an \a artist. The score
	 * is a number between 0 and 100, where a higher number indicates a better
	 * match. 
	 */
	class MB_API ArtistResult : public Result
	{
	public:
		
		/**
		 * Constructor.
		 */
		ArtistResult(Artist *artist, int score);
		
		/**
		 * Destructor.
		 */
		~ArtistResult();
		
		/**
		 * Returns the artist.
		 *
		 * @return a pointer to an Artist object
		 */
		Artist *getArtist();
		
		/**
		 * Sets the artist.
		 *
		 * @param artist a pointer to an Artist object
		 */
		void setArtist(Artist *artist);
		
	private:
		
		Artist *artist;
	};
	
	/**
	 * Represents an release result.
	 *
	 * An \a ReleaseResult consists of a \a score and an \a release. The score
	 * is a number between 0 and 100, where a higher number indicates a better
	 * match. 
	 */
	class MB_API ReleaseResult : public Result
	{
	public:
		
		/**
		 * Constructor.
		 */
		ReleaseResult(Release *release, int score);
		
		/**
		 * Destructor.
		 */
		~ReleaseResult();
		
		/**
		 * Returns the release.
		 *
		 * @return a pointer to a Release object
		 */
		Release *getRelease();
		
		/**
		 * Sets the release.
		 *
		 * @param release a pointer to a Release object
		 */
		void setRelease(Release *release);
		
	private:
		
		Release *release;
	};
	
	/**
	 * Represents an track result.
	 *
	 * An \a TrackResult consists of a \a score and an \a track. The score
	 * is a number between 0 and 100, where a higher number indicates a better
	 * match. 
	 */
	class MB_API TrackResult : public Result
	{
	public:
		
		/**
		 * Constructor.
		 */
		TrackResult(Track *track, int score);
		
		/**
		 * Destructor.
		 */
		~TrackResult();
		
		/**
		 * Returns the track.
		 *
		 * @return a pointer to a Track object
		 */
		Track *getTrack();
		
		/**
		 * Sets the track.
		 *
		 * @param track a pointer to a Track object
		 */
		void setTrack(Track *track);
		
	private:
		
		Track *track;
	};
	
}

#endif
