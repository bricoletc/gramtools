#include <boost/timer/timer.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

#include "common/timer_report.hpp"

using namespace gram;

void gram::TimerReport::start(std::string note) {
  this->note = note;
  timer.start();
}

void TimerReport::stop() {
  if (this->note.empty())
    std::cerr << "TimerReport stop called with empty note" << std::endl;
  boost::timer::cpu_times times = timer.elapsed();
  double elapsed_time = (times.user + times.system) * 1e-9;
  Entry entry = std::make_pair(note, elapsed_time);
  logger.push_back(entry);
  this->note = "";
}

void TimerReport::report() const {
  std::cout << "\nTimer report:" << std::endl;
  cout_row(" ", "seconds");

  double total_elapsed_time = 0;

  for (const auto &entry : TimerReport::logger) {
    Note note;
    double elapsed_time;
    std::tie(note, elapsed_time) = entry;

    cout_row(note, elapsed_time);
    total_elapsed_time += elapsed_time;
  }

  std::cout << std::endl
            << "Total elapsed time: " << total_elapsed_time << std::endl;
}

template <typename TypeCol1, typename TypeCol2>
void TimerReport::cout_row(TypeCol1 col1, TypeCol2 col2) const {
  std::cout << std::setw(20) << std::right << col1 << std::setw(10)
            << std::right << col2 << std::endl;
}