//
// Created by Dustin Cobas <dustin.cobas@gmail.com> on 1/18/21.
//

#include <iostream>
#include <filesystem>
#include <sys/stat.h>

#include <gflags/gflags.h>

#include <benchmark/benchmark.h>
#include <utils/grammar.h>
#include <SelfGrammarIndexBS.h>
#include <SelfGrammarIndexPTS.h>

DEFINE_string(data, "", "Data file. (MANDATORY)");
DEFINE_int32(min_s, 2, "Minimum sampling parameter s.");
DEFINE_int32(max_s, 2u << 6u, "Maximum sampling parameter s.");

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
  std::size_t n = 0;
  SelfGrammarIndexBS idx;
  grammar *not_compressed_grammar = nullptr;
  std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > > grammar_sfx;

  for (auto _ : t_state) {
    std::string data = load_data(t_data_path);
    n = data.size();

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
  t_state.counters["n"] = n;
};

inline bool file_exists(const std::string &name);

auto BM_BuildGIndexPTS = [](benchmark::State &t_state,
                            const auto &t_data_path,
                            const auto &t_basics_fn,
                            const auto &t_repair_fn,
                            const auto &t_suffixes_fn,
                            const auto &t_pts_idx_fn) {
  std::size_t s = t_state.range(0);
  std::size_t n = 0;

  auto output_path = std::to_string(s) + "_" + t_pts_idx_fn;
  if (file_exists(output_path)) {
    t_state.SkipWithMessage("The index already exists!");
    return;
  }

  SelfGrammarIndexPTS idx(s);

  for (auto _ : t_state) {
    std::string data = load_data(t_data_path);
    n = data.size();

    std::fstream fbasics(t_basics_fn, std::ios::in | std::ios::binary);
    idx.load_basics(fbasics);

    std::fstream fsuffixes(t_suffixes_fn, std::ios::in | std::ios::binary);
    std::fstream frepair(t_repair_fn, std::ios::in | std::ios::binary);

    idx.build_suffix(data, fsuffixes, frepair);
  }

  // Store G-IndexPTS
  std::fstream f_gidx(output_path, std::ios::out | std::ios::binary);
  idx.save(f_gidx);

  SetupDefaultCounters(t_state);
  t_state.counters["n"] = n;
  t_state.counters["s"] = s;
};

int main(int argc, char **argv) {
  gflags::SetUsageMessage("This program calculates the ri items for the given text.");
  gflags::AllowCommandLineReparsing();
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  if (FLAGS_data.empty()) {
    std::cerr << "Command-line error!!!" << std::endl;
    return 1;
  }

  auto data_path = std::filesystem::path(FLAGS_data);
  auto data_filename = data_path.filename().string();
  std::string basics_fn = "basics_" + data_filename + ".gi";
  std::string repair_fn = "grepair_" + data_filename + ".gi";
  std::string suffixes_fn = "suffixes_" + data_filename + ".gi";
  std::string pts_idx_fn = "pts-idx_" + data_filename + ".gi";

  if (!file_exists(basics_fn)) {
    benchmark::RegisterBenchmark("G-Index-PT", BM_BuildGIndexPT, data_path, basics_fn, repair_fn, suffixes_fn);
  }

  benchmark::RegisterBenchmark("G-Index-PT",
                               BM_BuildGIndexPTS,
                               data_path,
                               basics_fn,
                               repair_fn,
                               suffixes_fn,
                               pts_idx_fn)
      ->RangeMultiplier(2)
      ->Range(FLAGS_min_s, FLAGS_max_s);

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

  if (data.back() == '\0') {
    data.pop_back();
  }

  return data;
}