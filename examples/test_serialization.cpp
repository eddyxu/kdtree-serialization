#define KDTREE_DEFINE_OSTREAM_OPERATORS
#define KDTREE_SERIALIZATION 1

// Make SURE all our asserts() are checked
#undef NDEBUG

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>

#include <deque>
#include <iostream>
#include <vector>
#include <limits>
#include <functional>
#include <set>
#include <iostream>

#include "kdtree++/kdtree.hpp"

using namespace std;

// used to ensure all triplets that are accessed via the operator<< are initialised.
std::set<const void*> registered;

struct triplet
{
  typedef int value_type;

  triplet() {
    d[0] = d[1] = d[2] = 0;
  }

  triplet(value_type a, value_type b, value_type c)
  {
    d[0] = a;
    d[1] = b;
    d[2] = c;
  }

  triplet(const triplet & x)
  {
    d[0] = x.d[0];
    d[1] = x.d[1];
    d[2] = x.d[2];
  }

  ~triplet()
  {
  }

  double distance_to(triplet const& x) const
  {
     double dist = 0;
     for (int i = 0; i != 3; ++i)
        dist += (d[i]-x.d[i])*(d[i]-x.d[i]);
     return std::sqrt(dist);
  }

  inline value_type operator[](size_t const N) const { return d[N]; }

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & d;
  }

  value_type d[3];
};

inline bool operator==(triplet const& A, triplet const& B) {
  return A.d[0] == B.d[0] && A.d[1] == B.d[1] && A.d[2] == B.d[2];
}

std::ostream& operator<<(std::ostream& out, triplet const& T)
{
  assert(registered.find(&T) != registered.end());
  return out << '(' << T.d[0] << ',' << T.d[1] << ',' << T.d[2] << ')';
}

inline double tac( triplet t, size_t k ) { return t[k]; }


typedef KDTree::KDTree<3, triplet > tree_type;

int main()
{
  // check that it'll find nodes exactly MAX away
  {
    tree_type tree;
    triplet c0(55, 43, 28);
    tree.insert(c0);
    triplet target(74,43,58);
    tree.insert(target);
    triplet c2(22,33,55);
    tree.insert(c2);

    { 
      std::ofstream ofs("triple_serialized.txt");
      boost::archive::text_oarchive oa(ofs);
      oa << c0;
    }

    { 
      triplet new_trip;
      std::ifstream ifs("triple_serialized.txt");
      boost::archive::text_iarchive ia(ifs);
      ia >> new_trip;
      assert(c0 == new_trip);
    }

    {
      std::ofstream ofs("temp2.txt");
      boost::archive::text_oarchive oa(ofs);
      oa << tree;
    }

    tree_type new_tree;
    {
      std::ifstream ifs("temp2.txt");
      boost::archive::text_iarchive ia(ifs);
      ia >> new_tree;
    }
    assert(tree.size() == new_tree.size());
    tree_type::iterator old_it, new_it;
    old_it = tree.begin();
    new_it = new_tree.begin();
    while (old_it != tree.end()) {
      assert(*old_it == *new_it);
      ++old_it;
      ++new_it;
    }
  }


  return 0;
}

/* COPYRIGHT --
 *
 * This file is part of libkdtree++, a C++ template KD-Tree sorting container.
 * libkdtree++ is (c) 2004-2007 Martin F. Krafft <libkdtree@pobox.madduck.net>
 * and Sylvain Bougerel <sylvain.bougerel.devel@gmail.com> distributed under the
 * terms of the Artistic License 2.0. See the ./COPYING file in the source tree
 * root for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
