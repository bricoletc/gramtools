import logging
import time
from ..paths import SimulatePaths
from ... import common


log = logging.getLogger("gramtools")


def setup_parser(common_parser, subparsers):
    parser = subparsers.add_parser("simulate", parents=[common_parser])
    parser.add_argument(
        "--prg",
        help="A prg as made by (or passed to) gramtools build",
        type=str,
        required=True,
    )
    parser.add_argument(
        "--max_num_paths",
        "-n",
        help="Number of paths through the prg to simulate. \n"
        "Duplicates are removed, so this is an upper bound.",
        type=int,
        required=False,
        default=100,
    )
    parser.add_argument(
        "--sample_id",
        help="A name for your sampled paths.\n"
        "Prefixes the output filenames and used in the sample entries of the outputs.",
        required=False,
        default="sim",
    )
    parser.add_argument(
        "--output_dir",
        "-o",
        help="directory containing outputs",
        type=str,
        required=False,
        default=".",
    )
    parser.add_argument(
        "--seed",
        help="Fixing the seed will produce the same simulated paths across different runs."
        "By default, seed is randomly generated.",
        type=int,
        default=0,
        required=False,
    )


def run(args):
    simu_paths = SimulatePaths(args.output_dir, args.sample_id, args.prg)

    log.info("Start process: simulate")
    start_time = str(time.time()).split(".")[0]

    _execute_command_cpp_simulate(simu_paths, args)

    log.info("End process: simulate")


def _execute_command_cpp_simulate(simu_paths, args):
    command = [
        common.gramtools_exec_fpath,
        "simulate",
        "--prg",
        str(simu_paths.prg_fpath),
        "--n",
        str(args.max_num_paths),
        "--sample_id",
        args.sample_id,
        "--o",
        str(simu_paths.output_dir),
        "--seed",
        str(args.seed),
    ]

    if args.debug:
        command += ["--debug"]

    command_result, entire_stdout = common.run_subprocess(command)

    if command_result == False:
        exit(1)
