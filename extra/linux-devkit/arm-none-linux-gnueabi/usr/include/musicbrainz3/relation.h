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
 * $Id: relation.h 8466 2006-09-05 08:59:44Z luks $
 */
 
#ifndef __MUSICBRAINZ3_RELATION_H__
#define __MUSICBRAINZ3_RELATION_H__

#include <string>
#include <vector>
#include <musicbrainz3/musicbrainz.h>

namespace MusicBrainz
{
	
	class Entity;
	
	/**
	 * Represents a relation between two Entities.
	 *
	 * There may be an arbitrary number of relations between all first
	 * class objects in MusicBrainz. The Relation itself has multiple
	 * attributes, which may or may not be used for a given relation
	 * type.
	 *
	 * Note that a Relation object only contains the target but not
	 * the source end of the relation.
	 */
	class MB_API Relation
	{
	public:

		//! Identifies relations linking to an artist. 
		static const std::string TO_ARTIST;
		//! Identifies relations linking to a release.
		static const std::string TO_RELEASE;
		//! Identifies relations linking to a track.
		static const std::string TO_TRACK;
		//! Identifies relations linking to an URL.
		static const std::string TO_URL;  
	
		enum Direction {
			//! Relation reading direction doesn't matter. 
			DIR_BOTH,
			//! Relation reading direction is from source to target. 
			DIR_FORWARD,
			//! Relation reading direction is from target to source. 
			DIR_BACKWARD
		};
		
		//! A vector of strings (attributes).
		typedef std::vector<std::string> Attributes;
		
		/**
		 * Constructor.
		 *
		 * @param relationType a string containing an absolute URI
		 * @param targetType a string containing an absolute URI
		 * @param targetId a string containing an absolute URI
		 * @param direction one of Relation::DIR_FORWARD,
		 * Relation::DIR_BACKWARD, or Relation::DIR_BOTH
		 * @param attributes a list of strings containing absolute URIs
		 * @param beginDate a string containing a date
		 * @param endDate a string containing a date
		 * @param target an instance of a subclass of Entity, or NULL 
		 */
		Relation(const std::string &relationType = std::string(),
				 const std::string &targetType = std::string(),
				 const std::string &targetId = std::string(),
				 const Direction direction = DIR_BOTH,
				 const Attributes &attributes = Attributes(),
				 const std::string &beginDate = std::string(),
				 const std::string &endDate = std::string(),
				 Entity *target = NULL);
		
		/**
		 * Destructor.
		 */
		virtual ~Relation();
		
		/**
		 * Returns this relation's type.
		 *
		 * @return a string containing an absolute URI
		 */
		std::string getType() const;
		
		/**
		 * Sets this relation's type.
		 *
		 * @param type a string containing an absolute URI
		 */
		void setType(const std::string &type);
		
		/**
		 * Returns the target's ID.
		 *
		 * This is the ID the relation points to. It is an absolute
		 * URI, and in case of an URL relation, it is a URL.
		 *
		 * @return a string containing an absolute URI
		 */
		std::string getTargetId() const;
		
		/**
		 * Sets the target's ID.
		 *
		 * @param targetId a string containing an absolute URI
		 *
		 * @see getTargetId
		 */
		void setTargetId(const std::string &targetId);
		
		/**
		 * Returns the target's type.
		 *
		 * For MusicBrainz data, the following target types are defined:
		 *   - artists: Relation::TO_ARTIST
		 *   - releases: Relation::TO_RELEASE
		 *   - tracks: Relation::TO_TRACK
		 *   - urls: Relation::TO_URL
		 *
		 * @return a string containing an absolute URI
		 */
		std::string getTargetType() const;
		
		/**
		 * Sets the target's type.
		 *
		 * @param targetType a string containing an absolute URI
		 *
		 * @see getTargetType
		 */
		void setTargetType(const std::string &targetType);
		
		/**
		 * Returns the begin date.
		 *
		 * The definition depends on the relation's type. It may for
		 * example be the day of a marriage or the year an artist
		 * joined a band. For other relation types this may be
		 * undefined.
		 *
		 * @return a string containing a date
		 */
		std::string getBeginDate() const;
		
		/**
		 * Sets the begin date.
		 *
		 * @param dateStr a string containing a date
		 *
		 * @see getBeginDate
		 */
		void setBeginDate(const std::string &dateStr);
		
		/**
		 * Returns the end date.
		 *
		 * As with the begin date, the definition depends on the
		 * relation's type. Depending on the relation type, this may
		 * or may not be defined.
		 *
		 * @return a string containing a date
		 *
		 * @see getBeginDate
		 */
		std::string getEndDate() const;
		
		/**
		 * Sets the end date.
		 *
		 * @param dateStr: a string containing a date
		 *
		 * @see getBeginDate
		 */
		void setEndDate(const std::string &dateStr);
		
		/**
		 * Returns the reading direction.
		 *
		 * The direction may be one of Relation::DIR_FORWARD,
		 * Relation::DIR_BACKWARD, or Relation::DIR_BOTH,
		 * depending on how the relation should be read. For example,
		 * if direction is Relation::DIR_FORWARD for a cover relation,
		 * it is read as "X is a cover of Y". Some relations are
		 * bidirectional, like marriages. In these cases, the direction
		 * is Relation::DIR_BOTH.
		 *
		 * @return Relation::DIR_FORWARD, Relation::DIR_BACKWARD,
		 * or Relation::DIR_BOTH
		 */
		Direction getDirection() const;
		
		/**
		 * Sets the reading direction.
		 *
		 * @param direction Relation::DIR_FORWARD,
		 * Relation::DIR_BACKWARD, or Relation::DIR_BOTH
		 *
		 * @see getDirection
		 */
		void setDirection(const Direction direction);

		/**
		 * Returns a list of attributes describing this relation.
		 *
		 * The attributes permitted depend on the relation type.
		 *
		 * @return a list of strings containing absolute URIs
		 */
		Attributes &getAttributes();
		
		/**
		 * Returns number of attributes.
		 *
		 * This is equivalent to \c getAttributes().size()
		 *
		 * @return an int containing number of attributes
		 *
		 * @see getAttributes		 
		 */
		int getNumAttributes() const;
		
		/**
		 * Returns an attribute specified by index.
		 *
		 * This is equivalent to \c getAttributes()[index]
		 *
		 * @return a string containing the attribute
		 *
		 * @see getAttributes		 
		 */
		std::string getAttribute(int index) const;
		
		/**
		 * Adds an attribute to the list.
		 *
		 * @param attribute a string containing an absolute URI
		 */
		void addAttribute(const std::string &attribute);
		
		/**
		 * Returns this relation's target object.
		 *
		 * Note that URL relations never have a target object. Use the
		 * getTargetId method to get the URL.
		 * 
		 * @return a subclass of Entity, or NULL 
		 */ 
		Entity *getTarget() const;
		
		/**
		 * Sets this relation's target object.
		 *
		 * Note that URL relations never have a target object, they
		 * are set using setTargetId.
		 * 
		 * @param target a subclass of Entity, or NULL 
		 */
		void setTarget(Entity *target);
		
	private:
		
		class RelationPrivate;
		RelationPrivate *d;
	};
	
}

#endif

