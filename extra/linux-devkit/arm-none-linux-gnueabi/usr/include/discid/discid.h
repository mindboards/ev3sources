/* --------------------------------------------------------------------------

   MusicBrainz -- The Internet music metadatabase

   Copyright (C) 2006 Matthias Friedrich
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

     $Id: discid.h 9569 2007-10-26 22:04:08Z robert $

--------------------------------------------------------------------------- */
#ifndef MUSICBRAINZ_DISC_ID_H
#define MUSICBRAINZ_DISC_ID_H

#ifdef WIN32
#	ifdef libdiscid_EXPORTS
#		define LIBDISCID_API __declspec(dllexport)
#	else
#		define LIBDISCID_API __declspec(dllimport)
#	endif
#else
#	define LIBDISCID_API
#endif



#ifdef __cplusplus
  extern "C" {
#endif

/*!
 * \mainpage libdiscid
 * \section intro Introduction
 *
 * libdiscid is a C library for calculating DiscIDs for Audio CDs. It is a C
 * port of the DiscID-related code from libmusicbrainz2 (which is written in
 * C++). The idea is to have an easy to use library without any dependencies
 * that can be used from scripting languages.
 *
 * \section examples Examples
 *
 * This is an example of the most basic usage:
 *
 * \code
 *
 * DiscId *disc = discid_new();
 *
 * if ( discid_read(disc, "/dev/cdrom") == 0 ) {
 *     fprintf(stderr, "Error: %s\n", discid_get_error_msg(disc));
 *     return 1;
 * }
 * 
 * printf("DiscID        : %s\n", discid_get_id(disc));
 * printf("Submit via    : %s\n", discid_get_submission_url(disc));
 *
 * discid_free(disc); 
 *
 * \endcode
 *
 * \section Building
 *
 * libdiscid provides a pkg-config script that returns the necessary compiler and linker flags, as well as the
 * version number.  To build a small sample program one would use:
 *
 * <tt>gcc libdiscid-test.c `pkg-config libdiscid --cflags --libs` -o libdiscid-test</tt>
 *
 * \section Contact
 *
 *  - <a href="http://lists.musicbrainz.org/mailman/listinfo/musicbrainz-devel">MusicBrainz Development Mailing List</a>
 *  - <a href="http://bugs.musicbrainz.org/query?component=libdiscid">MusicBrainz Bug Tracker</a>
 *
 */ 


/**
 * A transparent handle for an Audio CD.
 *
 * This is returned by discid_new() and has to be passed as the first
 * parameter to all discid_*() functions.
 */
typedef void *DiscId;


/**
 * Return a handle for a new DiscId object.
 *
 * If no memory could be allocated, NULL is returned. Don't use the created
 * DiscId object before calling discid_read() or discid_put().
 *
 * @return a DiscId object, or NULL.
 */
LIBDISCID_API DiscId *discid_new();


/**
 * Release the memory allocated for the DiscId object.
 *
 * @param d a DiscId object created by discid_new()
 */
LIBDISCID_API void discid_free(DiscId *d);


/**
 * Read the disc in the given CD-ROM/DVD-ROM drive.
 *
 * This function reads the disc in the drive specified by the given device
 * identifier. If the device is NULL, the default drive, as returned by
 * discid_get_default_device() is used.
 *
 * On error, this function returns false and sets the error message which you
 * can access using discid_get_error_msg(). In this case, the other functions
 * won't return meaningful values and should not be used.
 *
 * This function may be used multiple times with the same DiscId object.
 *
 * @param d a DiscId object created by discid_new()
 * @param device an operating system dependent device identifier, or NULL
 * @return true if successful, or false on error.
 */
LIBDISCID_API int discid_read(DiscId *d, const char *device);


/**
 * Provides the TOC of a known CD.
 *
 * This function may be used if the TOC has been read earlier and you
 * want to calculate the disc ID afterwards, without accessing the disc
 * drive. It replaces the discid_read function in this case.
 *
 * On error, this function returns false and sets the error message which you
 * can access using discid_get_error_msg(). In this case, the other functions
 * won't return meaningful values and should not be used.
 *
 * The offsets parameter points to an array which contains the track offsets
 * for each track. The first element, offsets[0], is the leadout track. It
 * must contain the total number of sectors on the disc.
 *
 * @param d a DiscID object created by discid_new()
 * @param first the number of the first audio track on disc (usually one)
 * @param last the number of the last audio track on the disc
 * @param offsets a pointer to an array of 100 track offsets
 * @return true if the given data was valid, and false on error
 */
LIBDISCID_API int discid_put(DiscId *d, int first, int last, int *offsets);


/**
 * Return a human-readable error message.
 *
 * This function may only be used if discid_read() failed. The returned
 * error message is only valid as long as the DiscId object exists.
 *
 * @param d a DiscId object created by discid_new()
 * @return a string describing the error that occurred
 */
LIBDISCID_API char *discid_get_error_msg(DiscId *d);


/**
 * Return a MusicBrainz DiscID.
 *
 * The returned string is only valid as long as the DiscId object exists.
 *
 * @param d a DiscId object created by discid_new()
 * @return a string containing a MusicBrainz DiscID
 */
LIBDISCID_API char *discid_get_id(DiscId *d);


/**
 * Return a FreeDB DiscID.
 *
 * The returned string is only valid as long as the DiscId object exists.
 *
 * @param d a DiscId object created by discid_new()
 * @return a string containing a FreeDB DiscID
 */
LIBDISCID_API char *discid_get_freedb_id(DiscId *d);


/**
 * Return an URL for submitting the DiscID to MusicBrainz.
 *
 * The URL leads to an interactive disc submission wizard that guides the
 * user through the process of associating this disc's DiscID with a
 * release in the MusicBrainz database.
 *
 * The returned string is only valid as long as the DiscId object exists.
 *
 * @param d a DiscId object created by discid_new()
 * @return a string containing an URL
 */
LIBDISCID_API char *discid_get_submission_url(DiscId *d);

/**
 * Return an URL for retrieving CD information from MusicBrainz' web service
 *
 * The URL provides the CD information in XML. 
 * See http://musicbrainz.org/development/mmd for details.
 *
 * The returned string is only valid as long as the DiscId object exists.
 *
 * @param d a DiscId object created by discid_new()
 * @return a string containing an URL
 */
LIBDISCID_API char *discid_get_webservice_url(DiscId *d);

/**
 * Return the name of the default disc drive for this operating system.
 *
 * @return a string containing an operating system dependent device identifier
 */
LIBDISCID_API char *discid_get_default_device(void);


/**
 * Return the number of the first track on this disc.
 *
 * @param d a DiscId object created by discid_new()
 * @return the number of the first track
 */
LIBDISCID_API int discid_get_first_track_num(DiscId *d);


/**
 * Return the number of the last track on this disc.
 *
 * @param d a DiscId object created by discid_new()
 * @return the number of the last track
 */
LIBDISCID_API int discid_get_last_track_num(DiscId *d);


/**
 * Return the length of the disc in sectors.
 *
 * @param d a DiscId object created by discid_new()
 * @return the length of the disc in sectors
 */
LIBDISCID_API int discid_get_sectors(DiscId *d);


/**
 * Return the sector offset of a track.
 *
 * Only track numbers between (and including) discid_get_first_track_num()
 * and discid_get_last_track_num() may be used.
 *
 * @param d a DiscId object created by discid_new()
 * @param track_num the number of a track
 * @return sector offset of the specified track
 */
LIBDISCID_API int discid_get_track_offset(DiscId *d, int track_num);


/**
 * Return the length of a track in sectors.
 *
 * Only track numbers between (and including) discid_get_first_track_num()
 * and discid_get_last_track_num() may be used.
 *
 * @param d a DiscId object created by discid_new()
 * @param track_num the number of a track
 * @return length of the specified track
 */
LIBDISCID_API int discid_get_track_length(DiscId *d, int track_num);


#ifdef __cplusplus
  }
#endif

#endif /* MUSICBRAINZ_DISC_ID_H */
