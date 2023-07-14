//
// Created by Dustin Cobas <dustin.cobas@gmail.com> on 1/18/21.
//

#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <gflags/gflags.h>

#include <benchmark/benchmark.h>

#include <SelfGrammarIndexPTS.h>

#include "bm_locate.h"

DEFINE_string(patterns, "", "Patterns file. (MANDATORY)");
DEFINE_string(data_dir, "./", "Data directory.");
DEFINE_string(data_name, "data", "Data file basename.");

DEFINE_int32(min_s, 2, "Minimum sampling parameter s.");
DEFINE_int32(max_s, 32, "Maximum sampling parameter s.");

DEFINE_bool(report_stats, false, "Report statistics for benchmark (mean, median, ...).");
DEFINE_int32(reps, 10, "Repetitions for the locate query benchmark.");
DEFINE_double(min_time, 0, "Minimum time (seconds) for the locate query micro benchmark.");

DEFINE_bool(print_result, false, "Execute benchmark that print results per index.");

class Factory {
 public:
  Factory(std::string t_idx_dir, const std::string &t_data_name) : idx_dir_{std::move(t_idx_dir)} {
    idx_suffix_ = "pts-idx_" + t_data_name + ".gi";
  }

  struct Index {
    std::shared_ptr<SelfGrammarIndexPTS> idx;
    std::size_t size = 0;
  };

  Index make(std::size_t t_s) {
    auto it = indexes_.find(t_s);
    if (it != indexes_.end()) {
      return it->second;
    }

    Index index;
    std::fstream fpts(idx_dir_ + "/" + std::to_string(t_s) + "_" + idx_suffix_, std::ios::in | std::ios::binary);
    index.idx = std::make_shared<SelfGrammarIndexPTS>(t_s);
    index.idx->load(fpts);
    index.size = index.idx->size_in_bytes() - index.idx->get_grammar().get_right_trie().size_in_bytes()
        - index.idx->get_grammar().get_left_trie().size_in_bytes();

    indexes_[t_s] = index;

    return index;
  }

 private:
  std::string idx_dir_;
  std::string idx_suffix_;

  std::map<std::size_t, Index> indexes_;
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
  auto patterns = ReadPatterns(FLAGS_patterns);

  // Indexes
  auto factory = std::make_shared<Factory>(FLAGS_data_dir, FLAGS_data_name);
  auto n = filesize(FLAGS_data_dir + "/" + FLAGS_data_name);

  // Index builder
  auto make_index = [factory](const auto &tt_state) {
    auto idx = factory->make(tt_state.range(0));

    auto locate = [idx](auto ttt_pattern) {
      std::vector<uint> occs;
      idx.idx->locateNoTrie(ttt_pattern, occs);

      return occs;
    };

    return std::make_pair(locate, idx.size);
  };

  LocateBenchmarkConfig locate_bm_config{FLAGS_report_stats, FLAGS_reps, FLAGS_min_time, FLAGS_print_result};
  // Benchmarks
  const std::string name = "G-Index";
  auto bms = RegisterLocateBenchmarks(name, make_index, patterns, n, locate_bm_config, true);
  // Index with sampling
  for (auto &bm : bms) {
    bm.second->RangeMultiplier(2)->Range(FLAGS_min_s, FLAGS_max_s);
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
