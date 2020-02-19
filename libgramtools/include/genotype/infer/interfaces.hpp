/**
 * @file Common interface file for:
 * - genotyped sites
 * - genotyping models
 */
#ifndef INFER_IFC
#define INFER_IFC

#include <variant>
#include "genotype/infer/types.hpp"
#include "genotype/quasimap/coverage/types.hpp"
#include "json_spec/common.hpp"

using namespace gram::json;
namespace gram::genotype::infer {

    /**
     * Used in allele extraction but also in level genotyper
     */
    template <typename T>
     std::vector<T> prepend(std::vector<T> const& original_object, T const& to_prepend){
        std::vector<T> result;
        result.reserve(original_object.size() + 1);
        result.insert(result.end(), to_prepend);
        result.insert(result.end(), original_object.begin(), original_object.end());

        return result;
     }

    using GenotypeOrNull = std::variant<GtypedIndices, bool>;

    struct gtype_information{
        allele_vector alleles;
        GenotypeOrNull genotype;
        allele_coverages allele_covs;
        std::size_t total_coverage; /**< Total coverage on this site */
        AlleleIds haplogroups;
    };

    /**
     * Genotyped site interface
     */
    class GenotypedSite {
    protected:
        gtype_information gtype_info;
        covG_ptr site_end_node;
        std::size_t num_haplogroups = 0; /**< The number of outgoing edges from the bubble start */

        json_site_ptr json_site;

    public:
        GenotypedSite() = default;
        virtual ~GenotypedSite() {};

        gtype_information get_all_gtype_info() const {return this->gtype_info; }
        void populate_site(gtype_information const& gtype_info);
        GenotypeOrNull const get_genotype() const { return gtype_info.genotype; }
        allele_vector const get_alleles() const { return gtype_info.alleles; }
        covG_ptr const get_site_end_node() const { return site_end_node; }

        /** Whether the site is null genotyped */
        bool is_null() const {
            if (std::holds_alternative<bool>(gtype_info.genotype)) return true;
            else return false;
        };

        void make_null() {
            gtype_info.genotype = false;
            gtype_info.total_coverage = 0;
            this->null_model_specific_entries();
        }

        void set_alleles(allele_vector const& alleles){ gtype_info.alleles = alleles; };
        void set_genotype(GenotypeOrNull const& gtype){ gtype_info.genotype = gtype; }

        json_site_ptr get_JSON();
        virtual void add_model_specific_JSON(JSON& input_json) = 0;
        virtual void null_model_specific_entries() = 0;

        std::size_t const &get_num_haplogroups() { return num_haplogroups; }
        bool const has_alleles() const { return gtype_info.alleles.size() > 0; }

        void set_site_end_node(covG_ptr const &end_node) { site_end_node = end_node; }
        void set_num_haplogroups(std::size_t const &num_haps) { num_haplogroups = num_haps; }

        /**
         * Given alleles and GT, return the alleles referred to by GT
         */
        allele_vector const get_unique_genotyped_alleles
                (allele_vector const &all_alleles, GenotypeOrNull const &genotype) const;
        allele_vector const get_unique_genotyped_alleles() const {
            return get_unique_genotyped_alleles(gtype_info.alleles, gtype_info.genotype);
        }

        /**
         * Produce the haplogroups that have not been genotyped, for use in nested
         * site invalidation.
         */
        AlleleIds const get_nonGenotyped_haplogroups() const;

        AlleleIds const get_all_haplogroups() const {
            assert(num_haplogroups > 0);
            AlleleIds result;
            for (std::size_t idx{0}; idx < num_haplogroups; idx++) result.push_back(idx);
            return result;
        }

        AlleleIds get_genotyped_haplogroups(allele_vector const& input_alleles, GtypedIndices const& input_gts) const;
    };


    /**
     * Genotyping model interface.
     * Each derived model implements the production of an abstract site.
     */
    class GenotypingModel {
        virtual gt_site_ptr get_site() = 0;
    };


    class Genotyper {
    protected:
        gt_sites genotyped_records;
        coverage_Graph const *cov_graph;
        SitesGroupedAlleleCounts const *gped_covs;
        child_map child_m;

        json_prg_ptr json_prg;

        Genotyper() : cov_graph(nullptr), gped_covs(nullptr) {}
        Genotyper(gt_sites const& sites, child_map const& ch) :
                genotyped_records(sites), child_m(ch), cov_graph(nullptr), gped_covs(nullptr) {}

        /**
         * Populates the PRG-related entries (Lvl1_sites, child map) of this objects's json_prg.
         */
        void populate_json_prg();

        void add_json_sites();

    public:
        json_prg_ptr get_JSON();
        gt_sites const& get_genotyped_records() const {return genotyped_records;}

        void run_invalidation_process(gt_site_ptr const& genotyped_site, Marker const& site_ID);
        AlleleIds get_haplogroups_with_sites(Marker const& site_ID, AlleleIds candidate_haplogroups) const;
        void invalidate_if_needed(Marker const& parent_site_ID, AlleleIds haplogroups);
    };
}

#endif //INFER_IFC
