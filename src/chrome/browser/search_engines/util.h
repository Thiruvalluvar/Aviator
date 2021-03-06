// Copyright (c) [2013-2015] WhiteHat. All Rights Reserved. WhiteHat contributions included 
// in this file are licensed under the BSD-3-clause license (the "License") included in 
// the WhiteHat-LICENSE file included in the root directory of the distributed source code 
// archive. Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
// ANY KIND, either express or implied. See the License for the specific language governing 
// permissions and limitations under the License. 

#ifndef CHROME_BROWSER_SEARCH_ENGINES_UTIL_H_
#define CHROME_BROWSER_SEARCH_ENGINES_UTIL_H_

// This file contains utility functions for search engine functionality.
#include <set>
#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "chrome/browser/search_engines/template_url_service.h"

class Profile;
class TemplateURL;
class WDTypedResult;
class WebDataService;

// Returns the short name of the default search engine, or the empty string if
// none is set. |profile| may be NULL.
base::string16 GetDefaultSearchEngineName(Profile* profile);

// Returns a GURL that searches for |terms| using the default search engine of
// |profile|.
GURL GetDefaultSearchURLForSearchTerms(Profile* profile,
                                       const base::string16& terms);

// Returns matching URL from |template_urls| or NULL.
TemplateURL* FindURLByPrepopulateID(
    const TemplateURLService::TemplateURLVector& template_urls,
    int prepopulate_id);

	//champion -  To get default search engine by using Id instead of prepopulated id to set any search engine apart from prepopulated list -  Navneet 
TemplateURL* FindURLByPrepopulateIDDisconnectAdd(
    const TemplateURLService::TemplateURLVector& template_urls,
    int id); 
	
// Modifies |prepopulated_url| so that it contains user-modified fields from
// |original_turl|. Both URLs must have the same prepopulate_id.
void MergeIntoPrepopulatedEngineData(const TemplateURL* original_turl,
                                     TemplateURLData* prepopulated_url);

// CreateActionsFromCurrentPrepopulateData() (see below) takes in the current
// prepopulated URLs as well as the user's current URLs, and returns an instance
// of the following struct representing the changes necessary to bring the
// user's URLs in line with the prepopulated URLs.
//
// There are three types of changes:
// (1) Previous prepopulated engines that no longer exist in the current set of
//     prepopulated engines and thus should be removed from the user's current
//     URLs.
// (2) Previous prepopulated engines whose data has changed.  The existing
//     entries for these engines should be updated to reflect the new data,
//     except for any user-set names and keywords, which can be preserved.
// (3) New prepopulated engines not in the user's engine list, which should be
//     added.

// The pair of current search engine and its new value.
typedef std::pair<TemplateURL*, TemplateURLData> EditedSearchEngine;
typedef std::vector<EditedSearchEngine> EditedEngines;

struct ActionsFromPrepopulateData {
  ActionsFromPrepopulateData();
  ~ActionsFromPrepopulateData();

  TemplateURLService::TemplateURLVector removed_engines;
  EditedEngines edited_engines;
  std::vector<TemplateURLData> added_engines;
};

// Given the user's current URLs and the current set of prepopulated URLs,
// produces the set of actions (see above) required to make the user's URLs
// reflect the prepopulate data.  |default_search_provider| is used to avoid
// placing the current default provider on the "to be removed" list.
//
// NOTE: Takes ownership of, and clears, |prepopulated_urls|.
ActionsFromPrepopulateData CreateActionsFromCurrentPrepopulateData(
    ScopedVector<TemplateURLData>* prepopulated_urls,
    const TemplateURLService::TemplateURLVector& existing_urls,
    const TemplateURL* default_search_provider);

// Processes the results of WebDataService::GetKeywords, combining it with
// prepopulated search providers to result in:
//  * a set of template_urls (search providers). The caller owns the
//    TemplateURL* returned in template_urls.
//  * whether there is a new resource keyword version (and the value).
//    |*new_resource_keyword_version| is set to 0 if no new value. Otherwise,
//    it is the new value.
// Only pass in a non-NULL value for service if the WebDataService should be
// updated. If |removed_keyword_guids| is not NULL, any TemplateURLs removed
// from the keyword table in the WebDataService will have their Sync GUIDs
// added to it. |default_search_provider| will be used to prevent removing the
// current user-selected DSE, regardless of changes in prepopulate data.
void GetSearchProvidersUsingKeywordResult(
    const WDTypedResult& result,
    WebDataService* service,
    Profile* profile,
    TemplateURLService::TemplateURLVector* template_urls,
    TemplateURL* default_search_provider,
    const SearchTermsData& search_terms_data,
    int* new_resource_keyword_version,
    std::set<std::string>* removed_keyword_guids);

// Like GetSearchProvidersUsingKeywordResult(), but allows the caller to pass in
// engines in |template_urls| instead of getting them via processing a web data
// service request.
// |resource_keyword_version| should contain the version number of the current
// keyword data, i.e. the version number of the most recent prepopulate data
// that has been merged into the current keyword data.  On exit, this will be
// set as in GetSearchProvidersUsingKeywordResult().
void GetSearchProvidersUsingLoadedEngines(
    WebDataService* service,
    Profile* profile,
    TemplateURLService::TemplateURLVector* template_urls,
    TemplateURL* default_search_provider,
    const SearchTermsData& search_terms_data,
    int* resource_keyword_version,
    std::set<std::string>* removed_keyword_guids);

// Due to a bug, the |input_encodings| field of TemplateURLData could have
// contained duplicate entries.  This removes those entries and returns whether
// any were found.
bool DeDupeEncodings(std::vector<std::string>* encodings);

// Removes (and deletes) TemplateURLs from |template_urls| and |service| if they
// have duplicate prepopulate ids. If |removed_keyword_guids| is not NULL, the
// Sync GUID of each item removed from the DB will be added to it. This is a
// helper used by GetSearchProvidersUsingKeywordResult(), but is declared here
// so it's accessible by unittests.
void RemoveDuplicatePrepopulateIDs(
    WebDataService* service,
    const ScopedVector<TemplateURLData>& prepopulated_urls,
    TemplateURL* default_search_provider,
    TemplateURLService::TemplateURLVector* template_urls,
    const SearchTermsData& search_terms_data,
    std::set<std::string>* removed_keyword_guids);

#endif  // CHROME_BROWSER_SEARCH_ENGINES_UTIL_H_
