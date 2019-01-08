/**
 * @file
 * Routines for producing all required kmers, in sorted order. Either from:
 * * The prg, extracting only kmers overlapping variant sites
 * * All possible kmers
 *
 * The kmers are sorted such that each kmer shares the largest possible suffix with its predecessor in the set.
 * This reduces the number of prg quasimappings to compute to a minimum. Indeed, all the search states associated with a
 * given kmer suffix are kept in cache and then extended to produce larger kmer suffixes.
 *
 * For example, kmers '1111' and '2111' (corresponding to 'aaaa' and 'caaa' respectively) are stored consecutively.
 * Then the set of `SearchStates` corresponding to the string '111' in the PRG is computed only once, and used to extend
 * to both '1111' and '2111' during variant aware backward searching.
 */
#include <unordered_set>
#include <unordered_map>

#include <boost/functional/hash.hpp>

#include "prg/prg.hpp"
#include "common/parameters.hpp"
#include "common/utils.hpp"


#ifndef GRAMTOOLS_KMERS_HPP
#define GRAMTOOLS_KMERS_HPP

namespace gram {

    template<typename SEQUENCE>
    struct sequence_ordering_condition {
        std::size_t operator()(const SEQUENCE &lhs, const SEQUENCE &rhs) const {
            int64_t i = 0;
            while (i >= 0) {
                if (lhs[i] < rhs[i]) {
                    return (std::size_t) true;
                } else if (lhs[i] == rhs[i]) {
                    i++;
                } else if (lhs[i] > rhs[i]) {
                    return (std::size_t) false;
                }
                if (i == lhs.size())
                    break;
            }
            return (std::size_t) false;
        }
    };

    template<typename SEQUENCE>
    using unordered_vector_set = std::unordered_set<SEQUENCE, sequence_hash<SEQUENCE>>;

    template<typename SEQUENCE>
    using ordered_vector_set = std::set<SEQUENCE, sequence_ordering_condition<SEQUENCE>>;

    using PrgIndexRange = std::pair<uint64_t, uint64_t>;
    using KmerSuffixDiffs = std::vector<sdsl::int_vector<8>>;

    /**
    * Computes the start and end indices of all variant site markers in a given prg.
    */
    std::vector<PrgIndexRange> get_boundary_marker_indexes(const PRG_Info &prg_info);

    std::vector<PrgIndexRange> get_kmer_region_ranges(std::vector<PrgIndexRange> &boundary_marker_indexes,
                                                      const uint64_t &max_read_size,
                                                      const PRG_Info &prg_info);

    uint64_t find_site_end_boundary(const uint64_t &within_site_index,
                                    const PRG_Info &prg_info);

    Patterns get_site_ordered_alleles(const uint64_t &within_site_index,
                                      const PRG_Info &prg_info);

    std::list<uint64_t> sites_inrange_left(const uint64_t outside_site_start_index,
                                           const uint64_t kmer_size,
                                           const PRG_Info &prg_info);

    std::pair<uint64_t, uint64_t> get_nonvariant_region(const uint64_t &site_end_boundary_index,
                                                        const PRG_Info &prg_info);

    Pattern right_intersite_nonvariant_region(const uint64_t &site_end_boundary_index,
                                              const PRG_Info &prg_info);

    unordered_vector_set<Pattern> get_region_range_reverse_kmers(const PrgIndexRange &kmer_region_range,
                                                                 const uint64_t &kmer_size,
                                                                 const PRG_Info &prg_info);

    uint64_t find_site_start_boundary(const uint64_t &end_boundary_index,
                                      const PRG_Info &prg_info);

    std::list<Patterns> get_kmer_size_region_parts(const uint64_t &current_range_end_index,
                                                   const std::list<uint64_t> &inrange_sites,
                                                   const uint64_t kmer_size,
                                                   const PRG_Info &prg_info);

    unordered_vector_set<Pattern> get_region_parts_reverse_kmers(const std::list<Patterns> &region_parts,
                                                                 const uint64_t &kmer_size);

    bool update_allele_index_path(std::vector<uint64_t> &current_allele_index_path,
                                  const std::vector<uint64_t> &parts_allele_counts);

    unordered_vector_set<Pattern> get_path_reverse_kmers(const Pattern &path,
                                                         const uint64_t &kmer_size);

    unordered_vector_set<Pattern> get_sites_reverse_kmers(uint64_t &current_range_end_index,
                                                          const std::list<uint64_t> &inrange_sites,
                                                          const uint64_t kmer_size,
                                                          const PRG_Info &prg_info);

    std::vector<PrgIndexRange> combine_overlapping_regions(const std::vector<PrgIndexRange> &kmer_region_ranges);

    std::vector<Pattern> reverse(const ordered_vector_set<Pattern> &reverse_kmers);

    /**
     * Computes the minimal changes between kmers and their immediate predecessor in the ordered set.
     * The changes are listed left to right (prefix order).
     */
    std::vector<Pattern> get_prefix_diffs(const std::vector<Pattern> &kmers);

    ordered_vector_set<Pattern> get_prg_reverse_kmers(const Parameters &parameters,
                                                      const PRG_Info &prg_info);

    /**
     * High-level routine for extracting all kmers of interest and computing the prefix differences.
     * @see gram::kmer_index::build()
     */
    std::vector<Pattern> get_all_kmer_and_compute_prefix_diffs(const Parameters &parameters,
                                                               const PRG_Info &prg_info);

    /**
     * Core routine for producing kmers to index.
     * If `all_kmers_flag` is unset (the default), only the relevant kmers are produced; these are the kmers overlapping
     * variant sites in the prg.
     * If it is set, produces all the kmers of given size.
     */
    std::vector<Pattern> get_all_kmers(const Parameters &parameters,
                                       const PRG_Info &prg_info);

    /**
     * Generate all kmers of a given size, in order.
     * Order is a dictionary order ('1111'<'1121' < '1211' etc..)
     * Kmers themselves are produced in order ('1111' then '1112' etc..)
     * @see next_kmer()
     */
    ordered_vector_set<Pattern> generate_all_kmers(const uint64_t &kmer_size);

}

#endif //GRAMTOOLS_KMERS_HPP
