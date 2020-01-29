/**
 * Tests the internals of LevelGenotyperModel
 */
#include "gtest/gtest.h"
#include "genotype/infer/genotyping_models.hpp"

using namespace gram::genotype::infer;

TEST(HaploidCoverages, GivenSingletonCountsOnly_CorrectHaploidAndSingletonCovs){
   GroupedAlleleCounts  gp_covs{
                   {{0}, 5},
                   {{1}, 10},
                   {{3}, 1}
   };

   LevelGenotyperModel gtyper;
   gtyper.set_haploid_coverages(gp_covs, 4);
   PerAlleleCoverage expected_haploid_cov{5, 10, 0, 1};
   AlleleIdSet expected_singleton_cov{0, 1, 3};
   EXPECT_EQ(gtyper.get_haploid_covs(), expected_haploid_cov);
   EXPECT_EQ(gtyper.get_singleton_covs(), expected_singleton_cov);
}


TEST(HaploidCoverages, GivenMultiAllelicClasses_CorrectHaploidAndSingletonCovs){
    GroupedAlleleCounts  gp_covs{
            {{0}, 5},
            {{0, 1}, 4},
            {{1}, 10},
            {{2, 3}, 1}
    };

    LevelGenotyperModel gtyper;
    gtyper.set_haploid_coverages(gp_covs, 4);

    PerAlleleCoverage expected_haploid_cov{9, 14, 1, 1};
    AlleleIdSet expected_singleton_cov{0, 1};

    EXPECT_EQ(gtyper.get_haploid_covs(), expected_haploid_cov);
    EXPECT_EQ(gtyper.get_singleton_covs(), expected_singleton_cov);
}

TEST(DiploidCoverages, GivenMultiAllelicClasses_CorrectDiploidCovs){
    AlleleIds ids{0, 1}; // We want coverages of alleles 0 and 1

    GroupedAlleleCounts  gp_covs{
            {{0}, 7},
            {{0, 1}, 4},
            {{1}, 20},
            {{0, 3}, 3},
            {{2, 3}, 1}
    };

    // We have 10 units uniquely on 0, 20 uniquely on 1, and 4 shared between them.
    // These 4 should get dispatched in ratio 1:2 to alleles 0:1 (cf iqbal-lab-org/minos)

    LevelGenotyperModel gtyper;
    gtyper.set_haploid_coverages(gp_covs, 4);
    multiplicities haplogroup_multiplicities(4, false);
    auto diploid_covs = gtyper.compute_diploid_coverage(gp_covs, ids, haplogroup_multiplicities);
    EXPECT_FLOAT_EQ(diploid_covs.first, 10 + 4/3.);
    EXPECT_FLOAT_EQ(diploid_covs.second, 20 + 8/3.);
}

TEST(DiploidCoverages, GivenOnlyMultiAllelicClasses_CorrectDiploidCovs){
    AlleleIds ids{0, 1}; // We want coverages of alleles 0 and 1

    GroupedAlleleCounts  gp_covs{
            {{0, 1}, 3},
            {{2, 3}, 1}
    };

    // Edge case where singleton allele coverages are all 0
    // Then shared coverage should get dispatched equally (1:1 ratio)

    LevelGenotyperModel gtyper;
    gtyper.set_haploid_coverages(gp_covs, 4);
    multiplicities haplogroup_multiplicities(4, false);
    auto diploid_covs = gtyper.compute_diploid_coverage(gp_covs, ids, haplogroup_multiplicities);
    EXPECT_FLOAT_EQ(diploid_covs.first, 1.5);
    EXPECT_FLOAT_EQ(diploid_covs.second, 1.5);
}

class DiploidCoveragesOneDominatingClass : public ::testing::Test {
protected:
    void SetUp(){
        gtyper.set_haploid_coverages(gp_covs, 2);
    }

    GroupedAlleleCounts  gp_covs{
            {{0}, 8},
            {{0, 1}, 4},
    };

    LevelGenotyperModel gtyper;
};

TEST_F(DiploidCoveragesOneDominatingClass, GivenDifferentHaplogroups_CorrectDiploidCovs){
    // There is no unique coverage on haplogroup 1, thus all coverage goes to 0
    AlleleIds ids{0, 1};

    multiplicities haplogroup_multiplicities(2, false);
    auto diploid_covs = gtyper.compute_diploid_coverage(gp_covs, ids, haplogroup_multiplicities);
    EXPECT_FLOAT_EQ(diploid_covs.first, 12);
    EXPECT_FLOAT_EQ(diploid_covs.second, 0);
}

TEST_F(DiploidCoveragesOneDominatingClass, GivenSameHaplogroupTwice_CorrectDiploidCovs){
    // This can happen: when there is a nested site within, the extracted alleles have same haplogroup
    AlleleIds ids{0, 0};

    multiplicities haplogroup_multiplicities({true}); // The two alleles have the same haplogroup
    auto diploid_covs = gtyper.compute_diploid_coverage(gp_covs, ids, haplogroup_multiplicities);
    EXPECT_FLOAT_EQ(diploid_covs.first, 6);
    EXPECT_FLOAT_EQ(diploid_covs.second, 6);
}

TEST(CountCrediblePositions, GivenAlleleWithCredibleAndNonCrediblePositions_ReturnCrediblePositions){
   Allele test_allele{
      "ATCGCCG",
      {0, 0, 2, 3, 3, 5, 4},
      0
   };

   LevelGenotyperModel gtyper;
   auto num_credible = gtyper.count_credible_positions(3, test_allele);
   EXPECT_EQ(num_credible, 4);
}

TEST(CountTotalCov, GivenVariousCovStructures_CorrectTotalCoverages) {
    GroupedAlleleCounts gp_covs{};
    LevelGenotyperModel gtyper;
    EXPECT_EQ(gtyper.count_total_coverage(gp_covs), 0);

    GroupedAlleleCounts gp_covs2{
            {{0},    5},
            {{0, 1}, 4},
            {{1},    10},
            {{2, 3}, 1}
    };
    EXPECT_EQ(gtyper.count_total_coverage(gp_covs2), 20);
}

TEST(CountNumHaplogroups, GivenVariousAlleleVectors_CorrectNumHaplogroups){
    // Haplogroup should default to the same thing, consistently.
    allele_vector a1{
        Allele{"", {}},
        Allele{"", {}},
    };

    LevelGenotyperModel gtyper;
    multiplicities expected({true}); // Expect one entry, with more than one associated allele
    EXPECT_EQ(gtyper.count_num_haplogroups(a1), expected);

    allele_vector a2{
            Allele{"", {}, 0},
            Allele{"", {}, 1},
            Allele{"", {}, 1},
    };

    expected = multiplicities({false, true}); // Haplogroup 0 has 1 allele, haplogroup 1 has > 1 allele
    EXPECT_EQ(gtyper.count_num_haplogroups(a2), expected);
}

TEST(MakePermutations,GivenVariousParameters_CorrectPermutations){
    std::vector<GtypedIndices> expected;
    LevelGenotyperModel g;

    auto two_from_three = g.get_permutations(GtypedIndices{1,4,5}, 2);
    expected = {
            {1, 4},
            {1, 5},
            {4, 5}
    };
    EXPECT_EQ(two_from_three, expected);

    // Make sure result is internally sorted (at the genotype index level); needed for diploid coverage memoization
    auto from_unsorted = g.get_permutations(GtypedIndices{4,3,2}, 2);
    std::sort(from_unsorted.begin(), from_unsorted.end());
    expected = {
            {2, 3},
            {2, 4},
            {3, 4}
    };
    EXPECT_EQ(from_unsorted, expected);

    auto two_from_one = g.get_permutations(GtypedIndices{1}, 2); // Invalid call
    expected = {};
    EXPECT_EQ(two_from_one, expected);
}

TEST(RescaleGenotypes, GivenVariousGenotypes_CorrectRescaling){
    LevelGenotyperModel g;
    GtypedIndices no_zero_gt{1, 3};
    GtypedIndices no_zero_gt_rescaled{1, 2};

    EXPECT_EQ(g.rescale_genotypes(no_zero_gt), no_zero_gt_rescaled);

    GtypedIndices zero_and_repeated_gt{0, 4, 4};
    GtypedIndices  zero_and_repeated_gt_rescaled{0, 1, 1};
    EXPECT_EQ(g.rescale_genotypes( zero_and_repeated_gt ), zero_and_repeated_gt_rescaled);

    GtypedIndices shuffled_order{4, 2};
    GtypedIndices  shuffled_order_rescaled{1, 2};
    EXPECT_EQ(g.rescale_genotypes( shuffled_order ), shuffled_order_rescaled);
}