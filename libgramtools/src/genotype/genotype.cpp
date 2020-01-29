#include "genotype/genotype.hpp"
#include "genotype/quasimap/quasimap.hpp"

#include "common/timer_report.hpp"
#include "kmer_index/load.hpp"

using namespace gram;

ReadStats run_quasimap(const Parameters &parameters) {
    std::cout << "Executing quasimap command" << std::endl;
    auto timer = TimerReport();

    ReadStats readstats;

    std::string first_reads_fpath = parameters.reads_fpaths[0];
    readstats.compute_base_error_rate(first_reads_fpath);

    timer.start("Load data");
    std::cout << "Loading PRG data" << std::endl;
    const auto prg_info = load_prg_info(parameters);
    std::cout << "Loading kmer index data" << std::endl;
    const auto kmer_index = kmer_index::load(parameters);
    timer.stop();

    std::cout << "Running quasimap" << std::endl;
    timer.start("Quasimap");
    auto quasimap_stats = quasimap_reads(parameters, kmer_index, prg_info, readstats);

    // Commit the read stats into quasimap output dir.
    std::cout << "Writing read stats to " << parameters.read_stats_fpath << std::endl;
    readstats.serialise(parameters.read_stats_fpath);

    std::cout << std::endl;
    std::cout << "The following counts include generated reverse complement reads."
              << std::endl;
    std::cout << "Count all reads: " << quasimap_stats.all_reads_count << std::endl;
    std::cout << "Count skipped reads: " << quasimap_stats.skipped_reads_count << std::endl;
    std::cout << "Count mapped reads: " << quasimap_stats.mapped_reads_count << std::endl;
    timer.stop();
    timer.report();

    return readstats;
}

void commands::genotype::run(const Parameters &parameters){
    auto readstats = std::move(run_quasimap(parameters));
}