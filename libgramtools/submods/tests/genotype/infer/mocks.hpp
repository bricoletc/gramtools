#include "genotype/infer/genotyped_site.hpp"
#include "genotype/infer/probabilities.hpp"
#include "gmock/gmock.h"

using namespace gram;
using namespace gram::genotype::infer;

class MockGenotypedSite : public AbstractGenotypedSite{
public:
    MOCK_METHOD(AlleleIds const, get_genotype, (), (const, override));
    MOCK_METHOD(allele_vector const, get_alleles, (), (const, override));
    MOCK_METHOD(covG_ptr const, get_site_end_node, (), (const, override));
};

namespace gram::genotype::infer::probabilities{
    class MockPmf : public AbstractPmf{
    public:
        MOCK_METHOD(double, compute_prob, (params const& query), (const, override));
    };
}
