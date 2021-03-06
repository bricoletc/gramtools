#include <cassert>
#include <fstream>
#include <vector>

#include "genotype/quasimap/coverage/allele_sum.hpp"
#include "genotype/quasimap/coverage/coverage_common.hpp"

using namespace gram;

AlleleSumCoverage gram::coverage::generate::allele_sum_structure(
    const PRG_Info &prg_info) {
  uint64_t number_of_variant_sites = prg_info.num_variant_sites;
  AlleleSumCoverage allele_sum_coverage(number_of_variant_sites);

  Marker site_ID;
  std::size_t num_alleles;

  // Go through each bubble in the graph, and make room for coverage for each
  // edge in the bubble start.
  for (auto const &bubble_entry : prg_info.coverage_graph.bubble_map) {
    site_ID = bubble_entry.first->get_site_ID();
    auto site_index = siteID_to_index(site_ID);

    num_alleles = bubble_entry.first->get_num_edges();
    for (std::size_t i = 0; i < num_alleles; i++)
      allele_sum_coverage[site_index].push_back(0);
  }
  return allele_sum_coverage;
}

void gram::coverage::record::allele_sum(Coverage &coverage,
                                        const uniqueLoci &compatible_loci) {
  auto &allele_sum_coverage = coverage.allele_sum_coverage;

  for (const auto &locus : compatible_loci) {
    auto marker = locus.first;
    auto allele_id = locus.second;
    auto site_index = siteID_to_index(marker);

#pragma omp atomic
    allele_sum_coverage[site_index][allele_id] += 1;
  }
}

void gram::coverage::dump::allele_sum(const Coverage &coverage,
                                      const GenotypeParams &parameters) {
  std::ofstream file_handle(parameters.allele_sum_coverage_fpath);
  for (const auto &variant_site_coverage : coverage.allele_sum_coverage) {
    auto allele_count = 0;
    for (const auto &sum_coverage : variant_site_coverage) {
      file_handle << sum_coverage;
      auto not_last_coverage =
          allele_count++ < variant_site_coverage.size() - 1;
      if (not_last_coverage) file_handle << " ";
    }
    file_handle << std::endl;
  }
}