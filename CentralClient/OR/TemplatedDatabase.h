/**
 * File: TemplatedDatabase.h
 * Date: March 2011
 * Author: Dorian Galvez-Lopez
 * Description: templated database of images
 *
 * This file is licensed under a Creative Commons 
 * Attribution-NonCommercial-ShareAlike 3.0 license. 
 * This file can be freely used and users can use, download and edit this file 
 * provided that credit is attributed to the original author. No users are 
 * permitted to use this file for commercial purposes unless explicit permission
 * is given by the original author. Derivative works must be licensed using the
 * same or similar license.
 * Check http://creativecommons.org/licenses/by-nc-sa/3.0/ to obtain further
 * details.
 *
 */
 
#ifndef __D_T_TEMPLATED_DATABASE__
#define __D_T_TEMPLATED_DATABASE__

#include <vector>
#include <numeric>
#include <fstream>
#include <string>
#include <list>
#include <set>

#include "TemplatedVocabulary.h"
#include "QueryResults.h"
#include "ScoringObject.h"
#include "BowVector.h"
#include "FeatureVector.h"

#include "DUtils.h"

using namespace std;

namespace DBoW2 {

/// @param TDescriptor class of descriptor
/// @param F class of descriptor functions
template<class TDescriptor, class F>
/// Generic Database
class TemplatedDatabase
{
public:

  /**
   * Creates an empty database without vocabulary
   * @param use_di a direct index is used to store feature indexes
   * @param di_levels levels to go up the vocabulary tree to select the 
   *   node id to store in the direct index when adding images
   */
  TemplatedDatabase(bool use_di = true, int di_levels = 0);

  /**
   * Creates a database with the given vocabulary
   * @param voc vocabulary
   * @param use_di a direct index is used to store feature indexes
   * @param di_levels levels to go up the vocabulary tree to select the 
   *   node id to store in the direct index when adding images
   */
  TemplatedDatabase(const TemplatedVocabulary<TDescriptor, F> &voc,
    bool use_di = true, int di_levels = 0);

  /**
   * Copy constructor. Copies the vocabulary too
   * @param db object to copy
   */
  TemplatedDatabase(const TemplatedDatabase<TDescriptor, F> &db);

  /** 
   * Creates the database from a file
   * @param filename
   */
  TemplatedDatabase(const std::string &filename);

  /** 
   * Creates the database from a file
   * @param filename
   */
  TemplatedDatabase(const char *filename);

  /**
   * Destructor
   */
  virtual ~TemplatedDatabase(void);

  /**
   * Copies the given database and its vocabulary
   * @param db database to copy
   */
  TemplatedDatabase<TDescriptor,F>& operator=(
    const TemplatedDatabase<TDescriptor,F> &db);

  /**
   * Sets the vocabulary to use and clears the content of the database.
   * @param voc vocabulary to copy
   */
  inline void setVocabulary(const TemplatedVocabulary<TDescriptor,F> &voc);
  
  /**
   * Sets the vocabulary to use and the direct index parameters, and clears
   * the content of the database
   * @param voc vocabulary to copy
   * @param use_di a direct index is used to store feature indexes
   * @param di_levels levels to go up the vocabulary tree to select the 
   *   node id to store in the direct index when adding images
   */
  inline void setVocabulary(const TemplatedVocabulary<TDescriptor,F> &voc,
    bool use_di, int di_levels = 0);
  
  /**
   * Returns a reference to the vocabulary used
   * @return vocabulary
   */
  inline const TemplatedVocabulary<TDescriptor,F>& getVocabulary() const;

  /** 
   * Allocates some memory for the direct and inverted indexes
   * @param nd number of expected image entries in the database 
   * @param ni number of expected words per image
   * @note Use 0 to ignore a parameter
   */
  void allocate(int nd = 0, int ni = 0);

  /**
   * Adds an entry to the database and returns its index
   * @param features features of the new entry
   * @param bowvec if given, the bow vector of these features is returned
   * @param fvec if given, the vector of nodes and feature indexes is returned
   * @return id of new entry
   */
  EntryId add(const vector<TDescriptor> &features,
    BowVector *bowvec = NULL, FeatureVector *fvec = NULL);

  /**
   * Adss an entry to the database and returns its index
   * @param vec bow vector
   * @param fec feature vector to add the entry. Only necessary if using the
   *   direct index
   * @return id of new entry
   */
  EntryId add(const BowVector &vec, 
    const FeatureVector &fec = FeatureVector() );

  /**
   * Empties the database
   */
  inline void clear();

  /**
   * Returns the number of entries in the database 
   * @return number of entries in the database
   */
  inline unsigned int size() const;
  
  /**
   * Checks if the direct index is being used
   * @return true iff using direct index
   */
  inline bool usingDirectIndex() const;
  
  /**
   * Returns the di levels when using direct index
   * @return di levels
   */
  inline int getDirectIndexLevels() const;
  
  /**
   * Queries the database with some features
   * @param features query features
   * @param ret (out) query results
   * @param max_results number of results to return. <= 0 means all
   * @param max_id only entries with id <= max_id are returned in ret. 
   *   < 0 means all
   */
  void query(const vector<TDescriptor> &features, QueryResults &ret,
    int max_results = 1, int max_id = -1) const;
  
  /**
   * Queries the database with a vector
   * @param vec bow vector already normalized
   * @param ret results
   * @param max_results number of results to return. <= 0 means all
   * @param max_id only entries with id <= max_id are returned in ret. 
   *   < 0 means all
   */
  void query(const BowVector &vec, QueryResults &ret, 
    int max_results = 1, int max_id = -1) const;

  /**
   * Returns the a feature vector associated with a database entry
   * @param id entry id (must be < size())
   * @return const reference to map of nodes and their associated features in
   *   the given entry
   */
  const FeatureVector& retrieveFeatures(EntryId id) const;

  /**
   * Stores the database in a file
   * @param filename
   */
  void save(const string &filename) const;
  
  /**
   * Loads the database from a file
   * @param filename
   */
  void load(const string &filename);
  
  /** 
   * Stores the database in the given file storage structure
   * @param fs
   * @param name node name
   */
  void save(cv::FileStorage &fs, const std::string &name = "database") const;
  
  /** 
   * Loads the database from the given file storage structure
   * @param fs
   * @param name node name
   */
  void load(cv::FileStorage &fs, const std::string &name = "database");
  
protected:
  
  /// Query with L1 scoring
  void queryL1(const BowVector &vec, QueryResults &ret, 
    int max_results, int max_id) const;
  
  /// Query with L2 scoring
  void queryL2(const BowVector &vec, QueryResults &ret, 
    int max_results, int max_id) const;
  
  /// Query with Chi square scoring
  void queryChiSquare(const BowVector &vec, QueryResults &ret, 
    int max_results, int max_id) const;
  
  /// Query with Bhattacharyya scoring
  void queryBhattacharyya(const BowVector &vec, QueryResults &ret, 
    int max_results, int max_id) const;
  
  /// Query with KL divergence scoring  
  void queryKL(const BowVector &vec, QueryResults &ret, 
    int max_results, int max_id) const;
  
  /// Query with dot product scoring
  void queryDotProduct(const BowVector &vec, QueryResults &ret, 
    int max_results, int max_id) const;

protected:

  /* Inverted file declaration */
  
  /// Item of IFRow
  struct IFPair
  {
    /// Entry id
    EntryId entry_id;
    
    /// Word weight in this entry
    WordValue word_weight;
    
    /**
     * Creates an empty pair
     */
    IFPair(){}
    
    /**
     * Creates an inverted file pair
     * @param eid entry id
     * @param wv word weight
     */
    IFPair(EntryId eid, WordValue wv): entry_id(eid), word_weight(wv) {}
    
    /**
     * Compares the entry ids
     * @param eid
     * @return true iff this entry id is the same as eid
     */
    inline bool operator==(EntryId eid) const { return entry_id == eid; }
  };
  
  /// Row of InvertedFile
  typedef std::list<IFPair> IFRow;
  // IFRows are sorted in ascending entry_id order
  // ## map?
  
  /// Inverted index
  typedef std::vector<IFRow> InvertedFile; 
  // InvertedFile[word_id] --> inverted file of that word
  
  /* Direct file declaration */

  /// Direct index
  typedef std::vector<FeatureVector> DirectFile;
  // DirectFile[entry_id] --> [ directentry, ... ]

protected:

  /// Associated vocabulary
  TemplatedVocabulary<TDescriptor, F> m_voc;
  
  /// Flag to use direct index
  bool m_use_di;
  
  /// Levels to go up the vocabulary tree to select nodes to store
  /// in the direct index
  int m_dilevels;
  
  /// Inverted file (must have size() == |words|)
  InvertedFile m_ifile;
  
  /// Direct file (resized for allocation)
  DirectFile m_dfile;
  
  /// Number of valid entries in m_dfile
  int m_nentries;
  
};

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor, F>::TemplatedDatabase
  (bool use_di, int di_levels)
  : m_use_di(use_di), m_dilevels(di_levels)
{
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor, F>::TemplatedDatabase
  (const TemplatedVocabulary<TDescriptor, F> &voc, 
    bool use_di, int di_levels)
  : m_voc(voc), m_use_di(use_di), m_dilevels(di_levels)
{
  clear();
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor,F>::TemplatedDatabase
  (const TemplatedDatabase<TDescriptor,F> &db)
{
  *this = db;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor, F>::TemplatedDatabase
  (const std::string &filename)
{
  load(filename);
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor, F>::TemplatedDatabase
  (const char *filename)
{
  load(filename);
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor, F>::~TemplatedDatabase(void)
{
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
TemplatedDatabase<TDescriptor,F>& TemplatedDatabase<TDescriptor,F>::operator=
  (const TemplatedDatabase<TDescriptor,F> &db)
{
  if(this != &db)
  {
    m_dfile = db.m_dfile;
    m_dilevels = db.m_dilevels;
    m_ifile = db.m_ifile;
    m_nentries = db.m_nentries;
    m_use_di = db.m_use_di;
    m_voc = db.m_voc;
  }
  return *this;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
EntryId TemplatedDatabase<TDescriptor, F>::add(
  const vector<TDescriptor> &features,
  BowVector *bowvec, FeatureVector *fvec)
{
  BowVector aux;
  BowVector& v = (bowvec ? *bowvec : aux);
  
  if(m_use_di && fvec != NULL)
  {
    m_voc.transform(features, v, *fvec, m_dilevels); // with features
    return add(v, *fvec);
  }
  else if(m_use_di)
  {
    FeatureVector fv;
    m_voc.transform(features, v, fv, m_dilevels); // with features
    return add(v, fv);
  }
  else if(fvec != NULL)
  {
    m_voc.transform(features, v, *fvec, m_dilevels); // with features
    return add(v);
  }
  else //normal entry is here (ZJZ)
  {
    m_voc.transform(features, v); // with features //(ZJZ) get the most similar vector in the database
    return add(v);
  }
}

// ---------------------------------------------------------------------------

template<class TDescriptor, class F>
EntryId TemplatedDatabase<TDescriptor, F>::add(const BowVector &v,
  const FeatureVector &fv)
{
  EntryId entry_id = m_nentries++;

  BowVector::const_iterator vit;
  vector<unsigned int>::const_iterator iit;

  if(m_use_di)
  {
    // update direct file
    if(entry_id == m_dfile.size())
    {
      m_dfile.push_back(fv);
    }
    else
    {
      m_dfile[entry_id] = fv;
    }
  }
  
  // update inverted file
  for(vit = v.begin(); vit != v.end(); ++vit)
  {
    const WordId& word_id = vit->first;
    const WordValue& word_weight = vit->second;
    
    IFRow& ifrow = m_ifile[word_id];
    ifrow.push_back(IFPair(entry_id, word_weight));
  }
  
  return entry_id;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline void TemplatedDatabase<TDescriptor, F>::setVocabulary
  (const TemplatedVocabulary<TDescriptor,F>& voc)
{
  m_voc = voc;
  clear();
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline void TemplatedDatabase<TDescriptor, F>::setVocabulary
  (const TemplatedVocabulary<TDescriptor,F>& voc, 
  bool use_di, int di_levels)
{
  m_use_di = use_di;
  m_dilevels = di_levels;
  m_voc = voc;
  clear();
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline const TemplatedVocabulary<TDescriptor,F>& 
TemplatedDatabase<TDescriptor, F>::getVocabulary() const
{
  return m_voc;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline void TemplatedDatabase<TDescriptor, F>::clear()
{
  // resize vectors
  m_ifile.resize(0);
  m_ifile.resize(m_voc.size());
  m_dfile.resize(0);
  m_nentries = 0;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::allocate(int nd, int ni)
{
  // m_ifile already contains |words| items
  if(ni > 0)
  {
    typename std::vector<IFRow>::iterator rit;
    for(rit = m_ifile.begin(); rit != m_ifile.end(); ++rit)
    {
      int n = (int)rit->size();
      if(ni > n)
      {
        rit->resize(ni);
        rit->resize(n);
      }
    }
  }
  
  if(m_use_di && (int)m_dfile.size() < nd)
  {
    m_dfile.resize(nd);
  }
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline unsigned int TemplatedDatabase<TDescriptor, F>::size() const
{
  return m_nentries;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline bool TemplatedDatabase<TDescriptor, F>::usingDirectIndex() const
{
  return m_use_di;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
inline int TemplatedDatabase<TDescriptor, F>::getDirectIndexLevels() const
{
  return m_dilevels;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::query(
  const vector<TDescriptor> &features, 
  QueryResults &ret, int max_results, int max_id) const
{
  BowVector vec;
  m_voc.transform(features, vec);  // transform the sift descriptor to bag of words vector.
  query(vec, ret, max_results, max_id);
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::query(
  const BowVector &vec, 
  QueryResults &ret, int max_results, int max_id) const
{
  ret.resize(0);
  
  switch(m_voc.getScoringType())
  {
    case L1_NORM:
      queryL1(vec, ret, max_results, max_id);
      break;
      
    case L2_NORM:
      queryL2(vec, ret, max_results, max_id);
      break;
      
    case CHI_SQUARE:
      queryChiSquare(vec, ret, max_results, max_id);
      break;
      
    case KL:
      queryKL(vec, ret, max_results, max_id);
      break;
      
    case BHATTACHARYYA:
      queryBhattacharyya(vec, ret, max_results, max_id);
      break;
      
    case DOT_PRODUCT:
      queryDotProduct(vec, ret, max_results, max_id);
      break;
  }
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::queryL1(const BowVector &vec, 
  QueryResults &ret, int max_results, int max_id) const
{
  BowVector::const_iterator vit;
  typename IFRow::const_iterator rit;
  
  map<EntryId, double> pairs;
  map<EntryId, double>::iterator pit;
  
  for(vit = vec.begin(); vit != vec.end(); ++vit)
  {
    const WordId word_id = vit->first;
    const WordValue& qvalue = vit->second;
        
    const IFRow& row = m_ifile[word_id];
    
    // IFRows are sorted in ascending entry_id order
    
    for(rit = row.begin(); rit != row.end(); ++rit)
    {
      const EntryId entry_id = rit->entry_id;
      const WordValue& dvalue = rit->word_weight;
      
      if((int)entry_id < max_id || max_id == -1)
      {
        double value = fabs(qvalue - dvalue) - fabs(qvalue) - fabs(dvalue);
        
        pit = pairs.lower_bound(entry_id);
        if(pit != pairs.end() && !(pairs.key_comp()(entry_id, pit->first)))
        {
          pit->second += value;
        }
        else
        {
          pairs.insert(pit, 
            map<EntryId, double>::value_type(entry_id, value));
        }
      }
      
    } // for each inverted row
  } // for each query word
	
  // move to vector
  ret.reserve(pairs.size());
  for(pit = pairs.begin(); pit != pairs.end(); ++pit)
  {
    ret.push_back(Result(pit->first, pit->second));
  }
	
  // resulting "scores" are now in [-2 best .. 0 worst]	
  
  // sort vector in ascending order of score
  sort(ret.begin(), ret.end());
  // (ret is inverted now --the lower the better--)

  // cut vector
  if(max_results > 0 && (int)ret.size() > max_results)
    ret.resize(max_results);
  
  // complete and scale score to [0 worst .. 1 best]
  // ||v - w||_{L1} = 2 + Sum(|v_i - w_i| - |v_i| - |w_i|) 
  //		for all i | v_i != 0 and w_i != 0 
  // (Nister, 2006)
  // scaled_||v - w||_{L1} = 1 - 0.5 * ||v - w||_{L1}
  QueryResults::iterator qit;
  for(qit = ret.begin(); qit != ret.end(); qit++) 
    qit->Score = -qit->Score/2.0;
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::queryL2(const BowVector &vec, 
  QueryResults &ret, int max_results, int max_id) const
{
  BowVector::const_iterator vit;
  typename IFRow::const_iterator rit;
  
  map<EntryId, double> pairs;
  map<EntryId, double>::iterator pit;
  
  for(vit = vec.begin(); vit != vec.end(); ++vit)
  {
    const WordId word_id = vit->first;
    const WordValue& qvalue = vit->second;
    
    const IFRow& row = m_ifile[word_id];
    
    // IFRows are sorted in ascending entry_id order
    
    for(rit = row.begin(); rit != row.end(); ++rit)
    {
      const EntryId entry_id = rit->entry_id;
      const WordValue& dvalue = rit->word_weight;
      
      if((int)entry_id < max_id || max_id == -1)
      {
        double value = - qvalue * dvalue; // minus sign for sorting trick
        
        pit = pairs.lower_bound(entry_id);
        if(pit != pairs.end() && !(pairs.key_comp()(entry_id, pit->first)))
        {
          pit->second += value;
        }
        else
        {
          pairs.insert(pit, 
            map<EntryId, double>::value_type(entry_id, value));
        }
      }
      
    } // for each inverted row
  } // for each query word
	
  // move to vector
  ret.reserve(pairs.size());
  for(pit = pairs.begin(); pit != pairs.end(); ++pit)
  {
    ret.push_back(Result(pit->first, pit->second));
  }
	
  // resulting "scores" are now in [-1 best .. 0 worst]	
  
  // sort vector in ascending order of score
  sort(ret.begin(), ret.end());
  // (ret is inverted now --the lower the better--)

  // cut vector
  if(max_results > 0 && (int)ret.size() > max_results)
    ret.resize(max_results);

  // complete and scale score to [0 worst .. 1 best]
  // ||v - w||_{L2} = sqrt( 2 - 2 * Sum(v_i * w_i) 
	//		for all i | v_i != 0 and w_i != 0 )
	// (Nister, 2006)
	QueryResults::iterator qit;
  for(qit = ret.begin(); qit != ret.end(); qit++) 
  {
    if(qit->Score <= -1.0) // rounding error
      qit->Score = 1.0;
    else
      qit->Score = 1.0 - sqrt(1.0 + qit->Score); // [0..1]
      // the + sign is ok, it is due to - sign in 
      // value = - qvalue * dvalue
  }
  
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::queryChiSquare(const BowVector &vec, 
  QueryResults &ret, int max_results, int max_id) const
{
  BowVector::const_iterator vit;
  typename IFRow::const_iterator rit;
  
  map<EntryId, double> pairs;
  map<EntryId, double>::iterator pit;
  
  for(vit = vec.begin(); vit != vec.end(); ++vit)
  {
    const WordId word_id = vit->first;
    const WordValue& qvalue = vit->second;
    
    const IFRow& row = m_ifile[word_id];
    
    // IFRows are sorted in ascending entry_id order
    
    for(rit = row.begin(); rit != row.end(); ++rit)
    {
      const EntryId entry_id = rit->entry_id;
      const WordValue& dvalue = rit->word_weight;
      
      if((int)entry_id < max_id || max_id == -1)
      {
        // (v-w)^2/(v+w) - v - w = -4 vw/(v+w)
        // we move the 4 out
        double value = 0;
        if(qvalue + dvalue != 0.0) // words may have weight zero
          value = - qvalue * dvalue / (qvalue + dvalue);
        
        pit = pairs.lower_bound(entry_id);
        if(pit != pairs.end() && !(pairs.key_comp()(entry_id, pit->first)))
        {
          pit->second += value;
        }
        else
        {
          pairs.insert(pit, 
            map<EntryId, double>::value_type(entry_id, value));
        }
      }
      
    } // for each inverted row
  } // for each query word
	
  // move to vector
  ret.reserve(pairs.size());
  for(pit = pairs.begin(); pit != pairs.end(); ++pit)
  {
    ret.push_back(Result(pit->first, pit->second));
  }
	
  // resulting "scores" are now in [-2 best .. 0 worst]	
  // we have to add +2 to the scores to obtain the chi square score
  
  // sort vector in ascending order of score
  sort(ret.begin(), ret.end());
  // (ret is inverted now --the lower the better--)

  // cut vector
  if(max_results > 0 && (int)ret.size() > max_results)
    ret.resize(max_results);

  // complete and scale score to [0 worst .. 1 best]
  QueryResults::iterator qit;
  for(qit = ret.begin(); qit != ret.end(); qit++) 
    // this takes the 4 into account
    qit->Score = - 2. * qit->Score; // [0..1]
    
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::queryKL(const BowVector &vec, 
  QueryResults &ret, int max_results, int max_id) const
{
  BowVector::const_iterator vit;
  typename IFRow::const_iterator rit;
  
  map<EntryId, double> pairs;
  map<EntryId, double>::iterator pit;
  
  for(vit = vec.begin(); vit != vec.end(); ++vit)
  {
    const WordId word_id = vit->first;
    const WordValue& vi = vit->second;
    
    const IFRow& row = m_ifile[word_id];
    
    // IFRows are sorted in ascending entry_id order
    
    for(rit = row.begin(); rit != row.end(); ++rit)
    {
      const EntryId entry_id = rit->entry_id;
      const WordValue& wi = rit->word_weight;
      
      if((int)entry_id < max_id || max_id == -1)
      {
        double value = 0;
        if(vi != 0 && wi != 0) value = vi * log(vi/wi);
        
        pit = pairs.lower_bound(entry_id);
        if(pit != pairs.end() && !(pairs.key_comp()(entry_id, pit->first)))
        {
          pit->second += value;
        }
        else
        {
          pairs.insert(pit, 
            map<EntryId, double>::value_type(entry_id, value));
        }
      }
      
    } // for each inverted row
  } // for each query word
	
  // resulting "scores" are now in [-X worst .. 0 best .. X worst]
  // but we cannot make sure which ones are better without calculating
  // the complete score

  // complete scores and move to vector
  ret.reserve(pairs.size());
  for(pit = pairs.begin(); pit != pairs.end(); ++pit)
  {
    EntryId eid = pit->first;
    double value = 0.0;

    for(vit = vec.begin(); vit != vec.end(); ++vit)
    {
      const WordValue &vi = vit->second;
      const IFRow& row = m_ifile[vit->first];

      if(vi != 0)
      {
        if(row.end() == find(row.begin(), row.end(), eid ))
        {
          value += vi * (log(vi) - GeneralScoring::LOG_EPS);
        }
      }
    }

    pit->second += value;
    
    // to vector
    ret.push_back(Result(pit->first, pit->second));
  }
  
  // real scores are now in [0 best .. X worst]

  // sort vector in ascending order
  // (scores are inverted now --the lower the better--)
  sort(ret.begin(), ret.end());

  // cut vector
  if((int)ret.size() > max_results) ret.resize(max_results);

  // cannot scale scores
    
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::queryBhattacharyya(
  const BowVector &vec, QueryResults &ret, int max_results, int max_id) const
{
  BowVector::const_iterator vit;
  typename IFRow::const_iterator rit;
  
  map<EntryId, double> pairs;
  map<EntryId, double>::iterator pit;
  
  for(vit = vec.begin(); vit != vec.end(); ++vit)
  {
    const WordId word_id = vit->first;
    const WordValue& qvalue = vit->second;
    
    const IFRow& row = m_ifile[word_id];
    
    // IFRows are sorted in ascending entry_id order
    
    for(rit = row.begin(); rit != row.end(); ++rit)
    {
      const EntryId entry_id = rit->entry_id;
      const WordValue& dvalue = rit->word_weight;
      
      if((int)entry_id < max_id || max_id == -1)
      {
        double value = sqrt(qvalue * dvalue);
        
        pit = pairs.lower_bound(entry_id);
        if(pit != pairs.end() && !(pairs.key_comp()(entry_id, pit->first)))
        {
          pit->second += value;
        }
        else
        {
          pairs.insert(pit, 
            map<EntryId, double>::value_type(entry_id, value));
        }
      }
      
    } // for each inverted row
  } // for each query word
	
  // move to vector
  ret.reserve(pairs.size());
  for(pit = pairs.begin(); pit != pairs.end(); ++pit)
  {
    ret.push_back(Result(pit->first, pit->second));
  }
	
  // scores are already in [0..1]

  // sort vector in descending order
  sort(ret.begin(), ret.end(), Result::gt);

  // cut vector
  if((int)ret.size() > max_results) ret.resize(max_results);

}

// ---------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::queryDotProduct(
  const BowVector &vec, QueryResults &ret, int max_results, int max_id) const
{
  BowVector::const_iterator vit;
  typename IFRow::const_iterator rit;
  
  map<EntryId, double> pairs;
  map<EntryId, double>::iterator pit;
  
  for(vit = vec.begin(); vit != vec.end(); ++vit)
  {
    const WordId word_id = vit->first;
    const WordValue& qvalue = vit->second;
    
    const IFRow& row = m_ifile[word_id];
    
    // IFRows are sorted in ascending entry_id order
    
    for(rit = row.begin(); rit != row.end(); ++rit)
    {
      const EntryId entry_id = rit->entry_id;
      const WordValue& dvalue = rit->word_weight;
      
      if((int)entry_id < max_id || max_id == -1)
      {
        double value = qvalue * dvalue;
        
        pit = pairs.lower_bound(entry_id);
        if(pit != pairs.end() && !(pairs.key_comp()(entry_id, pit->first)))
        {
          pit->second += value;
        }
        else
        {
          pairs.insert(pit, 
            map<EntryId, double>::value_type(entry_id, value));
        }
      }
      
    } // for each inverted row
  } // for each query word
	
  // move to vector
  ret.reserve(pairs.size());
  for(pit = pairs.begin(); pit != pairs.end(); ++pit)
  {
    ret.push_back(Result(pit->first, pit->second));
  }
	
  // scores are the greater the better

  // sort vector in descending order
  sort(ret.begin(), ret.end(), Result::gt);

  // cut vector
  if((int)ret.size() > max_results) ret.resize(max_results);

  // these scores cannot be scaled
}

// ---------------------------------------------------------------------------

template<class TDescriptor, class F>
const FeatureVector& TemplatedDatabase<TDescriptor, F>::retrieveFeatures
  (EntryId id) const
{
  assert(id < size());
  return m_dfile[id];
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::save(const string &filename) const
{
  cv::FileStorage fs(filename.c_str(), cv::FileStorage::WRITE);
  if(!fs.isOpened()) throw string("Could not open file ") + filename;
  
  save(fs);
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::save(cv::FileStorage &fs,
  const std::string &name) const
{
  // Format YAML:
  // vocabulary { ... see TemplatedVocabulary::save }
  // database 
  // {
  //   nEntries: 
  //   usingDI: 
  //   diLevels: 
  //   invertedIndex
  //   [
  //     [
  //        { 
  //          imageId: 
  //          weight: 
  //        }
  //     ]
  //   ]
  //   directIndex
  //   [
  //      [
  //        {
  //          nodeId:
  //          features: [ ]
  //        }
  //      ]
  //   ]

  // invertedIndex[i] is for the i-th word
  // directIndex[i] is for the i-th entry
  // directIndex may be empty if not using direct index
  //
  // imageId's and nodeId's must be stored in ascending order
  // (according to the construction of the indexes)

  m_voc.save(fs);
 
  fs << name << "{";
  
  fs << "nEntries" << m_nentries;
  fs << "usingDI" << (m_use_di ? 1 : 0);
  fs << "diLevels" << m_dilevels;
  
  fs << "invertedIndex" << "[";
  
  typename InvertedFile::const_iterator iit;
  typename IFRow::const_iterator irit;
  for(iit = m_ifile.begin(); iit != m_ifile.end(); ++iit)
  {
    fs << "["; // word of IF
    for(irit = iit->begin(); irit != iit->end(); ++irit)
    {
      fs << "{:" 
        << "imageId" << (int)irit->entry_id
        << "weight" << irit->word_weight
        << "}";
    }
    fs << "]"; // word of IF
  }
  
  fs << "]"; // invertedIndex
  
  fs << "directIndex" << "[";
  
  typename DirectFile::const_iterator dit;
  typename FeatureVector::const_iterator drit;
  for(dit = m_dfile.begin(); dit != m_dfile.end(); ++dit)
  {
    fs << "["; // entry of DF
    
    for(drit = dit->begin(); drit != dit->end(); ++drit)
    {
      NodeId nid = drit->first;
      const vector<unsigned int>& features = drit->second;
      
      // save info of last_nid
      fs << "{";
      fs << "nodeId" << (int)nid;
      // msvc++ 2010 with opencv 2.3.1 does not allow FileStorage::operator<<
      // with vectors of unsigned int
      fs << "features" << "[" 
        << *(const vector<int>*)(&features) << "]";
      fs << "}";
    }
    
    fs << "]"; // entry of DF
  }
  
  fs << "]"; // directIndex
  
  fs << "}"; // database
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::load(const string &filename)
{
  cv::FileStorage fs(filename.c_str(), cv::FileStorage::READ);
  if(!fs.isOpened()) throw string("Could not open file ") + filename;
  
  load(fs);
}

// --------------------------------------------------------------------------

template<class TDescriptor, class F>
void TemplatedDatabase<TDescriptor, F>::load(cv::FileStorage &fs,
  const std::string &name) //default is "database"
{  
  // load voc first
  m_voc.load(fs);

  // load database now
  clear(); // resizes inverted file 
    
  cv::FileNode fdb = fs[name];
  
  m_nentries = (int)fdb["nEntries"]; 
  m_use_di = (int)fdb["usingDI"] != 0;
  m_dilevels = (int)fdb["diLevels"];
  
  cv::FileNode fn = fdb["invertedIndex"];
  for(WordId wid = 0; wid < fn.size(); ++wid)
  {
    cv::FileNode fw = fn[wid];
    
    for(unsigned int i = 0; i < fw.size(); ++i)
    {
      EntryId eid = (int)fw[i]["imageId"];
      WordValue v = fw[i]["weight"];
      
      m_ifile[wid].push_back(IFPair(eid, v));
    }
  }
  
  if(m_use_di)
  {
    fn = fdb["directIndex"];
    
    m_dfile.resize(fn.size());
    assert(m_nentries == fn.size());
    
    FeatureVector::iterator dit;
    for(EntryId eid = 0; eid < fn.size(); ++eid)
    {
      cv::FileNode fe = fn[eid];
      
      m_dfile[eid].clear();
      for(unsigned int i = 0; i < fe.size(); ++i)
      {
        NodeId nid = (int)fe[i]["nodeId"];
        
        dit = m_dfile[eid].insert(m_dfile[eid].end(), 
          make_pair(nid, vector<unsigned int>() )); 
        
        // this failed to compile with some opencv versions (2.3.1)
        //fe[i]["features"] >> dit->second;
        vector<int> aux;
        fe[i]["features"] >> aux;
        dit->second.resize(aux.size());
        std::copy(aux.begin(), aux.end(), dit->second.begin());
      }
    } // for each entry
  } // if use_id
  
}

// --------------------------------------------------------------------------

/**
 * Writes printable information of the database
 * @param os stream to write to
 * @param db
 */
template<class TDescriptor, class F>
std::ostream& operator<<(std::ostream &os, 
  const TemplatedDatabase<TDescriptor,F> &db)
{
  os << "Database: Entries = " << db.size() << ", "
    "Using direct index = " << (db.usingDirectIndex() ? "yes" : "no");
  
  if(db.usingDirectIndex())
    os << ", Direct index levels = " << db.getDirectIndexLevels();
  
  os << ". " << db.getVocabulary();
  return os;
}

// --------------------------------------------------------------------------

} // namespace DBoW2

#endif
