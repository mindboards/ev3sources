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
 * $Id: webservice.h 8470 2006-09-05 14:06:16Z luks $
 */
 
#ifndef __MUSICBRAINZ3_WEBSERVICE_H__
#define __MUSICBRAINZ3_WEBSERVICE_H__

#include <string>
#include <musicbrainz3/musicbrainz.h> 
#include <musicbrainz3/iwebservice.h>

namespace MusicBrainz
{
  
	/**
	 * A web service error has occurred.
	 * 
	 * This is the base class for several other web service related
	 * exceptions.
	 */
	class MB_API WebServiceError : public Exception
	{
	public:
		WebServiceError(const std::string &msg = std::string()) : Exception(msg) {}
	};
	
	/**
	 * Getting a server connection failed.
	 *
	 * This exception is mostly used if the client couldn't connect to
	 * the server because of an invalid host name or port. It doesn't
	 * make sense if the web service in question doesn't use the network. 
	 */
	class MB_API ConnectionError : public WebServiceError
	{
	public:
		ConnectionError(const std::string &msg = std::string()) : WebServiceError(msg) {}
	};
	
	/**
	 * Connection to a server timed out.
	 *
	 */
	class MB_API TimeOutError : public WebServiceError
	{
	public:
		TimeOutError(const std::string &msg = std::string()) : WebServiceError(msg) {}
	};
	
	/**
	 * No resource with the given ID exists.
	 */
	class MB_API ResourceNotFoundError : public WebServiceError
	{
	public:
		ResourceNotFoundError(const std::string &msg = std::string()) : WebServiceError(msg) {}
	};
	
	/**
	 * Authentication failed.
	 *
	 * This is thrown if user name, password or realm were invalid while
	 * trying to access a protected resource. 
	 */
	class MB_API AuthenticationError : public WebServiceError
	{
	public:
		AuthenticationError(const std::string &msg = std::string()) : WebServiceError(msg) {}
	};
	
	/**
	 * The returned resource was invalid.
	 *
	 * This may be due to a malformed XML document or if the requested
	 * data wasn't part of the response. It can only occur in case of
	 * bugs in the web service itself. 
	 */
	class MB_API ResponseError : public WebServiceError
	{
	public:
		ResponseError(const std::string &msg = std::string()) : WebServiceError(msg) {}
	};
	
	/**
	 * An invalid request was made.
	 *
	 * This exception is raised if the client made an invalid request.
	 * That could be syntactically invalid identifiers or unknown or
	 * invalid parameter values. 
	 */
	class MB_API RequestError : public WebServiceError
	{
	public:
		RequestError(const std::string &msg = std::string()) : WebServiceError(msg) {}
	};
	
	/**
	 * An interface to the MusicBrainz XML web service via HTTP.
	 *
	 * By default, this class uses the MusicBrainz server but may be
	 * configured for accessing other servers as well using the
	 * constructor (WebService::WebService). This implements IWebService, so
	 * additional documentation on method parameters can be found there.
	 */
	
	class MB_API WebService : public IWebService
	{
	public:
	
		/**
		 * Constructor.
		 * 
		 * This can be used without parameters. In this case, the
		 * MusicBrainz server will be used.
		 *
		 * @param host a string containing a host name
		 * @param port an integer containing a port number
		 * @param pathPrefix a string prepended to all URLs
		 * @param username a string containing a MusicBrainz user name
		 * @param password a string containing the user's password
		 * @param realm a string containing the realm used for authentication
		 */
		WebService(const std::string &host = "musicbrainz.org",
				   const int port = 80,
				   const std::string &pathPrefix = "/ws",
				   const std::string &username = std::string(),
				   const std::string &password = std::string(),
				   const std::string &realm = "musicbrainz.org");

		/**
		 * Destructor.
		 */
		virtual ~WebService();
		
		/**
		 * Sets the host. 
		 *
		 * @param host a string containing the host name   
		 */
		void setHost(const std::string &host);
		
		/**
		 * Returns the host.
		 *
		 * @return a string containing the host name  
		 */
		std::string getHost() const;
		
		/**
		 * Sets the port. 
		 *
		 * @param port an int containing the port number   
		 */
		void setPort(const int port);
		
		/**
		 * Returns the port.
		 *
		 * @return an int containing the port number  
		 */
		int getPort() const;
		
		/**
		 * Sets the path prefix. 
		 *
		 * @param pathPrefix a string containing the path prefix  
		 */
		void setPathPrefix(const std::string &pathPrefix);
		
		/**
		 * Returns the path prefix.
		 *
		 * @return a string containing the path prefix  
		 */
		std::string getPathPrefix() const;
		
		/**
		 * Sets the MusicBrainz user name. 
		 *
		 * @param username a string containing the user name  
		 */
		void setUserName(const std::string &username);
		
		/**
		 * Returns the MusicBrainz user name.
		 *
		 * @return a string containing the user name 
		 */
		std::string getUserName() const;
		
		/**
		 * Sets the MusicBrainz user password. 
		 *
		 * @param password a string containing the user password  
		 */
		void setPassword(const std::string &password);
		
		/**
		 * Returns the MusicBrainz user password.
		 *
		 * @return a string containing the user password 
		 */
		std::string getPassword() const;
		
		/**
		 * Sets the HTTP authentification realm. 
		 *
		 * @param realm a string containing the realm 
		 */
		void setRealm(const std::string &realm);
		
		/**
		 * Returns the HTTP authentification realm.
		 *
		 * @return a string containing the realm 
		 */
		std::string getRealm() const;
		
		/**
		 * Sets the proxy host. 
		 *
		 * @param host a string containing the host name   
		 */
		void setProxyHost(const std::string &host);
		
		/**
		 * Returns the proxy host.
		 *
		 * @return a string containing the host name  
		 */
		std::string getProxyHost() const;
		
		/**
		 * Sets the proxy port. 
		 *
		 * @param port an int containing the port number   
		 */
		void setProxyPort(const int port);
		
		/**
		 * Returns the proxy port.
		 *
		 * @return an int containing the port number  
		 */
		int getProxyPort() const;
		
		/**
		 * Sets the proxy user name. 
		 *
		 * @param username a string containing the user name   
		 */
		void setProxyUserName(const std::string &username);
		
		/**
		 * Returns the proxy user name.
		 *
		 * @return a string containing the user name  
		 */
		std::string getProxyUserName() const;
		
		/**
		 * Sets the proxy password. 
		 *
		 * @param password a string containing the password   
		 */
		void setProxyPassword(const std::string &password);
		
		/**
		 * Returns the proxy password.
		 *
		 * @return a string containing the password  
		 */
		std::string getProxyPassword() const;
		
		/**
		 * Query the web service via HTTP-GET.
		 *
		 * Returns a string containing the result or raises a
		 * WebServiceError. Conditions leading to errors may be
		 * invalid entities, IDs, \a include or \a filter parameters
		 * and unsupported version numbers.
		 *
		 * @throw ConnectionError couldn't connect to server
		 * @throw RequestError invalid IDs or parameters
		 * @throw AuthenticationError invalid user name and/or password
		 * @throw ResourceNotFoundError resource doesn't exist
		 * 
		 * @see IWebService::get  		 
		 */
		std::string get(const std::string &entity,
						const std::string &id,
						const IIncludes::IncludeList &include,
						const IFilter::ParameterList &filter,
						const std::string &version = "1"); 

		/**
		 * Send data to the web service via HTTP-POST.
		 *
		 * Note that this may require authentication. You can set
		 * user name, password and realm in the constructor
		 * (WebService::WebService).
		 * 
		 * @throw ConnectionError couldn't connect to server
		 * @throw RequestError invalid IDs or parameters
		 * @throw AuthenticationError invalid user name and/or password
		 * @throw ResourceNotFoundError resource doesn't exist
		 * 
		 * @see IWebService::post 
		 */		 
		 void post(const std::string &entity,
				   const std::string &id,
				   const std::string &data,
				   const std::string &version = "1");
		
	private:
	
		static int httpAuth(void *userdata, const char *realm, int attempts,
							char *username, char *password);
		static int proxyAuth(void *userdata, const char *realm, int attempts,
							char *username, char *password);
		static int httpResponseReader(void *userdata, const char *buf, size_t len);

		class WebServicePrivate;
		WebServicePrivate *d;
	};
	
}

#endif
