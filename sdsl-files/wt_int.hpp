/* sdsl - succinct data structures library
    Copyright (C) 2009 Simon Gog

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .
*/
/*! \file wt_int.hpp
    \brief wt_int.hpp contains a specialized class for a wavelet tree of a
           sequence of the numbers. This wavelet tree class takes
           less memory than the wt_pc class for large alphabets.
    \author Simon Gog, Shanika Kuruppu
*/
#ifndef INCLUDED_SDSL_UPDATED_INT_WAVELET_TREE
#define INCLUDED_SDSL_UPDATED_INT_WAVELET_TREE

#include <sdsl/wt_int.hpp>

//! Namespace for the succinct data structure library.
namespace sdsl
{

namespace updated
{

//! A wavelet tree class for integer sequences.
/*!
 *    \par Space complexity
 *        \f$\Order{n\log|\Sigma|}\f$ bits, where \f$n\f$ is the size of the vector the wavelet tree was build for.
 *
 *  \tparam t_bitvector   Type of the bitvector used for representing the wavelet tree.
 *  \tparam t_rank        Type of the support structure for rank on pattern `1`.
 *  \tparam t_select      Type of the support structure for select on pattern `1`.
 *  \tparam t_select_zero Type of the support structure for select on pattern `0`.
 *
 *   @ingroup wt
 */
template<class t_bitvector   = bit_vector,
         class t_rank        = typename t_bitvector::rank_1_type,
         class t_select      = typename t_bitvector::select_1_type,
         class t_select_zero = typename t_bitvector::select_0_type>
 class wt_int : public sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>
{
    public:

        typedef typename sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>::size_type              size_type;
        typedef typename sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>::value_type             value_type;
        typedef typename sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>::index_category                               index_category;

        typedef typename sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>::point_vec_type              point_vec_type;

        //! Default constructor
        wt_int() :sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>() {
        };

        //! Semi-external constructor
        /*! \param buf         File buffer of the int_vector for which the wt_int should be build.
         *  \param size        Size of the prefix of v, which should be indexed.
         *  \param max_level   Maximal level of the wavelet tree. If set to 0, determined automatically.
         *    \par Time complexity
         *        \f$ \Order{n\log|\Sigma|}\f$, where \f$n=size\f$
         *        I.e. we need \Order{n\log n} if rac is a permutation of 0..n-1.
         *    \par Space complexity
         *        \f$ n\log|\Sigma| + O(1)\f$ bits, where \f$n=size\f$.
         */
        template<uint8_t int_width>
        wt_int(int_vector_buffer<int_width>& buf, size_type size,
               uint32_t max_level=0) : sdsl::wt_int<t_bitvector, t_rank, t_select, t_select_zero>(buf, size, max_level) {
        }

        std::pair<size_type, std::vector<std::pair<value_type, size_type>>>
        range_search_2d2(size_type lb, size_type rb, value_type vlb, value_type vrb,
        bool report=true) const {
            std::vector<size_type> offsets(this->m_max_level+1);
            std::vector<size_type> ones_before_os(this->m_max_level+1);
            offsets[0] = 0;
            if (vrb > (1ULL << this->m_max_level))
                vrb = (1ULL << this->m_max_level);
            if (vlb > vrb)
                return make_pair(0, point_vec_type());
            size_type cnt_answers = 0;
            point_vec_type point_vec;
            _range_search_2d2(lb, rb, vlb, vrb, 0, 0, this->m_size, offsets, ones_before_os, 0, point_vec, report, cnt_answers);
            return make_pair(cnt_answers, point_vec);
        }

        void
        _range_search_2d2(size_type lb, size_type rb, value_type vlb, value_type vrb, size_type level,
                         size_type ilb, size_type node_size, std::vector<size_type>& offsets,
                         std::vector<size_type>& ones_before_os, size_type path,
                         point_vec_type& point_vec, bool report, size_type& cnt_answers) const {

            if (lb > rb)
                return;
            if (level == this->m_max_level) {
                if (report) {
                        point_vec.emplace_back(1, path);
                }
                ++cnt_answers;
                return;
            }
            size_type irb = ilb + (1ULL << (this->m_max_level-level));
            size_type mid = (irb + ilb)>>1;

            size_type offset = offsets[level];

            size_type ones_before_o    = this->m_tree_rank(offset);
            ones_before_os[level]      = ones_before_o;
            size_type ones_before_lb   = this->m_tree_rank(offset + lb);
            size_type ones_before_rb   = this->m_tree_rank(offset + rb + 1);
            size_type ones_before_end  = this->m_tree_rank(offset + node_size);
            size_type zeros_before_o   = offset - ones_before_o;
            size_type zeros_before_lb  = offset + lb - ones_before_lb;
            size_type zeros_before_rb  = offset + rb + 1 - ones_before_rb;
            size_type zeros_before_end = offset + node_size - ones_before_end;
            if (vlb < mid and mid) {
                size_type nlb    = zeros_before_lb - zeros_before_o;
                size_type nrb    = zeros_before_rb - zeros_before_o;
                offsets[level+1] = offset + this->m_size;
                if (nrb)
                    _range_search_2d2(nlb, nrb-1, vlb, std::min(vrb,mid-1), level+1, ilb, zeros_before_end - zeros_before_o, offsets, ones_before_os, path<<1, point_vec, report, cnt_answers);
            }
            if (vrb >= mid) {
                size_type nlb     = ones_before_lb - ones_before_o;
                size_type nrb     = ones_before_rb - ones_before_o;
                offsets[level+1]  = offset + this->m_size + (zeros_before_end - zeros_before_o);
                if (nrb)
                    _range_search_2d2(nlb, nrb-1, std::max(mid, vlb), vrb, level+1, mid, ones_before_end - ones_before_o, offsets, ones_before_os, (path<<1)+1 , point_vec, report, cnt_answers);
            }

        }

};

}// end namespace sdsl
}// end namespace sdsl
#endif
