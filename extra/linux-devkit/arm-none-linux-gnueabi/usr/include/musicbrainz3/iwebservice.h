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
 * $Id: iwebservice.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_IWEBSERVICE_H__
#define __MUSICBRAINZ3_IWEBSERVICE_H__

#include <string>
#include <musicbrainz3/musicbrainz.h> 
#include <musicbrainz3/includes.h> 
#include <musicbrainz3/filters.h> 

namespace MusicBrainz
{
  
	/**
	 * An interface all concrete web service classes have to implement.
	 *
	 * All web service classes have to implement this and follow the
	 * method specifications.  
	 */
	class MB_API IWebService
	{
	public:
	
		virtual ~IWebService() {};
	
		/**
		 * Query the web service.
		 *
		 * Using this method, you can either get a resource by id (using
		 * the \a id parameter, or perform a query on all resources of
		 * a type.
		 *
		 * The \a filter and the \a id parameter exclude each other. If
		 * you are using a filter, you may not set \a id and vice versa.
		 *
		 * Returns a file-like object containing the result or raises a
		 * WebServiceError or one of its subclasses in case of an
		 * error. Which one is used depends on the implementing class.
		 * 
		 * @param entity a string containing the entity's name
		 * @param id a string containing a UUID, or the empty string
		 * @param include a tuple containing values for the 'inc' parameter
		 * @param filter parameters, depending on the entity
		 * @param version a string containing the web service version to use
		 * 
		 * @return a string contaning the returned data
		 *
		 * @throw WebServiceError: in case of errors 
		 */
		 virtual std::string get(const std::string &entity,
								 const std::string &id,
								 const IIncludes::IncludeList &include,
								 const IFilter::ParameterList &filter,
								 const std::string &version = "1") = 0;
		 
		 /**
		  * Submit data to the web service.
		  *
		  * @param entity a string containing the entity's name
		  * @param id a string containing a UUID, or the empty string
		  * @param data a string containing the data to post
		  * @param version a string containing the web service version to use
		  * 
		  * @throw WebServiceError in case of errors 
		  */
		 virtual void post(const std::string &entity,
						   const std::string &id,
						   const std::string &data,
						   const std::string &version = "1") = 0;
	};
	
}

#endif
