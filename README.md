[![Build Status](https://travis-ci.org/iqbal-lab-org/gramtools.svg?branch=master)](https://travis-ci.org/iqbal-lab-org/gramtools)

# gramtools
**TL;DR** Genome inference using prior information encoded as a reference graph.

Gramtools builds a population reference genome (PRG) from a set of variants. 
Given sequence data from an individual, the graph is annotated with coverage and genotyped. 
 
 A personalised reference genome for the sample can be inferred and new variation discovered 
 against it. You can then build a new PRG from the initial + the new variants, and carry on forever!

## Install
```
pip3 install -vvv wheel git+https://github.com/iqbal-lab-org/gramtools
```
Requirements:
* C++17 compatible compiler: g++ >=8, clang >=7
<br>

If `sudo` is unavailable, we recommend using a Python virtual enviroment:
```
python3 -m venv gram_ve && source gram_ve/bin/activate
```
Note: installation fails with `virtualenv`.

## Usage
Gramtools currently consists of three commands. These commands are documented in the wiki 
(see links below). In the order you typically run them:
1) [build](https://github.com/iqbal-lab-org/gramtools/wiki/Commands%3A-build) - 
given a VCF and reference, construct the graph.

2) [genotype](https://github.com/iqbal-lab-org/gramtools/wiki/Commands%3A-quasimap) - 
    map reads to a graph generated in `build` and genotype the graph.

3) (TODO) [discover](https://github.com/iqbal-lab-org/gramtools/wiki/Commands%3A-discover) - 
infers a personalised reference genome for the sample and discovers new variation against it using
 one or more standard variant callers (currently: cortex).

Examples, documentation, and planned future enhancements can be found in the [wiki](https://github.com/iqbal-lab-org/gramtools/wiki).

```
Gramtools

Usage:
  gramtools build --gram_dir --vcf --reference --kmer_size 
  gramtools genotype --gram_dir --genotype_dir --reads [--ploidy {haploid,diploid}]
  gramtools discover --genotype_dir

  gramtools (-h | --help)
  gramtools --version

Options:
  --gram_dir 	Gramtools directory containing outputs of `build` 
  --kmer-size 	Kmer size at which to build the graph (used for seeding reads in `quasimap`)	

  --genotype_dir 	Stores outputs of `genotype` and `discover`
  --reads 	1+ reads file, in (fasta/fastq/sam/bam/cram) format

  -h --help             Show this screen
  --version             Show version

Subcommands:
  build         Construct the graph and supporting data structures
  genotype      Map reads to graph and genotype the graph
  discover	Discover new variation against inferred personalised reference genome
```

## Reference documentation

A [doxygen](http://doxygen.nl/) formatted documentation can be generated by running 
```doxygen doc/Doxyfile.in```
from inside the gramtools directory.

The documentation gets generated in doc/html/index.html and provides a useful reference for all files, classes, functions and data structures in gramtools.

## License

MIT
