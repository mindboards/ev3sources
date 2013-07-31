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
 * $Id: mb_c.h 9932 2008-07-02 06:48:17Z luks $
 */
 
#ifndef __MUSICBRAINZ3_MB_C_H__
#define __MUSICBRAINZ3_MB_C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <musicbrainz3/defines.h>

/* {{{ Typedefs */

typedef void *MbArtist;
typedef void *MbArtistAlias;
typedef void *MbArtistFilter;
typedef void *MbArtistIncludes;
typedef void *MbDisc;
typedef void *MbEntity;
typedef void *MbQuery;
typedef void *MbRelation;
typedef void *MbRelease;
typedef void *MbReleaseEvent;
typedef void *MbReleaseFilter;
typedef void *MbReleaseIncludes;
typedef void *MbResultList;
typedef void *MbTrack;
typedef void *MbTrackFilter;
typedef void *MbTrackIncludes;
typedef void *MbUser;
typedef void *MbUserFilter;
typedef void *MbWebService;

/* }}} */

/* {{{ MusicBrainz::WebService */

/**
 * Creates a new instance of MbWebService.
 *
 * @see MusicBrainz::WebService::WebService
 */
MB_API MbWebService
mb_webservice_new();

/**
 * Deletes the MbWebService instance.
 *
 * @see MusicBrainz::WebService::~WebService
 */
MB_API void
mb_webservice_free(MbWebService webservice);

/**
 * @see MusicBrainz::WebService::setHost
 */
MB_API void
mb_webservice_set_host(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setPort
 */
MB_API void
mb_webservice_set_port(MbWebService webservice, int port);

/**
 * @see MusicBrainz::WebService::setPathPrefix
 */
MB_API void
mb_webservice_set_path_prefix(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setUserName
 */
MB_API void
mb_webservice_set_username(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setPassword
 */
MB_API void
mb_webservice_set_password(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setRealm
 */
MB_API void
mb_webservice_set_realm(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setProxyHost
 */
MB_API void
mb_webservice_set_proxy_host(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setProxyPort
 */
MB_API void
mb_webservice_set_proxy_port(MbWebService webservice, int port);

/**
 * @see MusicBrainz::WebService::setProxyUserName
 */
MB_API void
mb_webservice_set_proxy_username(MbWebService webservice, const char *str);

/**
 * @see MusicBrainz::WebService::setProxyPassword
 */
MB_API void
mb_webservice_set_proxy_password(MbWebService webservice, const char *str);

/* }}} */

/* {{{ MusicBrainz::Query */

/**
 * Creates a new instance of MbQuery.
 *
 * @see MusicBrainz::Query::Query
 */
MB_API MbQuery
mb_query_new(MbWebService ws, const char *client_id);

/**
 * Deletes the MbQuery instance.
 *
 * @see MusicBrainz::Query::~Query
 */
MB_API void
mb_query_free(MbQuery query);

/**
 * Returns an artist.
 * 
 * @see MusicBrainz::Query::getArtistById
 */
MB_API MbArtist
mb_query_get_artist_by_id(MbQuery query, const char *id, MbArtistIncludes inc);

/**
 * Returns an release.
 * 
 * @see MusicBrainz::Query::getReleaseById
 */
MB_API MbRelease
mb_query_get_release_by_id(MbQuery query, const char *id, MbReleaseIncludes inc);

/**
 * Returns an track.
 * 
 * @see MusicBrainz::Query::getTrackById
 */
MB_API MbTrack
mb_query_get_track_by_id(MbQuery query, const char *id, MbTrackIncludes inc);

/**
 * Returns information about a MusicBrainz user.
 * 
 * @see MusicBrainz::Query::getUserByName
 */
MB_API MbUser
mb_query_get_user_by_name(MbQuery query, const char *name);

/* }}} */

MB_API void
mb_result_list_free(MbResultList list);

MB_API int
mb_result_list_get_size(MbResultList list);

MB_API int
mb_result_list_get_score(MbResultList list, int index);

MB_API MbArtist
mb_result_list_get_artist(MbResultList list, int index);

MB_API MbRelease
mb_result_list_get_release(MbResultList list, int index);

MB_API MbTrack
mb_result_list_get_track(MbResultList list, int index);

MB_API MbResultList
mb_query_get_artists(MbQuery query, MbArtistFilter flt);

MB_API MbResultList
mb_query_get_releases(MbQuery query, MbReleaseFilter flt);

MB_API MbResultList
mb_query_get_tracks(MbQuery query, MbTrackFilter flt);

/* {{{ MusicBrainz::Artist */

/**
 * Deletes the MbArtist instance.
 */
MB_API void
mb_artist_free(MbArtist artist);

/**
 * Returns the artist's ID.
 *
 * @see MusicBrainz::Artist::getId	
 */
MB_API void
mb_artist_get_id(MbArtist artist, char *str, int len);

/**
 * Returns the artist's type.
 *
 * @see MusicBrainz::Artist::getType	
 */
MB_API void
mb_artist_get_type(MbArtist artist, char *str, int len);

/**
 * Returns the artist's name.
 *
 * @see MusicBrainz::Artist::getName	
 */
MB_API void
mb_artist_get_name(MbArtist artist, char *str, int len);

/**
 * Returns the artist's sort name.
 *
 * @see MusicBrainz::Artist::getSortName	
 */
MB_API void
mb_artist_get_sortname(MbArtist artist, char *str, int len);

/**
 * Returns the disambiguation attribute. 
 *
 * @see MusicBrainz::Artist::getDisambiguation
 */
MB_API void
mb_artist_get_disambiguation(MbArtist artist, char *str, int len);

/**
 * Returns a unique artist name (using disambiguation). 
 *
 * @see MusicBrainz::Artist::getUniqueName
 */
MB_API void
mb_artist_get_unique_name(MbArtist artist, char *str, int len);

/**
 * Returns the birth/foundation date. 
 *
 * @see MusicBrainz::Artist::getBeginDate
 */
MB_API void
mb_artist_get_begin_date(MbArtist artist, char *str, int len);

/**
 * Returns the death/dissolving date. 
 *
 * @see MusicBrainz::Artist::getEndDate
 */
MB_API void
mb_artist_get_end_date(MbArtist artist, char *str, int len);

/**
 * Returns number of aliases. 
 *
 * @see MusicBrainz::Artist::getNumAliases
 */
MB_API int
mb_artist_get_num_aliases(MbArtist artist);

/**
 * Returns an alias specified by index. 
 *
 * @see MusicBrainz::Artist::getGetAlias
 */
MB_API MbArtistAlias
mb_artist_get_alias(MbArtist artist, int index);

/**
 * Returns number of releases. 
 *
 * @see MusicBrainz::Artist::getNumReleases
 */
MB_API int
mb_artist_get_num_releases(MbArtist artist);

/**
 * Returns an release specified by index. 
 *
 * @see MusicBrainz::Artist::getGetRelease
 */
MB_API MbRelease
mb_artist_get_release(MbArtist artist, int index);

/**
 * Returns the offset of the release list.
 *
 * @see MusicBrainz::Artist::getReleasesOffset
 */
MB_API int
mb_artist_get_releases_offset(MbArtist artist);

/**
 * Returns the number of releases on this artist.
 *
 * @see MusicBrainz::Artist::getReleasesCount
 */
MB_API int
mb_artist_get_releases_count(MbArtist artist);

/**
 * Returns number of relations. 
 *
 * @see MusicBrainz::Artist::getNumRelations
 */
MB_API int
mb_artist_get_num_relations(MbArtist artist);

/**
 * Returns a relation specified by index. 
 *
 * @see MusicBrainz::Artist::getRelation
 */
MB_API MbRelation
mb_artist_get_relation(MbArtist artist, int index);

/* }}} */

/* {{{ MusicBrainz::Release */

/**
 * Deletes the MbRelease instance.
 */
MB_API void
mb_release_free(MbRelease release);

/**
 * Returns the release's ID. 
 *
 * @see MusicBrainz::Release::getId
 */
MB_API void
mb_release_get_id(MbRelease release, char *str, int len);

/**
 * Returns the release's title. 
 *
 * @see MusicBrainz::Release::getTitle
 */
MB_API void
mb_release_get_title(MbRelease release, char *str, int len);

/**
 * Returns the language used in release and track titles. 
 *
 * @see MusicBrainz::Release::getTextLanguage
 */
MB_API void
mb_release_get_text_language(MbRelease release, char *str, int len);

/**
 * Returns the script used in release and track titles. 
 *
 * @see MusicBrainz::Release::getTextScript
 */
MB_API void
mb_release_get_text_script(MbRelease release, char *str, int len);

/**
 * Returns the Amazon shop identifier (ASIN). 
 *
 * @see MusicBrainz::Release::getAsin
 */
MB_API void
mb_release_get_asin(MbRelease release, char *str, int len);

/**
 * Returns the artist. 
 *
 * @see MusicBrainz::Release::getArtist
 */
MB_API MbArtist
mb_release_get_artist(MbRelease release);

/**
 * Returns the offset of the track list.
 *
 * @see MusicBrainz::Release::getTracksOffset
 */
MB_API int
mb_release_get_tracks_offset(MbRelease release);

/**
 * Returns the number of tracks on this release.
 *
 * @see MusicBrainz::Release::getTracksCount
 */
MB_API int
mb_release_get_tracks_count(MbRelease release);

/**
 * Returns number of relations. 
 *
 * @see MusicBrainz::Release::getNumRelations
 */
MB_API int
mb_release_get_num_relations(MbRelease release);

/**
 * Returns a relation specified by index. 
 *
 * @see MusicBrainz::Release::getRelation
 */
MB_API MbRelation
mb_release_get_relation(MbRelease release, int index);

/**
 * Returns number of tracks. 
 *
 * @see MusicBrainz::Release::getNumTracks
 */
MB_API int
mb_release_get_num_tracks(MbRelease release);

/**
 * Returns a track specified by index. 
 *
 * @see MusicBrainz::Release::getTrack
 */
MB_API MbTrack
mb_release_get_track(MbRelease release, int index);

/**
 * Returns number of discs. 
 *
 * @see MusicBrainz::Release::getNumDiscs
 */
MB_API int
mb_release_get_num_discs(MbRelease release);

/**
 * Returns a disc specified by index. 
 *
 * @see MusicBrainz::Release::getDisc
 */
MB_API MbDisc
mb_release_get_disc(MbRelease release, int index);

/**
 * Returns number of release events. 
 *
 * @see MusicBrainz::Release::getNumReleaseEvents
 */
MB_API int
mb_release_get_num_release_events(MbRelease release);

/**
 * Returns a release event specified by index. 
 *
 * @see MusicBrainz::Release::getReleaseEvent
 */
MB_API MbReleaseEvent
mb_release_get_release_event(MbRelease release, int index);

/**
 * Returns number of types.
 *
 * @see MusicBrainz::Release::getNumTypes
 */
MB_API int
mb_release_get_num_types(MbRelease release);

/**
 * Returns a type specified by index.
 *
 * @see MusicBrainz::Release::getType
 */
MB_API void
mb_release_get_type(MbRelease release, int index, char *str, int len);

/* }}} */

/* {{{ MusicBrainz::Track */

/**
 * Deletes the MbTrack instance.
 */
MB_API void
mb_track_free(MbTrack track);

/**
 * Returns the track's ID. 
 *
 * @see MusicBrainz::Track::getId
 */
MB_API void
mb_track_get_id(MbTrack track, char *str, int len);

/**
 * Returns the track's title. 
 *
 * @see MusicBrainz::Track::getTitle
 */
MB_API void
mb_track_get_title(MbTrack track, char *str, int len);

/**
 * Returns the track's duration. 
 *
 * @see MusicBrainz::Track::getDuration
 */
MB_API int
mb_track_get_duration(MbTrack track);

/**
 * Returns number of relations. 
 *
 * @see MusicBrainz::Track::getNumRelations
 */
MB_API int
mb_track_get_num_relations(MbTrack track);

/**
 * Returns a relation specified by index. 
 *
 * @see MusicBrainz::Track::getRelation
 */
MB_API MbRelation
mb_track_get_relation(MbTrack track, int index);

/**
 * Returns the artist. 
 *
 * @see MusicBrainz::Track::getArtist
 */
MB_API MbArtist
mb_track_get_artist(MbTrack track);

/* }}} */

/* {{{ MusicBrainz::ArtistAlias */

/**
 * Returns the alias.
 *
 * @see MusicBrainz::ArtistAlias::getValue
 */
MB_API void
mb_artist_alias_get_value(MbArtistAlias alias, char *str, int len);

/**
 * Returns the alias type. 
 *
 * @see MusicBrainz::ArtistAlias::getType
 */
MB_API void
mb_artist_alias_get_type(MbArtistAlias alias, char *str, int len);

/**
 * Returns the alias script.
 *
 * @see MusicBrainz::ArtistAlias::getScript
 */
MB_API void
mb_artist_alias_get_script(MbArtistAlias alias, char *str, int len);

/* }}} */

/* {{{ MusicBrainz::User */

/**
 * Deletes the MbUsers instance.
 */
MB_API void
mb_user_free(MbUser user);

/**
 * Returns the user name.
 *
 * @see MusicBrainz::User::getName
 */
MB_API void
mb_user_get_name(MbUser user, char *str, int len);

/**
 * Returns true if a nag screen should be displayed to the user.
 *
 * @see MusicBrainz::User::getShowNag
 */
MB_API int
mb_user_get_show_nag(MbUser user);

/**
 * Returns number of types. 
 *
 * @see MusicBrainz::User::getNumTypes
 */
MB_API int
mb_user_get_num_types(MbUser user);

/**
 * Returns a type specified by index. 
 *
 * @see MusicBrainz::User::getType
 */
MB_API void
mb_user_get_type(MbUser user, int index, char *str, int len);

/* }}} */

/* {{{ MusicBrainz::ArtistIncludes */

MB_API MbArtistIncludes
mb_artist_includes_new();

MB_API void
mb_artist_includes_free(MbArtistIncludes inc);

MB_API MbArtistIncludes
mb_artist_includes_aliases(MbArtistIncludes inc);

MB_API MbArtistIncludes
mb_artist_includes_releases(MbArtistIncludes inc, const char *type);

MB_API MbArtistIncludes
mb_artist_includes_va_releases(MbArtistIncludes inc, const char *type);

MB_API MbArtistIncludes
mb_artist_includes_artist_relations(MbArtistIncludes inc);

MB_API MbArtistIncludes
mb_artist_includes_release_relations(MbArtistIncludes inc);

MB_API MbArtistIncludes
mb_artist_includes_track_relations(MbArtistIncludes inc);

MB_API MbArtistIncludes
mb_artist_includes_url_relations(MbArtistIncludes inc);

MB_API MbArtistIncludes
mb_artist_includes_release_events(MbArtistIncludes inc);

/* }}} */

/* {{{ MusicBrainz::ReleaseIncludes */

MB_API MbReleaseIncludes
mb_release_includes_new();

MB_API void
mb_release_includes_free(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_artist(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_counts(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_release_events(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_discs(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_tracks(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_artist_relations(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_release_relations(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_track_relations(MbReleaseIncludes inc);

MB_API MbReleaseIncludes
mb_release_includes_url_relations(MbReleaseIncludes inc);

/* }}} */

/* {{{ MusicBrainz::TrackIncludes */

MB_API MbTrackIncludes
mb_track_includes_new();

MB_API void
mb_track_includes_free(MbArtistIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_artist(MbArtistIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_releases(MbArtistIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_puids(MbArtistIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_artist_relations(MbArtistIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_release_relations(MbTrackIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_track_relations(MbTrackIncludes inc);

MB_API MbTrackIncludes
mb_track_includes_url_relations(MbTrackIncludes inc);

/* }}} */

/* {{{ MusicBrainz::MbArtistFilter */

MB_API MbArtistFilter
mb_artist_filter_new();

MB_API void
mb_artist_filter_free(MbArtistFilter flt);

MB_API MbArtistFilter
mb_artist_filter_name(MbArtistFilter flt, const char *value);

MB_API MbArtistFilter
mb_artist_filter_limit(MbArtistFilter flt, int value);

/* }}} */

/* {{{ MusicBrainz::MbReleaseFilter */

MB_API MbReleaseFilter
mb_release_filter_new();

MB_API void
mb_release_filter_free(MbReleaseFilter flt);

MB_API MbReleaseFilter
mb_release_filter_title(MbReleaseFilter flt, const char *value);

MB_API MbReleaseFilter
mb_release_filter_disc_id(MbReleaseFilter flt, const char *value);

MB_API MbReleaseFilter
mb_release_filter_release_type(MbReleaseFilter flt, const char *value);

MB_API MbReleaseFilter
mb_release_filter_artist_name(MbReleaseFilter flt, const char *value);

MB_API MbReleaseFilter
mb_release_filter_artist_id(MbReleaseFilter flt, const char *value);

MB_API MbReleaseFilter
mb_release_filter_limit(MbReleaseFilter flt, int value);

/* }}} */

/* {{{ MusicBrainz::MbTrackFilter */

MB_API MbTrackFilter
mb_track_filter_new();

MB_API void
mb_track_filter_free(MbTrackFilter flt);

MB_API MbTrackFilter
mb_track_filter_title(MbTrackFilter flt, const char *value);

MB_API MbTrackFilter
mb_track_filter_artist_name(MbTrackFilter flt, const char *value);

MB_API MbTrackFilter
mb_track_filter_artist_id(MbTrackFilter flt, const char *value);

MB_API MbTrackFilter
mb_track_filter_release_title(MbTrackFilter flt, const char *value);

MB_API MbTrackFilter
mb_track_filter_release_id(MbTrackFilter flt, const char *value);

MB_API MbTrackFilter
mb_track_filter_duration(MbTrackFilter flt, int value);

MB_API MbTrackFilter
mb_track_filter_puid(MbTrackFilter flt, const char *value);

MB_API MbTrackFilter
mb_track_filter_limit(MbTrackFilter flt, int value);

/* }}} */

/* {{{ MusicBrainz::MbUserFilter */

MB_API MbUserFilter
mb_user_filter_new();

MB_API void
mb_user_filter_free(MbUserFilter flt);

MB_API MbUserFilter
mb_user_filter_name(MbUserFilter flt, const char *value);

/* }}} */

/* {{{ MusicBrainz::ArtistAlias */

/**
 * Returns the country a release took place.
 *
 * @see MusicBrainz::ReleaseEvent::getCountry 
 */
MB_API void
mb_release_event_get_country(MbReleaseEvent releaseEvent, char *str, int len);

/**
 * Returns the date a release took place.
 *
 * @see MusicBrainz::ReleaseEvent::getDate
 */
MB_API void
mb_release_event_get_date(MbReleaseEvent releaseEvent, char *str, int len);

/* }}} */

/* {{{ MusicBrainz::Relation */

/**
 * Returns this relation's type.
 *
 * @see MusicBrainz::Relation::getType
 */
MB_API void
mb_relation_get_type(MbRelation relation, char *str, int len);

/**
 * Returns the target's ID.
 *
 * @see MusicBrainz::Relation::getTargetId
 */
MB_API void
mb_relation_get_target_id(MbRelation relation, char *str, int len);

/**
 * Returns the target's type.
 *
 * @see MusicBrainz::Relation::getTargetType
 */
MB_API void
mb_relation_get_target_type(MbRelation relation, char *str, int len);

/**
 * Returns the begin date.
 *
 * @see MusicBrainz::Relation::getBeginDate
 */
MB_API void
mb_relation_get_begin_date(MbRelation relation, char *str, int len);

/**
 * Returns the end date.
 *
 * @see MusicBrainz::Relation::getEndDate
 */
MB_API void
mb_relation_get_end_date(MbRelation relation, char *str, int len);

/**
 * Returns the reading direction.
 *
 * @see MusicBrainz::Relation::getDirection
 */
MB_API int
mb_relation_get_direction(MbRelation relation);

/**
 * Returns this relation's target object.
 *
 * @see MusicBrainz::Relation::getTarget
 */
MB_API MbEntity
mb_relation_get_target(MbRelation relation);

/**
 * Returns number of attributes. 
 *
 * @see MusicBrainz::Relation::getNumAttributes
 */
MB_API int
mb_relation_get_num_attributes(MbRelation relation);

/**
 * Returns a attribute specified by index. 
 *
 * @see MusicBrainz::Relation::getAttribute
 */
MB_API void
mb_relation_get_attribute(MbRelation relation, int index, char *str, int len);

/* }}} */

/* {{{ MusicBrainz::Disc */

/**
 * Returns the MusicBrainz DiscID. 
 *
 * @see MusicBrainz::Disc::getId
 */
MB_API void
mb_disc_get_id(MbDisc disc, char *str, int len);

/**
 * Returns the length of the disc in sectors. 
 *
 * @see MusicBrainz::Disc::getSectors
 */
MB_API int
mb_disc_get_sectors(MbDisc disc);

/**
 * Returns the number of the first track on this disc.
 *
 * @see MusicBrainz::Disc::getFirstTrackNum
 */
MB_API int
mb_disc_get_first_track_num(MbDisc disc);

/**
 * Returns the number of the last track on this disc.
 *
 * @see MusicBrainz::Disc::getLastTrackNum
 */
MB_API int
mb_disc_get_last_track_num(MbDisc disc);

/**
 * Reads an Audio CD in the disc drive.
 *
 * @see MusicBrainz::readDisc
 */
MB_API MbDisc
mb_read_disc(const char *device_name);

/**
 * Returns a URL for adding a disc to the MusicBrainz database.
 *
 * @see MusicBrainz::getSubmissionUrl
 */
MB_API void
mb_get_submission_url(MbDisc disc, const char *host, int port, char *str, int len);

/* }}} */

#ifdef __cplusplus
}
#endif

#endif
