#pragma once
#include "common.hpp"
#include "candle.hpp"

namespace program
{
    class labelling final
    {
    private:
        std::filesystem::path m_input_file;
        const char* m_out_dir;

        std::vector<std::unique_ptr<candle>> m_candles;

    public:
        labelling(std::filesystem::path file_path, const char* out_dir) :
            m_input_file(file_path), m_out_dir(out_dir)
        {

        }
        virtual ~labelling()
        {

        }

        const char* const file_name()
        {
            return m_input_file.filename().c_str();
        }

        void clone_candle(candle* dst, candle* src)
        {
            memcpy(dst, src, sizeof(candle));
        }

        void read_binary_input()
        {
            std::ifstream file(m_input_file.c_str(), std::ios::in | std::ios::binary);

            while (!file.eof())
            {
                std::unique_ptr<candle> candle_data = std::make_unique<candle>();

                file.read((char*)candle_data.get(), 8 * 18);

                m_candles.push_back(std::move(candle_data));
            }
        }

        void start()
        {
            this->read_binary_input();

            // do normalization
            std::vector<double> cum_down;
            std::vector<double> cum_up;

            double last_max = 0;
            bool allow_sell = false;
            int last_max_index = 0;

            double last_min = 0;
            bool allow_buy = false;
            int last_min_index = 0;

            for (size_t i = 0; i < m_candles.size(); i++)
            {
                std::unique_ptr<candle>& candle = m_candles.at(i);

                if (cum_up.size() && cum_down.size())
                {
                    double prev_up = cum_up[cum_up.size() - 1];
                    double prev_down = cum_down[cum_down.size() - 1];

                    if (candle->m_close > 0)
                    {
                        if (prev_up - prev_down > 0.01)
                        {
                            if (prev_up - prev_down > last_max)
                            {
                                last_max_index = i;
                                last_max = prev_up - prev_down;
                                allow_sell = true;
                            }

                            if (allow_buy)
                            {
                                // set sell at last_max
                                m_candles.at(last_min_index)->m_target = eTarget::BUY;
                                last_min_index = 0;
                                last_min = 0;
                                allow_buy = false;
                            }

                            cum_up.clear();
                            cum_down.clear();
                        }
                    }
                    else if (candle->m_close < 0)
                    {
                        if (prev_down - prev_up > 0.01)
                        {
                            if (allow_sell)
                            {
                                // set sell at last_max
                                m_candles.at(last_max_index)->m_target = eTarget::SELL;
                                last_max_index = 0;
                                last_max = 0;
                                allow_sell = false;
                            }

                            if (prev_down - prev_up > last_min)
                            {
                                last_min_index = i;
                                last_max = prev_down - prev_up;
                                allow_buy = true;
                            }

                            // set buy and sell

                            cum_up.clear();
                            cum_down.clear();
                        }

                    }
                }

                if (candle->m_close > 0)
                {
                    double prev = cum_up.size() ? cum_up[cum_up.size() - 1] : 0;

                    cum_up.push_back(prev + candle->m_close);
                }
                else
                {
                    double prev = cum_down.size() ? cum_down[cum_down.size() - 1] : 0;

                    cum_down.push_back(prev + abs(candle->m_close));
                }
            }

            this->write_binary_out();
        }

        void write_binary_out()
        {
            std::string out_dir = m_out_dir / m_input_file.stem();
            std::ofstream output_stream(out_dir + ".bin", std::ios::binary | std::ios::trunc);

            for (const std::unique_ptr<candle>& candle_struct : m_candles)
                output_stream.write((char*)candle_struct.get(), sizeof(candle));

            output_stream.close();
        }
    };
}
