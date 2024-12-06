import time
from kiwi.reporter import BaseReporter
from kiwi.kiwi import Kiwi
from kiwi.experiment import Experiment
from kiwi.defaults import DEFAULT_SETTINGS, JSONReporter

import re
from itertools import product

# Create custom reporter
class CustomReporter(BaseReporter):
    @staticmethod
    def generate(results: list[dict]):
        report_id = time.time_ns()
        with open(f"results-{report_id}.csv", "w") as f:
            f.write("instance;exact_v;exact_t;hl_v;hl_t;hr_v;hr_t\n")
            for exp in results:
                for run in exp['_runs']:
                    row = f"{exp['name']};"
                    row += f"{run['@exact_solution']};"
                    row += f"{run['@exact_time']};"
                    row += f"{run['@hl_solution']};"
                    row += f"{run['@hl_time']};"
                    row += f"{run['@hr_solution']};"
                    row += f"{run['@hr_time']}\n"
                    f.write(row)

# Create an output handler
def extract_results(run_info: dict, byte_array: bytes):
    x_data = lambda x: [float(i.split(':')[1].strip()) for i in x.split('\t')]
    output = byte_array.decode()
    for line in output.splitlines():
        if re.match(r'KPC: ', line):
            params = x_data(line)
            run_info['@exact_solution'] = params[0]
            run_info['@exact_time'] = params[1]
        if re.match(r'HL: ', line):
            params = x_data(line)
            run_info['@hl_solution'] = params[0]
            run_info['@hl_time'] = params[1]
        if re.match(r'HR: ', line):
            params = x_data(line)
            run_info['@hr_solution'] = params[0]
            run_info['@hr_time'] = params[1]

def main():
    SETTINGS = DEFAULT_SETTINGS.copy()
    SETTINGS['threads'] = 8

    KiwiRunner = Kiwi(SETTINGS)
    N = [120]
    H = [0.04, 0.15, 0.4]
    F = [0.01, 0.1, 0.4]
    TYPES = ['uncorr', 'strcorr']

    for n, h, f, t in product(N, H, F, TYPES):
        id = f"{t}-{n}-{h}-{f}"
        instance_name = f"kpc-{id}.dat"
        exp_name = f"{id}"
        cmd = f"../build/kcp ../../ic2024/kpc/benchmark/{instance_name}"
        exp = Experiment(cmd, exp_name)
        exp.attach_output_handler(extract_results)
        KiwiRunner.add_experiment(exp)


    KiwiRunner.run_all()

    KiwiRunner.gen_report(CustomReporter) # Save raw report

if __name__ == '__main__':
    main()
