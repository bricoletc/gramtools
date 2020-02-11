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
#include "json_spec.hpp"

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

    using GtypedIndex = std::size_t; /**< The index of an allele in an allele vector */
    using GtypedIndices = std::vector<GtypedIndex>;
    using GenotypeOrNull = std::variant<GtypedIndices, bool>;
    using allele_coverages = std::vector<double>;

    class GenotypedSite;
    using gt_site = GenotypedSite;
    using gt_site_ptr = std::shared_ptr<GenotypedSite>;
    using gt_sites = std::vector<gt_site_ptr>;


    /**
     * Genotyped site interface
     */
    class GenotypedSite {
    protected:
        allele_vector alleles;
        GenotypeOrNull genotype;
        AlleleIds haplogroups;
        covG_ptr site_end_node;
        std::size_t num_haplogroups = 0; /**< The number of outgoing edges from the bubble start */

        JSON site_json;

    public:
        GenotypedSite() {
           for (const auto& element : json_::spec::site_fields.items()){
               site_json.emplace(element.key(), JSON::array());
           }
        }
        virtual ~GenotypedSite() {};
        virtual GenotypeOrNull const get_genotype() const = 0;
        virtual allele_vector const get_alleles() const = 0;
        virtual covG_ptr const get_site_end_node() const = 0;
        virtual bool is_null() const = 0;
        virtual void make_null() = 0;
        virtual JSON get_JSON() = 0;

        std::size_t const &get_num_haplogroups() { return num_haplogroups; }
        bool const has_alleles() const { return alleles.size() > 0; }

        void set_site_end_node(covG_ptr const &end_node) { site_end_node = end_node; }
        void set_num_haplogroups(std::size_t const &num_haps) { num_haplogroups = num_haps; }

        /**
         * Given alleles and GT, return the alleles referred to by GT
         */
        allele_vector const get_unique_genotyped_alleles
                (allele_vector const &all_alleles, GenotypeOrNull const &genotype) const;
        allele_vector const get_unique_genotyped_alleles() const {
            return get_unique_genotyped_alleles(alleles, genotype);
        }
        /**
         * This version exists for allowing to use mocked alleles and genotypes
         */
        allele_vector const extract_unique_genotyped_alleles() const {
            auto extracted_alleles = this->get_alleles();
            auto extracted_gts = this->get_genotype();
            return get_unique_genotyped_alleles(extracted_alleles, extracted_gts);
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

        Genotyper() : cov_graph(nullptr), gped_covs(nullptr) {}
        Genotyper(gt_sites const& sites) :
                genotyped_records(sites), cov_graph(nullptr), gped_covs(nullptr) {}

        JSON json_prg = json_::spec::json_prg;

        void add_json_sites(){
            for (auto const& site : genotyped_records)
                json_prg.at("Sites").push_back({site->get_JSON()});
        }
    public:
        virtual JSON get_JSON() = 0;
    };

}

#endif //INFER_IFC
