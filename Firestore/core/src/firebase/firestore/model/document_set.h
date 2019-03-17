/*
 * Copyright 2017 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_MODEL_DOCUMENT_SET_H_
#define FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_MODEL_DOCUMENT_SET_H_

#import <Foundation/Foundation.h>

#include <iosfwd>
#include <string>
#include <utility>

#include "Firestore/core/src/firebase/firestore/immutable/sorted_set.h"
#include "Firestore/core/src/firebase/firestore/model/document_key.h"
#include "Firestore/core/src/firebase/firestore/model/document_map.h"

@class FSTDocument;

NS_ASSUME_NONNULL_BEGIN

namespace firebase {
namespace firestore {
namespace model {

/**
 * A C++ comparator that returns less-than, implemented by delegating to
 * an NSComparator.
 */
class DocumentSetComparator {
 public:
  explicit DocumentSetComparator(NSComparator delegate = nil)
      : delegate_(delegate) {
  }

  bool operator()(FSTDocument* lhs, FSTDocument* rhs) const {
    return delegate_(lhs, rhs) == NSOrderedAscending;
  }

 private:
  NSComparator delegate_;
};

/**
 * DocumentSet is an immutable (copy-on-write) collection that holds documents
 * in order specified by the provided comparator. We always add a document key
 * comparator on top of what is provided to guarantee document equality based on
 * the key.
 */
class DocumentSet {
 public:
  /**
   * The type of the main collection of documents in an DocumentSet.
   * @see sorted_set_.
   */
  using SetType = immutable::SortedSet<FSTDocument*, DocumentSetComparator>;

  // STL container types
  using value_type = FSTDocument*;
  using const_iterator = SetType::const_iterator;

  /**
   * Creates a new, empty DocumentSet sorted by the given comparator, then by
   * keys.
   */
  explicit DocumentSet(NSComparator comparator);

  size_t size() const {
    return index_.size();
  }

  /** Returns true if the dictionary contains no elements. */
  bool empty() const {
    return index_.empty();
  }

  /** Returns true if this set contains a document with the given key. */
  bool ContainsKey(const DocumentKey& key) const;

  SetType::const_iterator begin() const {
    return sorted_set_.begin();
  }
  SetType::const_iterator end() const {
    return sorted_set_.end();
  }

  /**
   * Returns the document from this set with the given key if it exists or nil
   * if it doesn't.
   */
  FSTDocument* _Nullable GetDocument(const DocumentKey& key) const;

  /**
   * Returns the first document in the set according to its built in ordering,
   * or nil if the set is empty.
   */
  FSTDocument* _Nullable GetFirstDocument() const;

  /**
   * Returns the last document in the set according to its built in ordering, or
   * nil if the set is empty.
   */
  FSTDocument* _Nullable GetLastDocument() const;

  /**
   * Returns the index of the document with the provided key in the document
   * set. Returns NSNotFound if the key is not present.
   */
  size_t IndexOf(const DocumentKey& key) const;

  /**
   * Returns a copy of the documents in this set as an array. This is O(n) on
   * the size of the set.
   */
  NSArray<FSTDocument*>* GetArrayValue() const;

  /**
   * Returns the documents as a `DocumentMap`. This is O(1) as this leverages
   * our internal representation.
   */
  const DocumentMap& GetMapValue() const;

  /** Returns a new DocumentSet that contains the given document. */
  DocumentSet insert(FSTDocument* _Nullable document) const;

  /**
   * Returns a new DocumentSet that excludes any document associated with
   * the given key.
   */
  DocumentSet erase(const DocumentKey& key) const;

  friend bool operator==(const DocumentSet& lhs, const DocumentSet& rhs);

  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& os, const DocumentSet& set);

  size_t Hash() const;

 private:
  DocumentSet(DocumentMap&& index, SetType&& sorted_set)
      : index_(std::move(index)), sorted_set_(std::move(sorted_set)) {
  }

  /**
   * An index of the documents in the DocumentSet, indexed by document key.
   * The index exists to guarantee the uniqueness of document keys in the set
   * and to allow lookup and removal of documents by key.
   */
  DocumentMap index_;

  /**
   * The main collection of documents in the DocumentSet. The documents are
   * ordered by a comparator supplied from a query. The SetType collection
   * exists in addition to the index to allow ordered traversal of the
   * DocumentSet.
   */
  SetType sorted_set_;
};

}  // namespace model
}  // namespace firestore
}  // namespace firebase

NS_ASSUME_NONNULL_END

#endif  // FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_MODEL_DOCUMENT_SET_H_
