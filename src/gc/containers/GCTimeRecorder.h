#pragma once
#include <deque>
#include <chrono>
#include <sstream>

namespace gccpp::details {

    struct Record {
        double timestamp;
        double pause;
    };

#ifdef USE_GCTIME_PROFILER

    class Timer final {
    public:
        void start() {
            m_StartTime = std::chrono::system_clock::now();
        }

        void stop() {
            m_EndTime = std::chrono::system_clock::now();
        }

        double elapsedMilliseconds() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count();
        }

        Record report() {
            double timestamp = m_StartTime.time_since_epoch().count();
            return {timestamp,  elapsedMilliseconds()};
        }

    private:
        std::chrono::time_point<std::chrono::system_clock> m_StartTime;
        std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    };

    class GCTimeRecorder final {
    public:
        void append(Timer& timer) {
            records.push_back(timer.report());
        }

        void report(FILE* out) noexcept {
            std::ostringstream sstr;
            for (auto& i: records) {
                sstr << "Started: " << i.timestamp << " ms." << "\n"
                     << "\tPause: " << i.pause << " ms." << "\n";
            }
            std::fprintf(out, "%s", sstr.str().c_str());
        }
    private:
        std::deque<Record> records;
    };

#else
    class Timer final {
    public:
        void start() {}
        void stop() {}
        double elapsedMilliseconds() {
            return 0;
        }
        Record report() {
            return {};
        }
    };

    class GCTimeRecorder final {
    public:
        void append(Timer& timer) {
            (void)(timer);
        }

        void report(FILE* out) noexcept {
            (void)(out);
        }
    private:
        std::deque<Record> records;
    };
#endif
}