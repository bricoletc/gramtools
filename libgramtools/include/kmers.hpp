#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#include "fm_index.hpp"
#include "ranks.hpp"


#ifndef GRAMTOOLS_KMERS_HPP
#define GRAMTOOLS_KMERS_HPP


template<typename SEQUENCE>
struct seq_hash {
    std::size_t operator()(const SEQUENCE &seq) const {
        std::size_t hash = 0;
        boost::hash_range(hash, seq.begin(), seq.end());
        return hash;
    }
};

template<typename SEQUENCE, typename T>
using sequence_map = std::unordered_map<SEQUENCE, T, seq_hash<SEQUENCE>>;

template<typename SEQUENCE>
using sequence_set = std::unordered_set<SEQUENCE, seq_hash<SEQUENCE>>;

using Allele = std::vector<int>;
using VariantSiteMarker = uint64_t;
using VariantSite = std::pair<VariantSiteMarker, Allele>;
using Site = std::vector<VariantSite>;
using Sites = std::list<Site>;

using SA_Interval = std::pair<uint64_t, uint64_t>;
using SA_Intervals = std::list<SA_Interval>;

using Kmer = std::vector<uint8_t>;
using Kmers = std::vector<Kmer>;
using KmerSA_Intervals = sequence_map<Kmer, SA_Intervals>;
using KmerSites = sequence_map<Kmer, Sites>;

// Kmers added when not in variant site or when entierly within a single allele
using NonVariantKmers = sequence_set<Kmer>;

struct KmersData {
    KmerSA_Intervals sa_intervals_map;
    KmerSites sites_map;
    NonVariantKmers nonvar_kmers;
};

struct ThreadData {
    KmerSA_Intervals *sa_intervals_map;
    KmerSites *sites_map;
    NonVariantKmers *nonvar_kmers;
    std::vector<std::vector<uint8_t>> *kmers;
    int thread_id;
    const FM_Index *fm_index;
    const DNA_Rank *rank_all;
    const std::vector<int> *allele_mask;
    uint64_t max_alphabet_num;
};

uint8_t encode_dna_base(const char &base_str);

std::vector<uint8_t> encode_dna_bases(const std::string &dna_str);

std::string dump_kmer(const Kmer &kmer);

std::string dump_sa_intervals(const SA_Intervals &sa_intervals);

std::string dump_kmer_in_ref_flag(const Kmer &kmer,
                                  const NonVariantKmers &kmers_in_ref);

std::string dump_sites(const Kmer &kmer, const KmerSites &kmer_sites);

std::string dump_kmer_precalc_entry(const Kmer &kmer,
                                    const SA_Intervals &sa_intervals,
                                    const NonVariantKmers &kmers_in_ref,
                                    const KmerSites &kmer_sites);

void dump_thread_result(std::ofstream &precalc_file,
                        const KmerSA_Intervals &kmers_sa_intervals,
                        const NonVariantKmers &kmers_in_ref,
                        const KmerSites &kmer_sites);

void index_kmers(Kmers &kmers, KmerSA_Intervals &kmer_idx, KmerSites &kmer_sites, NonVariantKmers &kmers_in_ref, const uint64_t maxx,
                 const std::vector<int> &allele_mask, const DNA_Rank &rank_all, const FM_Index &fm_index);

void *worker(void *st);

void generate_kmers_encoding_threading(const std::vector<int> &allele_mask,
                                       const std::string &kmer_fname,
                                       const uint64_t maxx,
                                       const int thread_count,
                                       const DNA_Rank &rank_all,
                                       const FM_Index &fm_index);

inline bool fexists(const std::string &name);

static inline std::string &ltrim(std::string &s);

static inline std::string &rtrim(std::string &s);

static inline std::string &trim(std::string &s);

std::vector<std::string> split(const std::string &cad, const std::string &delim);

bool parse_in_reference_flag(const std::string &in_reference_flag_str);

Kmer parse_encoded_kmer(const std::string &encoded_kmer_str);

SA_Intervals parse_sa_intervals(const std::string &full_sa_intervals_str);

Site parse_site(const std::string &sites_part_str);

void parse_kmer_index_entry(KmersData &kmers, const std::string &line);

KmersData read_encoded_kmers(const std::string &encoded_kmers_fname);

int get_thread_count();

KmersData get_kmers(const std::string &kmer_fname,
                    const std::vector<int> &allele_mask,
                    const uint64_t maxx,
                    const DNA_Rank &rank_all,
                    const FM_Index &fm_index);


#endif //GRAMTOOLS_KMERS_HPP