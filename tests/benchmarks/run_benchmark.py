import tempfile
import os
import sys
import json
import argparse
import subprocess

class Repository:

    def __init__(self, url):
        self.dir = tempfile.mkdtemp(dir="/opt/workspace/tmp")
        self.url = url
        print(self.url)
        print(self.dir)

    def clone(self):
        print("clone")
        subprocess.run("git clone".split() + [self.url, self.dir])

class DB_bench:

    def __init__(self, repo):
        self.path = repo
        self.run_output = None

    def build(self):
        try:
            subprocess.run("make bench".split(), cwd=self.path, check=True)
        except CalledProcessError as e:
            print(f"Cannot build benchmark: {e}")


    def run(self, params, env):
        try:
            env["PATH"] = self.path
            self.run_output = subprocess.run(["pmemkv_bench"] + params, cwd=self.path, env=env, capture_output=True, check=True)
        except CalledProcessError as e:
            print(f"benchmark process failed: {e}")
        except Exception as e :
            print(f"Run failed: {e}")

    def _parse_results(self):
        return self.run_output.stdout.splitlines();

    def get_results(self):
        for line in self._parse_results():
            print(line)



if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Runs pmemkv_bench')
    parser.add_argument('config_path', help="Path to json config file")
    args = parser.parse_args()
    print(args.config_path)

    config = None
    with open(args.config_path) as config_path:
        config = json.loads(config_path.read())
    print(config)
    repo = Repository(config["repo_url"])
    repo.clone()
    print(f"{repo.dir=}")
    benchmark = DB_bench(repo.dir)
    benchmark.build()
    benchmark.run(params=config["params"], env=config["env"])
    benchmark.get_results()

