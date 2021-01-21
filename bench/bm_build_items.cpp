//
// Created by Dustin Cobas <dustin.cobas@gmail.com> on 1/18/21.
//

#include <iostream>
#include <sys/stat.h>

#include <gflags/gflags.h>

#include <benchmark/benchmark.h>
#include <utils/grammar.h>
#include <SelfGrammarIndexBS.h>
#include <SelfGrammarIndexPTS.h>

DEFINE_string(data, "", "Data file. (MANDATORY)");
DEFINE_bool(rebuild, false, "Rebuild all the items.");

void SetupDefaultCounters(benchmark::State &t_state) {
  t_state.counters["n"] = 0;
  t_state.counters["z"] = 0;
  t_state.counters["s"] = 0;
  t_state.counters["r'"] = 0;
  t_state.counters["mr'"] = 0;
}

// Benchmark Warm-up
static void BM_WarmUp(benchmark::State &t_state) {
  for (auto _ : t_state) {
    std::vector<int> empty_vector(1000000, 0);
  }

  SetupDefaultCounters(t_state);
}
BENCHMARK(BM_WarmUp);

std::string load_data(const std::string &collection);

auto BM_BuildGIndexPT = [](benchmark::State &t_state,
                           const auto &t_data_path,
                           const auto &t_basics_fn,
                           const auto &t_repair_fn,
                           const auto &t_suffixes_fn) {
  SelfGrammarIndexBS idx;
  grammar *not_compressed_grammar = nullptr;
  std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > > grammar_sfx;

  for (auto _ : t_state) {
    std::string data = load_data(t_data_path);

    if (not_compressed_grammar != nullptr) { delete not_compressed_grammar; }
    not_compressed_grammar = new grammar();
    grammar_sfx.clear();

    idx.build_basics(data, *not_compressed_grammar, grammar_sfx);
  }

  // Store G-Index's basics
  {
    std::fstream fbasics(t_basics_fn, std::ios::out | std::ios::binary);
    idx.save(fbasics);
  }
  {
    std::fstream frepair(t_repair_fn, std::ios::out | std::ios::binary);
    if (not_compressed_grammar != nullptr) {
      not_compressed_grammar->save(frepair);
      delete not_compressed_grammar;
    }
  }
  {
    std::fstream fsuffixes(t_suffixes_fn, std::ios::out | std::ios::binary);
    unsigned long num_sfx = grammar_sfx.size();
    sdsl::serialize(num_sfx, fsuffixes);

    for (auto &&e : grammar_sfx) {

      size_t p = e.first.first;
      sdsl::serialize(p, fsuffixes);
      p = e.first.second;
      sdsl::serialize(p, fsuffixes);
      p = e.second.first;
      sdsl::serialize(p, fsuffixes);
      p = e.second.second;
      sdsl::serialize(p, fsuffixes);

    }
  }

  SetupDefaultCounters(t_state);
};

auto BM_BuildGIndexPTS = [](benchmark::State &t_state,
                            const auto &t_data_path,
                            const auto &t_basics_fn,
                            const auto &t_repair_fn,
                            const auto &t_suffixes_fn,
                            const auto &t_pts_idx_fn) {
  std::size_t s = t_state.range(0);

  SelfGrammarIndexPTS idx(s);

  for (auto _ : t_state) {
    std::string data = load_data(t_data_path);

    std::fstream fbasics(t_basics_fn, std::ios::in | std::ios::binary);
    idx.load_basics(fbasics);

    std::fstream fsuffixes(t_suffixes_fn, std::ios::in | std::ios::binary);
    std::fstream frepair(t_repair_fn, std::ios::in | std::ios::binary);

    idx.build_suffix(data, fsuffixes, frepair);
  }

  // Store G-IndexPTS
  std::fstream f_gidx(std::to_string(s) + "_" + t_pts_idx_fn, std::ios::out | std::ios::binary);
  idx.save(f_gidx);

  SetupDefaultCounters(t_state);
};

inline bool file_exists(const std::string &name);

int main(int argc, char **argv) {
  gflags::SetUsageMessage("This program calculates the ri items for the given text.");
  gflags::AllowCommandLineReparsing();
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  if (FLAGS_data.empty()) {
    std::cerr << "Command-line error!!!" << std::endl;
    return 1;
  }

  std::string data_path = FLAGS_data;
  std::string basics_fn = "basics.gi";
  std::string repair_fn = "grepair.gi";
  std::string suffixes_fn = "suffixes.gi";
  std::string pts_idx_fn = "pts-idx.gi";

  if (!file_exists(basics_fn) || FLAGS_rebuild) {
    benchmark::RegisterBenchmark("BuildGIndexPT", BM_BuildGIndexPT, data_path, basics_fn, repair_fn, suffixes_fn);
  }

  if (!file_exists("16_" + pts_idx_fn) || FLAGS_rebuild) {
    benchmark::RegisterBenchmark("BuildGIndexPT",
                                 BM_BuildGIndexPTS,
                                 data_path,
                                 basics_fn,
                                 repair_fn,
                                 suffixes_fn,
                                 pts_idx_fn)
        ->RangeMultiplier(2)
        ->Range(2, 2u << 6u);
  }

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();

  return 0;
}

inline bool file_exists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

std::string load_data(const std::string &collection) {
  std::fstream f(collection, std::ios::in | std::ios::binary);
  if (!f.is_open()) {
    std::cout << "Error the file could not opened!!\n";
    return "";
  }

  std::string data;
  std::string buff;
  unsigned char buffer[1000];
  while (!f.eof()) {
    f.read((char *) buffer, 1000);
    data.append((char *) buffer, f.gcount());
  }

  return data;
}