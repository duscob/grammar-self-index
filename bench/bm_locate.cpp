//
// Created by Dustin Cobas <dustin.cobas@gmail.com> on 1/18/21.
//

#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <gflags/gflags.h>

#include <benchmark/benchmark.h>

#include <SelfGrammarIndexPTS.h>

DEFINE_string(patterns, "", "Patterns file. (MANDATORY)");
DEFINE_string(data_dir, "./", "Data directory.");
DEFINE_string(data_name, "data", "Data file basename.");
DEFINE_bool(print_result, false, "Execute benchmark that print results per index.");

void SetupDefaultCounters(benchmark::State &t_state) {
  t_state.counters["Collection_Size(bytes)"] = 0;
  t_state.counters["Size(bytes)"] = 0;
  t_state.counters["Bits_x_Symbol"] = 0;
  t_state.counters["Patterns"] = 0;
  t_state.counters["Time_x_Pattern"] = 0;
  t_state.counters["Occurrences"] = 0;
  t_state.counters["Time_x_Occurrence"] = 0;
}

// Benchmark Warm-up
static void BM_WarmUp(benchmark::State &_state) {
  for (auto _ : _state) {
    std::vector<int> empty_vector(1000000, 0);
  }

  SetupDefaultCounters(_state);
}
BENCHMARK(BM_WarmUp);

auto BM_QueryLocate = [](benchmark::State &t_state,
                         const auto &t_idx_dir,
                         const auto &t_pts_idx_fn,
                         const auto &t_patterns,
                         auto t_seq_size) {
  std::size_t s = t_state.range(0);

  std::fstream fpts(t_idx_dir + "/" + std::to_string(s) + "_" + t_pts_idx_fn, std::ios::in | std::ios::binary);
  SelfGrammarIndexPTS idx(s);
  idx.load(fpts);

  std::size_t total_occs = 0;

  for (auto _ : t_state) {
    total_occs = 0;
    for (const auto &pattern: t_patterns) {
      auto pat = pattern;
      std::vector<uint> occs;

      idx.locateNoTrie(pat, occs);

      total_occs += occs.size();
    }
  }

  auto idx_size = idx.size_in_bytes() - idx.get_grammar().get_right_trie().size_in_bytes() - idx.get_grammar().get_left_trie().size_in_bytes();
  SetupDefaultCounters(t_state);
  t_state.counters["Collection_Size(bytes)"] = t_seq_size;
  t_state.counters["Size(bytes)"] = idx_size;
  t_state.counters["Bits_x_Symbol"] = idx_size * 8.0 / t_seq_size;
  t_state.counters["Patterns"] = t_patterns.size();
  t_state.counters["Time_x_Pattern"] = benchmark::Counter(
      t_patterns.size(), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
  t_state.counters["Occurrences"] = total_occs;
  t_state.counters["Time_x_Occurrence"] = benchmark::Counter(
      total_occs, benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
};

auto BM_PrintQueryLocate = [](benchmark::State &t_state,
                              const auto &t_idx_name,
                              const auto &t_idx_dir,
                              const auto &t_pts_idx_fn,
                              const auto &t_patterns,
                              auto t_seq_size) {
  std::size_t s = t_state.range(0);

  std::string idx_name = t_idx_name;
  replace(idx_name.begin(), idx_name.end(), '/', '_');
  std::string output_filename = "result-" + idx_name + "-" + std::to_string(s) + ".txt";

  std::fstream fpts(t_idx_dir + "/" + std::to_string(s) + "_" + t_pts_idx_fn, std::ios::in | std::ios::binary);
  SelfGrammarIndexPTS idx(s);
  idx.load(fpts);

  std::size_t total_occs = 0;

  for (auto _ : t_state) {
    std::ofstream out(output_filename);
    total_occs = 0;
    for (const auto &pattern: t_patterns) {
      out << pattern << std::endl;
      auto pat = pattern;
      std::vector<uint> occs;

      idx.locateNoTrie(pat, occs);

      total_occs += occs.size();

      sort(occs.begin(), occs.end());
      for (const auto &item  : occs) {
        out << "  " << item << std::endl;
      }
    }
  }

  auto idx_size = idx.size_in_bytes() - idx.get_grammar().get_right_trie().size_in_bytes() - idx.get_grammar().get_left_trie().size_in_bytes();
  SetupDefaultCounters(t_state);
  t_state.counters["Collection_Size(bytes)"] = t_seq_size;
  t_state.counters["Size(bytes)"] = idx_size;
  t_state.counters["Bits_x_Symbol"] = idx_size * 8.0 / t_seq_size;
  t_state.counters["Patterns"] = t_patterns.size();
  t_state.counters["Time_x_Pattern"] = benchmark::Counter(
      t_patterns.size(), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
  t_state.counters["Occurrences"] = total_occs;
  t_state.counters["Time_x_Occurrence"] = benchmark::Counter(
      total_occs, benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
};
long filesize(const std::string &t_filename);

int main(int argc, char **argv) {
  gflags::AllowCommandLineReparsing();
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  if (FLAGS_patterns.empty() || FLAGS_data_name.empty() || FLAGS_data_dir.empty()) {
    std::cerr << "Command-line error!!!" << std::endl;
    return 1;
  }

  // Query patterns
  std::vector<std::string> patterns;
  {
    std::ifstream pattern_file(FLAGS_patterns.c_str(), std::ios_base::binary);
    if (!pattern_file) {
      std::cerr << "ERROR: Failed to open patterns file!" << std::endl;
      return 3;
    }

    std::string buf;
    while (std::getline(pattern_file, buf)) {
      if (buf.empty())
        continue;

      patterns.emplace_back(buf);
    }
    pattern_file.close();
  }

  // Indexes
  std::string pts_idx_fn = "pts-idx_" + FLAGS_data_name + ".gi";
  auto coll_size = filesize(FLAGS_data_dir + "/" + FLAGS_data_name);

  std::string index_name = "g-index";
  benchmark::RegisterBenchmark(index_name.c_str(), BM_QueryLocate, FLAGS_data_dir, pts_idx_fn, patterns, coll_size)
      ->RangeMultiplier(2)
      ->Range(2, 1u << 6u);

  std::string print_bm_prefix = "Print-";
  if (FLAGS_print_result) {
    auto print_bm_name = print_bm_prefix + index_name;
    benchmark::RegisterBenchmark(print_bm_name.c_str(),
                                 BM_PrintQueryLocate,
                                 index_name,
                                 FLAGS_data_dir,
                                 pts_idx_fn,
                                 patterns,
                                 coll_size)
        ->RangeMultiplier(2)
        ->Range(2, 1u << 6u);
  }

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();

  return 0;
}

long filesize(const std::string &t_filename) {
  struct stat stat_buf;
  int rc = stat(t_filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}
