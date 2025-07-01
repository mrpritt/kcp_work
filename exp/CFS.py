"""
CFS runner
"""
import glob
import time
from kiwi import BaseReporter, Kiwi, Experiment, DEFAULT_SETTINGS, gen_extractor

# To edit cmd args must go below
EXEC='~/kcp_work/bin/CFS'
FILE_GLOB='/home/gustavo/ic2024/data/set2/C1/*.bz2'
MAX_TIME=600
THREADS=8
OUT_FILE='C1-CFS600.csv'

# Create custom reporter
class Reporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        with open(f"{OUT_FILE}", "w") as f:
            f.write("instance,weight,time,steps\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']},"
                    row += f"{run['@weight']},"
                    row += f"{run['_runtime']},"
                    row += f"{run['@steps']}"
                    row += "\n"
                    f.write(row)

# pattern = r"(?:\[w:(\d+),([0-9.eE+-]+)s\])|(?:nSteps: (\d+))"
pattern = r"(?:(\[w:\d+,[0-9.eE+-]+s\]|\[w=\d+\]))|(?:nSteps: (\d+))"
def parse_weight(s):
    if ':' in s:
        weight, time = s.split(',')
        return (int(weight[3:]), float(time[:-2]))
    elif '=' in s:
        return (int(s.split('=')[1][:-1]), 0)
    return (0,0)

matcher = [
    (('@weight', '@time'), parse_weight, (0, 0)),
    ('@steps', int, 0)
]
extractor = gen_extractor(pattern, matcher)

def main():
    SETTINGS = DEFAULT_SETTINGS.copy()
    SETTINGS['threads'] = THREADS

    KiwiRunner = Kiwi(SETTINGS)
    files = glob.glob(FILE_GLOB)

    for file in files:
        id = f"{"/".join(file.split('/')[-2:])}"
        exp_name = f"{id}"
        cmd = f"bash -c 'timeout {MAX_TIME}s {EXEC} <(bzcat {file})'"
        exp = Experiment(cmd, exp_name)
        exp.attach_output_handler(extractor)
        KiwiRunner.add_experiment(exp)


    KiwiRunner.run_all()

    KiwiRunner.gen_report(Reporter)

if __name__ == '__main__':
    main()
