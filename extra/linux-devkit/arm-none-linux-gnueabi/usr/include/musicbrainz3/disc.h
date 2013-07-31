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
 * $Id: disc.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_DISC_H__
#define __MUSICBRAINZ3_DISC_H__

#include <string>
#include <vector>
#include <utility>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{

	/**
	 * Represents an Audio CD.
	 *
	 * This class represents an Audio CD. A disc can have an ID (the
	 * MusicBrainz DiscID), which is calculated from the CD's table of
	 * contents (TOC). There may also be data from the TOC like the length
	 * of the disc in sectors, as well as position and length of the tracks.
	 *
	 * Note that different TOCs, maybe due to different pressings, lead to
	 * different DiscIDs. Conversely, if two different discs have the same
	 * TOC, they also have the same DiscID (which is unlikely but not
	 * impossible). DiscIDs are always 28 characters long and look like this:
	 * \a 'J68I_CDcUFdCRCIbHSEbTBCbooA-'. Sometimes they are also referred
	 * to as CDIndex IDs.
	 *
	 * The MusicBrainz web service (WebService) only returns
	 * the DiscID and the number of sectors. The DiscID calculation function 
	 * readDisc, however, can retrieve the other
	 * attributes of Disc from an Audio CD in the disc drive. 
	 */
	class MB_API Disc
	{
	public:

		typedef std::pair<int, int> Track;
		typedef std::vector<Disc::Track> TrackList;
	
		/**
		 * Constructor.
		 *
		 * @param id a string containing a 28-character DiscID	
		 */
		Disc(const std::string &id = std::string());

		/**
		 * Destructor.
		 */
		virtual ~Disc();
		
		/**
		 * Returns the MusicBrainz DiscID. 
		 * 
		 * @return a string containing a 28-character DiscID  
		 */
		std::string getId() const;
		
		/**
		 * Sets the MusicBrainz DiscId. 
		 *
		 * @param value a string containing a 28-character DiscID 
		 */
		void setId(const std::string &value);
		
		/**
		 * Returns the length of the disc in sectors.
		 *
		 * @return the length in sectors as an integer
		 */
		int getSectors() const;
		
		/**
		 * Sets the length of the disc in sectors.
		 *
		 * @param sectors the length in sectors as an integer 
		 */
		void setSectors(const int sectors);
		
		/**
		 * Returns the number of the first track on this disc.
		 *
		 * @return an int containing the track number
		 */
		int getFirstTrackNum() const;
		
		/** 
		 * Sets the number of the first track on this disc.
		 *
		 * @param trackNum an int containing the track number
		 */
		void setFirstTrackNum(const int trackNum);
		
		/**
		 * Returns the number of the last track on this disc.
		 *
		 * @return an int containing the track number
		 */
		int getLastTrackNum() const;
		
		/**
		 * Sets the number of the last track on this disc.
		 *
		 * @param trackNum an int containing the track number
		 */
		void setLastTrackNum(const int trackNum);
		
		/**
		 * Returns the sector offset and length of this disc.
		 *
		 * This method returns a list of tuples containing the track
		 * offset and length in sectors for all tracks on this disc.
		 * The track offset is measured from the beginning of the disc,
		 * the length is relative to the track's offset. Note that the
		 * leadout track is \e not included.
		 * 
		 * @return a vector of (offset, length) pairs (values are ints)
		 */
		Disc::TrackList &getTracks();
		
		/**
		 * Adds a track to the list.
		 *
		 * This method adds an (offset, length) pair to the list of
		 * tracks. The leadout track must \e not be added. The total
		 * length of the disc can be set using setSectors.
		 *
		 * @param track an (offset, length) pair (values are ints)
		 *
		 * @see getTracks 
		 */
		void addTrack(Disc::Track track);
		
	private:

		class DiscPrivate;
		DiscPrivate *d;
	};
	
	/**
	 * The Audio CD could not be read.
	 *
	 * This may be simply because no disc was in the drive, the device name
	 * was wrong or the disc can't be read. Reading errors can occur in case
	 * of a damaged disc or a copy protection mechanism, for example. 
	 */
	class MB_API DiscError : public Exception
	{
	public:
		DiscError(const std::string &msg = std::string()) : Exception(msg) {}
	};
	
	/**
	 * Reads an Audio CD in the disc drive.
	 *
	 * This reads a CD's table of contents (TOC) and calculates the MusicBrainz
	 * DiscID, which is a 28 character ASCII string. This DiscID can be used
	 * to retrieve a list of matching releases from the web service (see
	 * MusicBrainz::Query).
	 *
	 * Note that an Audio CD has to be in drive for this to work. The
	 * \a deviceName argument may be used to set the device. The default
	 * depends on the operating system (on linux, it's \c "/dev/cdrom").
	 * No network connection is needed for this function.
	 *
	 * If the device doesn't exist or there's no valid Audio CD in the drive,
	 * a DiscError exception is raised.
	 *
	 * @param deviceName a string containing the CD drive's device name
	 *
	 * @return a pointer to Disc object
	 *
	 * @throw DiscError if there was a problem reading the disc
	 */
	MB_API Disc *readDisc(const std::string &deviceName = std::string());

	/**
	 * Returns a URL for adding a disc to the MusicBrainz database.
	 *
	 * A fully initialized Disc object is needed, as
	 * returned by readDisc. A disc object returned by the web service
	 * doesn't provide the necessary information.
	 *
	 * Note that the created URL is intended for interactive use and points
	 * to the MusicBrainz disc submission wizard by default. This method
	 * just returns a URL, no network connection is needed. The disc drive
	 * isn't used.
	 *
	 * @param disc a fully initialized Disc object
	 * @param host a string containing a host name
	 * @param port an integer containing a port number
	 * 
	 * @return a string containing the submission URL
	 * 
	 * @see readDisc 
	 */
	MB_API std::string getSubmissionUrl(Disc *disc,
		const std::string &host = "mm.musicbrainz.org",
		int port = 80);
	
}

#endif

