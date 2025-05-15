#!/usr/bin/env python3
import re
import sys
import csv
import glob
import argparse

# Define the CSV columns in the desired order
FIELDNAMES = [
    'benchmark',
    '#procedures',
    '#nodes',
    '#globals',
    '#partitions',
    '#max_partition_size',
    '#max_ddg_size',
    '#total_ddg_size',
    '#max_context_size',
    '#total_context_size',
    '#max_constant_size',
    '#total_constant_size',
    '#max_dead_size',
    '#total_dead_size',
]

# Regular expressions to capture each metric
PATTERNS = {
    '#procedures':          r'#Procedures:\s*(\d+)',
    '#nodes':               r'CFG nodes:\s*(\d+)',
    '#partitions':          r'Partitions:\s*(\d+)',
    '#max_partition_size':  r'Max partition size:\s*(\d+)',
    '#globals':             r'Total globals:\s*(\d+)',
    '#max_ddg_size':        r'Max DFG size:\s*(\d+)',
    '#total_ddg_size':      r'Total DFG size:\s*(\d+)',
    '#max_context_size':    r'Max number of contexts:\s*(\d+)',
    '#total_context_size':  r'Total number of contexts:\s*(\d+)',
    '#max_constant_size':   r'Max number of propagated values:\s*(\d+)',
    '#total_constant_size': r'Total number of propagated values:\s*(\d+)',
    '#max_dead_size':       r'Max number of dead QDefs:\s*(\d+)',
    '#total_dead_size':     r'Total number of dead QDefs:\s*(\d+)',
}


def extract_benchmark(cmd_line):
    """
    Given a command string like "./build/cos_ssa --tool=all --debug benchmarks/dds7.ll",
    extract 'dds7' as the benchmark name.
    """
    m = re.search(r'benchmarks/([^/]+)\.ll', cmd_line)
    if m:
        return m.group(1)
    base = cmd_line.strip().split()[-1]
    return re.sub(r'\.ll$', '', base)


def parse_stats(text):
    data = {}
    # Find the Command being timed
    cmd = None
    for line in text.splitlines():
        if line.startswith('\tCommand being timed:'):
            cmd = line.split(':',1)[1].strip().strip('"')
            break
    if not cmd:
        raise ValueError("Could not find the 'Command being timed' line.")
    data['benchmark'] = extract_benchmark(cmd)

    # Extract each numeric field
    for key, pat in PATTERNS.items():
        m = re.search(pat, text)
        if not m:
            raise ValueError(f"Could not find pattern for {key!r}")
        data[key] = m.group(1)

    return {
        'benchmark': data['benchmark'],
        '#procedures': data['#procedures'],
        '#nodes': data['#nodes'],
        '#globals': data['#globals'],
        '#partitions': data['#partitions'],
        '#max_partition_size': data['#max_partition_size'],
        '#max_ddg_size': data['#max_ddg_size'],
        '#total_ddg_size': data['#total_ddg_size'],
        '#max_context_size': data['#max_context_size'],
        '#total_context_size': data['#total_context_size'],
        '#max_constant_size': data['#max_constant_size'],
        '#total_constant_size': data['#total_constant_size'],
        '#max_dead_size': data['#max_dead_size'],
        '#total_dead_size': data['#total_dead_size'],
    }


def main():
    parser = argparse.ArgumentParser(
        description="Batch-parse stats dumps into a single CSV."
    )
    parser.add_argument('-i', '--indir', default='benchmarks',
                        help="Directory containing .log files (default: benchmarks)")
    parser.add_argument('-o', '--outfile', default='eval.csv',
                        help="Output CSV file name (default: eval.csv)")
    args = parser.parse_args()

    rows = []
    # Iterate over all .log files in the input directory
    pattern = f"{args.indir}/*.log"
    for filepath in glob.glob(pattern):
        with open(filepath, 'r') as f:
            text = f.read()
        try:
            row = parse_stats(text)
            rows.append(row)
        except Exception as e:
            sys.stderr.write(f"Warning: Failed to parse {filepath}: {e}\n")

    # Sort rows alphabetically by benchmark name
    rows.sort(key=lambda r: r['benchmark'])
    
    # Write out the composite CSV
    with open(args.outfile, 'w', newline='') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=FIELDNAMES)
        writer.writeheader()
        writer.writerows(rows)

    print(f"Written {len(rows)} entries to {args.outfile}")

if __name__ == '__main__':
    main()
