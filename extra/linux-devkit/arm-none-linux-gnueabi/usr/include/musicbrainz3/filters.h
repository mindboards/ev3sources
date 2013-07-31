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
 * $Id: filters.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_FILTERS_H__
#define __MUSICBRAINZ3_FILTERS_H__

#include <string>
#include <vector>
#include <utility> 
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{

	/**
	 * A filter for collections.
	 *
	 * This is the interface all filters have to implement. 
	 */
	class MB_API IFilter
	{
	public:
		
		typedef std::vector<std::pair<std::string, std::string> > ParameterList;
	
		virtual ~IFilter() {};
	
		/**
		 * Create a map of query parameters.
		 *
		 * @return: a string->string map of parameters 
		 */
		virtual ParameterList createParameters() const = 0;
	};
	
	/**
	 * A filter for the artist collection.
	 */
	class MB_API ArtistFilter : public IFilter
	{
	public:
		ArtistFilter &name(const std::string &name);
		ArtistFilter &limit(const int limit);
		ParameterList createParameters() const;
	private:
		ParameterList parameters;
	};
	
	/**
	 * A filter for the release collection.
	 *
	 * If \a discId or \a artistId are set, only releases matching
	 * those IDs are returned. The \a releaseType parameter allows
	 * to limit the types of the releases returned. You can set it to
	 * Release::TYPE_ALBUM and Release::TYPE_OFFICIAL, for example,
	 * to only get officially released albums. Note that those values
	 * are connected using the \e AND operator. MusicBrainz' support
	 * is currently very limited, so Release::TYPE_LIVE and
	 * Release::TYPE_COMPILATION exclude each other (see <a 
	 * href="http://wiki.musicbrainz.org/AlbumAttribute">the
	 * documentation on release attributes </a>) for more
	 * information and all valid values).
	 *
	 * If both the \a artistName and the \a artistId parameter are
	 * given, the server will ignore \a artistName.
	 *
	 * @note The original Python library has \a releaseTypes parameter
	 * that accepts a list of release types. Due to C++ limitations, the
	 * library has \a releaseType parameter instead, that accepts only one
	 * release type, but you can use it multiple times. For example, to
	 * convert Python code:
	 * \code
	 * ReleaseFilter(releaseTypes=(Release.TYPE_ALBUM, Release.TYPE_OFFICIAL))
	 * \endcode
	 * you can use following C++ code:
	 * \code
	 * ReleaseFilter().releaseType(Release::TYPE_ALBUM).releaseType(Release::TYPE_OFFICIAL)
	 * \endcode
	 */
	class MB_API ReleaseFilter : public IFilter
	{
	public:
		ReleaseFilter &title(const std::string &value);
		ReleaseFilter &discId(const std::string &value);
		ReleaseFilter &releaseType(const std::string &value);
		ReleaseFilter &artistName(const std::string &value);
		ReleaseFilter &artistId(const std::string &value);
		ReleaseFilter &limit(const int value);
		ParameterList createParameters() const;
	private:
		ParameterList parameters;
	};
	
	/**
	 * A filter for the track collection.
	 *
	 * If \a artistId, \a releaseId or \a puid are set, only tracks
	 * matching those IDs are returned.
	 *
	 * The server will ignore \a artistName and \a releaseTitle if
	 * \a artistId or \a releaseId are set respectively. 	 
	 */
	class MB_API TrackFilter : public IFilter
	{
	public:
		TrackFilter &title(const std::string &value);
		TrackFilter &artistName(const std::string &value);
		TrackFilter &artistId(const std::string &value);
		TrackFilter &releaseTitle(const std::string &value);
		TrackFilter &releaseId(const std::string &value);
		TrackFilter &duration(const int value);
		TrackFilter &puid(const std::string &value);
		TrackFilter &limit(const int value);
		ParameterList createParameters() const;
	private:
		ParameterList parameters;
	};
	
	/**
	 * A filter for the user collection.
	 */
	class MB_API UserFilter : public IFilter
	{
	public:
		UserFilter &name(const std::string &name);
		ParameterList createParameters() const;
	private:
		ParameterList parameters;
	};
	
}

#endif
