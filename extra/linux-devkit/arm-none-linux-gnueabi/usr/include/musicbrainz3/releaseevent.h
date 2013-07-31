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
 * $Id: releaseevent.h 9218 2007-06-25 20:11:09Z luks $
 */
 
#ifndef __MUSICBRAINZ3_RELEASEEVENT_H__
#define __MUSICBRAINZ3_RELEASEEVENT_H__

#include <string>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{
	
	/**
	 * A release event, indicating where and when a release took place.
	 *
	 * All country codes used must be valid ISO-3166 country codes (i.e. "DE",
	 * "UK" or "FR"). The dates are strings and must have the format "YYYY",
	 * "YYYY-MM" or "YYYY-MM-DD". 
	 */
	class MB_API ReleaseEvent
	{
	public:
	
		/**
		 * Constructor.
		 *
		 * @param country a string containing an ISO-3166 country code 
		 * @param dateStr a string containing a date string 
		 */
		ReleaseEvent(const std::string &country = std::string(),
					 const std::string &dateStr = std::string());

		/**
		 * Destructor.
		 */
		virtual ~ReleaseEvent();
		
		/**
		 * Returns the country a release took place.
		 *
		 * @note Due to a server limitation, the web service does not
		 * return country IDs for release collection queries. This only
		 * affects the Query::getReleases query.
		 *
		 * @return a string containing an ISO-3166 country code
		 *
		 * @see getCountryName 
		 */
		std::string getCountry() const;
		
		/**
		 * Sets the country a release took place.
		 *
		 * @param country a string containing an ISO-3166 country code 
		 */
		void setCountry(const std::string &country);
		
		/**
		 * Returns the date a release took place.
		 *
		 * @return a string containing a date 
		 */
		std::string getDate() const;
		
		/**
		 * Sets the date a release took place.
		 *
		 * @param dateStr a string containing a date  
		 */
		void setDate(const std::string &dateStr);
		
		void setCatalogNumber(const std::string &catalogNumber);
		std::string getCatalogNumber() const;
		
		void setBarcode(const std::string &barcode);
		std::string getBarcode() const;

		void setLabel(Label *label);
		Label *getLabel();

	private:
		
		class ReleaseEventPrivate;
		ReleaseEventPrivate *d;
	};
	
}

#endif

