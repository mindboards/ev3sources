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
 * $Id: entity.h 9216 2007-06-25 19:42:20Z luks $
 */
 
#ifndef __MUSICBRAINZ3_ENTITY_H__
#define __MUSICBRAINZ3_ENTITY_H__

#include <string>
#include <musicbrainz3/musicbrainz.h> 
#include <musicbrainz3/lists.h> 
#include <musicbrainz3/relation.h> 
#include <musicbrainz3/tag.h> 

namespace MusicBrainz
{

	/**
	 * A first-level MusicBrainz class.
	 *
	 * All entities in MusicBrainz have unique IDs (which are absolute URIs)
	 * and may have any number of relations (Relation) to other entities.
	 * This class is abstract and should not be instantiated.
	 * 
	 * Relations are differentiated by their \e target \e type, that means,
	 * where they link to. MusicBrainz currently supports four target types
	 * (artists, releases, tracks, and URLs) each identified using a URI.
	 * To get all relations with a specific target type, you can use
	 * getRelations and pass one of the following constants as the
	 * parameter:
	 *
	 *	 - Relation::TO_ARTIST
	 *	 - Relation::TO_RELEASE
	 *	 - Relation::TO_TRACK
	 *	 - Relation::TO_URL
	 *	   
	 * @see Relation 
	 */
	class MB_API Entity
	{
	protected:
	
		/**
		 * Constructor.
		 *
		 * This should only used by derived classes.
		 *
		 * @param id a string containing an absolute URI 
		 */	 
		Entity(const std::string &id);
		
	public:
		
		/**
		 * Destructor.
		 */
		virtual ~Entity();
		
		/**
		 * Returns a MusicBrainz ID.
		 * 
		 * @return a string containing a URI
		 */
		virtual std::string getId() const;
		
		/**
		 * Sets a MusicBrainz ID.
		 *
		 * @param value a string containing an absolute URI	 
		 */
		virtual void setId(const std::string &value);

		/**
		 * Returns a list of relations.
		 *
		 * If \a targetType is given, only relations of that target
		 * type are returned. For MusicBrainz, the following target
		 * types are defined:
		 *
		 *  - Relation::TO_ARTIST
		 *  - Relation::TO_RELEASE
		 *  - Relation::TO_TRACK
		 *  - Relation::TO_URL
		 *
		 * If \a targetType is Relation::TO_ARTIST, for example,
		 * this method returns all relations between this Entity and
		 * artists.
		 *
		 * You may use the \a relationType parameter to further restrict
		 * the selection. If it is set, only relations with the given
		 * relation type are returned.
		 *
		 * @param targetType a string containing an absolute URI
		 * @param relationType a string containing an absolute URI
		 *
		 * @return a list of pointers to Relation objects
		 *
		 * @see Entity
		 */
		RelationList getRelations(const std::string &targetType = std::string(),
								  const std::string &relationType = std::string()) const;
		
		/**
		 * Adds a relation.
		 *
		 * This method adds \a relation to the list of relations. The
		 * given relation has to be initialized, at least the target
		 * type has to be set.
		 *
		 * @param relation a pointer to Relation object to add
		 */
		void addRelation(Relation *relation);
		
		/**
		 * Returns number of relations.
		 *
		 * This is equivalent to \c getRelations().size()
		 *
		 * @return an int containing number of relations
		 *
		 * @see getRelations		 
		 */
		int getNumRelations() const;
		
		/**
		 * Returns an relation specified by index.
		 *
		 * This is equivalent to \c getRelations()[index]
		 *
		 * @return a pointer to Relation instance
		 *
		 * @see getRelations		 
		 */
		Relation *getRelation(int index);

		TagList &getTags();
		int getNumTags() const;
		Tag *getTag(int index);
		
	private:

		class EntityPrivate;
		EntityPrivate *d;
	};
	
}

#endif

