#ifndef GTYPE_MAKE_JSON_HPP
#define GTYPE_MAKE_JSON_HPP

#include "genotype/infer/interfaces.hpp"
#include "json_prg_spec.hpp"
#include "json_site_spec.hpp"

using namespace gram::json;
using namespace gram::json::spec;
using namespace gram::genotype;
using namespace gram::genotype::infer;

namespace gram::genotype {
class SegmentTracker;
}

json_prg_ptr make_json_prg(gtyper_ptr const& gtyper, SegmentTracker& tracker);

/**
 * Populates the PRG-related entries (Lvl1_sites, child map) of a Json_Prg
 * class.
 */
void populate_json_prg(Json_Prg& json_prg, gtyper_ptr const& gtyper);

json_site_ptr make_json_site(gt_site_ptr const& gt_site);

#endif  // GTYPE_MAKE_JSON_HPP
