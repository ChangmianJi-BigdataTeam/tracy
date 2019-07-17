/*
============================================================================
Tracy: Trace File Handling
============================================================================
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
============================================================================
Contact: Tobias Rausch (rausch@embl.de)
============================================================================
*/

#ifndef PROFILE_H
#define PROFILE_H

#include <boost/progress.hpp>

namespace tracy
{


  template<typename TProfile>
  inline void
  createProfile(Trace const& tr, BaseCalls const& bc, TProfile& p) {
    p.resize(boost::extents[6][bc.bcPos.size()]);   // 'A', 'C', 'G', 'T', 'N', '-'
    for(uint32_t j = 0; j < bc.bcPos.size(); ++j) {
      float totalsig = 0;
      for(uint32_t k = 0; k<4; ++k) totalsig += tr.traceACGT[k][bc.bcPos[j]];
      p[4][j] = 0;
      p[5][j] = 0;
      if (totalsig == 0) {
	for(uint32_t k = 0; k<4; ++k) p[k][j] = 0.25;
      } else {
	for(uint32_t k = 0; k<4; ++k) p[k][j] = ((float) (tr.traceACGT[k][bc.bcPos[j]]) / totalsig);
      }
    }
  }
  
  template<typename TProfile>
  inline void
  createProfile(Trace const& tr, BaseCalls const& bc, TProfile& p, int32_t const trimleft, int32_t const trimright) {
    if (trimleft + trimright >= (int32_t) bc.bcPos.size()) return createProfile(tr, bc, p);
    else {
      int32_t sz = bc.bcPos.size() - (trimleft + trimright);
      p.resize(boost::extents[6][sz]);   // 'A', 'C', 'G', 'T', 'N', '-'
      for(int32_t j = trimleft; j < (trimleft + sz); ++j) {
	float totalsig = 0;
	for(uint32_t k = 0; k<4; ++k) totalsig += tr.traceACGT[k][bc.bcPos[j]];
	p[4][j-trimleft] = 0;
	p[5][j-trimleft] = 0;
	if (totalsig == 0) {
	  for(uint32_t k = 0; k<4; ++k) p[k][j] = 0.25;
	} else {
	  for(uint32_t k = 0; k<4; ++k) p[k][j-trimleft] = ((float) (tr.traceACGT[k][bc.bcPos[j]]) / totalsig);
	}
      }
    }
  }

  template<typename TConfig, typename TProfile>
  inline void
  createProfile(TConfig const& c, ReferenceSlice const& rs, TProfile& p) {
    if (rs.filetype != 2) return _createProfile(rs.refslice, p);
    else {
      // Create profile from trace
      Trace wt;
      if (!readab(c.genome.string(), wt)) return;
      BaseCalls wtbc;
      basecall(wt, wtbc, c.pratio);
      return createProfile(wt, wtbc, p);
    }
  }

  template<typename TProfile>
  inline void
  reverseComplementProfile(TProfile const& p, TProfile& out) {
    out.resize(boost::extents[6][p.shape()[1]]);   // 'A', 'C', 'G', 'T', 'N', '-'
    int32_t pIdx = p.shape()[1] - 1;
    int32_t outIdx = 0;
    while (pIdx >= 0) {
      out[0][outIdx] = p[3][pIdx];
      out[1][outIdx] = p[2][pIdx];
      out[2][outIdx] = p[1][pIdx];
      out[3][outIdx] = p[0][pIdx];
      out[4][outIdx] = p[4][pIdx];
      out[5][outIdx] = p[5][pIdx];
      --pIdx;
      ++outIdx;
    }
  }

  template<typename TProfile>
  inline void
  copyProfile(TProfile const& p, TProfile& out) {
    out.resize(boost::extents[p.shape()[0]][p.shape()[1]]);
    for(uint32_t i = 0; i < p.shape()[0]; ++i) {
      for(uint32_t j = 0; j < p.shape()[1]; ++j) out[i][j] = p[i][j];
    }
  }
}

#endif
