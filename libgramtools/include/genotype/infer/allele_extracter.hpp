#ifndef ALLELE_EXTRACTER_HPP
#define ALLELE_EXTRACTER_HPP

#include "prg/types.hpp"
#include "types.hpp"

using namespace gram;

namespace gram::genotype::infer {

/**
 * Class in charge of producing the set of `Allele`s that get genotyped.
 * The procedure scans through each haplogroup of a site, pasting sequence &
 * coverage from previously genotyped (=nested) sites when encountered.
 *
 * It additionally always produced a REF allele by picking the first allele
 * (haplogroup) of each site.
 */
class AlleleExtracter {
 private:
  allele_vector alleles;
  gt_sites const* genotyped_sites;
  bool _ref_allele_got_made_naturally;  // Used for testing purposes only
 public:
  AlleleExtracter() : genotyped_sites(nullptr){};

  AlleleExtracter(covG_ptr site_start, covG_ptr site_end, gt_sites& sites);

  AlleleExtracter(gt_sites& sites) : genotyped_sites(&sites) {}

  allele_vector const get_alleles() const { return alleles; }

  void place_ref_as_first_allele(allele_vector& alleles,
                                 Allele const& ref_allele);

  /**
   * Linear traversal of an allelic haplogroup, extracting all relevant
   * combinations of alleles. In the absence of incident nested sites, always
   * produces a single allele.
   */
  allele_vector extract_alleles(AlleleId const haplogroup,
                                covG_ptr haplogroup_start, covG_ptr site_end);

  /**
   * From the set of genotypes of a site, combines them with existing alleles.
   * @param existing the starting alleles
   * @param site_index the previously genotyped site
   * @return Cartesian product of `existing` and distinct genotyped alleles
   */
  allele_vector allele_combine(allele_vector const& existing,
                               std::size_t site_index);

  /**
   * From a set of existing alleles, paste sequence and pb coverage to the end
   * of each of them. This deals with a node common to a haplogroup.
   * @param existing the starting alleles
   * @param sequence_node  the haplogroup common node
   * @return void because `existing` is modified in place
   */
  void allele_paste(allele_vector& existing, covG_ptr sequence_node);

  bool ref_allele_got_made_naturally() const {
    return _ref_allele_got_made_naturally;
  }
};
}  // namespace gram::genotype::infer

#endif  // TEST_SRC_COMMON