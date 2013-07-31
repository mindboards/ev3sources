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
 * $Id: track.h 8794 2007-01-18 23:37:36Z luks $
 */
 
#ifndef __MUSICBRAINZ3_TRACK_H__
#define __MUSICBRAINZ3_TRACK_H__

#include <string>
#include <musicbrainz3/musicbrainz.h>
#include <musicbrainz3/entity.h>

namespace MusicBrainz
{
	
	class Artist;
	
	/**
	 * Represents a track.
	 *
	 * This class represents a track which may appear on one or more releases.
	 * A track may be associated with exactly one artist (the I{main} artist).
	 *
	 * Using getReleases, you can find out on which releases this track
	 * appears. To get the track number, too, use the
	 * Release::getTracksOffset method.
	 *
	 * @note Currently, the MusicBrainz server doesn't support tracks to
	 *		 be on more than one release.
	 *
	 * @see Release, Artist 
	 */
	
	class MB_API Track : public Entity
	{
	public:
	
		/**
		 * Constructor.
		 *
		 * @param id a string containing an absolute URI 
		 * @param title a string containing the title 
		 */
		Track(const std::string &id = std::string(),
			  const std::string &title = std::string());

		/**
		 * Destructor.
		 */
		virtual ~Track();
		
		/**
		 * Returns the track's title.
		 *
		 * The style and format of this attribute is specified by the
		 * style guide.
		 *
		 * @return a string containing an absolute URI
		 * 
		 * @see <a href="http://musicbrainz.org/style.html">The MusicBrainz
		 *		Style Guidelines</a> 
		 */
		std::string getTitle() const;
		
		/**
		 * Sets the track's title.
		 *
		 * @param title: a string containing the title
		 *
		 * @see: getTitle 
		 */
		void setTitle(const std::string &title);
		
		/**
		 * Returns the main artist of this track.
		 *
		 * @return: a pointer to Artist object, or NULL
		 */
		Artist *getArtist();
		
		/**
		 * Sets this track's main artist.
		 *
		 * @param artist a pointer to Artist object, or NULL
		 */
		void setArtist(Artist *artist);
		
		/**
		 * Returns the duration of this track in milliseconds.
		 *
		 * @return an int containing the duration in milliseconds
		 */
		int getDuration() const;
		
		/**
		 * Sets the duration of this track in milliseconds.
		 *
		 * @param duration an int containing the duration in milliseconds
		 */
		void setDuration(const int duration);
		
		/**
		 * Returns a list of releases from this artist.
		 *
		 * This may also include releases where this artist isn't the
		 * \e main artist but has just contributed one or more tracks
		 * (aka VA-Releases).
		 *
		 * @return: a list of pointers to Release objects 
		 */
		ReleaseList &getReleases();
		
		/**
		 * Returns number of releases.
		 *
		 * This is equivalent to \c getReleases().size()
		 *
		 * @return an int containing number of releases
		 *
		 * @see getReleases		 
		 */
		int getNumReleases() const;
		
		/**
		 * Returns an release specified by index.
		 *
		 * This is equivalent to \c getReleases()[index]
		 *
		 * @return a pointer to Release instance
		 *
		 * @see getReleases		 
		 */
		Release *getRelease(int index);
		
		/**
		 * Adds a release to this artist's list of releases.
		 *
		 * @param release a pointer to Release object
		 */
		void addRelease(Release *release);

		/**
		 * Returns the offset of the release list.
		 * 
		 * This is used if the track list is incomplete (ie. the web
		 * service only returned part of the tracks on this release).
		 * Note that the offset value is zero-based, which means track
		 * \a 0 is the first track.
		 *
		 * @return an integer containing the offset
		 *
		 * @see getReleases
		 */
		int getReleasesOffset() const;
		
		/**
		 * Sets the offset of the release list. 
		 *
		 * @param offset an integer containing the offset
		 *
		 * @see getReleasesOffset 
		 */
		void setReleasesOffset(const int offset);
		
		/**
		 * Returns the number of existing releases. 
		 * 
		 * This may or may not match with the number of elements that 
		 * getReleases and getNumReleases returns. If the count is higher than 
		 * the list, it indicates that the list is incomplete. 
		 *
		 * @return an integer containing the count
		 *
		 * @see getReleases 
		 */
		int getReleasesCount() const;
		
		/**
		 * Sets the count of the release list.
		 *
		 * @param count an integer containing the count
		 *
		 * @see getReleasesCount 
		 */
		void setReleasesCount(const int count);
		
	private:
		
		class TrackPrivate;
		TrackPrivate *d;
	};
	
}

#endif

