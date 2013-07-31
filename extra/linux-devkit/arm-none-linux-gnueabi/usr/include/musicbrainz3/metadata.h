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
 * $Id: metadata.h 9218 2007-06-25 20:11:09Z luks $
 */
 
#ifndef __MUSICBRAINZ3_METADATA_H__
#define __MUSICBRAINZ3_METADATA_H__

#include <musicbrainz3/model.h>
#include <musicbrainz3/results.h>

namespace MusicBrainz
{
	
	/**
	 * Represents a parsed Music Metadata XML document.
	 *
	 * The Music Metadata XML format is very flexible and may contain a
	 * diverse set of data (e.g. an artist, a release and a list of tracks),
	 * but usually only a small subset is used (either an artist, a release
	 * or a track, or a lists of objects from one class).
	 *
	 * @see MbXmlParser for reading
	 */
	class MB_API Metadata 
	{
	public:
	
		Metadata();
		
		virtual ~Metadata();
		
		/**
		 * Returns the artist.
		 *
		 * @param remove if set to \c true, the object will be removed from 
		 * this Metadata instance and the caller is responsible for
		 * deleting it when it is no longer needed
		 *
		 * @return a pointer to Artist object, or \c NULL
		 */
		Artist *getArtist(bool remove = false);
		
		/**
		 * Sets the artist.
		 *
		 * @param artist a pointer to Artist object, or \c NULL
		 */
		void setArtist(Artist *artist);
		
		/**
		 * Returns the release.
		 *
		 * @param remove if set to \c true, the object will be removed from 
		 * this Metadata instance and the caller is responsible for
		 * deleting it when it is no longer needed
		 *
		 * @return a pointer to Release object, or \c NULL
		 */
		Release *getRelease(bool remove = false);
		
		/**
		 * Sets the release.
		 *
		 * @param release a pointer to Release object, or \c NULL
		 */
		void setRelease(Release *release);
		
		/**
		 * Returns a track.
		 *
		 * @param remove if set to \c true, the object will be removed from 
		 * this Metadata instance and the caller is responsible for
		 * deleting it when it is no longer needed
		 *
		 * @return a pointer to Track object, or \c NULL
		 */
		Track *getTrack(bool remove = false);
		
		/**
		 * Sets the track.
		 *
		 * @param track a pointer to Track object, or \c NULL
		 */
		void setTrack(Track *track);
		
		/**
		 * Returns the label.
		 *
		 * @param remove if set to \c true, the object will be removed from 
		 * this Metadata instance and the caller is responsible for
		 * deleting it when it is no longer needed
		 *
		 * @return a pointer to Label object, or \c NULL
		 */
		Label *getLabel(bool remove = false);
		
		/**
		 * Sets the label.
		 *
		 * @param label a pointer to Label object, or \c NULL
		 */
		void setLabel(Label *label);
		
		/**
		 * Returns a list of users.
		 *
		 * @return a vector of pointers to User objects
		 *
		 * @note This is a MusicBrainz extension.
		 */
		UserList &getUserList();
		UserList getUserList(bool remove);
		
		/**
		 * Returns a artist result list. 
		 *
		 * @return a vector of pointers to ArtistResult objects
		 */
		ArtistResultList &getArtistResults();
		ArtistResultList getArtistResults(bool remove);
		
		/**
		 * Returns a track result list. 
		 *
		 * @return a vector of pointers to TrackResult objects
		 */
		TrackResultList &getTrackResults();
		TrackResultList getTrackResults(bool remove);
		
		/**
		 * Returns a release result list. 
		 *
		 * @return a vector of pointers to ReleaseResult objects
		 */
		ReleaseResultList &getReleaseResults();
		ReleaseResultList getReleaseResults(bool remove);
		
	private:
	
		class MetadataPrivate;
		MetadataPrivate *d;
	};
	
}

#endif

