//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/join_order/join_relation.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/common.hpp"
#include "duckdb/common/unordered_map.hpp"
#include "duckdb/common/unordered_set.hpp"
#include "duckdb/common/optional_ptr.hpp"

namespace duckdb {
class LogicalOperator;

//! Represents a single relation and any metadata accompanying that relation
struct SingleJoinRelation {
	LogicalOperator &op;
	optional_ptr<LogicalOperator> parent;

	SingleJoinRelation(LogicalOperator &op, optional_ptr<LogicalOperator> parent) : op(op), parent(parent) {
	}
};

//! Set of relations, used in the join graph.
struct JoinRelationSet {
	JoinRelationSet(unsafe_unique_array<idx_t> relations, idx_t count) : relations(std::move(relations)), count(count) {
	}

	string ToString() const;

	unsafe_unique_array<idx_t> relations;
	idx_t count;

	static bool IsSubset(JoinRelationSet &super, JoinRelationSet &sub);
};

//! The JoinRelationTree is a structure holding all the created JoinRelationSet objects and allowing fast lookup on to
//! them
class JoinRelationSetManager {
public:
	//! Contains a node with a JoinRelationSet and child relations
	// FIXME: this structure is inefficient, could use a bitmap for lookup instead (todo: profile)
	struct JoinRelationTreeNode {
		unique_ptr<JoinRelationSet> relation;
		unordered_map<idx_t, unique_ptr<JoinRelationTreeNode>> children;
	};

public:
	//! Create or get a JoinRelationSet from a single node with the given index
	JoinRelationSet &GetJoinRelation(idx_t index);
	//! Create or get a JoinRelationSet from a set of relation bindings
	JoinRelationSet &GetJoinRelation(const unordered_set<idx_t> &bindings);
	//! Create or get a JoinRelationSet from a (sorted, duplicate-free!) list of relations
	JoinRelationSet &GetJoinRelation(unsafe_unique_array<idx_t> relations, idx_t count);
	//! Union two sets of relations together and create a new relation set
	JoinRelationSet &Union(JoinRelationSet &left, JoinRelationSet &right);
	// //! Create the set difference of left \ right (i.e. all elements in left that are not in right)
	// JoinRelationSet *Difference(JoinRelationSet *left, JoinRelationSet *right);

private:
	JoinRelationTreeNode root;
};

class NeighborSubset {
public:
	class ConstIterator {
	private:
		idx_t index;
		const NeighborSubset &subset;

	public:
		ConstIterator(const NeighborSubset &subset, idx_t index) : index(index), subset(subset) {
		}

		bool operator==(const ConstIterator &other) const {
			D_ASSERT(&subset == &other.subset);
			return index == other.index;
		}

		bool operator!=(const ConstIterator &other) const {
			return !(*this == other);
		}

		const unordered_set<idx_t> &operator*() const {
			D_ASSERT(index < subset.Size());
			return subset.all_subsets[index].first;
		}

		ConstIterator &operator++() {
			index++;
			return *this;
		}
	};

	explicit NeighborSubset(const vector<idx_t> &neighbors);

	ConstIterator Begin() const {
		return ConstIterator(*this, 0);
	}

	ConstIterator End() const {
		return ConstIterator(*this, all_subsets.size());
	}

	idx_t Size() const {
		return all_subsets.size();
	}

private:
	vector<std::pair<unordered_set<idx_t>, idx_t>> all_subsets;
};

} // namespace duckdb
