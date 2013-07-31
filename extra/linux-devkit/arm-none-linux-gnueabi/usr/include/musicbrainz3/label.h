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
 * $Id: label.h 9218 2007-06-25 20:11:09Z luks $
 */
 
#ifndef __MUSICBRAINZ3_LABEL_H__
#define __MUSICBRAINZ3_LABEL_H__

#include <string>
#include <vector>
#include <musicbrainz3/musicbrainz.h>
#include <musicbrainz3/entity.h>
#include <musicbrainz3/lists.h>

namespace MusicBrainz
{

	/**
	 * Represents a record label.
	 */
	class MB_API Label : public Entity
	{
	public:
	
		static const std::string TYPE_PERSON; 
		static const std::string TYPE_GROUP;
	
		/**
		 * Constructor.
		 *
		 * @param id a string containing an absolute URI
		 * @param type a string containing an absolute URI
		 * @param name a string containing the label's name
		 * @param sortName a string containing the label's sort name 
		 */
		Label(const std::string &id = "", const std::string &type = "",
			  const std::string &name = "", const std::string &sortName = "");
		
		/**
		 * Destructor.
		 */
		virtual ~Label();
		
		/**
		 * Returns the label's type.
		 *
		 * @return a string containing an absolute URI	
		 */
		std::string getType() const;
		
		/**
		 * Sets the label's type.
		 *
		 * @param type a string containing an absolute URI 
		 */
		void setType(const std::string &type);
		
		/**
		 * Returns the label's name.
		 * 
		 * @return a string containing the label's name, or empty string
		 */ 
		std::string getName() const;
		
		/**
		 * Sets the label's name.
		 *
		 * @param name a string containing the label's name
		 */
		void setName(const std::string &name);

		/**
		 * Returns the label's sort name.
		 *
		 * <p>The sort name is the label's name in a special format which
		 * is better suited for lexicographic sorting. The MusicBrainz
		 * style guide specifies this format.</p>
		 * 
		 * @see <a href="http://musicbrainz.org/style.html">The MusicBrainz Style Guidelines</a>
		 */ 
		std::string getSortName() const;
		
		/**
		 * Sets the label's sort name.
		 *
		 * @param sortName: a string containing the label's sort name
		 *
		 * @see #getSortName		 
		 */
		void setSortName(const std::string &sortName);

		/**
		 * Returns the disambiguation attribute.
		 *
		 * This attribute may be used if there is more than one label
		 * with the same name. In this case, disambiguation attributes
		 * are added to the labels' names to keep them apart.
		 *
		 * For example, there are at least three bands named 'Vixen'.
		 * Each band has a different disambiguation in the MusicBrainz
		 * database, like 'Hip-hop' or 'all-female rock/glam band'.
		 *
		 * @return a disambiguation string
		 *
		 * @see getUniqueName
		 */
		std::string getDisambiguation() const;
		
		/**
		 * Sets the disambiguation attribute.
		 *
		 * @param disambiguation a disambiguation string
		 *
		 * @see getDisambiguation, getUniqueName 
		 */
		void setDisambiguation(const std::string &disambiguation);
		
		/**
		 * Returns a unique label name (using disambiguation).
		 *
		 * This method returns the label name together with the
		 * disambiguation attribute in parenthesis if it exists.
		 * Example: 'Vixen (Hip-hop)'.
		 *
		 * @return a string containing the unique name
		 *
		 * @see getDisambiguation
		 */
		std::string getUniqueName() const;
		
		/**
		 * Returns the birth/foundation date.
		 *
		 * The definition of the \e begin \e date depends on the label's
		 * type. For persons, this is the day of birth, for groups it
		 * is the day the group was founded.
		 *
		 * The returned date has the format "YYYY", "YYYY-MM", or 
		 * "YYYY-MM-DD", depending on how much detail is known.
		 *
		 * @return a string containing the date
		 *
		 * @see getType
		 */
		std::string getBeginDate() const;
		
		/**
		 * Sets the begin/foundation date.
		 *
		 * @param dateStr a date string
		 *
		 * @see getBeginDate
		 */
		void setBeginDate(const std::string &dateStr);
		
		/**
		 * Get the death/dissolving date.
		 *
		 * The definition of the \e end \e date depends on the label's
		 * type. For persons, this is the day of death, for groups it
		 * is the day the group was dissolved.
		 *
		 * @return a string containing a date
		 *
		 * @see getBeginDate
		 */
		std::string getEndDate() const;
		
		/**
		 * Sets the death/dissolving date.
		 *
		 * @param dateStr a string containing a date
		 *
		 * @see: setEndDate, getBeginDate
		 */
		void setEndDate(const std::string &dateStr);
		
		/**
		 * Returns a list of releases from this label.
		 *
		 * This may also include releases where this label isn't the
		 * \e main label but has just contributed one or more tracks
		 * (aka VA-Releases).
		 *
		 * @return: a list of pointers to Release objects 
		 */
		ReleaseList &getReleases();
		
		/**
		 * Returns number of releases.
		 *
		 * This is equivalent to \c getReleases().size()
		 *
		 * @return an int containing number of releases
		 *
		 * @see getReleases		 
		 */
		int getNumReleases() const;
		
		/**
		 * Returns an release specified by index.
		 *
		 * This is equivalent to \c getReleases()[index]
		 *
		 * @return a pointer to Release instance
		 *
		 * @see getReleases		 
		 */
		Release *getRelease(int index);
		
		/**
		 * Adds a release to this label's list of releases.
		 *
		 * @param release a pointer to Release object
		 */
		void addRelease(Release *release);

		/**
		 * Returns the offset of the release list.
		 * 
		 * This is used if the track list is incomplete (ie. the web
		 * service only returned part of the tracks on this release).
		 * Note that the offset value is zero-based, which means track
		 * \a 0 is the first track.
		 *
		 * @return an integer containing the offset
		 *
		 * @see getReleases
		 */
		int getReleasesOffset() const;
		
		/**
		 * Sets the offset of the release list. 
		 *
		 * @param offset an integer containing the offset
		 *
		 * @see getReleasesOffset 
		 */
		void setReleasesOffset(const int offset);
		
		/**
		 * Returns the number of existing releases. 
		 * 
		 * This may or may not match with the number of elements that 
		 * getReleases and getNumReleases returns. If the count is higher than 
		 * the list, it indicates that the list is incomplete. 
		 *
		 * @return an integer containing the count
		 *
		 * @see getReleases 
		 */
		int getReleasesCount() const;
		
		/**
		 * Sets the count of the release list.
		 *
		 * @param count an integer containing the count
		 *
		 * @see getReleasesCount 
		 */
		void setReleasesCount(const int count);
		
		/**
		 * Returns the list of aliases for this label.
		 *
		 * @return a list of pointers to LabelAlias objects
		 */
		LabelAliasList &getAliases();
		
		/**
		 * Returns number of aliases.
		 *
		 * This is equivalent to \c getAliases().size()
		 *
		 * @return an int containing number of aliases
		 *
		 * @see getAliases		 
		 */
		int getNumAliases() const;
		
		/**
		 * Returns alias specified by index.
		 *
		 * This is equivalent to \c getAliases()[index]
		 *
		 * @return a pointer to LabelAlias instance
		 *
		 * @see getAliases		 
		 */
		LabelAlias *getAlias(int index);
		
		/**
		 * Adds an alias for this label.
		 *
		 * @param alias a pointer to LabelAlias object
		 */
		void addAlias(LabelAlias *alias);
		
	private:

		class LabelPrivate;
		LabelPrivate *d;
	};
	
}

#endif
