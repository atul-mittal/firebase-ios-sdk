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

#ifndef FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_CORE_QUERY_LISTENER_H_
#define FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_CORE_QUERY_LISTENER_H_

#if !defined(__OBJC__)
#error "This header only supports Objective-C++"
#endif  // !defined(__OBJC__)

#import <Foundation/Foundation.h>

#include <utility>

#include "Firestore/core/src/firebase/firestore/core/listen_options.h"
#include "Firestore/core/src/firebase/firestore/core/view_snapshot.h"
#include "Firestore/core/src/firebase/firestore/model/types.h"
#include "Firestore/core/src/firebase/firestore/util/status.h"

@class FSTQuery;

NS_ASSUME_NONNULL_BEGIN

namespace firebase {
namespace firestore {
namespace core {

/**
 * QueryListener takes a series of internal view snapshots and determines when
 * to raise user-facing events.
 */
class QueryListener {
 public:
  QueryListener(FSTQuery* query,
                ListenOptions options,
                ViewSnapshotHandler&& listener)
      : query_(query),
        options_(std::move(options)),
        listener_(std::move(listener)) {
  }

  FSTQuery* query() const {
    return query_;
  }

  /** The last received view snapshot. */
  const absl::optional<ViewSnapshot>& snapshot() const {
    return snapshot_;
  }

  void OnViewSnapshot(ViewSnapshot snapshot);
  void OnError(Status error);
  void OnOnlineStateChanged(model::OnlineState online_state);

 private:
  bool ShouldRaiseInitialEvent(const ViewSnapshot& snapshot,
                               OnlineState online_state) const;
  bool ShouldRaiseEvent(const ViewSnapshot& snapshot) const;
  void RaiseInitialEvent(const ViewSnapshot& snapshot);

  FSTQuery* query_;
  ListenOptions options_;

  /** The ViewSnapshotHandler associated with this query listener. */
  ViewSnapshotHandler listener_;

  /**
   * Initial snapshots (e.g. from cache) may not be propagated to the
   * ViewSnapshotHandler. This flag is set to YES once we've actually raised an
   * event.
   */
  bool raised_initial_event_ = false;

  /** The last online state this query listener got. */
  model::OnlineState online_state_ = model::OnlineState::Unknown;

  absl::optional<ViewSnapshot> snapshot_;
};

}  // namespace core
}  // namespace firestore
}  // namespace firebase

NS_ASSUME_NONNULL_END

#endif  // FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_CORE_QUERY_LISTENER_H_
